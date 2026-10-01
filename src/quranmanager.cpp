#include "quranmanager.h"
#include "settingshelper.h"

#include <algorithm>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QRegularExpression>
#include <QSslConfiguration>
#include <QSslError>
#include <QSvgRenderer>
#include <QPainter>
#include <QSet>
#include <QPointer>
#include <QRunnable>
#include <QThreadPool>
#include <sailfishapp.h>

class PreloadNextAyahRunnable : public QRunnable
{
public:
    PreloadNextAyahRunnable(QuranManager *mgr, int page, int surah, int ayah, int rId, bool dark)
        : m_mgr(mgr), m_page(page), m_surah(surah), m_ayah(ayah), m_rId(rId), m_dark(dark)
    {
        setAutoDelete(true);
    }

    void run() override
    {
        if (m_mgr) {
            m_mgr->renderPageImage(m_page, m_surah, m_ayah, m_dark, m_rId);
        }
    }

private:
    QPointer<QuranManager> m_mgr;
    int m_page;
    int m_surah;
    int m_ayah;
    int m_rId;
    bool m_dark;
};

QuranManager::QuranManager(QObject *parent)
    : QObject(parent)
    , m_riwayah(1)
    , m_fontSize(32)
    , m_tafsirFontSize(24)
    , m_viewMode(0) // Default to Text View
    , m_darkMode(false)
    , m_tajweedMode(false)
    , m_lastSurah(1)
    , m_lastAyah(1)
    , m_lastPage(1)
    , m_player(nullptr)
    , m_playingSurah(0)
    , m_playingAyah(0)
    , m_maxAyahInPlayingSurah(7)
    , m_audioProgress(0.0)
    , m_currentDuration(0)
    , m_currentAudioFileIndex(0)
    , m_pendingSeekMs(0)
    , m_netManager(new QNetworkAccessManager(this))
    , m_isDownloadingPages(false)
    , m_bulkPagesRiwayah(1)
    , m_bulkPagesTotalInBatch(0)
    , m_bulkPagesDownloadedInBatch(0)
    , m_bulkPagesActiveCount(0)
    , m_isDownloadingAudio(false)
    , m_audioTotalInBatch(0)
    , m_audioDownloadedInBatch(0)
    , m_activeAudioReply(nullptr)
    , m_memCachedPageNumber(-1)
    , m_memCachedRiwayah(-1)
{
    m_pageImageCache.setMaxCost(30);
    m_netManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

    connect(&m_ncm, &QNetworkConfigurationManager::onlineStateChanged, this, [this](bool) {
        emit onlineStateChanged(isOnline());
    });
    connect(m_netManager, &QNetworkAccessManager::networkAccessibleChanged, this, [this](QNetworkAccessManager::NetworkAccessibility) {
        emit onlineStateChanged(isOnline());
    });

    // Load persisted settings
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    m_fontSize = s.value(QStringLiteral("quran/fontSize"), 32).toInt();
    m_tafsirFontSize = s.value(QStringLiteral("quran/tafsirFontSize"), 24).toInt();
    m_viewMode = s.value(QStringLiteral("quran/viewMode"), 0).toInt();
    m_darkMode = s.value(QStringLiteral("quran/darkMode"), false).toBool();
    m_tajweedMode = s.value(QStringLiteral("quran/tajweedMode"), false).toBool();
    m_lastSurah = s.value(QStringLiteral("quran/lastSurah"), 1).toInt();
    m_lastAyah = s.value(QStringLiteral("quran/lastAyah"), 1).toInt();
    m_lastPage = s.value(QStringLiteral("quran/lastPage"), 1).toInt();
    m_selectedTafsirEditionId = s.value(QStringLiteral("quran/tafsirEdition"), QStringLiteral("ar.muyassar")).toString();

    // Default Riwayah: check if previously chosen by user
    if (s.contains(QStringLiteral("quran/riwayah"))) {
        m_riwayah = s.value(QStringLiteral("quran/riwayah"), 1).toInt();
    } else {
        // Auto-detect: if country is Algeria / Maghreb, default to Warsh (2)!
        QString country = s.value(QStringLiteral("city/country")).toString().toLower();
        QString tz = s.value(QStringLiteral("city/tz")).toString().toLower();
        if (country.contains(QStringLiteral("algeria")) || country.contains(QString::fromUtf8("الجزائر"))
            || country.contains(QStringLiteral("morocco")) || country.contains(QString::fromUtf8("المغرب"))
            || country.contains(QStringLiteral("tunisia")) || country.contains(QString::fromUtf8("تونس"))
            || country.contains(QStringLiteral("mauritania")) || country.contains(QString::fromUtf8("موريتانيا"))
            || tz.contains(QStringLiteral("algiers")) || tz.contains(QStringLiteral("casablanca"))
            || tz.contains(QStringLiteral("tunis"))) {
            m_riwayah = 2; // Warsh 'an Nafi'
        } else {
            m_riwayah = 1; // Hafs 'an 'Asim
        }
    }

    m_selectedReciterId = s.value(QStringLiteral("quran/reciterId")).toString();

    initDatabase();
    initAudio();
}

QuranManager::~QuranManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QString QuranManager::findDatabasePath() const
{
    // 1. SailfishApp path
    QString p1 = SailfishApp::pathTo("data/quran.db").toLocalFile();
    if (QFile::exists(p1)) return p1;

    // 2. Installed path
    QString p2 = QStringLiteral("/usr/share/harbour-thakir/data/quran.db");
    if (QFile::exists(p2)) return p2;

    // 3. Local relative path (development)
    QString p3 = QDir::current().filePath("data/quran.db");
    if (QFile::exists(p3)) return p3;

    // 4. App directory relative
    QString p4 = QCoreApplication::applicationDirPath() + QStringLiteral("/../data/quran.db");
    if (QFile::exists(p4)) return p4;

    return p1;
}

void QuranManager::initDatabase()
{
    QString dbFile = findDatabasePath();
    qDebug() << "QuranManager: Loading database from:" << dbFile;

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("quran_connection"));
    m_db.setDatabaseName(dbFile);

    if (!m_db.open()) {
        qWarning() << "QuranManager: Failed to open SQLite database:" << m_db.lastError().text();
    } else {
        qDebug() << "QuranManager: Database successfully opened.";
        loadWarshToHafsMap();
        int exactPage = getPageForAyah(m_lastSurah, m_lastAyah, m_riwayah);
        if (exactPage > 0) {
            m_lastPage = exactPage;
        }
    }

    // Ensure default reciter is set if empty
    if (m_selectedReciterId.isEmpty()) {
        QVariantList reciters = getAvailableReciters();
        if (!reciters.isEmpty()) {
            m_selectedReciterId = reciters.first().toMap().value(QStringLiteral("id")).toString();
        }
    }
}

int QuranManager::mapHafsToWarsh(int surah, int hafsAyah) const
{
    if (m_riwayah != 2) return hafsAyah;
    quint32 hKey = (static_cast<quint32>(surah) << 16) | static_cast<quint32>(hafsAyah);
    return m_hafsToWarshMap.value(hKey, static_cast<quint16>(hafsAyah));
}

void QuranManager::loadWarshToHafsMap()
{
    m_warshToHafsMap.clear();
    m_hafsToWarshMap.clear();
    m_warshPageMap.clear();
    if (!m_db.isOpen()) return;

    QSqlQuery q(QStringLiteral("SELECT surah_number, warsh_ayah, hafs_ayah_start, hafs_ayah_end, start_ms, end_ms FROM warsh_to_hafs_map"), m_db);
    while (q.next()) {
        int surah = q.value(0).toInt();
        int wAyah = q.value(1).toInt();
        int hStart = q.value(2).toInt();
        int hEnd = q.value(3).toInt();
        HafsRange r;
        r.start = hStart;
        r.end = hEnd;
        r.startMs = q.value(4).toInt();
        r.endMs = q.value(5).toInt();
        quint32 key = (static_cast<quint32>(surah) << 16) | static_cast<quint32>(wAyah);
        m_warshToHafsMap.insert(key, r);

        for (int h = hStart; h <= hEnd; ++h) {
            quint32 hKey = (static_cast<quint32>(surah) << 16) | static_cast<quint32>(h);
            m_hafsToWarshMap.insert(hKey, static_cast<quint16>(wAyah));
        }
    }

    QSqlQuery qP(QStringLiteral("SELECT surah_number, ayah_number, page_number FROM ayahs WHERE riwayah_id = 2"), m_db);
    while (qP.next()) {
        quint32 key = (static_cast<quint32>(qP.value(0).toInt()) << 16) | static_cast<quint32>(qP.value(1).toInt());
        m_warshPageMap.insert(key, static_cast<quint16>(qP.value(2).toInt()));
    }

    qDebug() << "[QuranManager] Loaded" << m_warshToHafsMap.size() << "Warsh-to-Hafs mappings and" << m_warshPageMap.size() << "Warsh pages.";
}

bool QuranManager::reciterNeedsHafsMapping(const QString &reciterId) const
{
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    return rId == QStringLiteral("yassin_al_jazaery");
}

QList<int> QuranManager::getAudioFilesForAyah(const QString &reciterId, int surah, int ayah) const
{
    QList<int> result;
    if (m_riwayah == 2 && reciterNeedsHafsMapping(reciterId)) {
        quint32 key = (static_cast<quint32>(surah) << 16) | static_cast<quint32>(ayah);
        if (m_warshToHafsMap.contains(key)) {
            HafsRange range = m_warshToHafsMap.value(key);
            for (int h = range.start; h <= range.end; ++h) {
                result.append(h);
            }
            return result;
        }
    }
    result.append(ayah);
    return result;
}

int QuranManager::getAudioStartOffsetMs(const QString &reciterId, int surah, int ayah) const
{
    if (m_riwayah == 2 && reciterNeedsHafsMapping(reciterId)) {
        quint32 key = (static_cast<quint32>(surah) << 16) | static_cast<quint32>(ayah);
        if (m_warshToHafsMap.contains(key)) {
            return m_warshToHafsMap.value(key).startMs;
        }
    }
    return 0;
}

int QuranManager::getEffectiveAudioAyah(const QString &reciterId, int surah, int ayah) const
{
    QList<int> files = getAudioFilesForAyah(reciterId, surah, ayah);
    return files.isEmpty() ? ayah : files.first();
}

void QuranManager::initAudio()
{
    m_player = new QMediaPlayer(this);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &QuranManager::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &QuranManager::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &QuranManager::onDurationChanged);
    connect(m_player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
            this, &QuranManager::onPlayerError);
}

void QuranManager::onPlayerError(QMediaPlayer::Error error)
{
    if (error != QMediaPlayer::NoError) {
        qWarning() << "[QuranManager] QMediaPlayer error:" << error << (m_player ? m_player->errorString() : QString());
        bool isNetwork = (error == QMediaPlayer::NetworkError || error == QMediaPlayer::ResourceError);
        emit audioPlaybackFailed(m_playingSurah, m_playingAyah, isNetwork);
    }
}

int QuranManager::riwayah() const
{
    return m_riwayah;
}

void QuranManager::setRiwayah(int rId)
{
    if (rId != 1 && rId != 2) return;
    if (m_riwayah != rId) {
        if (isPlaying()) {
            stopAudio();
        }
        m_riwayah = rId;
        m_memCachedPageNumber = -1;
        m_memCachedRiwayah = -1;
        m_memCachedSvgContent.clear();

        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/riwayah"), m_riwayah);

        // Update default reciter for new Riwayah
        QVariantList reciters = getAvailableReciters();
        if (!reciters.isEmpty()) {
            m_selectedReciterId = reciters.first().toMap().value(QStringLiteral("id")).toString();
            s.setValue(QStringLiteral("quran/reciterId"), m_selectedReciterId);
            emit reciterChanged();
        }

        int exactPage = getPageForAyah(m_lastSurah, m_lastAyah, m_riwayah);
        if (exactPage > 0) {
            m_lastPage = exactPage;
            s.setValue(QStringLiteral("quran/lastPage"), m_lastPage);
        }
        s.sync();

        emit riwayahChanged();
        emit lastPositionChanged();
    }
}

QString QuranManager::riwayahCode() const
{
    return m_riwayah == 2 ? QStringLiteral("warsh") : QStringLiteral("hafs");
}

QString QuranManager::riwayahName() const
{
    return m_riwayah == 2 ? QString::fromUtf8("ورش عن نافع") : QString::fromUtf8("حفص عن عاصم");
}

QString QuranManager::riwayahDescription() const
{
    return m_riwayah == 2
        ? QString::fromUtf8("رواية الإمام ورش عن نافع المدني من طريق الأزرق، الرواية الرسمية في الجزائر والمغرب العربي.")
        : QString::fromUtf8("رواية الإمام حفص عن عاصم، الرواية الأكثر انتشاراً في العالم الإسلامي.");
}

int QuranManager::fontSize() const
{
    return m_fontSize;
}

void QuranManager::setFontSize(int size)
{
    int clamped = qBound(18, size, 75);
    if (m_fontSize != clamped) {
        m_fontSize = clamped;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/fontSize"), m_fontSize);
        s.sync();
        emit fontSizeChanged();
    }
}

int QuranManager::tafsirFontSize() const
{
    return m_tafsirFontSize;
}

void QuranManager::setTafsirFontSize(int size)
{
    int clamped = qBound(14, size, 56);
    if (m_tafsirFontSize != clamped) {
        m_tafsirFontSize = clamped;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/tafsirFontSize"), m_tafsirFontSize);
        s.sync();
        emit tafsirFontSizeChanged();
    }
}

int QuranManager::viewMode() const
{
    return m_viewMode;
}

void QuranManager::setViewMode(int mode)
{
    if (mode < 0 || mode > 1) mode = 0;
    if (m_viewMode != mode) {
        m_viewMode = mode;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/viewMode"), m_viewMode);
        s.sync();
        emit viewModeChanged();
    }
}

bool QuranManager::darkMode() const
{
    return m_darkMode;
}

void QuranManager::setDarkMode(bool dark)
{
    if (m_darkMode != dark) {
        m_darkMode = dark;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/darkMode"), m_darkMode);
        s.sync();
        emit darkModeChanged();
    }
}

bool QuranManager::tajweedMode() const
{
    return m_tajweedMode;
}

void QuranManager::setTajweedMode(bool enabled)
{
    if (m_tajweedMode != enabled) {
        m_tajweedMode = enabled;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/tajweedMode"), m_tajweedMode);
        s.sync();
        emit tajweedModeChanged();
    }
}

static bool isTransparentArabic(QChar c)
{
    ushort u = c.unicode();
    return (u >= 0x064B && u <= 0x065F) // harakat, sukun, shaddah, tanween
        || (u == 0x0670)                // dagger alif
        || (u >= 0x06D6 && u <= 0x06ED) // Quranic marks
        || (u == 0x0653)                // maddah above
        || (u >= 0x08D4 && u <= 0x08ED)
        || (u >= 0x200B && u <= 0x200F); // formatting marks (ZWJ, ZWNJ, etc.)
}

static bool connectsToNextArabic(QChar c)
{
    ushort u = c.unicode();
    // Dual-joining letters connect to the following letter:
    switch (u) {
    case 0x0628: case 0x062A: case 0x062B: // ب ت ث
    case 0x062C: case 0x062D: case 0x062E: // ج ح خ
    case 0x0633: case 0x0634:             // س ش
    case 0x0635: case 0x0636:             // ص ض
    case 0x0637: case 0x0638:             // ط ظ
    case 0x0639: case 0x063A:             // ع غ
    case 0x0641: case 0x0642:             // ف ق
    case 0x0643: case 0x0644: case 0x0645: case 0x0646: case 0x0647: case 0x064A: // ك ل م ن ه ي
    case 0x0626:                         // ئ
    case 0x0640:                         // Tatweel ـ
    case 0x200D:                         // ZWJ
    case 0x066E: case 0x066F:             // Dotless baa / qaf
    case 0x067E: case 0x0686: case 0x06A9: case 0x06AF: case 0x06CC:
        return true;
    default:
        return false;
    }
}

static bool connectsToPrevArabic(QChar c)
{
    if (connectsToNextArabic(c)) return true;
    ushort u = c.unicode();
    // Right-joining letters:
    switch (u) {
    case 0x0627: case 0x0622: case 0x0623: case 0x0625: case 0x0671: // ا آ أ إ ٱ
    case 0x062F: case 0x0630: // د ذ
    case 0x0631: case 0x0632: // ر ز
    case 0x0648: case 0x0624: // و ؤ
    case 0x0629:             // ة
    case 0x0649:             // ى
    case 0x0672: case 0x0673: case 0x0675: case 0x0688: case 0x0698:
        return true;
    default:
        return false;
    }
}

static QChar lastBaseChar(const QString &s)
{
    for (int i = s.length() - 1; i >= 0; --i) {
        if (!isTransparentArabic(s.at(i)))
            return s.at(i);
    }
    return QChar();
}

