#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QMap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

// Forward declaration
struct Reminder;

class ReminderManager : public QObject
{
    Q_OBJECT

public:
    explicit ReminderManager(QObject *parent = nullptr);
    ~ReminderManager();

    void addReminder(const QString& title, const QString& description, const QDateTime& datetime);
    void removeReminder(int id);
    QList<Reminder> getAllReminders() const;
    void saveReminders();
    void loadReminders();

private slots:
    void checkReminders();

private:
    QMap<int, Reminder> m_reminders;
    QTimer* m_checkTimer;
    int m_nextId = 1;
    QString m_savePath;

    void minimizeFullscreenApps();
    void showNotification(const Reminder& reminder);

signals:
    void reminderTriggered(const Reminder& reminder);
    void remindersChanged();
};

// Определение структуры Reminder
struct Reminder {
    int id;
    QString title;
    QString description;
    QDateTime datetime;
    bool isActive;
    bool isNotified;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["title"] = title;
        obj["description"] = description;
        obj["datetime"] = datetime.toString(Qt::ISODate);
        obj["isActive"] = isActive;
        obj["isNotified"] = isNotified;
        return obj;
    }

    static Reminder fromJson(const QJsonObject& obj) {
        Reminder r;
        r.id = obj["id"].toInt();
        r.title = obj["title"].toString();
        r.description = obj["description"].toString();
        r.datetime = QDateTime::fromString(obj["datetime"].toString(), Qt::ISODate);
        r.isActive = obj["isActive"].toBool();
        r.isNotified = obj["isNotified"].toBool();
        return r;
    }
};

#endif // REMINDERMANAGER_H