#ifndef POWERBUTTONWATCHER_H
#define POWERBUTTONWATCHER_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>

class PowerButtonWatcher : public QObject
{
    Q_OBJECT
public:
    explicit PowerButtonWatcher(QObject *parent = nullptr)
        : QObject(parent), m_initialDisplayState(getDisplayStatus())
    {
        QDBusConnection sysBus = QDBusConnection::systemBus();
        if (!sysBus.isConnected()) {
            qWarning() << "PowerButtonWatcher: System D-Bus is not connected!";
        } else {
            bool ok1 = sysBus.connect(QStringLiteral("com.nokia.mce"),
                                      QStringLiteral("/com/nokia/mce/signal"),
                                      QStringLiteral("com.nokia.mce.signal"),
                                      QStringLiteral("display_status_ind"),
                                      this,
                                      SLOT(onDisplayStatusInd(QString)));

            bool ok2 = sysBus.connect(QStringLiteral("com.nokia.mce"),
                                      QStringLiteral("/com/nokia/mce/signal"),
                                      QStringLiteral("com.nokia.mce.signal"),
                                      QStringLiteral("alarm_ui_feedback_ind"),
                                      this,
                                      SLOT(onAlarmUiFeedbackInd(QString)));

            qDebug() << "PowerButtonWatcher: Initialized (initial display:"
                     << m_initialDisplayState << ", signal connections:" << ok1 << ok2 << ")";
        }
    }

    static QString getDisplayStatus()
    {
        QDBusConnection sysBus = QDBusConnection::systemBus();
        if (!sysBus.isConnected())
            return QStringLiteral("off");

        QDBusInterface mce(QStringLiteral("com.nokia.mce"),
                           QStringLiteral("/com/nokia/mce/request"),
                           QStringLiteral("com.nokia.mce.request"),
                           sysBus);
        if (mce.isValid()) {
            QDBusMessage reply = mce.call(QStringLiteral("get_display_status"));
            if (reply.type() != QDBusMessage::ErrorMessage && !reply.arguments().isEmpty()) {
                return reply.arguments().at(0).toString();
            }
        }
        return QStringLiteral("off");
    }

signals:
    void powerButtonPressed();

public slots:
    void onDisplayStatusInd(const QString &state)
    {
        qDebug() << "PowerButtonWatcher: display_status_ind =" << state
                 << "(initial was:" << m_initialDisplayState << ")";
        if (state != m_initialDisplayState) {
            emit powerButtonPressed();
        }
    }

    void onAlarmUiFeedbackInd(const QString &event)
    {
        qDebug() << "PowerButtonWatcher: alarm_ui_feedback_ind =" << event;
        if (event == QStringLiteral("powerkey") || event == QStringLiteral("flipover")) {
            emit powerButtonPressed();
        }
    }

private:
    QString m_initialDisplayState;
};

#endif // POWERBUTTONWATCHER_H