static QChar firstBaseChar(const QString &s)
{
    for (int i = 0; i < s.length(); ++i) {
        if (!isTransparentArabic(s.at(i)))
            return s.at(i);
    }
    return QChar();
}

QString QuranManager::formatTajweedHtml(const QString &tajweedText, bool isDark) const
{
    if (tajweedText.isEmpty()) return QString();

    auto getColor = [isDark](QChar rule) -> QString {
        switch (rule.toLatin1()) {
        case 'h': // Hamzat Wasl (همزة وصل)
        case 's': // Silent (حروف لا تنطق)
        case 'l': // Laam Shamsiyah (لام شمسية)
            return isDark ? QStringLiteral("#888888") : QStringLiteral("#999999");
        case 'n': // Madd Tabii (مد طبيعي - حركتان)
            return isDark ? QStringLiteral("#7BA0FF") : QStringLiteral("#537FFF");
        case 'p': // Madd Aaridh (مد عارض للسكون - 2 أو 4 أو 6 حركات)
            return isDark ? QStringLiteral("#6E88FF") : QStringLiteral("#4050FF");
        case 'm': // Madd Lazim (مد لازم - 6 حركات)
            return isDark ? QStringLiteral("#506FFF") : QStringLiteral("#000EBC");
        case 'o': // Madd Wajib / Jaiz Munfasil (مد واجب أو منفصل - 4-5 حركات)
            return isDark ? QStringLiteral("#5F7DF5") : QStringLiteral("#2144C1");
        case 'q': // Qalqalah (قلقلة)
            return isDark ? QStringLiteral("#FF5555") : QStringLiteral("#DD0008");
        case 'g': // Ghunnah (غنة)
            return isDark ? QStringLiteral("#FFA540") : QStringLiteral("#FF7E1E");
        case 'f': // Ikhfa (إخفاء)
            return isDark ? QStringLiteral("#D055E8") : QStringLiteral("#9400A8");
        case 'c': // Ikhfa Shafawi (إخفاء شفوي)
            return isDark ? QStringLiteral("#F260E0") : QStringLiteral("#D500B7");
        case 'i': // Iqlab (إقلاب)
            return isDark ? QStringLiteral("#60D5FF") : QStringLiteral("#26BFFD");
        case 'a': // Idgham with Ghunnah (إدغام بغنة)
        case 'u': // Idgham without Ghunnah (إدغام بغير غنة)
            return isDark ? QStringLiteral("#4ED436") : QStringLiteral("#169200");
        case 'w': // Idgham Shafawi (إدغام شفوي)
            return isDark ? QStringLiteral("#8CE442") : QStringLiteral("#58B800");
        case 'd': // Idgham Mutajanisayn (إدغام متجانسين)
        case 'b': // Idgham Mutaqaribayn (إدغام متقاربين)
            return isDark ? QStringLiteral("#B0B0B0") : QStringLiteral("#8E8E8E");
        default:
            return QString();
        }
    };

    QString text = tajweedText;

    // Fix 1: Merge [s[و][n[ٲ] or [s[و][n[ٰ] into [n[وٰ] (Madd Tabii on Waw with Dagger Alif)
    // In words like ٱلرِّبَوٰا۟, ٱلصَّلَوٰةَ, ٱلزَّكَوٰةَ, ٱلْحَيَوٰةَ:
    // the source dataset used [s[و][n[ٲ], which renders as an extra standalone Alef
    // instead of a dagger alif on top of waw.
    static const QRegularExpression wawDaggerRx(QStringLiteral("\\[s[a-z0-9_:]*\\[\\x{0648}\\]\\[n[a-z0-9_:]*\\[[\\x{0672}\\x{0670}]\\]"));
    text.replace(wawDaggerRx, QStringLiteral("[n[") + QChar(0x0648) + QChar(0x0670) + QStringLiteral("]"));

    // Fix 2: Normalize all remaining occurrences of 0x0672 (Alef with wavy hamza)
    // to 0x0670 (Arabic letter superscript/dagger alif).
    text.replace(QChar(0x0672), QChar(0x0670));

    // Fix 3: Collapse multiple consecutive tatweels (ـ) to a single tatweel
    // Words like يَسْــَٔلُونَ, تَسْــَٔلُوا, تُسْــَٔلُونَ had double tatweels (ـ)
    // which breaks the baseline in OpenType fonts and leaves a wide gap with a detached hamza.
    static const QRegularExpression multiTatweelRx(QStringLiteral("\\x{0640}{2,}"));
    text.replace(multiTatweelRx, QString(QChar(0x0640)));

    // Fix 4: For Madd Tabii tags containing a dagger alif (such as [n[َٰ], [n[ٰ], or [n[ـٰ]),
    // pull the preceding Arabic base consonant into the tag (e.g. مَ[n[ٰ]] -> [n[مَٰ]]).
    // This allows OpenType / HarfBuzz to anchor the dagger alif HIGH UP (superscript)
    // above the letter rather than falling to the baseline on an artificial tatweel stroke.
    static const QRegularExpression daggerPullRx(QStringLiteral("([\\x{0621}-\\x{064A}][\\x{064B}-\\x{065F}]*)\\[([a-z])[a-z0-9_:]*\\[[\\x{0640}]*([\\x{064E}]?[\\x{0670}][\\x{064B}-\\x{065F}]*)\\]"));
    text.replace(daggerPullRx, QStringLiteral("[\\2[\\1") + QChar(0x0670) + QStringLiteral("]]"));

    // Fallback: If any other combining-only tag has no preceding consonant, inject a carrier
    static const QRegularExpression markOnlyTagRx(QStringLiteral("\\[([a-z])[a-z0-9_:]*\\[([\\x{064B}-\\x{065F}\\x{0670}\\x{0653}]+)\\]"));
    text.replace(markOnlyTagRx, QStringLiteral("[\\1[") + QChar(0x0640) + QStringLiteral("\\2]"));

    // Step 1: Absorb trailing diacritics directly after ']' into the tag
    static const QRegularExpression orphanDiacriticRx(QStringLiteral("\\]([\\x{064B}-\\x{065F}\\x{0670}\\x{0653}]+)"));
    text.replace(orphanDiacriticRx, QStringLiteral("\\1]"));

    // Step 2: Parse into contiguous runs of text with assigned rules
    struct TajweedRun {
        QString text;
        QChar rule;
    };
    QList<TajweedRun> runs;
    QList<QChar> stack;
    QString currentText;

    static const QRegularExpression openTagRx(QStringLiteral("^\\[([a-z])[a-z0-9_:]*\\["));

    int i = 0;
    const int len = text.length();
    while (i < len) {
        if (text.at(i) == QLatin1Char('[')) {
            QRegularExpressionMatch m = openTagRx.match(text.mid(i));
            if (m.hasMatch()) {
                if (!currentText.isEmpty()) {
                    TajweedRun r;
                    r.text = currentText;
                    r.rule = stack.isEmpty() ? QChar() : stack.last();
                    runs.append(r);
                    currentText.clear();
                }
                stack.append(m.captured(1).at(0));
                i += m.capturedLength();
                continue;
            } else {
                i++;
                continue;
            }
        } else if (text.at(i) == QLatin1Char(']')) {
            if (!currentText.isEmpty()) {
                TajweedRun r;
                r.text = currentText;
                r.rule = stack.isEmpty() ? QChar() : stack.last();
                runs.append(r);
                currentText.clear();
            }
            if (!stack.isEmpty()) {
                stack.removeLast();
            }
            i++;
            continue;
        } else {
            currentText.append(text.at(i));
            i++;
        }
    }
    if (!currentText.isEmpty()) {
        TajweedRun r;
        r.text = currentText;
        r.rule = stack.isEmpty() ? QChar() : stack.last();
        runs.append(r);
    }

    // Step 3: Merge adjacent runs having the exact same rule/color
    QList<TajweedRun> merged;
    for (const TajweedRun &r : runs) {
        if (r.text.isEmpty()) continue;
        if (!merged.isEmpty() && merged.last().rule == r.rule) {
            merged.last().text.append(r.text);
        } else {
            merged.append(r);
        }
    }

    // Step 4: Ligature Repair via Zero-Width Joiner (ZWJ U+200D)
    // Whenever an intra-word boundary connects from left to right, insert ZWJ
    static const QChar zwj(0x200D);
    for (int k = 0; k < merged.size() - 1; ++k) {
        TajweedRun &r1 = merged[k];
        TajweedRun &r2 = merged[k + 1];

        QChar cLast = lastBaseChar(r1.text);
        QChar cFirst = firstBaseChar(r2.text);

        if (connectsToNextArabic(cLast) && connectsToPrevArabic(cFirst)) {
            r1.text.append(zwj);
            r2.text.prepend(zwj);
        }
    }

    // Step 5: Format into final HTML spans
    QString result;
    result.reserve(text.length() * 2);
    for (const TajweedRun &r : merged) {
        QString col = r.rule.isNull() ? QString() : getColor(r.rule);
        if (!col.isEmpty()) {
            result.append(QStringLiteral("<span style=\"color:%1;\">%2</span>").arg(col, r.text));
        } else {
            result.append(r.text);
        }
    }

    return result;
}

int QuranManager::lastSurah() const
{
    return m_lastSurah;
}

int QuranManager::lastAyah() const
{
    return m_lastAyah;
}

int QuranManager::lastPage() const
{
    if (m_lastSurah > 0 && m_lastAyah > 0) {
        int p = getPageForAyah(m_lastSurah, m_lastAyah, m_riwayah);
        if (p >= 1 && p <= 604) {
            return p;
        }
    }
    return m_lastPage;
}

QString QuranManager::selectedTafsirEditionId() const
{
    return m_selectedTafsirEditionId;
}

void QuranManager::setSelectedTafsirEditionId(const QString &editionId)
{
    if (editionId.isEmpty()) return;
    if (m_selectedTafsirEditionId != editionId) {
        m_selectedTafsirEditionId = editionId;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/tafsirEdition"), m_selectedTafsirEditionId);
        s.sync();
        emit selectedTafsirEditionChanged();
    }
}

bool QuranManager::isPlaying() const
{
    return m_player && (m_player->state() == QMediaPlayer::PlayingState);
}

int QuranManager::playingSurah() const
{
    return m_playingSurah;
}

int QuranManager::playingAyah() const
{
    return m_playingAyah;
}

QString QuranManager::selectedReciterId() const
{
    return m_selectedReciterId;
}

QString QuranManager::selectedReciterName() const
{
    QVariantList reciters = const_cast<QuranManager*>(this)->getAvailableReciters();
    for (const QVariant &r : reciters) {
        QVariantMap m = r.toMap();
        if (m.value(QStringLiteral("id")).toString() == m_selectedReciterId) {
            return m.value(QStringLiteral("name_ar")).toString();
        }
    }
    if (!reciters.isEmpty()) {
        return reciters.first().toMap().value(QStringLiteral("name_ar")).toString();
    }
    return QString();
}

double QuranManager::audioProgress() const
{
    return m_audioProgress;
}

QVariantList QuranManager::getRiwayat()
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT id, code, name_ar, name_en, description_ar, total_verses FROM riwayat ORDER BY id ASC"));
    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            m[QStringLiteral("id")] = q.value(0).toInt();
            m[QStringLiteral("code")] = q.value(1).toString();
            m[QStringLiteral("name_ar")] = q.value(2).toString();
            m[QStringLiteral("name_en")] = q.value(3).toString();
            m[QStringLiteral("description_ar")] = q.value(4).toString();
            m[QStringLiteral("total_verses")] = q.value(5).toInt();
            list.append(m);
        }
    }
    return list;
}

QVariantList QuranManager::getSurahs()
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    // Riwayah-specific verse counts and start pages
    QMap<int, int> verseCounts;
    QMap<int, int> startPages;
    QSqlQuery qMeta(m_db);
    qMeta.prepare(QStringLiteral("SELECT surah_number, COUNT(*), MIN(page_number) FROM ayahs WHERE riwayah_id = ? GROUP BY surah_number"));
    qMeta.addBindValue(m_riwayah);
    if (qMeta.exec()) {
        while (qMeta.next()) {
            int sNum = qMeta.value(0).toInt();
            verseCounts[sNum] = qMeta.value(1).toInt();
            startPages[sNum] = qMeta.value(2).toInt();
        }
    }

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT number, name_ar, name_en, name_translation, revelation_type, total_verses, start_page FROM surahs ORDER BY number ASC"));
    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            int sNum = q.value(0).toInt();
            m[QStringLiteral("number")] = sNum;
            m[QStringLiteral("name_ar")] = q.value(1).toString();
            m[QStringLiteral("name_en")] = q.value(2).toString();
            m[QStringLiteral("name_translation")] = q.value(3).toString();
            m[QStringLiteral("revelation_type")] = q.value(4).toString();
            m[QStringLiteral("total_verses")] = verseCounts.value(sNum, q.value(5).toInt());
            m[QStringLiteral("start_page")] = startPages.value(sNum, q.value(6).toInt());
            list.append(m);
        }
    }
    return list;
}

QVariantMap QuranManager::getSurah(int surahNumber)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    int totalVerses = 0;
    int startPage = 0;
    QSqlQuery qMeta(m_db);
    qMeta.prepare(QStringLiteral("SELECT COUNT(*), MIN(page_number) FROM ayahs WHERE riwayah_id = ? AND surah_number = ?"));
    qMeta.addBindValue(m_riwayah);
    qMeta.addBindValue(surahNumber);
    if (qMeta.exec() && qMeta.next()) {
        totalVerses = qMeta.value(0).toInt();
        startPage = qMeta.value(1).toInt();
    }

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT number, name_ar, name_en, name_translation, revelation_type, total_verses, start_page FROM surahs WHERE number = ?"));
    q.addBindValue(surahNumber);
    if (q.exec() && q.next()) {
        m[QStringLiteral("number")] = q.value(0).toInt();
        m[QStringLiteral("name_ar")] = q.value(1).toString();
        m[QStringLiteral("name_en")] = q.value(2).toString();
        m[QStringLiteral("name_translation")] = q.value(3).toString();
        m[QStringLiteral("revelation_type")] = q.value(4).toString();
        m[QStringLiteral("total_verses")] = totalVerses > 0 ? totalVerses : q.value(5).toInt();
        m[QStringLiteral("start_page")] = startPage > 0 ? startPage : q.value(6).toInt();
    }
    return m;
}

QVariantList QuranManager::getJuzs()
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT j.number, j.name_ar, j.surah_number, j.ayah_number, j.start_page, s.name_ar, s.name_en FROM juzs j JOIN surahs s ON j.surah_number = s.number ORDER BY j.number ASC"));
    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            m[QStringLiteral("number")] = q.value(0).toInt();
            m[QStringLiteral("name_ar")] = q.value(1).toString();
            m[QStringLiteral("surah_number")] = q.value(2).toInt();
            m[QStringLiteral("ayah_number")] = q.value(3).toInt();
            m[QStringLiteral("start_page")] = q.value(4).toInt();
            m[QStringLiteral("surah_name_ar")] = q.value(5).toString();
            m[QStringLiteral("surah_name_en")] = q.value(6).toString();
            list.append(m);
        }
    }
    return list;
}

QVariantList QuranManager::getHizbs()
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT h.number, h.juz_number, h.surah_number, h.ayah_number, h.start_page, s.name_ar, s.name_en FROM hizbs h JOIN surahs s ON h.surah_number = s.number ORDER BY h.number ASC"));
    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            m[QStringLiteral("number")] = q.value(0).toInt();
            m[QStringLiteral("juz_number")] = q.value(1).toInt();
            m[QStringLiteral("surah_number")] = q.value(2).toInt();
            m[QStringLiteral("ayah_number")] = q.value(3).toInt();
            m[QStringLiteral("start_page")] = q.value(4).toInt();
            m[QStringLiteral("surah_name_ar")] = q.value(5).toString();
            m[QStringLiteral("surah_name_en")] = q.value(6).toString();
            list.append(m);
        }
    }
    return list;
}

static QString cleanUthmaniText(const QString &text)
{
    if (text.isEmpty()) return text;
    QString result = text;
    // Fix broken yaa + tatweel + hamza sequences in raw text (e.g. Warsh: يُنَبِّيـُٔهُمُ -> يُنَبِّئُهُمُ, جِيـْٔتَ -> جِئْتَ, شِيـْٔتَ -> شِئْتَ)
    result.replace(QString::fromUtf8("\u064a\u0640\u0654"), QString::fromUtf8("\u0626"));
    result.replace(QString::fromUtf8("\u064a\u0654"), QString::fromUtf8("\u0626"));

    // Fix double tatweel + hamza (e.g. شَيْــٔاً -> شَيْئاً)
    result.replace(QString::fromUtf8("\u0640\u0640\u0654"), QString::fromUtf8("\u0626"));
    result.replace(QString::fromUtf8("\u0640\u0640\u0655"), QString::fromUtf8("\u0626"));

    // Fix single tatweel + hamza (ـٔ / ـٕ)
    result.replace(QString::fromUtf8("\u0640\u0654"), QString::fromUtf8("\u0626"));
    result.replace(QString::fromUtf8("\u0640\u0655"), QString::fromUtf8("\u0626"));

    // Remove any leftover unnecessary tatweels that break word appearance into long stretched lines
    result.replace(QChar(0x0640), QString());

    return result;
}

