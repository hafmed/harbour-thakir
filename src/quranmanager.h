#ifndef QURANMANAGER_H
#define QURANMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QImage>
#include <QSize>
#include <QCache>
#include <QMutex>

class QuranManager : public QObject
{
    Q_OBJECT

    // Active Riwayah (1 = Hafs, 2 = Warsh)
    Q_PROPERTY(int riwayah READ riwayah WRITE setRiwayah NOTIFY riwayahChanged)
    Q_PROPERTY(QString riwayahCode READ riwayahCode NOTIFY riwayahChanged)
    Q_PROPERTY(QString riwayahName READ riwayahName NOTIFY riwayahChanged)
    Q_PROPERTY(QString riwayahDescription READ riwayahDescription NOTIFY riwayahChanged)

    // Reader UI Preferences
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(int tafsirFontSize READ tafsirFontSize WRITE setTafsirFontSize NOTIFY tafsirFontSizeChanged)
    Q_PROPERTY(int viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged) // 0 = Text / 1 = Mushaf Page
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY darkModeChanged)
    Q_PROPERTY(bool tajweedMode READ tajweedMode WRITE setTajweedMode NOTIFY tajweedModeChanged)
    Q_PROPERTY(int lastSurah READ lastSurah NOTIFY lastPositionChanged)
    Q_PROPERTY(int lastAyah READ lastAyah NOTIFY lastPositionChanged)
    Q_PROPERTY(int lastPage READ lastPage NOTIFY lastPositionChanged)
    Q_PROPERTY(QString selectedTafsirEditionId READ selectedTafsirEditionId WRITE setSelectedTafsirEditionId NOTIFY selectedTafsirEditionChanged)

    // Audio Playback
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY audioStateChanged)
    Q_PROPERTY(int playingSurah READ playingSurah NOTIFY playingAyahChanged)
    Q_PROPERTY(int playingAyah READ playingAyah NOTIFY playingAyahChanged)
    Q_PROPERTY(QString selectedReciterId READ selectedReciterId NOTIFY reciterChanged)
    Q_PROPERTY(QString selectedReciterName READ selectedReciterName NOTIFY reciterChanged)
    Q_PROPERTY(double audioProgress READ audioProgress NOTIFY audioProgressChanged)

    // Bulk Downloads & Offline
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(bool isDownloadingPages READ isDownloadingPages NOTIFY bulkPagesProgressChanged)
    Q_PROPERTY(int downloadedPagesCount READ downloadedPagesCount NOTIFY bulkPagesProgressChanged)
    Q_PROPERTY(int totalPagesToDownload READ totalPagesToDownload NOTIFY bulkPagesProgressChanged)
    Q_PROPERTY(double pagesDownloadProgress READ pagesDownloadProgress NOTIFY bulkPagesProgressChanged)

    Q_PROPERTY(bool isDownloadingAudio READ isDownloadingAudio NOTIFY audioDownloadProgressChanged)
    Q_PROPERTY(int downloadedAudioCount READ downloadedAudioCount NOTIFY audioDownloadProgressChanged)
    Q_PROPERTY(int totalAudioToDownload READ totalAudioToDownload NOTIFY audioDownloadProgressChanged)
    Q_PROPERTY(double audioDownloadProgress READ audioDownloadProgress NOTIFY audioDownloadProgressChanged)
    Q_PROPERTY(QString audioDownloadStatus READ audioDownloadStatus NOTIFY audioDownloadProgressChanged)

