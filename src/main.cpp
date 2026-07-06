#include <QApplication>
#include <QIcon>
#include "ReminderWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Напоминания");
    app.setOrganizationName("YourCompany");
    app.setQuitOnLastWindowClosed(false);

    // QIcon icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    // app.setWindowIcon(icon);

    ReminderWidget widget;
    widget.show();

    return app.exec();
}