QVariantList QuranManager::getAyahsForSurah(int surahNumber)
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT ayah_number, text_uthmani, page_number, juz_number, hizb_number, rub_number, sajda, text_tajweed FROM ayahs WHERE riwayah_id = ? AND surah_number = ? ORDER BY ayah_number ASC"));
    q.addBindValue(m_riwayah);
    q.addBindValue(surahNumber);

    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            int aNum = q.value(0).toInt();
            m[QStringLiteral("surah_number")] = surahNumber;
            m[QStringLiteral("ayah_number")] = aNum;
            m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(1).toString());
            m[QStringLiteral("page_number")] = q.value(2).toInt();
            m[QStringLiteral("juz_number")] = q.value(3).toInt();
            m[QStringLiteral("hizb_number")] = q.value(4).toInt();
            m[QStringLiteral("rub_number")] = q.value(5).toInt();
            m[QStringLiteral("sajda")] = q.value(6).toInt() == 1;
            m[QStringLiteral("text_tajweed")] = q.value(7).toString();
            m[QStringLiteral("isBookmarked")] = isBookmarked(surahNumber, aNum);
            list.append(m);
        }
    }
    return list;
}

QVariantList QuranManager::getAyahsForPage(int pageNumber)
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT a.surah_number, a.ayah_number, a.text_uthmani, a.juz_number, a.hizb_number, a.rub_number, a.sajda, s.name_ar, s.name_en, a.text_tajweed FROM ayahs a JOIN surahs s ON a.surah_number = s.number WHERE a.riwayah_id = ? AND a.page_number = ? ORDER BY a.surah_number ASC, a.ayah_number ASC"));
    q.addBindValue(m_riwayah);
    q.addBindValue(pageNumber);

    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            int sNum = q.value(0).toInt();
            int aNum = q.value(1).toInt();
            m[QStringLiteral("surah_number")] = sNum;
            m[QStringLiteral("ayah_number")] = aNum;
            m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(2).toString());
            m[QStringLiteral("juz_number")] = q.value(3).toInt();
            m[QStringLiteral("hizb_number")] = q.value(4).toInt();
            m[QStringLiteral("rub_number")] = q.value(5).toInt();
            m[QStringLiteral("sajda")] = q.value(6).toInt() == 1;
            m[QStringLiteral("surah_name_ar")] = q.value(7).toString();
            m[QStringLiteral("surah_name_en")] = q.value(8).toString();
            m[QStringLiteral("text_tajweed")] = q.value(9).toString();
            m[QStringLiteral("page_number")] = pageNumber;
            m[QStringLiteral("isBookmarked")] = isBookmarked(sNum, aNum);
            list.append(m);
        }
    }
    return list;
}

int QuranManager::getPageForAyah(int surah, int ayah, int riwayahId) const
{
    if (!m_db.isOpen()) return 1;
    int rId = (riwayahId > 0) ? riwayahId : m_riwayah;
    int sNum = qBound(1, surah, 114);
    int aNum = qMax(1, ayah);

    if (rId == 2) {
        quint32 key = (static_cast<quint32>(sNum) << 16) | static_cast<quint32>(aNum);
        if (m_warshPageMap.contains(key)) {
            return m_warshPageMap.value(key);
        }
    }

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT page_number FROM ayahs WHERE riwayah_id = ? AND surah_number = ? AND ayah_number = ?"));
    q.addBindValue(rId);
    q.addBindValue(sNum);
    q.addBindValue(aNum);
    if (q.exec() && q.next()) {
        int p = q.value(0).toInt();
        if (p >= 1 && p <= 604) {
            return p;
        }
    }

    QSqlQuery qSurah(m_db);
    qSurah.prepare(QStringLiteral("SELECT start_page FROM surahs WHERE number = ?"));
    qSurah.addBindValue(sNum);
    if (qSurah.exec() && qSurah.next()) {
        int sp = qSurah.value(0).toInt();
        if (sp >= 1 && sp <= 604) {
            return sp;
        }
    }

    return 1;
}

QVariantMap QuranManager::getAyah(int surahNumber, int ayahNumber, int riwayahId)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    int rId = (riwayahId > 0) ? riwayahId : m_riwayah;
    int sNum = qBound(1, surahNumber, 114);

    // Surah metadata
    int totalVerses = 0;
    QSqlQuery qSurah(m_db);
    qSurah.prepare(QStringLiteral("SELECT name_ar, name_en, total_verses FROM surahs WHERE number = ?"));
    qSurah.addBindValue(sNum);
    if (qSurah.exec() && qSurah.next()) {
        m[QStringLiteral("surah_name_ar")] = qSurah.value(0).toString();
        m[QStringLiteral("surah_name_en")] = qSurah.value(1).toString();
        totalVerses = qSurah.value(2).toInt();
    }

    // Riwayah-specific verse count
    QSqlQuery qCount(m_db);
    qCount.prepare(QStringLiteral("SELECT COUNT(*) FROM ayahs WHERE riwayah_id = ? AND surah_number = ?"));
    qCount.addBindValue(rId);
    qCount.addBindValue(sNum);
    if (qCount.exec() && qCount.next()) {
        int cnt = qCount.value(0).toInt();
        if (cnt > 0) {
            totalVerses = cnt;
        }
    }
    m[QStringLiteral("total_verses")] = totalVerses;

    int effectiveAyah = (totalVerses > 0) ? qBound(1, ayahNumber, totalVerses) : qMax(1, ayahNumber);

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT a.ayah_number, a.text_uthmani, a.page_number, a.juz_number, a.hizb_number, a.rub_number, a.sajda, s.name_ar, s.name_en, a.text_tajweed FROM ayahs a JOIN surahs s ON a.surah_number = s.number WHERE a.riwayah_id = ? AND a.surah_number = ? AND a.ayah_number = ?"));
    q.addBindValue(rId);
    q.addBindValue(sNum);
    q.addBindValue(effectiveAyah);

    if (q.exec() && q.next()) {
        m[QStringLiteral("surah_number")] = sNum;
        m[QStringLiteral("ayah_number")] = q.value(0).toInt();
        m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(1).toString());
        m[QStringLiteral("page_number")] = q.value(2).toInt();
        m[QStringLiteral("juz_number")] = q.value(3).toInt();
        m[QStringLiteral("hizb_number")] = q.value(4).toInt();
        m[QStringLiteral("rub_number")] = q.value(5).toInt();
        m[QStringLiteral("sajda")] = q.value(6).toInt() == 1;
        m[QStringLiteral("surah_name_ar")] = q.value(7).toString();
        m[QStringLiteral("surah_name_en")] = q.value(8).toString();
        m[QStringLiteral("text_tajweed")] = q.value(9).toString();
        m[QStringLiteral("isBookmarked")] = isBookmarked(sNum, effectiveAyah);
    } else {
        m[QStringLiteral("surah_number")] = sNum;
        m[QStringLiteral("ayah_number")] = effectiveAyah;
    }
    return m;
}

QString QuranManager::normalizeArabic(const QString &input) const
{
    QString s = input;
    // Remove Tashkeel
    static const QRegularExpression tashkeelRx(QStringLiteral("[\\x{0617}-\\x{061A}\\x{064B}-\\x{065F}\\x{0670}\\x{06D6}-\\x{06ED}\\x{08D4}-\\x{08ED}\\x{06DF}\\x{06E0}\\x{06E2}\\x{06E3}\\x{06E5}\\x{06E6}\\x{06EA}-\\x{06EC}]"));
    s.remove(tashkeelRx);
    s.remove(QChar(0x0640)); // Tatweel
    // Normalize Alef
    static const QRegularExpression alefRx(QStringLiteral("[إأآٱ]"));
    s.replace(alefRx, QStringLiteral("ا"));
    s.replace(QStringLiteral("ءا"), QStringLiteral("ا"));
    s.replace(QChar(0x06D2), QStringLiteral("ي"));
    s.replace(QChar(0x06D3), QStringLiteral("ي"));
    s.replace(QStringLiteral("ة"), QStringLiteral("ه"));
    s.replace(QStringLiteral("ى"), QStringLiteral("ي"));

    bool startsWithSpace = s.startsWith(QLatin1Char(' '));
    bool endsWithSpace = s.endsWith(QLatin1Char(' '));
    s = s.simplified();
    if (s.isEmpty()) {
        return QString();
    }
    if (startsWithSpace) {
        s.prepend(QLatin1Char(' '));
    }
    if (endsWithSpace) {
        s.append(QLatin1Char(' '));
    }
    return s;
}

static QRegularExpression buildWordWithAffixesRegex(const QString &word, QStringList *outStems = nullptr)
{
    QString w = word.trimmed();
    QString stem = w;
    bool isSpecial = (w == QStringLiteral("الله") || w == QStringLiteral("اللهم") || w.startsWith(QStringLiteral("الذ")));
    if (!isSpecial && stem.startsWith(QStringLiteral("ال")) && stem.length() > 3) {
        stem = stem.mid(2);
    }

    QStringList stems;
    stems.append(stem);

    // Feminine taa marbuta / sound feminine plurals
    if (stem.endsWith(QStringLiteral("ه")) && stem.length() > 2) {
        stems.append(stem.left(stem.length() - 1) + QStringLiteral("ات"));
        stems.append(stem.left(stem.length() - 1) + QStringLiteral("ت"));
    } else if (stem.endsWith(QStringLiteral("ات")) && stem.length() > 3) {
        stems.append(stem.left(stem.length() - 2) + QStringLiteral("ه"));
    }

    // Common Quranic word forms (Dr. AbdulAzeez / QuranProgress)
    if (stem == QStringLiteral("كتاب")) {
        stems.append(QStringLiteral("كتب"));
    } else if (stem == QStringLiteral("رسول")) {
        stems.append(QStringLiteral("رسل"));
    } else if (stem == QStringLiteral("يوم")) {
        stems.append(QStringLiteral("ايام"));
    } else if (stem == QStringLiteral("ملك")) {
        stems.append(QStringLiteral("ملائكه"));
        stems.append(QStringLiteral("ملئكه"));
        stems.append(QStringLiteral("ملكين"));
    } else if (stem == QStringLiteral("من")) {
        stems.append(QStringLiteral("مما"));
        stems.append(QStringLiteral("ممن"));
    } else if (stem == QStringLiteral("سماء")) {
        stems.append(QStringLiteral("سموات"));
        stems.append(QStringLiteral("سماوات"));
    } else if (stem == QStringLiteral("نفس")) {
        stems.append(QStringLiteral("انفس"));
    } else if (stem == QStringLiteral("الله")) {
        stems.append(QStringLiteral("لله"));
        stems.append(QStringLiteral("اللهم"));
        stems.append(QStringLiteral("تالله"));
    }

    if (outStems) {
        for (const QString &s : stems) {
            if (!outStems->contains(s)) {
                outStems->append(s);
            }
        }
    }

    QStringList escaped;
    for (const QString &s : stems) {
        escaped.append(QRegularExpression::escape(s));
    }
    QString stemPat = escaped.join(QLatin1Char('|'));

    // Proclitics: و, ف, ب, ك, ل, ال and compounds (وال, فال, بال, كال, لل, ولل, فلل, ول, فل, وب, فب, وك, فك)
    // 'ت' is excluded to prevent matching verbs like 'تقوم'; swearing 'تالله' is handled in special stems.
    QString pref = QStringLiteral("(?:(?:[وف]?(?:[بلك]?ال|لل|[بلك]))|[وف])?");

    // Enclitics: pronouns, dual, plurals, tanween
    QString suff = QStringLiteral("(?:[هك]|ها|هم|هن|كم|كن|ي|نا|هما|كما|ون|ين|ان|ات|ه|ئذ|ا)?");

    QString pattern = QStringLiteral("(?:^|\\s)%1(?:%2)%3(?=\\s|$)").arg(pref, stemPat, suff);
    return QRegularExpression(pattern);
}

QString QuranManager::cleanTashkeelText(const QString &text) const
{
    if (text.isEmpty()) return text;
    QString s = cleanUthmaniText(text);

    // 1. Normalize Quranic sukoon \u06E1 to standard Arabic sukoon \u0652
    s.replace(QChar(0x06E1), QChar(0x0652));

    // 2. Normalize open / sequential tanweens \u08F0..\u08F2 to standard \u064B..\u064D
    s.replace(QChar(0x08F0), QChar(0x064B));
    s.replace(QChar(0x08F1), QChar(0x064C));
    s.replace(QChar(0x08F2), QChar(0x064D));

    // 3. Normalize Wasla to regular Alef
    s.replace(QChar(0x0671), QChar(0x0627));

    // 4. Remove Quranic recitation and stop marks, keep Harakat and Shaddah
    static const QRegularExpression marksRx(QStringLiteral("[\\x{06D6}-\\x{06DC}\\x{06DF}-\\x{06E0}\\x{06E2}-\\x{06E8}\\x{06EA}-\\x{06ED}\\x{08D4}-\\x{08ED}\\x{0640}]"));
    s.remove(marksRx);

    // 5. Canonicalize Shaddah + Vowel order: always Shaddah first (\u0651), then Vowel
    static const QRegularExpression vowelThenShaddahRx(QStringLiteral("([\\x{064B}-\\x{0650}\\x{0652}])\\x{0651}"));
    s.replace(vowelThenShaddahRx, QStringLiteral("\u0651\\1"));

    // 6. Normalize whitespace
    static const QRegularExpression wsRx(QStringLiteral("\\s+"));
    s.replace(wsRx, QStringLiteral(" "));

    return s.trimmed();
}

QVariantList QuranManager::search(const QString &query, int surahFilter)
{
    return searchAdvanced(query, WordWithAffixes, surahFilter > 0 ? ScopeSurah : ScopeAll, surahFilter);
}