public:
    enum SearchMode {
        WordWithAffixes = 0,
        ExactLiteral = 1,
        PartialMatch = 2,
        RootSearch = 3,
        PhraseSearch = 4,
        TranslationSearch = 5
    };
    Q_ENUM(SearchMode)

    enum SearchScope {
        ScopeAll = 0,
        ScopeSurah = 1,
        ScopeJuz = 2,
        ScopePage = 3
    };
    Q_ENUM(SearchScope)

    explicit QuranManager(QObject *parent = nullptr);
    ~QuranManager();

    // Riwayah
    int riwayah() const;
    void setRiwayah(int rId);
    QString riwayahCode() const;
    QString riwayahName() const;
    QString riwayahDescription() const;

    // View Preferences
    int fontSize() const;
    void setFontSize(int size);
    int tafsirFontSize() const;
    void setTafsirFontSize(int size);
    int viewMode() const;
    void setViewMode(int mode);
    bool darkMode() const;
    void setDarkMode(bool dark);
    bool tajweedMode() const;
    void setTajweedMode(bool enabled);
    Q_INVOKABLE QString formatTajweedHtml(const QString &tajweedText, bool isDark = false) const;
    int lastSurah() const;
    int lastAyah() const;
    int lastPage() const;
    QString selectedTafsirEditionId() const;
    void setSelectedTafsirEditionId(const QString &editionId);

    // Audio
    bool isPlaying() const;
    int playingSurah() const;
    int playingAyah() const;
    QString selectedReciterId() const;
    QString selectedReciterName() const;
    double audioProgress() const;

    // Bulk Downloads Getters
    bool isDownloadingPages() const { return m_isDownloadingPages; }
    int downloadedPagesCount() const;
    int totalPagesToDownload() const { return 604; }
    double pagesDownloadProgress() const;

    bool isDownloadingAudio() const { return m_isDownloadingAudio; }
    int downloadedAudioCount() const;
    int totalAudioToDownload() const { return m_audioTotalInBatch; }
    double audioDownloadProgress() const;
    QString audioDownloadStatus() const { return m_audioDownloadStatus; }

    // Invocable API for QML
    Q_INVOKABLE QVariantList getRiwayat();
    Q_INVOKABLE QVariantList getSurahs();
    Q_INVOKABLE QVariantMap getSurah(int surahNumber);
    Q_INVOKABLE QVariantList getJuzs();
    Q_INVOKABLE QVariantList getHizbs();
    Q_INVOKABLE QVariantList getAyahsForSurah(int surahNumber);
    Q_INVOKABLE QVariantList getAyahsForPage(int pageNumber);
    Q_INVOKABLE int getPageForAyah(int surah, int ayah, int riwayahId = 0) const;
    Q_INVOKABLE QVariantMap getAyah(int surahNumber, int ayahNumber, int riwayahId = 0);
    Q_INVOKABLE QVariantList search(const QString &query, int surahFilter = 0);
    Q_INVOKABLE QVariantList searchAdvanced(const QString &query, int searchMode = 0, int scopeType = 0, int scopeValue = 0, const QString &translationEdition = QString(), bool respectTashkeel = false);
    Q_INVOKABLE QVariantMap getSearchStats(const QString &query, int searchMode = 0, bool respectTashkeel = false, const QString &translationEdition = QString());
    Q_INVOKABLE QString cleanTashkeelText(const QString &text) const;
    int mapHafsToWarsh(int surah, int hafsAyah) const;

    // Hybrid Page View & Caching
    Q_INVOKABLE QString getPageImageUrl(int pageNumber);
    Q_INVOKABLE QString getRenderedPageUrl(int pageNumber, int highlightSurah, int highlightAyah, bool darkMode);
    Q_INVOKABLE bool isPageCached(int pageNumber);
    Q_INVOKABLE bool isPageDownloading(int pageNumber) const;
    Q_INVOKABLE void preloadPage(int pageNumber);
    Q_INVOKABLE QVariantMap getAyahAtCoordinate(int pageNumber, double normalizedX, double normalizedY, int riwayahId = 0);
    QImage renderPageImage(int pageNumber, int highlightSurah, int highlightAyah, bool darkMode, int riwayahId = 0, const QSize &requestedSize = QSize(), QSize *size = nullptr);

    // Bulk Pages Download
    Q_INVOKABLE void startBulkPagesDownload(int riwayahId = 0);
    Q_INVOKABLE void cancelBulkPagesDownload();

    // Audio Control
    Q_INVOKABLE void playAyah(int surah, int ayah);
    Q_INVOKABLE void playSurah(int surah, int startAyah = 1);
    Q_INVOKABLE void pauseAudio();
    Q_INVOKABLE void resumeAudio();
    Q_INVOKABLE void stopAudio();
    Q_INVOKABLE QVariantList getAvailableReciters();
    Q_INVOKABLE void setReciter(const QString &reciterId);

    // Audio Download & Offline
    Q_INVOKABLE void startSurahAudioDownload(int surahNumber, const QString &reciterId = QString());
    Q_INVOKABLE void startFullAudioDownload(const QString &reciterId = QString());
    Q_INVOKABLE void cancelAudioDownload();
    Q_INVOKABLE bool isAyahAudioCached(int surah, int ayah, const QString &reciterId = QString()) const;
    Q_INVOKABLE bool isSurahAudioCached(int surah, const QString &reciterId = QString()) const;
    Q_INVOKABLE QString getAyahAudioUrl(const QString &reciterId, int surah, int ayah) const;

    // Cache Stats & Management
    Q_INVOKABLE bool isOnline() const;
    Q_INVOKABLE int getDownloadedPagesCount(int riwayahId = 0) const;
    Q_INVOKABLE qint64 getPagesCacheSize(int riwayahId = 0) const;
    Q_INVOKABLE void clearPagesCache(int riwayahId = 0);

    Q_INVOKABLE int getDownloadedAudioCount(const QString &reciterId = QString()) const;
    Q_INVOKABLE qint64 getAudioCacheSize(const QString &reciterId = QString()) const;
    Q_INVOKABLE void clearAudioCache(const QString &reciterId = QString());
    Q_INVOKABLE QString formatFileSize(qint64 bytes) const;

    // Bookmarks and Last Position
    Q_INVOKABLE bool toggleBookmark(int surah, int ayah);
    Q_INVOKABLE void removeBookmark(int surah, int ayah);
    Q_INVOKABLE void clearAllBookmarks();
    Q_INVOKABLE bool isBookmarked(int surah, int ayah);
    Q_INVOKABLE QVariantList getBookmarks();
    Q_INVOKABLE void saveLastPosition(int surah, int ayah, int page);

    // Tafsir and Translation
    Q_INVOKABLE QVariantList getAvailableTafsirEditions() const;
    Q_INVOKABLE QString getAyahTafsir(int surah, int ayah, const QString &editionId = QString(), int riwayahId = 0) const;
    Q_INVOKABLE QVariantList getTafsirAndTranslations(int surah, int ayah, int riwayahId = 0) const;
    Q_INVOKABLE QString createTextShareFile(const QString &title, const QString &content) const;

    // Daily Listening Statistics (stored in settings)
    Q_INVOKABLE void recordAyahListened(int surah, int ayah);
    Q_INVOKABLE QVariantList getDailyListeningStats() const;
    Q_INVOKABLE int getTodayListenedCount() const;
    Q_INVOKABLE int getTotalListenedCount() const;
    Q_INVOKABLE void clearListeningHistory();

