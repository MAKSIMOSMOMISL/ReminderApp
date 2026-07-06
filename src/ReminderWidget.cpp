#include "ReminderWidget.h"
#include <QApplication>
#include <QStyle>
#include <QCloseEvent>
#include <QGroupBox>
#include <QIcon>

ReminderWidget::ReminderWidget(QWidget *parent)
    : QWidget(parent)
    , m_manager(new ReminderManager(this))
{
    setupUI();
    setupTray();
    refreshList();

    connect(m_manager, &ReminderManager::remindersChanged, this, &ReminderWidget::refreshList);
    connect(m_manager, &ReminderManager::reminderTriggered, this, &ReminderWidget::showReminder);

    // ===== ИКОНКА ДЛЯ ОКНА ИЗ ФАЙЛА .ICO =====
    QIcon windowIcon("icon.ico");
    setWindowIcon(windowIcon);

    setWindowTitle("📋 Напоминания");
    setMinimumSize(600, 500);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
}

ReminderWidget::~ReminderWidget()
{
    delete m_trayIcon;
}

void ReminderWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* header = new QLabel("Мои напоминания");
    QFont font = header->font();
    font.setPointSize(14);
    font.setBold(true);
    header->setFont(font);
    mainLayout->addWidget(header);

    m_reminderList = new QListWidget(this);
    mainLayout->addWidget(m_reminderList, 1);

    QGroupBox* inputGroup = new QGroupBox("Добавить напоминание");
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("Название события...");
    inputLayout->addWidget(m_titleEdit);

    m_descEdit = new QTextEdit(this);
    m_descEdit->setPlaceholderText("Описание...");
    m_descEdit->setMaximumHeight(60);
    inputLayout->addWidget(m_descEdit);

    QHBoxLayout* bottomLayout = new QHBoxLayout();

    m_datetimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(60), this);
    m_datetimeEdit->setCalendarPopup(true);
    bottomLayout->addWidget(m_datetimeEdit);

    m_addBtn = new QPushButton("➕ Добавить", this);
    connect(m_addBtn, &QPushButton::clicked, this, &ReminderWidget::addReminder);
    bottomLayout->addWidget(m_addBtn);

    m_deleteBtn = new QPushButton("🗑️ Удалить", this);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ReminderWidget::deleteReminder);
    bottomLayout->addWidget(m_deleteBtn);

    inputLayout->addLayout(bottomLayout);
    mainLayout->addWidget(inputGroup);

    setStyleSheet(R"(
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QListWidget {
            background-color: #3c3c3c;
            border: 1px solid #555;
            border-radius: 5px;
            padding: 5px;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #444;
        }
        QListWidget::item:hover {
            background-color: #4a4a4a;
        }
        QLineEdit, QTextEdit, QDateTimeEdit {
            background-color: #3c3c3c;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 5px;
            color: #fff;
        }
        QPushButton {
            background-color: #4a6fa5;
            border: none;
            border-radius: 3px;
            padding: 8px 15px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #5b82b8;
        }
        QPushButton:pressed {
            background-color: #3a5a8a;
        }
        QGroupBox {
            border: 1px solid #555;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
    )");
}

void ReminderWidget::setupTray()
{
    m_trayIcon = new QSystemTrayIcon(this);

    // ===== ИКОНКА ДЛЯ ТРЕЯ ИЗ ФАЙЛА .ICO =====
    QIcon icon("icon.ico");
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip("Напоминания");

    m_trayMenu = new QMenu(this);
    QAction* showAction = new QAction("Показать", this);
    QAction* quitAction = new QAction("Выход", this);

    connect(showAction, &QAction::triggered, this, &QWidget::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayMenu->addAction(showAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &ReminderWidget::trayActivated);
}

void ReminderWidget::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        showNormal();
        raise();
        activateWindow();
    }
}

void ReminderWidget::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon->isVisible()) {
        hide();
        event->ignore();
        m_trayIcon->showMessage(
            "Напоминания",
            "Приложение свернуто в трей. Дважды кликните по иконке для открытия.",
            QSystemTrayIcon::Information,
            2000
        );
    } else {
        event->accept();
    }
}

void ReminderWidget::addReminder()
{
    QString title = m_titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название события!");
        return;
    }

    QString description = m_descEdit->toPlainText().trimmed();
    QDateTime datetime = m_datetimeEdit->dateTime();

    if (datetime <= QDateTime::currentDateTime()) {
        QMessageBox::warning(this, "Ошибка", "Дата и время должны быть в будущем!");
        return;
    }

    m_manager->addReminder(title, description, datetime);

    m_titleEdit->clear();
    m_descEdit->clear();
    m_datetimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(60));

    refreshList();
}

void ReminderWidget::deleteReminder()
{
    int currentRow = m_reminderList->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "Информация", "Выберите напоминание для удаления");
        return;
    }

    QList<Reminder> reminders = m_manager->getAllReminders();
    if (currentRow < reminders.size()) {
        m_manager->removeReminder(reminders[currentRow].id);
        refreshList();
    }
}

void ReminderWidget::refreshList()
{
    updateList();
}

void ReminderWidget::updateList()
{
    m_reminderList->clear();
    QList<Reminder> reminders = m_manager->getAllReminders();

    std::sort(reminders.begin(), reminders.end(),
        [](const Reminder& a, const Reminder& b) {
            return a.datetime < b.datetime;
        });

    for (const auto& r : reminders) {
        if (r.isActive) {
            QListWidgetItem* item = new QListWidgetItem(formatReminder(r));
            item->setData(Qt::UserRole, r.id);
            m_reminderList->addItem(item);
        }
    }
}

QString ReminderWidget::formatReminder(const Reminder& r) const
{
    QString status = r.isNotified ? "✅" : "⏳";
    return QString("%1 %2\n   📅 %3\n   📝 %4")
        .arg(status)
        .arg(r.title)
        .arg(r.datetime.toString("dd.MM.yyyy HH:mm"))
        .arg(r.description);
}

void ReminderWidget::showReminder(const Reminder& reminder)
{
    refreshList();
}