QVariantList QuranManager::searchAdvanced(const QString &query, int searchMode, int scopeType, int scopeValue, const QString &translationEdition, bool respectTashkeel)
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    // Translation search mode (Mode 5)
    if (searchMode == TranslationSearch) {
        QString clean = query.trimmed();
        if (clean.length() < 2) return list;

        // tafsir_translations rows are indexed by Hafs verse numbering (1..6236).
        // Using CROSS JOIN forces SQLite to filter tafsir_translations first via text LIKE,
        // and then look up ayahs/surahs using primary/indexed keys, avoiding a 194M comparison full join scan.
        QString sql = QStringLiteral(
            "SELECT a.surah_number, a.ayah_number, a.text_uthmani, a.page_number, "
            "s.name_ar, s.name_en, t.text, t.edition_id, t.edition_name_ar, t.edition_name_en, t.language "
            "FROM tafsir_translations t "
            "CROSS JOIN ayahs a ON a.riwayah_id = 1 AND a.surah_number = t.surah_number AND a.ayah_number = t.ayah_number "
            "JOIN surahs s ON s.number = t.surah_number "
            "WHERE 1=1 "
        );

        QVariantMap binds;

        if (scopeType == ScopeSurah && scopeValue > 0) {
            sql += QStringLiteral(" AND t.surah_number = :scopeSurah");
            binds[QStringLiteral(":scopeSurah")] = scopeValue;
        } else if (scopeType == ScopeJuz && scopeValue > 0) {
            sql += QStringLiteral(" AND a.juz_number = :scopeJuz");
            binds[QStringLiteral(":scopeJuz")] = scopeValue;
        } else if (scopeType == ScopePage && scopeValue > 0) {
            sql += QStringLiteral(" AND a.page_number = :scopePage");
            binds[QStringLiteral(":scopePage")] = scopeValue;
        }

        if (!translationEdition.isEmpty() && translationEdition != QStringLiteral("all")) {
            sql += QStringLiteral(" AND t.edition_id = :editionId");
            binds[QStringLiteral(":editionId")] = translationEdition;
        }

        // Support accent-insensitive search for Latin languages (e.g. moise matches Moïse, pelerinage matches pèlerinage)
        bool hasLatin = false;
        for (const QChar &ch : clean) {
            if ((ch >= QLatin1Char('a') && ch <= QLatin1Char('z')) || (ch >= QLatin1Char('A') && ch <= QLatin1Char('Z'))) {
                hasLatin = true;
                break;
            }
        }
        QString wild;
        if (hasLatin && clean.length() >= 3) {
            static const QRegularExpression accentVowels(QStringLiteral("[aáàâäeéèêëiíîïoóôöuúûücç]"), QRegularExpression::CaseInsensitiveOption);
            wild = clean;
            wild.replace(accentVowels, QStringLiteral("_"));
        }

        if (!wild.isEmpty() && wild != clean) {
            sql += QStringLiteral(" AND (t.text LIKE :pattern OR t.text LIKE :wildPattern) ");
            binds[QStringLiteral(":pattern")] = QStringLiteral("%%1%").arg(clean);
            binds[QStringLiteral(":wildPattern")] = QStringLiteral("%%1%").arg(wild);
        } else {
            sql += QStringLiteral(" AND t.text LIKE :pattern ");
            binds[QStringLiteral(":pattern")] = QStringLiteral("%%1%").arg(clean);
        }

        sql += QStringLiteral(" ORDER BY t.surah_number ASC, t.ayah_number ASC");

        QSqlQuery q(m_db);
        q.prepare(sql);
        for (auto it = binds.begin(); it != binds.end(); ++it) {
            q.bindValue(it.key(), it.value());
        }

        static auto stripDiacritics = [](const QString &str) -> QString {
            QString norm = str.normalized(QString::NormalizationForm_D);
            QString out;
            out.reserve(norm.size());
            for (const QChar &ch : norm) {
                if (ch.category() != QChar::Mark_NonSpacing) {
                    out.append(ch);
                }
            }
            return out;
        };

        QString normQuery = stripDiacritics(clean).toLower();

        if (q.exec()) {
            while (q.next()) {
                QString transText = q.value(6).toString();
                // Filter out false positives from single-char wildcards
                if (!wild.isEmpty() && wild != clean && !transText.contains(clean, Qt::CaseInsensitive)) {
                    QString normTrans = stripDiacritics(transText).toLower();
                    if (!normTrans.contains(normQuery)) {
                        continue;
                    }
                }

                int sNum = q.value(0).toInt();
                int hAyah = q.value(1).toInt();
                int effAyah = hAyah;
                int effPage = q.value(3).toInt();

                // If user is currently in Warsh, map verse number and page to Warsh instantly in memory
                if (m_riwayah == 2) {
                    effAyah = mapHafsToWarsh(sNum, hAyah);
                    quint32 wKey = (static_cast<quint32>(sNum) << 16) | static_cast<quint32>(effAyah);
                    effPage = m_warshPageMap.value(wKey, static_cast<quint16>(effPage));
                }

                QVariantMap m;
                m[QStringLiteral("surah_number")] = sNum;
                m[QStringLiteral("ayah_number")] = effAyah;
                m[QStringLiteral("hafs_ayah_number")] = hAyah;
                m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(2).toString());
                m[QStringLiteral("page_number")] = effPage;
                m[QStringLiteral("surah_name_ar")] = q.value(4).toString();
                m[QStringLiteral("surah_name_en")] = q.value(5).toString();
                m[QStringLiteral("translation_text")] = transText;
                m[QStringLiteral("edition_id")] = q.value(7).toString();
                QString edAr = q.value(8).toString();
                QString edEn = q.value(9).toString();
                m[QStringLiteral("edition_name_ar")] = edAr;
                m[QStringLiteral("edition_name_en")] = edEn;
                m[QStringLiteral("edition_name")] = !edEn.isEmpty() ? edEn : edAr;
                m[QStringLiteral("language")] = q.value(10).toString();
                m[QStringLiteral("occurrence_count")] = 1;
                list.append(m);
            }
        }
        return list;
    }

    // Arabic Root Search Mode (Mode 3)
    if (searchMode == RootSearch) {
        QString norm = normalizeArabic(query);
        QString rootLetters;
        for (const QChar &ch : norm) {
            if (ch.isLetter() && ch.unicode() >= 0x0600 && ch.unicode() <= 0x06FF) {
                rootLetters.append(ch);
            }
        }
        if (rootLetters.length() < 3) return list;
        if (rootLetters.length() > 4) rootLetters = rootLetters.left(4);

        QString sql = QStringLiteral(
            "SELECT a.surah_number, a.ayah_number, a.text_uthmani, a.page_number, "
            "s.name_ar, s.name_en, a.text_search "
            "FROM ayahs a "
            "JOIN surahs s ON a.surah_number = s.number "
            "WHERE a.riwayah_id = :riwayah "
        );

        QVariantMap binds;
        binds[QStringLiteral(":riwayah")] = m_riwayah;

        if (scopeType == ScopeSurah && scopeValue > 0) {
            sql += QStringLiteral(" AND a.surah_number = :scopeSurah");
            binds[QStringLiteral(":scopeSurah")] = scopeValue;
        } else if (scopeType == ScopeJuz && scopeValue > 0) {
            sql += QStringLiteral(" AND a.juz_number = :scopeJuz");
            binds[QStringLiteral(":scopeJuz")] = scopeValue;
        } else if (scopeType == ScopePage && scopeValue > 0) {
            sql += QStringLiteral(" AND a.page_number = :scopePage");
            binds[QStringLiteral(":scopePage")] = scopeValue;
        }

        QRegularExpression rx;
        if (rootLetters.length() == 3) {
            QChar r1 = rootLetters[0];
            QChar r2 = rootLetters[1];
            QChar r3 = rootLetters[2];
            sql += QStringLiteral(" AND a.text_search LIKE :likePrefilter");
            binds[QStringLiteral(":likePrefilter")] = QStringLiteral("%%1%%2%%3%").arg(r1).arg(r2).arg(r3);

            QString pattern = QStringLiteral("(?:^|\\s)[والفبكتيمنسأإا]*?%1[اويتن]*?%2[اويتن]*?%3[هكماوناينتمةى]*(?=\\s|$)")
                              .arg(r1).arg(r2).arg(r3);
            rx.setPattern(pattern);
        } else {
            QChar r1 = rootLetters[0];
            QChar r2 = rootLetters[1];
            QChar r3 = rootLetters[2];
            QChar r4 = rootLetters[3];
            sql += QStringLiteral(" AND a.text_search LIKE :likePrefilter");
            binds[QStringLiteral(":likePrefilter")] = QStringLiteral("%%1%%2%%3%%4%").arg(r1).arg(r2).arg(r3).arg(r4);

            QString pattern = QStringLiteral("(?:^|\\s)[والفبكتيمنسأإا]*?%1[اويتن]*?%2[اويتن]*?%3[اويتن]*?%4[هكماوناينتمةى]*(?=\\s|$)")
                              .arg(r1).arg(r2).arg(r3).arg(r4);
            rx.setPattern(pattern);
        }

        sql += QStringLiteral(" ORDER BY a.surah_number ASC, a.ayah_number ASC");

        QSqlQuery q(m_db);
        q.prepare(sql);
        for (auto it = binds.begin(); it != binds.end(); ++it) {
            q.bindValue(it.key(), it.value());
        }

        if (q.exec()) {
            while (q.next()) {
                QString textSearch = q.value(6).toString();
                QRegularExpressionMatchIterator it = rx.globalMatch(textSearch);
                int occCount = 0;
                QString matchedWord;
                while (it.hasNext()) {
                    QRegularExpressionMatch m = it.next();
                    occCount++;
                    if (matchedWord.isEmpty()) {
                        matchedWord = m.captured(0).trimmed();
                    }
                }

                if (occCount > 0) {
                    QVariantMap m;
                    m[QStringLiteral("surah_number")] = q.value(0).toInt();
                    m[QStringLiteral("ayah_number")] = q.value(1).toInt();
                    m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(2).toString());
                    m[QStringLiteral("page_number")] = q.value(3).toInt();
                    m[QStringLiteral("surah_name_ar")] = q.value(4).toString();
                    m[QStringLiteral("surah_name_en")] = q.value(5).toString();
                    m[QStringLiteral("matched_word")] = matchedWord;
                    m[QStringLiteral("occurrence_count")] = occCount;
                    list.append(m);
                }
            }
        }
        return list;
    }

    // Word With Affixes Mode (Mode 0 - standard Qur'anic word search matching QuranProgress / Dr. AbdulAzeez)
    if (searchMode == WordWithAffixes) {
        QString norm = normalizeArabic(query).trimmed();
        if (norm.length() < 2) return list;

        QStringList words = norm.split(QLatin1Char(' '), QString::SkipEmptyParts);
        if (words.isEmpty()) return list;

        QList<QRegularExpression> regexList;
        QStringList allStems;
        for (const QString &w : words) {
            regexList.append(buildWordWithAffixesRegex(w, &allStems));
        }

        QString sql = QStringLiteral(
            "SELECT a.surah_number, a.ayah_number, a.text_uthmani, a.page_number, "
            "s.name_ar, s.name_en, a.text_search "
            "FROM ayahs a "
            "JOIN surahs s ON a.surah_number = s.number "
            "WHERE a.riwayah_id = :riwayah "
        );

        QVariantMap binds;
        binds[QStringLiteral(":riwayah")] = m_riwayah;

        if (scopeType == ScopeSurah && scopeValue > 0) {
            sql += QStringLiteral(" AND a.surah_number = :scopeSurah");
            binds[QStringLiteral(":scopeSurah")] = scopeValue;
        } else if (scopeType == ScopeJuz && scopeValue > 0) {
            sql += QStringLiteral(" AND a.juz_number = :scopeJuz");
            binds[QStringLiteral(":scopeJuz")] = scopeValue;
        } else if (scopeType == ScopePage && scopeValue > 0) {
            sql += QStringLiteral(" AND a.page_number = :scopePage");
            binds[QStringLiteral(":scopePage")] = scopeValue;
        }

        if (!allStems.isEmpty()) {
            QStringList stemLikes;
            for (int i = 0; i < allStems.size(); ++i) {
                QString pName = QStringLiteral(":stem%1").arg(i);
                stemLikes.append(QStringLiteral("a.text_search LIKE %1").arg(pName));
                binds[pName] = QStringLiteral("%%1%").arg(allStems[i]);
            }
            sql += QStringLiteral(" AND (%1)").arg(stemLikes.join(QStringLiteral(" OR ")));
        }

        sql += QStringLiteral(" ORDER BY a.surah_number ASC, a.ayah_number ASC");

        QSqlQuery q(m_db);
        q.prepare(sql);
        for (auto it = binds.begin(); it != binds.end(); ++it) {
            q.bindValue(it.key(), it.value());
        }

        if (q.exec()) {
            while (q.next()) {
                QString textSearch = q.value(6).toString();
                bool allMatched = true;
                QString firstMatchedWord;
                int totalOccInAyah = 0;

                for (const QRegularExpression &rx : regexList) {
                    QRegularExpressionMatchIterator it = rx.globalMatch(textSearch);
                    int countForThisWord = 0;
                    while (it.hasNext()) {
                        QRegularExpressionMatch m = it.next();
                        countForThisWord++;
                        if (firstMatchedWord.isEmpty()) {
                            firstMatchedWord = m.captured(0).trimmed();
                        }
                    }
                    if (countForThisWord == 0) {
                        allMatched = false;
                        break;
                    }
                    totalOccInAyah += countForThisWord;
                }

                if (allMatched) {
                    int occCount = totalOccInAyah > 0 ? totalOccInAyah : 1;
                    QString matchedWord = firstMatchedWord;

                    if (respectTashkeel) {
                        QString cleanVerse = cleanTashkeelText(q.value(2).toString());
                        QString cleanQuery = cleanTashkeelText(query);
                        if (!cleanQuery.isEmpty()) {
                            int occ = 0;
                            int pos = 0;
                            while ((pos = cleanVerse.indexOf(cleanQuery, pos)) != -1) {
                                occ++;
                                pos += cleanQuery.length();
                            }
                            if (occ == 0) {
                                continue;
                            }
                            occCount = occ;
                            matchedWord = cleanQuery;
                        }
                    }

                    QVariantMap m;
                    m[QStringLiteral("surah_number")] = q.value(0).toInt();
                    m[QStringLiteral("ayah_number")] = q.value(1).toInt();
                    m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(2).toString());
                    m[QStringLiteral("page_number")] = q.value(3).toInt();
                    m[QStringLiteral("surah_name_ar")] = q.value(4).toString();
                    m[QStringLiteral("surah_name_en")] = q.value(5).toString();
                    m[QStringLiteral("matched_word")] = matchedWord;
                    m[QStringLiteral("occurrence_count")] = occCount;
                    list.append(m);
                }
            }
        }
        return list;
    }

    // ExactLiteral (1), PartialMatch (2), PhraseSearch (4)
    QString norm = normalizeArabic(query).trimmed();
    if (norm.length() < 2) return list;

    QString sql = QStringLiteral(
        "SELECT a.surah_number, a.ayah_number, a.text_uthmani, a.page_number, "
        "s.name_ar, s.name_en, a.text_search "
        "FROM ayahs a "
        "JOIN surahs s ON a.surah_number = s.number "
        "WHERE a.riwayah_id = :riwayah "
    );

    QVariantMap binds;
    binds[QStringLiteral(":riwayah")] = m_riwayah;

    if (scopeType == ScopeSurah && scopeValue > 0) {
        sql += QStringLiteral(" AND a.surah_number = :scopeSurah");
        binds[QStringLiteral(":scopeSurah")] = scopeValue;
    } else if (scopeType == ScopeJuz && scopeValue > 0) {
        sql += QStringLiteral(" AND a.juz_number = :scopeJuz");
        binds[QStringLiteral(":scopeJuz")] = scopeValue;
    } else if (scopeType == ScopePage && scopeValue > 0) {
        sql += QStringLiteral(" AND a.page_number = :scopePage");
        binds[QStringLiteral(":scopePage")] = scopeValue;
    }

    if (searchMode == ExactLiteral) {
        QStringList words = norm.split(QLatin1Char(' '), QString::SkipEmptyParts);
        for (int i = 0; i < words.size(); ++i) {
            QString paramName = QStringLiteral(":w%1").arg(i);
            sql += QStringLiteral(" AND (' ' || a.text_search || ' ') LIKE %1").arg(paramName);
            binds[paramName] = QStringLiteral("%%1%").arg(QStringLiteral(" %1 ").arg(words[i]));
        }
    } else if (searchMode == PhraseSearch) {
        sql += QStringLiteral(" AND (' ' || a.text_search || ' ') LIKE :phrase");
        binds[QStringLiteral(":phrase")] = QStringLiteral("%%1%").arg(QStringLiteral(" %1 ").arg(norm));
    } else { // PartialMatch
        sql += QStringLiteral(" AND a.text_search LIKE :partial");
        binds[QStringLiteral(":partial")] = QStringLiteral("%%1%").arg(norm);
    }

    sql += QStringLiteral(" ORDER BY a.surah_number ASC, a.ayah_number ASC");

    QSqlQuery q(m_db);
    q.prepare(sql);
    for (auto it = binds.begin(); it != binds.end(); ++it) {
        q.bindValue(it.key(), it.value());
    }

    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            m[QStringLiteral("surah_number")] = q.value(0).toInt();
            m[QStringLiteral("ayah_number")] = q.value(1).toInt();
            m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(2).toString());
            m[QStringLiteral("page_number")] = q.value(3).toInt();
            m[QStringLiteral("surah_name_ar")] = q.value(4).toString();
            m[QStringLiteral("surah_name_en")] = q.value(5).toString();

            if (respectTashkeel) {
                QString cleanVerse = cleanTashkeelText(q.value(2).toString());
                QString cleanQuery = cleanTashkeelText(query);
                int countInAyah = 0;

                if (searchMode == ExactLiteral) {
                    QRegularExpression exactRx(QStringLiteral("(?:^|\\s)%1(?=\\s|$)").arg(QRegularExpression::escape(cleanQuery)));
                    QRegularExpressionMatchIterator it = exactRx.globalMatch(cleanVerse);
                    while (it.hasNext()) {
                        it.next();
                        countInAyah++;
                    }
                } else {
                    int pos = 0;
                    while ((pos = cleanVerse.indexOf(cleanQuery, pos)) != -1) {
                        countInAyah++;
                        pos += cleanQuery.length();
                    }
                }

                if (countInAyah == 0) {
                    continue;
                }
                m[QStringLiteral("occurrence_count")] = countInAyah;
                list.append(m);
                continue;
            }

            // Calculate occurrence count in ayah (non-tashkeel mode)
            QString textSearch = q.value(6).toString();
            int countInAyah = 0;
            int pos = 0;
            while ((pos = textSearch.indexOf(norm, pos)) != -1) {
                countInAyah++;
                pos += norm.length();
            }
            m[QStringLiteral("occurrence_count")] = countInAyah > 0 ? countInAyah : 1;
            list.append(m);
        }
    }

    return list;
}