signals:
    void riwayahChanged();
    void fontSizeChanged();
    void tafsirFontSizeChanged();
    void viewModeChanged();
    void darkModeChanged();
    void tajweedModeChanged();
    void lastPositionChanged();
    void selectedTafsirEditionChanged();
    void audioStateChanged();
    void playingAyahChanged();
    void reciterChanged();
    void audioProgressChanged();
    void pageCached(int pageNumber);
    void pageDownloadFailed(int pageNumber);
    void bookmarksChanged();
    void bulkPagesProgressChanged();
    void audioDownloadProgressChanged();
    void audioStreamingNotice(int surah, int ayah);
    void audioPlaybackFailed(int surah, int ayah, bool isNetworkError);
    void listeningHistoryChanged();
    void onlineStateChanged(bool isOnline);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onPlayerError(QMediaPlayer::Error error);
    void onPageDownloaded();

private:
    struct AudioItem {
        int surah;
        int ayah;
        QString reciterId;
    };

    void initDatabase();
    void initAudio();
    QString findDatabasePath() const;
    QString getCacheDirectory(int riwayahId) const;
    QString getRenderedPagesCacheDir() const;
    QString getLocalPagePath(int riwayahId, int pageNumber) const;
    QString getRemotePageUrl(int riwayahId, int pageNumber) const;
    QString getAudioCacheDirectory(const QString &reciterId) const;
    QString getLocalAudioPath(const QString &reciterId, int surah, int ayah) const;
    QString getLocalAudioPathByAudioAyah(const QString &reciterId, int surah, int audioAyah) const;
    QString getAyahAudioUrlByAudioAyah(const QString &reciterId, int surah, int audioAyah) const;
    QString normalizeArabic(const QString &input) const;
    int getEffectiveAudioAyah(const QString &reciterId, int surah, int ayah) const;
    int getAudioStartOffsetMs(const QString &reciterId, int surah, int ayah) const;
    QList<int> getAudioFilesForAyah(const QString &reciterId, int surah, int ayah) const;
    bool reciterNeedsHafsMapping(const QString &reciterId) const;
    void loadWarshToHafsMap();
    void playCurrentAudioSubPart();
    void fetchNextBulkPage();
    void fetchNextAudio();
    void cacheAyahAudioInBackground(const QString &reciterId, int surah, int ayah);
    void cacheAyahAudioInBackgroundByAudioAyah(const QString &reciterId, int surah, int audioAyah);
    void preloadNextAyahPage(int surah, int ayah);

    QSqlDatabase m_db;
    int m_riwayah;
    int m_fontSize;
    int m_tafsirFontSize;
    int m_viewMode;
    bool m_darkMode;
    bool m_tajweedMode;
    int m_lastSurah;
    int m_lastAyah;
    int m_lastPage;
    QString m_selectedTafsirEditionId;

    // Audio
    QMediaPlayer *m_player;
    int m_playingSurah;
    int m_playingAyah;
    int m_maxAyahInPlayingSurah;
    QString m_selectedReciterId;
    double m_audioProgress;
    qint64 m_currentDuration;
    QList<int> m_currentAyahAudioFiles;
    int m_currentAudioFileIndex;
    int m_pendingSeekMs;

    // Network for caching pages
    QNetworkAccessManager *m_netManager;
    QNetworkConfigurationManager m_ncm;
    QMap<QNetworkReply*, int> m_activeDownloads;

    // Bulk Pages State
    bool m_isDownloadingPages;
    int m_bulkPagesRiwayah;
    QList<int> m_bulkPagesQueue;
    int m_bulkPagesTotalInBatch;
    int m_bulkPagesDownloadedInBatch;
    int m_bulkPagesActiveCount;

    // Audio Download State
    bool m_isDownloadingAudio;
    QList<AudioItem> m_audioDownloadQueue;
    int m_audioTotalInBatch;
    int m_audioDownloadedInBatch;
    QString m_audioDownloadStatus;
    QNetworkReply *m_activeAudioReply;

    // In-memory SVG page cache to prevent repeated disk reads during ayah playback
    mutable int m_memCachedPageNumber;
    mutable int m_memCachedRiwayah;
    mutable QString m_memCachedSvgContent;

    // Warsh to Hafs verse mapping cache
    struct HafsRange {
        int start;
        int end;
        int startMs;
        int endMs;
    };
    QHash<quint32, HafsRange> m_warshToHafsMap;
    QHash<quint32, quint16> m_hafsToWarshMap;
    QHash<quint32, quint16> m_warshPageMap;
    mutable QCache<QString, QImage> m_pageImageCache;
    mutable QMutex m_renderMutex;
};

#endif // QURANMANAGER_H
