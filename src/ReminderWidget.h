#ifndef REMINDERWIDGET_H
#define REMINDERWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QListWidget>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "ReminderManager.h"

class ReminderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReminderWidget(QWidget *parent = nullptr);
    ~ReminderWidget();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addReminder();
    void deleteReminder();
    void refreshList();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void showReminder(const Reminder& reminder);  // Это имя должно совпадать с .cpp

private:
    ReminderManager* m_manager;

    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayMenu;

    QListWidget* m_reminderList;
    QLineEdit* m_titleEdit;
    QTextEdit* m_descEdit;
    QDateTimeEdit* m_datetimeEdit;
    QPushButton* m_addBtn;
    QPushButton* m_deleteBtn;

    void setupUI();
    void setupTray();
    void updateList();
    QString formatReminder(const Reminder& r) const;
};

#endif // REMINDERWIDGET_H