QVariantMap QuranManager::getSearchStats(const QString &query, int searchMode, bool respectTashkeel, const QString &translationEdition)
{
    QVariantMap res;
    if (!m_db.isOpen()) return res;

    // Retrieve full matching set
    QVariantList matches = searchAdvanced(query, searchMode, ScopeAll, 0, translationEdition, respectTashkeel);
    if (matches.isEmpty()) {
        res[QStringLiteral("query")] = query;
        res[QStringLiteral("totalOccurrences")] = 0;
        res[QStringLiteral("totalVerses")] = 0;
        res[QStringLiteral("surahsCount")] = 0;
        res[QStringLiteral("surahs")] = QVariantList();
        return res;
    }

    struct SurahStat {
        int surahNumber;
        QString nameAr;
        QString nameEn;
        QString revelationType;
        int startPage;
        int totalVerses;
        int ayahCount = 0;
        int occurrenceCount = 0;
    };

    QMap<int, SurahStat> statsMap;
    int totalOccurrences = 0;
    int totalVerses = 0;
    int makkiOccurrences = 0;
    int madaniOccurrences = 0;

    for (const QVariant &itemVar : matches) {
        QVariantMap item = itemVar.toMap();
        int sNum = item.value(QStringLiteral("surah_number")).toInt();
        int occInAyah = item.value(QStringLiteral("occurrence_count"), 1).toInt();
        if (occInAyah <= 0) occInAyah = 1;

        if (!statsMap.contains(sNum)) {
            QVariantMap sInfo = getSurah(sNum);
            SurahStat st;
            st.surahNumber = sNum;
            st.nameAr = sInfo.value(QStringLiteral("name_ar")).toString();
            st.nameEn = sInfo.value(QStringLiteral("name_en")).toString();
            st.revelationType = sInfo.value(QStringLiteral("revelation_type")).toString();
            st.startPage = sInfo.value(QStringLiteral("start_page")).toInt();
            st.totalVerses = sInfo.value(QStringLiteral("total_verses")).toInt();
            statsMap.insert(sNum, st);
        }

        statsMap[sNum].ayahCount += 1;
        statsMap[sNum].occurrenceCount += occInAyah;
        totalOccurrences += occInAyah;
        totalVerses += 1;

        if (statsMap[sNum].revelationType == QStringLiteral("Meccan")) {
            makkiOccurrences += occInAyah;
        } else {
            madaniOccurrences += occInAyah;
        }
    }

    QList<SurahStat> list = statsMap.values();
    std::sort(list.begin(), list.end(), [](const SurahStat &a, const SurahStat &b) {
        if (a.occurrenceCount != b.occurrenceCount) {
            return a.occurrenceCount > b.occurrenceCount;
        }
        return a.surahNumber < b.surahNumber;
    });

    int maxSurahOccurrences = list.isEmpty() ? 0 : list.first().occurrenceCount;
    QString topSurahName = list.isEmpty() ? QString() : list.first().nameAr;

    QVariantList surahsList;
    for (const SurahStat &st : list) {
        QVariantMap sm;
        sm[QStringLiteral("surah_number")] = st.surahNumber;
        sm[QStringLiteral("name_ar")] = st.nameAr;
        sm[QStringLiteral("name_en")] = st.nameEn;
        sm[QStringLiteral("revelation_type")] = st.revelationType;
        sm[QStringLiteral("start_page")] = st.startPage;
        sm[QStringLiteral("total_verses")] = st.totalVerses;
        sm[QStringLiteral("ayah_count")] = st.ayahCount;
        sm[QStringLiteral("occurrence_count")] = st.occurrenceCount;

        double pct = (totalOccurrences > 0) ? ((double)st.occurrenceCount * 100.0 / totalOccurrences) : 0.0;
        sm[QStringLiteral("percentage")] = pct;
        sm[QStringLiteral("percentage_str")] = QString::number(pct, 'f', 1);
        sm[QStringLiteral("bar_ratio")] = (maxSurahOccurrences > 0) ? ((double)st.occurrenceCount / (double)maxSurahOccurrences) : 0.0;
        surahsList.append(sm);
    }

    res[QStringLiteral("query")] = query;
    res[QStringLiteral("totalOccurrences")] = totalOccurrences;
    res[QStringLiteral("totalVerses")] = totalVerses;
    res[QStringLiteral("surahsCount")] = list.size();
    res[QStringLiteral("makkiOccurrences")] = makkiOccurrences;
    res[QStringLiteral("madaniOccurrences")] = madaniOccurrences;
    double makkiPct = (totalOccurrences > 0) ? ((double)makkiOccurrences * 100.0 / totalOccurrences) : 0.0;
    double madaniPct = (totalOccurrences > 0) ? ((double)madaniOccurrences * 100.0 / totalOccurrences) : 0.0;
    res[QStringLiteral("makkiPercentage")] = QString::number(makkiPct, 'f', 1);
    res[QStringLiteral("madaniPercentage")] = QString::number(madaniPct, 'f', 1);
    res[QStringLiteral("maxSurahOccurrences")] = maxSurahOccurrences;
    res[QStringLiteral("topSurahName")] = topSurahName;
    res[QStringLiteral("surahs")] = surahsList;
    res[QStringLiteral("respectTashkeel")] = respectTashkeel;

    return res;
}


QVariantList QuranManager::getAvailableTafsirEditions() const
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(QStringLiteral("SELECT DISTINCT edition_id, edition_name_ar, edition_name_en, edition_type, language FROM tafsir_translations ORDER BY CASE WHEN language = 'ar' THEN 0 ELSE 1 END, id ASC"), m_db);
    while (q.next()) {
        QVariantMap m;
        m[QStringLiteral("id")] = q.value(0).toString();
        m[QStringLiteral("name_ar")] = q.value(1).toString();
        m[QStringLiteral("name_en")] = q.value(2).toString();
        m[QStringLiteral("type")] = q.value(3).toString();
        m[QStringLiteral("language")] = q.value(4).toString();
        list.append(m);
    }
    return list;
}

QString QuranManager::getAyahTafsir(int surah, int ayah, const QString &editionId, int riwayahId) const
{
    if (!m_db.isOpen()) return QString();

    QString edId = editionId.isEmpty() ? m_selectedTafsirEditionId : editionId;
    if (edId.isEmpty()) edId = QStringLiteral("ar.muyassar");

    int rId = (riwayahId > 0) ? riwayahId : m_riwayah;
    int sNum = qBound(1, surah, 114);

    // Get max verses in this surah for this riwayah
    int maxVerses = 0;
    QSqlQuery qCount(m_db);
    qCount.prepare(QStringLiteral("SELECT COUNT(*) FROM ayahs WHERE riwayah_id = ? AND surah_number = ?"));
    qCount.addBindValue(rId);
    qCount.addBindValue(sNum);
    if (qCount.exec() && qCount.next()) {
        maxVerses = qCount.value(0).toInt();
    }
    int effAyah = (maxVerses > 0) ? qBound(1, ayah, maxVerses) : qMax(1, ayah);

    int startAyah = effAyah;
    int endAyah = effAyah;

    if (rId == 2) { // Warsh
        quint32 key = (static_cast<quint32>(sNum) << 16) | static_cast<quint32>(effAyah);
        if (m_warshToHafsMap.contains(key)) {
            const HafsRange &r = m_warshToHafsMap.value(key);
            startAyah = r.start;
            endAyah = r.end;
        }
    }

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT text FROM tafsir_translations WHERE edition_id = ? AND surah_number = ? AND ayah_number >= ? AND ayah_number <= ? ORDER BY ayah_number ASC"));
    q.addBindValue(edId);
    q.addBindValue(sNum);
    q.addBindValue(startAyah);
    q.addBindValue(endAyah);

    QStringList parts;
    if (q.exec()) {
        while (q.next()) {
            QString t = q.value(0).toString().trimmed();
            if (!t.isEmpty()) {
                parts.append(t);
            }
        }
    }
    return parts.join(QStringLiteral("\n\n"));
}

QVariantList QuranManager::getTafsirAndTranslations(int surah, int ayah, int riwayahId) const
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    int rId = (riwayahId > 0) ? riwayahId : m_riwayah;
    int sNum = qBound(1, surah, 114);

    // Get max verses in this surah for this riwayah
    int maxVerses = 0;
    QSqlQuery qCount(m_db);
    qCount.prepare(QStringLiteral("SELECT COUNT(*) FROM ayahs WHERE riwayah_id = ? AND surah_number = ?"));
    qCount.addBindValue(rId);
    qCount.addBindValue(sNum);
    if (qCount.exec() && qCount.next()) {
        maxVerses = qCount.value(0).toInt();
    }
    int effAyah = (maxVerses > 0) ? qBound(1, ayah, maxVerses) : qMax(1, ayah);

    int startAyah = effAyah;
    int endAyah = effAyah;

    if (rId == 2) { // Warsh
        quint32 key = (static_cast<quint32>(sNum) << 16) | static_cast<quint32>(effAyah);
        if (m_warshToHafsMap.contains(key)) {
            const HafsRange &r = m_warshToHafsMap.value(key);
            startAyah = r.start;
            endAyah = r.end;
        }
    }

    QVariantList editions = getAvailableTafsirEditions();
    for (const QVariant &edVar : editions) {
        QVariantMap edMap = edVar.toMap();
        QString edId = edMap.value(QStringLiteral("id")).toString();

        QSqlQuery q(m_db);
        q.prepare(QStringLiteral("SELECT text FROM tafsir_translations WHERE edition_id = ? AND surah_number = ? AND ayah_number >= ? AND ayah_number <= ? ORDER BY ayah_number ASC"));
        q.addBindValue(edId);
        q.addBindValue(sNum);
        q.addBindValue(startAyah);
        q.addBindValue(endAyah);

        QStringList parts;
        if (q.exec()) {
            while (q.next()) {
                QString t = q.value(0).toString().trimmed();
                if (!t.isEmpty()) {
                    parts.append(t);
                }
            }
        }
        edMap[QStringLiteral("text")] = parts.join(QStringLiteral("\n\n"));
        list.append(edMap);
    }
    return list;
}

QString QuranManager::createTextShareFile(const QString &title, const QString &content) const
{
    Q_UNUSED(title);
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (baseDir.isEmpty()) {
        baseDir = QDir::tempPath();
    }
    QDir d(baseDir);
    d.mkpath(QStringLiteral("."));

    // Clean up any stale share files
    QStringList oldFiles = d.entryList(QStringList() << QStringLiteral("ayah_share_*.txt"), QDir::Files);
    for (const QString &old : oldFiles) {
        d.remove(old);
    }

    QString fileName = QStringLiteral("ayah_share_%1.txt").arg(QDateTime::currentMSecsSinceEpoch());
    QString filePath = d.filePath(fileName);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << content;
        file.close();
    }
    return filePath;
}

QString QuranManager::getCacheDirectory(int riwayahId) const
{
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath() + QStringLiteral("/.local/share/org.hafsoftdz/harbour-thakir");
    }
    QString rCode = (riwayahId == 2) ? QStringLiteral("warsh") : QStringLiteral("hafs");
    QString path = QStringLiteral("%1/quran_svg/%2").arg(baseDir, rCode);
    QDir().mkpath(path);
    return path;
}

QString QuranManager::getRenderedPagesCacheDir() const
{
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath() + QStringLiteral("/.local/share/org.hafsoftdz/harbour-thakir");
    }
    QString path = QStringLiteral("%1/rendered_pages").arg(baseDir);
    QDir().mkpath(path);
    return path;
}

QString QuranManager::getLocalPagePath(int riwayahId, int pageNumber) const
{
    QString cacheDir = getCacheDirectory(riwayahId);
    QString userPath = QStringLiteral("%1/raw_%2.svg").arg(cacheDir, QString::number(pageNumber));
    if (QFile::exists(userPath)) {
        return userPath;
    }

    QString rCode = (riwayahId == 2) ? QStringLiteral("warsh") : QStringLiteral("hafs");

    // Check SailfishApp::pathTo
    QString appPath = SailfishApp::pathTo(QStringLiteral("quran_svg/%1/raw_%2.svg").arg(rCode, QString::number(pageNumber))).toLocalFile();
    if (QFile::exists(appPath)) {
        return appPath;
    }

    // Check standard /usr/share path
    QString bundled = QStringLiteral("/usr/share/harbour-thakir/quran_svg/%1/raw_%2.svg").arg(rCode, QString::number(pageNumber));
    if (QFile::exists(bundled)) {
        return bundled;
    }

    return userPath;
}

QString QuranManager::getRemotePageUrl(int riwayahId, int pageNumber) const
{
    QString rCode = (riwayahId == 2) ? QStringLiteral("warsh") : QStringLiteral("hafs");
    return QStringLiteral("https://quranpedia.net/api/page/%1/%2").arg(rCode).arg(pageNumber);
}

bool QuranManager::isPageCached(int pageNumber)
{
    return QFile::exists(getLocalPagePath(m_riwayah, pageNumber));
}

bool QuranManager::isPageDownloading(int pageNumber) const
{
    return m_activeDownloads.values().contains(pageNumber);
}

QString QuranManager::getPageImageUrl(int pageNumber)
{
    return getRenderedPageUrl(pageNumber, m_playingSurah, m_playingAyah, m_darkMode);
}

QString QuranManager::getRenderedPageUrl(int pageNumber, int highlightSurah, int highlightAyah, bool darkMode)
{
    if (pageNumber < 1 || pageNumber > 604) pageNumber = 1;

    QString rawPath = getLocalPagePath(m_riwayah, pageNumber);
    if (!QFile::exists(rawPath)) {
        preloadPage(pageNumber);
        if (pageNumber < 604) preloadPage(pageNumber + 1);
        if (pageNumber > 1) preloadPage(pageNumber - 1);
        return QString(); // Return empty string so QML displays loading indicator while downloading
    }

    // Dynamic rendering of Quranpedia SVG with verse highlight and Dark/Light palette
    QString cacheDir = getCacheDirectory(m_riwayah);
    QString renderedFileName = QStringLiteral("active_%1_%2_%3_%4.svg")
            .arg(pageNumber)
            .arg(highlightSurah)
            .arg(highlightAyah)
            .arg(darkMode ? 1 : 0);
    QString renderedPath = cacheDir + QLatin1Char('/') + renderedFileName;

    if (!QFile::exists(renderedPath)) {
        QFile inFile(rawPath);
        if (inFile.open(QIODevice::ReadOnly)) {
            QString content = QString::fromUtf8(inFile.readAll());
            inFile.close();

            // Background rect
            QString bgColor = darkMode ? QStringLiteral("#12151b") : QStringLiteral("#fffdf5");
            int svgIdx = content.indexOf(QStringLiteral("<svg"));
            int svgTagEnd = (svgIdx != -1) ? content.indexOf(QLatin1Char('>'), svgIdx) : -1;
            if (svgTagEnd != -1) {
                QString bgRect = QStringLiteral("\n<rect width=\"100%\" height=\"100%\" fill=\"%1\" />\n").arg(bgColor);
                content.insert(svgTagEnd + 1, bgRect);
            }

            // Dark mode text coloring (pure Quranpedia dark palette)
            if (darkMode) {
                content.replace(QStringLiteral("fill=\"#231f20\""), QStringLiteral("fill=\"#ede4ce\""));
                content.replace(QStringLiteral("fill=\"#000000\""), QStringLiteral("fill=\"#ede4ce\""));
                content.replace(QStringLiteral("fill=\"#1a1a1a\""), QStringLiteral("fill=\"#ede4ce\""));
            }

            // Ayah Polygon highlighting (Quranpedia signature style)
            if (highlightSurah > 0 && highlightAyah > 0) {
                QString fillColor = darkMode ? QStringLiteral("#ffd700") : QStringLiteral("#007c89");
                QString fillOp = darkMode ? QStringLiteral("0.36") : QStringLiteral("0.24");
                QString strokeColor = darkMode ? QStringLiteral("#ffe066") : QStringLiteral("#007c89");
                QString strokeWidth = darkMode ? QStringLiteral("2") : QStringLiteral("1.5");

                // Target: class="ayahPolygon" ... surah="X" ayah="Y"
                QRegExp rx(QStringLiteral("(<path[^>]*class=\"ayahPolygon\"[^>]*surah=\"%1\"[^>]*ayah=\"%2\"[^>]*>)").arg(highlightSurah).arg(highlightAyah));
                if (rx.indexIn(content) != -1) {
                    QString tag = rx.cap(1);
                    QString replacement = tag;
                    replacement.remove(QRegExp(QStringLiteral("fill-opacity=\"[^\"]*\"")));
                    replacement.remove(QRegExp(QStringLiteral("fill=\"[^\"]*\"")));
                    if (replacement.endsWith(QStringLiteral("/>"))) {
                        replacement.chop(2);
                    } else if (replacement.endsWith(QStringLiteral(">"))) {
                        replacement.chop(1);
                    }
                    replacement += QStringLiteral(" fill=\"%1\" fill-opacity=\"%2\" stroke=\"%3\" stroke-width=\"%4\" stroke-opacity=\"0.85\" />")
                            .arg(fillColor, fillOp, strokeColor, strokeWidth);
                    content.replace(tag, replacement);
                }

                // Also check reverse attribute order: ayah="Y" ... surah="X"
                QRegExp rxRev(QStringLiteral("(<path[^>]*class=\"ayahPolygon\"[^>]*ayah=\"%1\"[^>]*surah=\"%2\"[^>]*>)").arg(highlightAyah).arg(highlightSurah));
                if (rxRev.indexIn(content) != -1) {
                    QString tag = rxRev.cap(1);
                    QString replacement = tag;
                    replacement.remove(QRegExp(QStringLiteral("fill-opacity=\"[^\"]*\"")));
                    replacement.remove(QRegExp(QStringLiteral("fill=\"[^\"]*\"")));
                    if (replacement.endsWith(QStringLiteral("/>"))) {
                        replacement.chop(2);
                    } else if (replacement.endsWith(QStringLiteral(">"))) {
                        replacement.chop(1);
                    }
                    replacement += QStringLiteral(" fill=\"%1\" fill-opacity=\"%2\" stroke=\"%3\" stroke-width=\"%4\" stroke-opacity=\"0.85\" />")
                            .arg(fillColor, fillOp, strokeColor, strokeWidth);
                    content.replace(tag, replacement);
                }
            }

            QFile outFile(renderedPath);
            if (outFile.open(QIODevice::WriteOnly)) {
                outFile.write(content.toUtf8());
                outFile.close();
            }
        }
    }

    return QUrl::fromLocalFile(renderedPath).toString();
}

