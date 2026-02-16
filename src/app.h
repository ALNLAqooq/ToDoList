#ifndef APP_H
#define APP_H

#include <QObject>

class App : public QObject
{
    Q_OBJECT

public:
    explicit App(QObject *parent = nullptr);
    ~App();

    void init();

private:
    void initDatabase();
    void initLogger();
    void initSettings();
    void initTheme();
    void initWindow();
};

#endif // APP_H
