#include "ReminderManager.h"
#include <QApplication>
#include <QMessageBox>
#include <QWindow>
#include <QScreen>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winuser.h>
#endif

ReminderManager::ReminderManager(QObject *parent)
    : QObject(parent)
{
    m_savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/reminders.json";

    QFileInfo info(m_savePath);
    QDir().mkpath(info.absolutePath());

    loadReminders();

    m_checkTimer = new QTimer(this);
    m_checkTimer->setInterval(10000);
    connect(m_checkTimer, &QTimer::timeout, this, &ReminderManager::checkReminders);
    m_checkTimer->start();
}

ReminderManager::~ReminderManager()
{
    saveReminders();
}

void ReminderManager::addReminder(const QString& title, const QString& description, const QDateTime& datetime)
{
    Reminder r;
    r.id = m_nextId++;
    r.title = title;
    r.description = description;
    r.datetime = datetime;
    r.isActive = true;
    r.isNotified = false;

    m_reminders[r.id] = r;
    saveReminders();
    emit remindersChanged();

    qDebug() << "Added reminder:" << title << "at" << datetime;
}

void ReminderManager::removeReminder(int id)
{
    if (m_reminders.contains(id)) {
        m_reminders.remove(id);
        saveReminders();
        emit remindersChanged();
    }
}

QList<Reminder> ReminderManager::getAllReminders() const
{
    return m_reminders.values();
}

void ReminderManager::checkReminders()
{
    QDateTime now = QDateTime::currentDateTime();

    for (auto& reminder : m_reminders) {
        if (reminder.isActive && !reminder.isNotified) {
            qint64 diff = now.secsTo(reminder.datetime);
            if (diff >= -10 && diff <= 5) {
                minimizeFullscreenApps();
                showNotification(reminder);

                reminder.isNotified = true;
                saveReminders();
                emit reminderTriggered(reminder);
            }
        }
    }
}

void ReminderManager::minimizeFullscreenApps()
{
#ifdef Q_OS_WIN
    HWND hWnd = GetForegroundWindow();
    if (hWnd) {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        if (rect.left == 0 && rect.top == 0 &&
            rect.right == screenWidth && rect.bottom == screenHeight) {
            ShowWindow(hWnd, SW_MINIMIZE);
        }
    }
#elif defined(Q_OS_LINUX)
    QProcess::execute("xdotool", {"getactivewindow", "windowminimize"});
#elif defined(Q_OS_MAC)
    QProcess::execute("osascript", {"-e", "tell application \"System Events\" to keystroke \"m\" using command down"});
#endif
}

void ReminderManager::showNotification(const Reminder& reminder)
{
    QString message = QString("📅 %1\n%2\n%3")
                          .arg(reminder.title)
                          .arg(reminder.description)
                          .arg(reminder.datetime.toString("dd.MM.yyyy HH:mm"));

    QMessageBox msgBox;
    msgBox.setWindowTitle("Напоминание");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
}

void ReminderManager::saveReminders()
{
    QJsonArray jsonArray;
    for (const auto& reminder : m_reminders) {
        jsonArray.append(reminder.toJson());
    }

    QJsonDocument doc(jsonArray);
    QFile file(m_savePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void ReminderManager::loadReminders()
{
    QFile file(m_savePath);
    if (!file.exists()) return;

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            for (const auto& value : jsonArray) {
                if (value.isObject()) {
                    Reminder r = Reminder::fromJson(value.toObject());
                    m_reminders[r.id] = r;
                    if (r.id >= m_nextId) {
                        m_nextId = r.id + 1;
                    }
                }
            }
        }
    }
}