QImage QuranManager::renderPageImage(int pageNumber, int highlightSurah, int highlightAyah, bool darkMode, int riwayahId, const QSize &requestedSize, QSize *size)
{
    if (pageNumber < 1 || pageNumber > 604) pageNumber = 1;
    if (riwayahId != 1 && riwayahId != 2) riwayahId = m_riwayah;

    // Mutex lock to ensure thread safety across concurrent background render requests
    QMutexLocker locker(&m_renderMutex);

    QString ramCacheKey = QStringLiteral("%1_%2_%3_%4_%5_%6x%7")
            .arg(riwayahId)
            .arg(pageNumber)
            .arg(highlightSurah)
            .arg(highlightAyah)
            .arg(darkMode ? 1 : 0)
            .arg(requestedSize.width())
            .arg(requestedSize.height());

    // 1. Check in-memory RAM cache (instant response < 1 ms)
    if (m_pageImageCache.contains(ramCacheKey)) {
        QImage *cached = m_pageImageCache.object(ramCacheKey);
        if (cached && !cached->isNull()) {
            if (size) *size = cached->size();
            return *cached;
        }
    }

    bool isBasePage = (highlightSurah <= 0 || highlightAyah <= 0);
    QString diskCachedPath = QStringLiteral("%1/base_r%2_p%3_d%4.png")
            .arg(getRenderedPagesCacheDir())
            .arg(riwayahId)
            .arg(pageNumber)
            .arg(darkMode ? 1 : 0);

    // 2. Check on-disk lossless PNG cache for base pages (loads in ~10 ms instead of 2000 ms SVG rasterization)
    if (isBasePage && (requestedSize.width() <= 0 || requestedSize.width() == 1080) && QFile::exists(diskCachedPath)) {
        QImage diskImg(diskCachedPath);
        if (!diskImg.isNull()) {
            m_pageImageCache.insert(ramCacheKey, new QImage(diskImg));
            if (size) *size = diskImg.size();
            return diskImg;
        }
    }

    // 3. Load SVG source
    QString content;
    if (m_memCachedPageNumber == pageNumber && m_memCachedRiwayah == riwayahId && !m_memCachedSvgContent.isEmpty()) {
        content = m_memCachedSvgContent;
    } else {
        QString rawPath = getLocalPagePath(riwayahId, pageNumber);
        if (!QFile::exists(rawPath)) {
            preloadPage(pageNumber);
            return QImage();
        }

        QFile inFile(rawPath);
        if (!inFile.open(QIODevice::ReadOnly)) {
            return QImage();
        }
        content = QString::fromUtf8(inFile.readAll());
        inFile.close();

        m_memCachedPageNumber = pageNumber;
        m_memCachedRiwayah = riwayahId;
        m_memCachedSvgContent = content;
    }

    // Background color
    QString bgColor = darkMode ? QStringLiteral("#12151b") : QStringLiteral("#fffdf5");

    // Dark mode font colors
    if (darkMode) {
        content.replace(QStringLiteral("fill=\"#231f20\""), QStringLiteral("fill=\"#ede4ce\""));
        content.replace(QStringLiteral("fill=\"#000000\""), QStringLiteral("fill=\"#ede4ce\""));
        content.replace(QStringLiteral("fill=\"#1a1a1a\""), QStringLiteral("fill=\"#ede4ce\""));
    }

    // Ayah Polygon highlighting (rendered BEHIND text for crisp readability)
    if (highlightSurah > 0 && highlightAyah > 0) {
        QString fillColor = darkMode ? QStringLiteral("#d4a017") : QStringLiteral("#c8961e");
        QString fillOp = darkMode ? QStringLiteral("0.30") : QStringLiteral("0.22");

        // Fast string search instead of slow regex loop across 250 KB
        QString needleS1 = QStringLiteral("surah=\"%1\"").arg(highlightSurah);
        QString needleS2 = QStringLiteral("surah='%1'").arg(highlightSurah);
        QString needleA1 = QStringLiteral("ayah=\"%1\"").arg(highlightAyah);
        QString needleA2 = QStringLiteral("ayah='%1'").arg(highlightAyah);

        QString targetPathTag;
        int sSearchPos = 0;
        while (sSearchPos < content.length()) {
            int idx1 = content.indexOf(needleS1, sSearchPos);
            int idx2 = content.indexOf(needleS2, sSearchPos);
            int idx = -1;
            if (idx1 != -1 && idx2 != -1) idx = qMin(idx1, idx2);
            else if (idx1 != -1) idx = idx1;
            else if (idx2 != -1) idx = idx2;

            if (idx == -1) break;

            int tagStart = content.lastIndexOf(QStringLiteral("<path"), idx);
            if (tagStart != -1) {
                int tagEnd = content.indexOf(QLatin1Char('>'), idx);
                if (tagEnd != -1) {
                    QString tag = content.mid(tagStart, tagEnd - tagStart + 1);
                    if (tag.contains(QStringLiteral("ayahPolygon")) &&
                        (tag.contains(needleA1) || tag.contains(needleA2))) {
                        targetPathTag = tag;
                        break;
                    }
                }
            }
            sSearchPos = idx + needleS1.length();
        }

        if (!targetPathTag.isEmpty()) {
            content.remove(targetPathTag);

            QString highlightTag = targetPathTag;
            highlightTag.remove(QRegExp(QStringLiteral("fill-opacity=[\"'][^\"']*[\"']")));
            highlightTag.remove(QRegExp(QStringLiteral("fill=[\"'][^\"']*[\"']")));
            highlightTag.remove(QRegExp(QStringLiteral("stroke=[\"'][^\"']*[\"']")));
            highlightTag.remove(QRegExp(QStringLiteral("stroke-width=[\"'][^\"']*[\"']")));
            highlightTag.remove(QRegExp(QStringLiteral("stroke-opacity=[\"'][^\"']*[\"']")));
            if (highlightTag.endsWith(QStringLiteral("/>"))) highlightTag.chop(2);
            else if (highlightTag.endsWith(QStringLiteral(">"))) highlightTag.chop(1);
            highlightTag += QStringLiteral(" fill=\"%1\" fill-opacity=\"%2\" stroke=\"none\" />")
                    .arg(fillColor, fillOp);

            int svgTagStart = content.indexOf(QStringLiteral("<svg"));
            if (svgTagStart != -1) {
                int svgTagEnd = content.indexOf(QLatin1Char('>'), svgTagStart);
                if (svgTagEnd != -1) {
                    content.insert(svgTagEnd + 1, highlightTag);
                }
            }
        }
    }

    QSvgRenderer renderer(content.toUtf8());
    if (!renderer.isValid()) {
        qWarning() << "[QuranManager] QSvgRenderer failed to parse SVG for page" << pageNumber;
        return QImage();
    }

    QRectF vb = renderer.viewBoxF();
    qreal aspect = (vb.width() > 0) ? (vb.height() / vb.width()) : 1.5942;

    int targetW = 1080; // Sony Xperia X native display width (1080x1920)
    int targetH = qRound(targetW * aspect);

    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        qreal reqAspect = (qreal)requestedSize.height() / (qreal)requestedSize.width();
        if (reqAspect > aspect) {
            targetW = requestedSize.width();
            targetH = qRound(targetW * aspect);
        } else {
            targetH = requestedSize.height();
            targetW = qRound(targetH / aspect);
        }
    } else if (requestedSize.width() > 0) {
        targetW = requestedSize.width();
        targetH = qRound(targetW * aspect);
    } else if (requestedSize.height() > 0) {
        targetH = requestedSize.height();
        targetW = qRound(targetH / aspect);
    }

    targetW = qBound(320, targetW, 2160);
    targetH = qBound(480, targetH, 3840);

    QImage img(targetW, targetH, QImage::Format_ARGB32_Premultiplied);
    img.fill(QColor(bgColor));

    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    renderer.render(&painter, QRectF(0, 0, targetW, targetH));
    painter.end();

    // Cache rendered base page as lossless PNG on disk for instant future loads
    if (isBasePage && targetW == 1080) {
        img.save(diskCachedPath, "PNG");
    }

    // Store in RAM cache
    m_pageImageCache.insert(ramCacheKey, new QImage(img));

    if (size) *size = img.size();
    return img;
}

QVariantMap QuranManager::getAyahAtCoordinate(int pageNumber, double normalizedX, double normalizedY, int riwayahId)
{
    QVariantMap res;
    if (pageNumber < 1 || pageNumber > 604) return res;
    if (riwayahId != 1 && riwayahId != 2) riwayahId = m_riwayah;

    // Load SVG content (uses m_memCachedSvgContent if already in RAM)
    QString content;
    if (m_memCachedPageNumber == pageNumber && m_memCachedRiwayah == riwayahId && !m_memCachedSvgContent.isEmpty()) {
        content = m_memCachedSvgContent;
    } else {
        QString rawPath = getLocalPagePath(riwayahId, pageNumber);
        if (QFile::exists(rawPath)) {
            QFile inFile(rawPath);
            if (inFile.open(QIODevice::ReadOnly)) {
                content = QString::fromUtf8(inFile.readAll());
                inFile.close();
            }
        }
    }

    if (content.isEmpty()) {
        return res;
    }

    struct AyahPoly {
        int surah;
        int ayah;
        QVector<QPolygonF> subPolys;
    };
    QVector<AyahPoly> polys;

    double minX = 999999.0, maxX = -999999.0;
    double minY = 999999.0, maxY = -999999.0;

    static const QRegularExpression pathTagRx(QStringLiteral("<path[^>]*class=[\"']ayahPolygon[\"'][^>]*>"));
    static const QRegularExpression surahRx(QStringLiteral("surah=[\"'](\\d+)[\"']"));
    static const QRegularExpression ayahRx(QStringLiteral("ayah=[\"'](\\d+)[\"']"));
    static const QRegularExpression dRx(QStringLiteral("\\bd=[\"']([^\"']+)[\"']"));
    static const QRegularExpression numRx(QStringLiteral("[-+]?\\d*\\.?\\d+"));

    auto it = pathTagRx.globalMatch(content);
    while (it.hasNext()) {
        QString tag = it.next().captured(0);
        auto mS = surahRx.match(tag);
        auto mA = ayahRx.match(tag);
        auto mD = dRx.match(tag);
        if (!mS.hasMatch() || !mA.hasMatch() || !mD.hasMatch()) continue;

        AyahPoly ap;
        ap.surah = mS.captured(1).toInt();
        ap.ayah = mA.captured(1).toInt();

        QString d = mD.captured(1);
        QStringList chunks = d.split(QRegularExpression(QStringLiteral("[ZM]")), QString::SkipEmptyParts);
        for (const QString &chunk : chunks) {
            auto numIt = numRx.globalMatch(chunk);
            QVector<double> vals;
            while (numIt.hasNext()) {
                vals.append(numIt.next().captured(0).toDouble());
            }
            if (vals.size() >= 4) {
                QPolygonF poly;
                for (int i = 0; i + 1 < vals.size(); i += 2) {
                    double x = vals[i];
                    double y = vals[i + 1];
                    poly << QPointF(x, y);
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
                if (!poly.isEmpty()) {
                    ap.subPolys.append(poly);
                }
            }
        }
        if (!ap.subPolys.isEmpty()) {
            polys.append(ap);
        }
    }

    if (polys.isEmpty() || minX >= maxX || minY >= maxY) {
        return res;
    }

    // Top/bottom margins of the page frame (~6%)
    const double marginSide = 0.05;
    const double marginTop = 0.06;
    const double marginBottom = 0.06;

    double relX = (normalizedX - marginSide) / (1.0 - 2.0 * marginSide);
    double relY = (normalizedY - marginTop) / (1.0 - marginTop - marginBottom);
    relX = qBound(0.0, relX, 1.0);
    relY = qBound(0.0, relY, 1.0);

    double targetX = minX + relX * (maxX - minX);
    double targetY = minY + relY * (maxY - minY);
    QPointF pt(targetX, targetY);

    // 1. Direct point-in-polygon test
    for (const AyahPoly &ap : polys) {
        for (const QPolygonF &poly : ap.subPolys) {
            if (poly.containsPoint(pt, Qt::OddEvenFill)) {
                res[QStringLiteral("surah")] = ap.surah;
                res[QStringLiteral("ayah")] = ap.ayah;
                return res;
            }
        }
    }

    // 2. Closest vertical distance fallback
    double bestDist = 1e9;
    int bestSurah = 0;
    int bestAyah = 0;
    for (const AyahPoly &ap : polys) {
        for (const QPolygonF &poly : ap.subPolys) {
            QRectF r = poly.boundingRect();
            double dist = 0.0;
            if (targetY < r.top()) dist = r.top() - targetY;
            else if (targetY > r.bottom()) dist = targetY - r.bottom();
            else dist = 0.0;

            if (dist < bestDist) {
                bestDist = dist;
                bestSurah = ap.surah;
                bestAyah = ap.ayah;
            }
        }
    }

    if (bestSurah > 0 && bestAyah > 0) {
        res[QStringLiteral("surah")] = bestSurah;
        res[QStringLiteral("ayah")] = bestAyah;
    }
    return res;
}

void QuranManager::preloadPage(int pageNumber)
{
    if (pageNumber < 1 || pageNumber > 604) return;
    QString localPath = getLocalPagePath(m_riwayah, pageNumber);
    if (QFile::exists(localPath)) return;

    // Ensure network accessibility
    m_netManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

    // Check if download is already in progress for this page and riwayah
    for (auto it = m_activeDownloads.begin(); it != m_activeDownloads.end(); ++it) {
        if (it.value() == pageNumber && it.key()->property("riwayah").toInt() == m_riwayah) return;
    }

    QUrl url(getRemotePageUrl(m_riwayah, pageNumber));
    qDebug() << "[QuranManager] Preloading page" << pageNumber << "for riwayah" << m_riwayah << "from" << url.toString();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Mobile Safari/537.36"));
    request.setRawHeader("Accept", "image/svg+xml,image/*,*/*;q=0.8");
    request.setRawHeader("Accept-Language", "ar,en-US;q=0.9,en;q=0.8");
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    QNetworkReply *reply = m_netManager->get(request);
    reply->setProperty("pageNumber", pageNumber);
    reply->setProperty("riwayah", m_riwayah);
    reply->ignoreSslErrors();
    m_activeDownloads[reply] = pageNumber;
    connect(reply, &QNetworkReply::finished, this, &QuranManager::onPageDownloaded);
    connect(reply, static_cast<void(QNetworkReply::*)(const QList<QSslError>&)>(&QNetworkReply::sslErrors),
            reply, static_cast<void(QNetworkReply::*)()>(&QNetworkReply::ignoreSslErrors));
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, [reply, pageNumber](QNetworkReply::NetworkError err) {
        qWarning() << "[QuranManager] Network error downloading page" << pageNumber << ":" << err << reply->errorString();
    });
}

void QuranManager::onPageDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    int pageNumber = reply->property("pageNumber").toInt();
    if (pageNumber <= 0) pageNumber = m_activeDownloads.value(reply, 0);
    int riwayah = reply->property("riwayah").toInt();
    if (riwayah != 1 && riwayah != 2) riwayah = m_riwayah;
    m_activeDownloads.remove(reply);

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "[QuranManager] Page" << pageNumber << "riwayah" << riwayah << "download completed. Status:" << statusCode << "Error:" << reply->error();

    if (reply->error() == QNetworkReply::NoError && pageNumber > 0) {
        QByteArray data = reply->readAll();
        qDebug() << "[QuranManager] Page" << pageNumber << "received bytes:" << data.size();
        if (data.size() > 500 && data.contains("<svg")) {
            QString cacheDir = getCacheDirectory(riwayah);
            QString localPath = QStringLiteral("%1/raw_%2.svg").arg(cacheDir, QString::number(pageNumber));
            QFile file(localPath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                qDebug() << "[QuranManager] Successfully cached page" << pageNumber << "to" << localPath;
                emit pageCached(pageNumber);
            } else {
                qWarning() << "[QuranManager] Could not open file for writing:" << localPath << file.errorString();
                emit pageDownloadFailed(pageNumber);
            }
        } else {
            qWarning() << "[QuranManager] Page" << pageNumber << "content is not valid SVG! Size:" << data.size();
            emit pageDownloadFailed(pageNumber);
        }
    } else {
        qWarning() << "[QuranManager] Page" << pageNumber << "download failed with error:" << reply->errorString();
        emit pageDownloadFailed(pageNumber);
    }
    reply->deleteLater();
}

