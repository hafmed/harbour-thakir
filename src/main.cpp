#include <sailfishapp.h>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QMediaPlayer>
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QTimeZone>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

#ifdef HAVE_NEMONOTIFICATIONS
#include <notification.h>
#endif

#include <QEventLoop>
#include <QTranslator>
#include <QLocale>
#include "prayermanager.h"
#include "prayertimes.h"
#include "playbackcontroller.h"
#include "settingshelper.h"
#include "powerbuttonwatcher.h"
#include "athannotification.h"
#include "silentmodehelper.h"

static void installAppTranslator(QCoreApplication *app)
{
    PrayerManager::installAppTranslator(app);
}

// ---------------------------------------------------------------------
// --check-and-play mode: a quick, one-shot, stateless-ish check ("is any
// enabled prayer due right now and not already played today? if so,
// play it - and separately, is a configurable pre-alert lead time
// before some prayer due, and not already alerted today? if so, play a
// short beep") triggered periodically by a SYSTEM-level systemd timer
// with WakeSystem=true (see systemd/harbour-thakir-check.timer). Runs,
// does its check, and exits immediately - no long-running process at
// all.
//
// This replaces an earlier design: a long-running --daemon process
// holding an MCE "cpu keepalive" (com.nokia.mce.request /
// req_cpu_keepalive_start) for the whole wait between prayers, renewed
// periodically. That approach worked for waits up to ~74 minutes in
// testing, but a real overnight test showed the keepalive renewals
// themselves silently stopped firing after ~75-80 minutes, well before
// the ~6+ hour wait to the next prayer completed - strong evidence that
// something (most likely Sailfish/systemd-logind freezing the whole user
// session's process group once it's considered idle) was stopping the
// entire process, not just failing to prevent CPU sleep. cpu_keepalive
// only addresses the latter.
//
// WakeSystem=true sidesteps this entirely: it uses the hardware RTC
// alarm to wake the actual system from suspend, at a level below
// whatever software session-freezing defeated the keepalive approach.
// The cost of switching to a periodic-check design is up to ~3 minutes
// of lateness (bounded by the check interval and a matching tolerance
// window - see the WAKE_INTERVAL/TOLERANCE comment below) rather than
// exact-second timing - a reasonable trade for an athan reminder app,
// and it also happens to be far more battery-friendly than the old
// approach, since the device fully suspends between checks instead of
// staying awake for hours.
//
// This also does NOT go through the system "timed" daemon - an earlier
// attempt at that was abandoned after timed consistently rejected our
// events regardless of Sailjail sandboxing, command form, or event
// flags (see git history / project README for the full story).
// ---------------------------------------------------------------------

// How often the system-level timer wakes us (see the .timer unit,
// currently every minute) and how far past a prayer's time we still
// consider it "due". The tolerance is wider than the wake interval so a
// single missed/delayed wake doesn't silently skip a prayer - several
// consecutive wakes still fall inside a 3 minute tolerance window.
// (Was 5 min / 10 min originally - tightened for closer-to-exact timing
// once the underlying mechanism was confirmed reliable via a real
// fajr-to-dhuhr overnight test.)
static const int CHECK_TOLERANCE_SECONDS = 3 * 60;

// --- Silent-mode helpers (delegated to SilentModeHelper) ----------------
static QString getCurrentProfile()
{
    return SilentModeHelper::getCurrentProfile();
}

static void checkSilentModeExpiry(QSettings &s)
{
    SilentModeHelper::checkSilentModeExpiry(s);
}

static void maybeStartSilentMode(QSettings &s, const QString &prayer,
                                 const QDateTime &prayerDt, const QDateTime &now,
                                 const QString &todayStr)
{
    SilentModeHelper::maybeStartSilentMode(s, prayer, prayerDt, now, todayStr);
}
// -------------------------------------------------------------------------

