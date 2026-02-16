#include <QApplication>
#include "app.h"
#include "utils/logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("ToDoList");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("GoodIdea");

    App appController;
    appController.init();

    return app.exec();
}