QVariantList QuranManager::getAvailableReciters()
{
    QVariantList list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT reciters_json FROM riwayat WHERE id = ?"));
    q.addBindValue(m_riwayah);
    if (q.exec() && q.next()) {
        QByteArray jsonBytes = q.value(0).toByteArray();
        QJsonDocument doc = QJsonDocument::fromJson(jsonBytes);
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            for (const QJsonValue &v : arr) {
                list.append(v.toObject().toVariantMap());
            }
        }
    }
    return list;
}

void QuranManager::setReciter(const QString &reciterId)
{
    if (m_selectedReciterId != reciterId) {
        m_selectedReciterId = reciterId;
        QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
        s.setValue(QStringLiteral("quran/reciterId"), m_selectedReciterId);
        s.sync();
        emit reciterChanged();
    }
}

QString QuranManager::getAudioCacheDirectory(const QString &reciterId) const
{
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath() + QStringLiteral("/.local/share/org.hafsoftdz/harbour-thakir");
    }
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    if (rId.isEmpty()) rId = QStringLiteral("default");
    QString path = QStringLiteral("%1/quran_audio/%2").arg(baseDir, rId);
    QDir().mkpath(path);
    return path;
}

QString QuranManager::getLocalAudioPath(const QString &reciterId, int surah, int ayah) const
{
    int effAyah = getEffectiveAudioAyah(reciterId, surah, ayah);
    return getLocalAudioPathByAudioAyah(reciterId, surah, effAyah);
}

QString QuranManager::getLocalAudioPathByAudioAyah(const QString &reciterId, int surah, int audioAyah) const
{
    QString dir = getAudioCacheDirectory(reciterId);
    return QStringLiteral("%1/%2%3.mp3")
            .arg(dir)
            .arg(surah, 3, 10, QLatin1Char('0'))
            .arg(audioAyah, 3, 10, QLatin1Char('0'));
}

QString QuranManager::getAyahAudioUrl(const QString &reciterId, int surah, int ayah) const
{
    int effAyah = getEffectiveAudioAyah(reciterId, surah, ayah);
    return getAyahAudioUrlByAudioAyah(reciterId, surah, effAyah);
}

QString QuranManager::getAyahAudioUrlByAudioAyah(const QString &reciterId, int surah, int audioAyah) const
{
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    QString urlTemplate;
    QVariantList reciters = const_cast<QuranManager*>(this)->getAvailableReciters();
    for (const QVariant &r : reciters) {
        QVariantMap m = r.toMap();
        if (m.value(QStringLiteral("id")).toString() == rId) {
            urlTemplate = m.value(QStringLiteral("url_template")).toString();
            break;
        }
    }
    if (urlTemplate.isEmpty() && !reciters.isEmpty()) {
        urlTemplate = reciters.first().toMap().value(QStringLiteral("url_template")).toString();
    }
    if (urlTemplate.isEmpty()) return QString();

    QString audioUrl = urlTemplate;
    audioUrl.replace(QStringLiteral("{surah:03d}"), QStringLiteral("%1").arg(surah, 3, 10, QLatin1Char('0')));
    audioUrl.replace(QStringLiteral("{ayah:03d}"), QStringLiteral("%1").arg(audioAyah, 3, 10, QLatin1Char('0')));
    return audioUrl;
}

void QuranManager::cacheAyahAudioInBackground(const QString &reciterId, int surah, int ayah)
{
    int effAyah = getEffectiveAudioAyah(reciterId, surah, ayah);
    cacheAyahAudioInBackgroundByAudioAyah(reciterId, surah, effAyah);
}

void QuranManager::cacheAyahAudioInBackgroundByAudioAyah(const QString &reciterId, int surah, int audioAyah)
{
    QString localPath = getLocalAudioPathByAudioAyah(reciterId, surah, audioAyah);
    if (QFile::exists(localPath) && QFile(localPath).size() > 1000) return;

    QString urlStr = getAyahAudioUrlByAudioAyah(reciterId, surah, audioAyah);
    if (urlStr.isEmpty()) return;

    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0"));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    QNetworkReply *reply = m_netManager->get(req);
    reply->setProperty("savePath", localPath);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            if (data.size() > 1000) {
                QString path = reply->property("savePath").toString();
                QFile f(path);
                if (f.open(QIODevice::WriteOnly)) {
                    f.write(data);
                    f.close();
                }
            }
        }
        reply->deleteLater();
    });
}

void QuranManager::playCurrentAudioSubPart()
{
    if (m_currentAyahAudioFiles.isEmpty() || m_currentAudioFileIndex >= m_currentAyahAudioFiles.size()) {
        return;
    }

    int audioAyah = m_currentAyahAudioFiles.at(m_currentAudioFileIndex);
    QString localPath = getLocalAudioPathByAudioAyah(m_selectedReciterId, m_playingSurah, audioAyah);
    if (QFile::exists(localPath) && QFile(localPath).size() > 1000) {
        qDebug() << "[QuranManager] Playing offline local audio:" << localPath;
        m_player->setMedia(QUrl::fromLocalFile(localPath));
    } else {
        QString audioUrl = getAyahAudioUrlByAudioAyah(m_selectedReciterId, m_playingSurah, audioAyah);
        if (audioUrl.isEmpty()) return;
        qDebug() << "[QuranManager] Streaming audio from internet:" << audioUrl;
        emit audioStreamingNotice(m_playingSurah, audioAyah);
        m_player->setMedia(QUrl(audioUrl));
        cacheAyahAudioInBackgroundByAudioAyah(m_selectedReciterId, m_playingSurah, audioAyah);
    }
    m_player->play();
}

void QuranManager::playAyah(int surah, int ayah)
{
    m_playingSurah = surah;
    m_playingAyah = ayah;

    recordAyahListened(surah, ayah);

    QVariantMap sInfo = getSurah(surah);
    m_maxAyahInPlayingSurah = sInfo.value(QStringLiteral("total_verses"), 7).toInt();

    m_currentAyahAudioFiles = getAudioFilesForAyah(m_selectedReciterId, surah, ayah);
    m_currentAudioFileIndex = 0;

    playCurrentAudioSubPart();

    // Check if this ayah starts with an offset in the audio file (e.g. Warsh 2 in Surah 5)
    int seekMs = getAudioStartOffsetMs(m_selectedReciterId, surah, ayah);
    if (seekMs > 0 && m_player) {
        m_player->setPosition(seekMs);
    }

    emit audioStateChanged();
    emit playingAyahChanged();

    // Proactively pre-render the next ayah into RAM cache so transition between verses is instant and never blanks
    preloadNextAyahPage(surah, ayah);
}

void QuranManager::preloadNextAyahPage(int surah, int ayah)
{
    if (surah <= 0 || ayah <= 0 || !m_db.isOpen()) return;

    int nextSurah = surah;
    int nextAyah = ayah + 1;
    if (nextAyah > m_maxAyahInPlayingSurah) {
        if (surah < 114) {
            nextSurah = surah + 1;
            nextAyah = 1;
        } else {
            return;
        }
    }

    int rId = m_riwayah;
    bool dark = m_darkMode;

    int nextPage = 0;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT page_number FROM ayahs WHERE riwayah_id = ? AND surah_number = ? AND ayah_number = ?"));
    q.addBindValue(rId);
    q.addBindValue(nextSurah);
    q.addBindValue(nextAyah);
    if (q.exec() && q.next()) {
        nextPage = q.value(0).toInt();
    }

    if (nextPage > 0) {
        // Ensure raw SVG is downloaded if needed
        preloadPage(nextPage);

        // Pre-render into RAM cache on background worker thread
        QThreadPool::globalInstance()->start(new PreloadNextAyahRunnable(this, nextPage, nextSurah, nextAyah, rId, dark));
    }
}

void QuranManager::playSurah(int surah, int startAyah)
{
    playAyah(surah, startAyah);
}

void QuranManager::pauseAudio()
{
    if (m_player) {
        m_player->pause();
        emit audioStateChanged();
    }
}

void QuranManager::resumeAudio()
{
    if (m_player) {
        m_player->play();
        emit audioStateChanged();
    }
}

void QuranManager::stopAudio()
{
    if (m_player) {
        m_player->stop();
        m_playingSurah = 0;
        m_playingAyah = 0;
        m_currentAyahAudioFiles.clear();
        m_currentAudioFileIndex = 0;
        m_pendingSeekMs = 0;
        m_audioProgress = 0.0;
        emit audioStateChanged();
        emit playingAyahChanged();
        emit audioProgressChanged();
    }
}

void QuranManager::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    emit audioStateChanged();

    // If media has buffered or loaded and we have a pending seek offset (e.g. Warsh 2 in Surah 5)
    if (m_pendingSeekMs > 0 && (status == QMediaPlayer::BufferedMedia || status == QMediaPlayer::LoadedMedia)) {
        if (m_player) {
            m_player->setPosition(m_pendingSeekMs);
        }
        m_pendingSeekMs = 0;
    }

    // Auto-advance to next sub-part or next ayah when current finishes
    if (status == QMediaPlayer::EndOfMedia) {
        // If current ayah spans multiple audio files (e.g. Warsh 1 spanning Hafs 1 + 2),
        // play the next sub-part while keeping m_playingAyah and the highlight intact!
        m_currentAudioFileIndex++;
        if (m_currentAudioFileIndex < m_currentAyahAudioFiles.size()) {
            qDebug() << "[QuranManager] Playing next sub-part" << m_currentAudioFileIndex
                     << "for Surah" << m_playingSurah << "Ayah" << m_playingAyah;
            playCurrentAudioSubPart();
            return;
        }

        // All audio sub-parts for this ayah finished. Advance to next distinct ayah
        int curLastAudioAyah = m_currentAyahAudioFiles.isEmpty() ? m_playingAyah : m_currentAyahAudioFiles.last();
        int nextAyah = m_playingAyah + 1;

        // Skip any subsequent Warsh ayahs whose audio files were already fully covered (if no startMs offset)
        while (nextAyah <= m_maxAyahInPlayingSurah) {
            QList<int> nextFiles = getAudioFilesForAyah(m_selectedReciterId, m_playingSurah, nextAyah);
            int nextStartOffset = getAudioStartOffsetMs(m_selectedReciterId, m_playingSurah, nextAyah);
            if (!nextFiles.isEmpty() && nextFiles.first() <= curLastAudioAyah && nextFiles.last() <= curLastAudioAyah && nextStartOffset == 0) {
                nextAyah++;
            } else {
                break;
            }
        }

        if (nextAyah <= m_maxAyahInPlayingSurah) {
            playAyah(m_playingSurah, nextAyah);
        } else {
            // Surah finished: proceed to next surah if valid
            if (m_playingSurah < 114) {
                playAyah(m_playingSurah + 1, 1);
            } else {
                stopAudio();
            }
        }
    }
}

void QuranManager::onPositionChanged(qint64 position)
{
    if (m_currentDuration > 0) {
        m_audioProgress = qBound(0.0, static_cast<double>(position) / static_cast<double>(m_currentDuration), 1.0);
        emit audioProgressChanged();
    }

    // Check intra-file ayah transitions (e.g. Surah 5 Ayah 1 -> Ayah 2 at 4800ms)
    if (m_riwayah == 2 && reciterNeedsHafsMapping(m_selectedReciterId) && m_playingAyah > 0) {
        quint32 key = (static_cast<quint32>(m_playingSurah) << 16) | static_cast<quint32>(m_playingAyah);
        if (m_warshToHafsMap.contains(key)) {
            HafsRange range = m_warshToHafsMap.value(key);
            if (range.endMs > 0 && position >= range.endMs) {
                int nextAyah = m_playingAyah + 1;
                if (nextAyah <= m_maxAyahInPlayingSurah) {
                    quint32 nextKey = (static_cast<quint32>(m_playingSurah) << 16) | static_cast<quint32>(nextAyah);
                    if (m_warshToHafsMap.contains(nextKey)) {
                        HafsRange nextRange = m_warshToHafsMap.value(nextKey);
                        if (nextRange.start == range.end) {
                            m_playingAyah = nextAyah;
                            emit playingAyahChanged();
                        }
                    }
                }
            }
        }
    }
}

void QuranManager::onDurationChanged(qint64 duration)
{
    m_currentDuration = duration;
}

bool QuranManager::isBookmarked(int surah, int ayah)
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList bmarks = s.value(QStringLiteral("quran/bookmarks")).toStringList();
    QString key = QStringLiteral("%1:%2").arg(surah).arg(ayah);
    return bmarks.contains(key);
}

bool QuranManager::toggleBookmark(int surah, int ayah)
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList bmarks = s.value(QStringLiteral("quran/bookmarks")).toStringList();
    QString key = QStringLiteral("%1:%2").arg(surah).arg(ayah);

    bool added = false;
    if (bmarks.contains(key)) {
        bmarks.removeAll(key);
        added = false;
    } else {
        bmarks.append(key);
        added = true;
    }

    s.setValue(QStringLiteral("quran/bookmarks"), bmarks);
    s.sync();
    emit bookmarksChanged();
    return added;
}

void QuranManager::removeBookmark(int surah, int ayah)
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList bmarks = s.value(QStringLiteral("quran/bookmarks")).toStringList();
    QString key = QStringLiteral("%1:%2").arg(surah).arg(ayah);

    if (bmarks.contains(key)) {
        bmarks.removeAll(key);
        s.setValue(QStringLiteral("quran/bookmarks"), bmarks);
        s.sync();
        emit bookmarksChanged();
    }
}

void QuranManager::clearAllBookmarks()
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    s.remove(QStringLiteral("quran/bookmarks"));
    s.sync();
    emit bookmarksChanged();
}

QVariantList QuranManager::getBookmarks()
{
    QVariantList list;
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList bmarks = s.value(QStringLiteral("quran/bookmarks")).toStringList();

    for (const QString &b : bmarks) {
        QStringList parts = b.split(QLatin1Char(':'));
        if (parts.size() == 2) {
            int sNum = parts[0].toInt();
            int aNum = parts[1].toInt();

            QVariantMap m;
            m[QStringLiteral("surah_number")] = sNum;
            m[QStringLiteral("ayah_number")] = aNum;

            // Fetch ayah text and surah name
            if (m_db.isOpen()) {
                QSqlQuery q(m_db);
                q.prepare(QStringLiteral("SELECT a.text_uthmani, a.page_number, s.name_ar, s.name_en FROM ayahs a JOIN surahs s ON a.surah_number = s.number WHERE a.riwayah_id = ? AND a.surah_number = ? AND a.ayah_number = ?"));
                q.addBindValue(m_riwayah);
                q.addBindValue(sNum);
                q.addBindValue(aNum);
                if (q.exec() && q.next()) {
                    m[QStringLiteral("text_uthmani")] = cleanUthmaniText(q.value(0).toString());
                    m[QStringLiteral("page_number")] = q.value(1).toInt();
                    m[QStringLiteral("surah_name_ar")] = q.value(2).toString();
                    m[QStringLiteral("surah_name_en")] = q.value(3).toString();
                }
            }
            list.append(m);
        }
    }
    return list;
}

void QuranManager::saveLastPosition(int surah, int ayah, int page)
{
    if (!m_db.isOpen()) return;

    int sNum = qBound(1, surah, 114);

    int maxVerses = 0;
    QSqlQuery qCount(m_db);
    qCount.prepare(QStringLiteral("SELECT COUNT(*) FROM ayahs WHERE riwayah_id = ? AND surah_number = ?"));
    qCount.addBindValue(m_riwayah);
    qCount.addBindValue(sNum);
    if (qCount.exec() && qCount.next()) {
        maxVerses = qCount.value(0).toInt();
    }
    int aNum = (maxVerses > 0) ? qBound(1, ayah, maxVerses) : qMax(1, ayah);

    // Look up the true page number for this (surah, ayah) in the current riwayah
    int pNum = getPageForAyah(sNum, aNum, m_riwayah);
    if (pNum < 1 || pNum > 604) {
        pNum = qBound(1, page, 604);
    }

    m_lastSurah = sNum;
    m_lastAyah = aNum;
    m_lastPage = pNum;

    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    s.setValue(QStringLiteral("quran/lastSurah"), m_lastSurah);
    s.setValue(QStringLiteral("quran/lastAyah"), m_lastAyah);
    s.setValue(QStringLiteral("quran/lastPage"), m_lastPage);
    s.sync();

    emit lastPositionChanged();
}

int QuranManager::downloadedPagesCount() const
{
    return getDownloadedPagesCount(m_isDownloadingPages ? m_bulkPagesRiwayah : m_riwayah);
}

