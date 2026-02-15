#include <QApplication>
#include "app/app.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    App todoApp;
    todoApp.init();
    return app.exec();
}