static void runCheckAndPlayMode()
{
    QSettings s(SettingsHelper::settingsFilePath(), QSettings::IniFormat);
    SettingsHelper::ensureSanity(s);
    QString cityName = s.value("city/name").toString();
    if (cityName.isEmpty()) {
        qDebug() << "harbour-thakir --check-and-play: no city configured, nothing to do";
        return;
    }

    double lat = s.value("city/lat", 0.0).toDouble();
    double lon = s.value("city/lon", 0.0).toDouble();
    QString tzId = s.value("city/tz", "UTC").toString();
    int method = s.value("prefs/method", 0).toInt();
    int madhab = s.value("prefs/madhab", 0).toInt();
    int highLatitudeRule = s.value("prefs/highLatitudeRule", 1).toInt();

    PrayerTimes calc;
    calc.setMethod(static_cast<PrayerTimes::Method>(method));
    calc.setMadhab(static_cast<PrayerTimes::Madhab>(madhab));
    calc.setHighLatitudeRule(static_cast<PrayerTimes::HighLatitudeRule>(highLatitudeRule));
    calc.setLocation(lat, lon);

    QDate today = QDate::currentDate();
    QTimeZone tz(tzId.toUtf8());
    double tzOffset = 0.0;
    if (tz.isValid()) {
        QDateTime noon(today, QTime(12, 0, 0), tz);
        tzOffset = tz.offsetFromUtc(noon) / 3600.0;
    }
    calc.setTimezone(tzOffset);
    static const char *names[6] = {"fajr", "sunrise", "dhuhr", "asr", "maghrib", "isha"};
    for (int i = 0; i < 6; ++i) {
        int offset = s.value(QStringLiteral("adjustments/%1").arg(names[i]), 0).toInt();
        calc.setPrayerOffset(i, offset);
    }

    PrayerTimes::Times t = calc.computeForDate(today);
    if (!t.valid) {
        qWarning() << "harbour-thakir --check-and-play: could not compute prayer times";
        return;
    }

    double values[6] = {t.fajr, t.sunrise, t.dhuhr, t.asr, t.maghrib, t.isha};

    QDateTime now = QDateTime::currentDateTime();
    QString todayStr = today.toString(Qt::ISODate);

    qDebug() << "harbour-thakir --check-and-play: checking at" << now;

    bool respectSilentMode = s.value("respectSilentMode", true).toBool();

    checkSilentModeExpiry(s);

    for (int i = 0; i < 6; ++i) {
        QString p = names[i];
        QString hm = PrayerTimes::formatTime(values[i]);
        QDateTime prayerDt(today, QTime::fromString(hm, "HH:mm"));

        maybeStartSilentMode(s, p, prayerDt, now, todayStr);

        bool enabled = s.value(QString("enabled/%1").arg(p), p != QStringLiteral("sunrise")).toBool();
        if (!enabled) {
            qDebug() << "  " << p << "- athan disabled, skipping sound";
            continue;
        }

        qint64 secsSincePrayer = prayerDt.secsTo(now);
        bool due = secsSincePrayer >= 0 && secsSincePrayer <= CHECK_TOLERANCE_SECONDS;

        QString playedKey = QString("played/%1").arg(p);
        bool alreadyPlayedToday = (s.value(playedKey).toString() == todayStr);

        qDebug() << "  " << p << "at" << prayerDt << "- secsSincePrayer:" << secsSincePrayer
                  << "due:" << due << "alreadyPlayedToday:" << alreadyPlayedToday;

        // Pre-alert: a short "heads up" beep some number of minutes
        // before the prayer, e.g. "10 minutes left". 0 (or the prayer
        // itself being disabled) means no pre-alert for it.
        // Sunrise does NOT have a pre-alert.
        if (p != QStringLiteral("sunrise")) {
            int preAlertMinutes = s.value(QString("prealert/%1").arg(p), 10).toInt();
            if (preAlertMinutes > 0) {
                QDateTime preAlertDt = prayerDt.addSecs(-preAlertMinutes * 60);
                qint64 secsSincePreAlert = preAlertDt.secsTo(now);
                bool preAlertDue = secsSincePreAlert >= 0 && secsSincePreAlert <= CHECK_TOLERANCE_SECONDS;

                QString preAlertedKey = QString("prealerted/%1").arg(p);
                bool alreadyPreAlertedToday = (s.value(preAlertedKey).toString() == todayStr);

                if (preAlertDue && !alreadyPreAlertedToday) {
                    QString currentProfile = respectSilentMode ? getCurrentProfile() : QString();
                    if (respectSilentMode && currentProfile == QStringLiteral("silent")) {
                        qDebug() << "harbour-thakir --check-and-play: phone is in silent mode,"
                                  << "skipping pre-alert for" << p;
                        s.setValue(preAlertedKey, todayStr);
                    } else {
                        QString bipPath = "/usr/share/harbour-thakir/sounds/bip.ogg";
                        qDebug() << "harbour-thakir --check-and-play: playing pre-alert for" << p
                                  << "(" << preAlertMinutes << "min before,"
                                  << secsSincePreAlert << "s after alert time )";
                        int rc = QProcess::execute("/usr/bin/paplay", {bipPath});
                        if (rc != 0) {
                            qWarning() << "harbour-thakir --check-and-play: paplay failed for"
                                      << p << "pre-alert (exit" << rc << ") - will retry";
                        } else {
                            s.setValue(preAlertedKey, todayStr);
                        }
                    }
                }
            }
        }

        if (due && !alreadyPlayedToday) {
            QString currentProfile = respectSilentMode ? getCurrentProfile() : QString();
            if (respectSilentMode && currentProfile == QStringLiteral("silent")) {
                qDebug() << "harbour-thakir --check-and-play: phone is in silent mode,"
                          << "skipping playback for" << p;
                s.setValue(playedKey, todayStr);
            } else {
                // Sunrise and Friday Dhuhr (jumu'ah time) use the short bip instead of the full athan
                bool isSunrise = (p == QStringLiteral("sunrise"));
                bool fridayDhuhr = (p == QStringLiteral("dhuhr")) && (today.dayOfWeek() == 5);
                QString soundPath = (isSunrise || fridayDhuhr)
                        ? QStringLiteral("/usr/share/harbour-thakir/sounds/bip.ogg")
                        : s.value(QString("sound/%1").arg(p),
                                   "/usr/share/harbour-thakir/sounds/adhan_court.ogg").toString();
                qDebug() << "harbour-thakir --check-and-play: playing"
                          << (isSunrise ? "bip (Sunrise)" : (fridayDhuhr ? "bip (Friday Dhuhr)" : "athan"))
                          << "for" << p << soundPath
                          << "(" << secsSincePrayer << "s after scheduled time )";

                bool stopWithPower = s.value(QStringLiteral("stopWithPowerButton"), true).toBool();
                bool showNotification = s.value(QStringLiteral("showNotification"), true).toBool();

                QProcess paplay;
                paplay.start(QStringLiteral("/usr/bin/paplay"), {soundPath});
                if (!paplay.waitForStarted(3000)) {
                    qWarning() << "harbour-thakir --check-and-play: paplay failed to start for" << p
                              << "(error:" << paplay.errorString() << ") - will retry";
                } else {
                    bool stoppedByPower = false;
                    bool stoppedByNotification = false;
                    QEventLoop loop;
                    PowerButtonWatcher *watcher = nullptr;
                    AthanNotification *notification = nullptr;

                    if (showNotification) {
                        notification = new AthanNotification(p, &loop);
                        notification->show();
                        QObject::connect(notification, &AthanNotification::stopRequested, &loop, [&]() {
                            qDebug() << "harbour-thakir --check-and-play: notification stop requested for" << p;
                            stoppedByNotification = true;
                            paplay.terminate();
                            if (!paplay.waitForFinished(1000)) {
                                paplay.kill();
                            }
                            loop.quit();
                        });
                    }

                    if (stopWithPower) {
                        watcher = new PowerButtonWatcher(&loop);
                        QObject::connect(watcher, &PowerButtonWatcher::powerButtonPressed, &loop, [&]() {
                            qDebug() << "harbour-thakir --check-and-play: power button pressed! Stopping athan for" << p;
                            stoppedByPower = true;
                            paplay.terminate();
                            if (!paplay.waitForFinished(1000)) {
                                paplay.kill();
                            }
                            loop.quit();
                        });
                    }

                    QObject::connect(&paplay, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                                     &loop, &QEventLoop::quit);

                    loop.exec();

                    if (notification) {
                        notification->close();
                    }

                    if (stoppedByPower || stoppedByNotification || (paplay.exitStatus() == QProcess::NormalExit && paplay.exitCode() == 0)) {
                        s.setValue(playedKey, todayStr);
                    } else {
                        qWarning() << "harbour-thakir --check-and-play: paplay failed for" << p
                                  << "(exit" << paplay.exitCode() << ") - NOT marking as played, will retry";
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------
// --play <prayer> mode: on-demand/manual playback for testing, with an
// on-screen Stop control. --check-and-play does not use this path - it
// calls paplay directly - this is purely for previewing from the GUI or
// a terminal.
// ---------------------------------------------------------------------
static int runPlayMode(QGuiApplication &app, const QString &prayer)
{
    // Deliberately does NOT apply the Friday-Dhuhr-uses-bip rule that
    // --check-and-play does - this mode is for previewing/testing a
    // prayer's assigned sound on demand, so always plays that sound
    // regardless of what day it happens to be run on.
    PrayerManager manager;
    QString soundPath = manager.audioPathFor(prayer);

    auto *player = new QMediaPlayer(&app);
    player->setMedia(QUrl::fromLocalFile(soundPath));
    player->setVolume(100);

    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, &app,
        [&app](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia) {
                app.quit();
            }
        });

    player->play();

    // Safety timeout in case audio playback state never resolves, or the
    // person never taps Stop - do not hang around indefinitely.
    QTimer::singleShot(180000, &app, &QCoreApplication::quit);

    // On-screen Stop control - see qml/pages/StopPage.qml. Tapping Stop
    // (or just leaving/closing this screen) silences it immediately,
    // rather than only the 3-minute safety timeout above.
    PlaybackController playback(prayer, player, &app);
    QObject::connect(&playback, &PlaybackController::stopped, &app, &QCoreApplication::quit);

    // Notification stop support:
    bool showNotification = SettingsHelper::value(QStringLiteral("showNotification"), true).toBool();
    AthanNotification *notification = nullptr;
    if (showNotification) {
        notification = new AthanNotification(prayer, &app);
        notification->show();
        QObject::connect(notification, &AthanNotification::stopRequested, &playback, &PlaybackController::stop);
    }

    // Power button stop support:
    bool stopWithPower = SettingsHelper::value(QStringLiteral("stopWithPowerButton"), true).toBool();
    if (stopWithPower) {
        auto *watcher = new PowerButtonWatcher(&app);
        QObject::connect(watcher, &PowerButtonWatcher::powerButtonPressed, &playback, &PlaybackController::stop);
    }

    QQuickView *view = SailfishApp::createView();
    view->rootContext()->setContextProperty("playback", &playback);
    view->setSource(SailfishApp::pathTo("qml/pages/StopPage.qml"));
    view->show();

    return app.exec();
}

int main(int argc, char *argv[])
{
    QStringList rawArgs;
    for (int i = 1; i < argc; ++i) rawArgs << QString::fromLocal8Bit(argv[i]);

    if (rawArgs.contains("--check-and-play")) {
        // Deliberately does not call app.exec() - this is a one-shot
        // synchronous check-and-exit, triggered periodically by the
        // WakeSystem timer. A QCoreApplication instance is still needed
        // for QProcess to work, just not its event loop.
        QCoreApplication app(argc, argv);
        app.setOrganizationName("org.hafsoftdz");
        app.setApplicationName("harbour-thakir");
        runCheckAndPlayMode();
        return 0;
    }

    int playIdx = rawArgs.indexOf("--play");
    if (playIdx >= 0 && playIdx + 1 < rawArgs.size()) {
        QGuiApplication *app = SailfishApp::application(argc, argv);
        app->setOrganizationName("org.hafsoftdz");
        app->setApplicationName("harbour-thakir");
        installAppTranslator(app);
        return runPlayMode(*app, rawArgs.at(playIdx + 1));
    }

    // Normal GUI startup.
    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("org.hafsoftdz");
    app->setApplicationName("harbour-thakir");
    installAppTranslator(app);

    QQuickView *view = SailfishApp::createView();

    PrayerManager manager;
    manager.setQmlEngine(view->engine());
    view->rootContext()->setContextProperty("prayerManager", &manager);

    view->setSource(SailfishApp::pathTo("qml/harbour-thakir.qml"));
    view->show();

    return app->exec();
}