double QuranManager::pagesDownloadProgress() const
{
    if (m_bulkPagesTotalInBatch <= 0) return 0.0;
    return qBound(0.0, (double)m_bulkPagesDownloadedInBatch / (double)m_bulkPagesTotalInBatch, 1.0);
}

int QuranManager::getDownloadedPagesCount(int riwayahId) const
{
    if (riwayahId != 1 && riwayahId != 2) riwayahId = m_riwayah;
    int count = 0;
    for (int p = 1; p <= 604; ++p) {
        if (QFile::exists(getLocalPagePath(riwayahId, p))) count++;
    }
    return count;
}

qint64 QuranManager::getPagesCacheSize(int riwayahId) const
{
    if (riwayahId != 1 && riwayahId != 2) riwayahId = m_riwayah;
    QString dir = getCacheDirectory(riwayahId);
    QDir d(dir);
    qint64 total = 0;
    const QFileInfoList list = d.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : list) {
        total += fi.size();
    }

    QString renderedDir = getRenderedPagesCacheDir();
    QDir rd(renderedDir);
    const QFileInfoList rList = rd.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : rList) {
        total += fi.size();
    }

    return total;
}

void QuranManager::clearPagesCache(int riwayahId)
{
    cancelBulkPagesDownload();
    if (riwayahId != 1 && riwayahId != 2) riwayahId = m_riwayah;
    QString dir = getCacheDirectory(riwayahId);
    QDir d(dir);
    const QFileInfoList list = d.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : list) {
        QFile::remove(fi.absoluteFilePath());
    }

    QString renderedDir = getRenderedPagesCacheDir();
    QDir rd(renderedDir);
    const QFileInfoList rList = rd.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : rList) {
        QFile::remove(fi.absoluteFilePath());
    }

    QMutexLocker locker(&m_renderMutex);
    m_pageImageCache.clear();
    m_memCachedPageNumber = -1;
    m_memCachedSvgContent.clear();
    emit bulkPagesProgressChanged();
    emit pageCached(1);
}

bool QuranManager::isOnline() const
{
    if (m_netManager && m_netManager->networkAccessible() == QNetworkAccessManager::NotAccessible) {
        return false;
    }
    return m_ncm.isOnline();
}

void QuranManager::startBulkPagesDownload(int riwayahId)
{
    if (!isOnline()) return;
    if (riwayahId != 1 && riwayahId != 2) riwayahId = m_riwayah;
    if (m_isDownloadingPages) return;

    m_bulkPagesRiwayah = riwayahId;
    m_bulkPagesQueue.clear();
    for (int p = 1; p <= 604; ++p) {
        if (!QFile::exists(getLocalPagePath(riwayahId, p))) {
            m_bulkPagesQueue.append(p);
        }
    }

    if (m_bulkPagesQueue.isEmpty()) {
        emit bulkPagesProgressChanged();
        return;
    }

    m_bulkPagesTotalInBatch = m_bulkPagesQueue.size();
    m_bulkPagesDownloadedInBatch = 0;
    m_bulkPagesActiveCount = 0;
    m_isDownloadingPages = true;
    emit bulkPagesProgressChanged();

    fetchNextBulkPage();
    fetchNextBulkPage();
}

void QuranManager::fetchNextBulkPage()
{
    if (!m_isDownloadingPages) return;

    if (m_bulkPagesQueue.isEmpty()) {
        if (m_bulkPagesActiveCount <= 0) {
            m_isDownloadingPages = false;
            emit bulkPagesProgressChanged();
        }
        return;
    }

    int pageNumber = m_bulkPagesQueue.takeFirst();
    m_bulkPagesActiveCount++;

    QUrl url(getRemotePageUrl(m_bulkPagesRiwayah, pageNumber));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36"));
    req.setRawHeader("Accept", "image/svg+xml,image/*,*/*;q=0.8");
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    QNetworkReply *reply = m_netManager->get(req);
    reply->setProperty("bulkPage", true);
    reply->setProperty("pageNumber", pageNumber);
    reply->setProperty("riwayah", m_bulkPagesRiwayah);
    reply->ignoreSslErrors();

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int page = reply->property("pageNumber").toInt();
        int rId = reply->property("riwayah").toInt();
        m_bulkPagesActiveCount--;

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            if (data.size() > 500 && data.contains("<svg")) {
                QString localPath = QStringLiteral("%1/raw_%2.svg").arg(getCacheDirectory(rId), QString::number(page));
                QFile f(localPath);
                if (f.open(QIODevice::WriteOnly)) {
                    f.write(data);
                    f.close();
                    emit pageCached(page);
                }
            }
        }
        reply->deleteLater();

        if (m_isDownloadingPages) {
            m_bulkPagesDownloadedInBatch++;
            emit bulkPagesProgressChanged();
            fetchNextBulkPage();
        }
    });
}

void QuranManager::cancelBulkPagesDownload()
{
    m_isDownloadingPages = false;
    m_bulkPagesQueue.clear();
    emit bulkPagesProgressChanged();
}

int QuranManager::downloadedAudioCount() const
{
    return getDownloadedAudioCount(m_selectedReciterId);
}

double QuranManager::audioDownloadProgress() const
{
    if (m_audioTotalInBatch <= 0) return 0.0;
    return qBound(0.0, (double)m_audioDownloadedInBatch / (double)m_audioTotalInBatch, 1.0);
}

int QuranManager::getDownloadedAudioCount(const QString &reciterId) const
{
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    QString dir = getAudioCacheDirectory(rId);
    QDir d(dir);
    QStringList files = d.entryList(QStringList() << QStringLiteral("*.mp3"), QDir::Files);
    return files.size();
}

qint64 QuranManager::getAudioCacheSize(const QString &reciterId) const
{
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    QString dir = getAudioCacheDirectory(rId);
    QDir d(dir);
    qint64 total = 0;
    const QFileInfoList list = d.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : list) {
        total += fi.size();
    }
    return total;
}

void QuranManager::clearAudioCache(const QString &reciterId)
{
    cancelAudioDownload();
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    QString dir = getAudioCacheDirectory(rId);
    QDir d(dir);
    const QFileInfoList list = d.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : list) {
        QFile::remove(fi.absoluteFilePath());
    }
    emit audioDownloadProgressChanged();
}

bool QuranManager::isAyahAudioCached(int surah, int ayah, const QString &reciterId) const
{
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    QList<int> files = getAudioFilesForAyah(rId, surah, ayah);
    if (files.isEmpty()) files.append(ayah);
    for (int audioAyah : files) {
        QString path = getLocalAudioPathByAudioAyah(rId, surah, audioAyah);
        if (!QFile::exists(path) || QFile(path).size() <= 1000) return false;
    }
    return true;
}

bool QuranManager::isSurahAudioCached(int surah, const QString &reciterId) const
{
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    QVariantMap sInfo = const_cast<QuranManager*>(this)->getSurah(surah);
    int total = sInfo.value(QStringLiteral("total_verses"), 0).toInt();
    if (total <= 0) return false;
    for (int a = 1; a <= total; ++a) {
        if (!isAyahAudioCached(surah, a, rId)) return false;
    }
    return true;
}

void QuranManager::startSurahAudioDownload(int surahNumber, const QString &reciterId)
{
    if (!isOnline()) return;
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    if (rId.isEmpty()) return;

    QVariantMap sInfo = getSurah(surahNumber);
    int totalVerses = sInfo.value(QStringLiteral("total_verses"), 0).toInt();
    if (totalVerses <= 0) return;

    cancelAudioDownload();

    m_audioDownloadQueue.clear();
    m_audioTotalInBatch = 0;
    m_audioDownloadedInBatch = 0;

    QSet<QString> queuedPaths;
    for (int a = 1; a <= totalVerses; ++a) {
        QList<int> files = getAudioFilesForAyah(rId, surahNumber, a);
        for (int audioAyah : files) {
            QString localPath = getLocalAudioPathByAudioAyah(rId, surahNumber, audioAyah);
            if ((!QFile::exists(localPath) || QFile(localPath).size() < 1000) && !queuedPaths.contains(localPath)) {
                queuedPaths.insert(localPath);
                AudioItem item;
                item.surah = surahNumber;
                item.ayah = audioAyah;
                item.reciterId = rId;
                m_audioDownloadQueue.append(item);
                m_audioTotalInBatch++;
            }
        }
    }

    if (m_audioDownloadQueue.isEmpty()) {
        m_audioDownloadStatus = QString::fromUtf8("تم تحميل سورة %1 مسبقاً").arg(sInfo.value(QStringLiteral("name_ar")).toString());
        emit audioDownloadProgressChanged();
        return;
    }

    m_isDownloadingAudio = true;
    m_audioDownloadStatus = QString::fromUtf8("بدء تحميل سورة %1...").arg(sInfo.value(QStringLiteral("name_ar")).toString());
    emit audioDownloadProgressChanged();

    fetchNextAudio();
}

void QuranManager::startFullAudioDownload(const QString &reciterId)
{
    if (!isOnline()) return;
    QString rId = reciterId.isEmpty() ? m_selectedReciterId : reciterId;
    if (rId.isEmpty()) return;

    cancelAudioDownload();

    m_audioDownloadQueue.clear();
    m_audioTotalInBatch = 0;
    m_audioDownloadedInBatch = 0;

    QSet<QString> queuedFullPaths;
    for (int s = 1; s <= 114; ++s) {
        QVariantMap sInfo = getSurah(s);
        int totalVerses = sInfo.value(QStringLiteral("total_verses"), 0).toInt();
        for (int a = 1; a <= totalVerses; ++a) {
            QList<int> files = getAudioFilesForAyah(rId, s, a);
            for (int audioAyah : files) {
                QString localPath = getLocalAudioPathByAudioAyah(rId, s, audioAyah);
                if ((!QFile::exists(localPath) || QFile(localPath).size() < 1000) && !queuedFullPaths.contains(localPath)) {
                    queuedFullPaths.insert(localPath);
                    AudioItem item;
                    item.surah = s;
                    item.ayah = audioAyah;
                    item.reciterId = rId;
                    m_audioDownloadQueue.append(item);
                    m_audioTotalInBatch++;
                }
            }
        }
    }

    if (m_audioDownloadQueue.isEmpty()) {
        m_audioDownloadStatus = QString::fromUtf8("تم تحميل التلاوة كاملة مسبقاً");
        emit audioDownloadProgressChanged();
        return;
    }

    m_isDownloadingAudio = true;
    m_audioDownloadStatus = QString::fromUtf8("بدء تحميل المصحف الصوتي كاملاً...");
    emit audioDownloadProgressChanged();

    fetchNextAudio();
}

void QuranManager::fetchNextAudio()
{
    if (!m_isDownloadingAudio) return;

    if (m_audioDownloadQueue.isEmpty()) {
        m_isDownloadingAudio = false;
        m_audioDownloadStatus = QString::fromUtf8("اكتمل التحميل بنجاح");
        emit audioDownloadProgressChanged();
        return;
    }

    AudioItem item = m_audioDownloadQueue.takeFirst();
    QString localPath = getLocalAudioPathByAudioAyah(item.reciterId, item.surah, item.ayah);

    if (QFile::exists(localPath) && QFile(localPath).size() > 1000) {
        m_audioDownloadedInBatch++;
        emit audioDownloadProgressChanged();
        fetchNextAudio();
        return;
    }

    QString urlStr = getAyahAudioUrlByAudioAyah(item.reciterId, item.surah, item.ayah);
    if (urlStr.isEmpty()) {
        fetchNextAudio();
        return;
    }

    QVariantMap sInfo = getSurah(item.surah);
    QString surahName = sInfo.value(QStringLiteral("name_ar")).toString();
    m_audioDownloadStatus = QString::fromUtf8("%1: آية %2 (%3/%4)")
            .arg(surahName)
            .arg(item.ayah)
            .arg(m_audioDownloadedInBatch + 1)
            .arg(m_audioTotalInBatch);
    emit audioDownloadProgressChanged();

    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0"));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    m_activeAudioReply = m_netManager->get(req);
    m_activeAudioReply->ignoreSslErrors();
    m_activeAudioReply->setProperty("localPath", localPath);

    connect(m_activeAudioReply, &QNetworkReply::finished, this, [this]() {
        if (!m_activeAudioReply) return;
        QNetworkReply *reply = m_activeAudioReply;
        m_activeAudioReply = nullptr;

        QString path = reply->property("localPath").toString();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            if (data.size() > 1000) {
                QFile f(path);
                if (f.open(QIODevice::WriteOnly)) {
                    f.write(data);
                    f.close();
                }
            }
        }
        reply->deleteLater();

        if (m_isDownloadingAudio) {
            m_audioDownloadedInBatch++;
            emit audioDownloadProgressChanged();
            fetchNextAudio();
        }
    });
}

void QuranManager::cancelAudioDownload()
{
    m_isDownloadingAudio = false;
    m_audioDownloadQueue.clear();
    if (m_activeAudioReply) {
        m_activeAudioReply->abort();
        m_activeAudioReply->deleteLater();
        m_activeAudioReply = nullptr;
    }
    m_audioDownloadStatus = QString::fromUtf8("تم إيقاف التحميل");
    emit audioDownloadProgressChanged();
}

QString QuranManager::formatFileSize(qint64 bytes) const
{
    if (bytes < 1024) return QStringLiteral("\u200E%1 B\u200E").arg(bytes);
    if (bytes < 1024 * 1024) return QStringLiteral("\u200E%1 KB\u200E").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024 * 1024 * 1024) return QStringLiteral("\u200E%1 MB\u200E").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    return QStringLiteral("\u200E%1 GB\u200E").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}

void QuranManager::recordAyahListened(int surah, int ayah)
{
    if (surah <= 0 || ayah <= 0) return;

    QString today = QDate::currentDate().toString(Qt::ISODate);
    QString key = QStringLiteral("%1:%2").arg(surah).arg(ayah);

    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QString settingKey = QStringLiteral("quran/listeningHistory/%1").arg(today);
    QStringList ayahs = s.value(settingKey).toStringList();

    if (!ayahs.contains(key)) {
        ayahs.append(key);
        s.setValue(settingKey, ayahs);

        QStringList dateList = s.value(QStringLiteral("quran/listeningHistoryDates")).toStringList();
        if (!dateList.contains(today)) {
            dateList.append(today);
            s.setValue(QStringLiteral("quran/listeningHistoryDates"), dateList);
        }

        s.sync();
        emit listeningHistoryChanged();
    }
}

QVariantList QuranManager::getDailyListeningStats() const
{
    QVariantList result;
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList dateList = s.value(QStringLiteral("quran/listeningHistoryDates")).toStringList();

    // Sort dates descending (newest first)
    std::sort(dateList.begin(), dateList.end(), [](const QString &d1, const QString &d2) {
        return d1 > d2;
    });

    for (const QString &dStr : dateList) {
        QString settingKey = QStringLiteral("quran/listeningHistory/%1").arg(dStr);
        QStringList ayahs = s.value(settingKey).toStringList();
        if (ayahs.isEmpty()) continue;

        QDate d = QDate::fromString(dStr, Qt::ISODate);

        QVariantMap dayMap;
        dayMap[QStringLiteral("date")] = dStr;
        dayMap[QStringLiteral("count")] = ayahs.size();
        dayMap[QStringLiteral("dayOfWeek")] = d.isValid() ? d.dayOfWeek() : 0;
        dayMap[QStringLiteral("year")] = d.isValid() ? d.year() : 0;
        dayMap[QStringLiteral("month")] = d.isValid() ? d.month() : 0;
        dayMap[QStringLiteral("day")] = d.isValid() ? d.day() : 0;

        result.append(dayMap);
    }
    return result;
}

int QuranManager::getTodayListenedCount() const
{
    QString today = QDate::currentDate().toString(Qt::ISODate);
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QString settingKey = QStringLiteral("quran/listeningHistory/%1").arg(today);
    return s.value(settingKey).toStringList().size();
}

int QuranManager::getTotalListenedCount() const
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList dateList = s.value(QStringLiteral("quran/listeningHistoryDates")).toStringList();
    QSet<QString> allUniqueAyahs;

    for (const QString &dStr : dateList) {
        QString settingKey = QStringLiteral("quran/listeningHistory/%1").arg(dStr);
        QStringList ayahs = s.value(settingKey).toStringList();
        for (const QString &a : ayahs) {
            allUniqueAyahs.insert(a);
        }
    }
    return allUniqueAyahs.size();
}

void QuranManager::clearListeningHistory()
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    QStringList dateList = s.value(QStringLiteral("quran/listeningHistoryDates")).toStringList();
    for (const QString &dStr : dateList) {
        s.remove(QStringLiteral("quran/listeningHistory/%1").arg(dStr));
    }
    s.remove(QStringLiteral("quran/listeningHistory"));
    s.remove(QStringLiteral("quran/listeningHistoryDates"));
    s.sync();
    emit listeningHistoryChanged();
}

