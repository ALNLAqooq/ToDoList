#ifndef APP_H
#define APP_H

#include <QObject>

class QTimer;

class App : public QObject
{
    Q_OBJECT

public:
    explicit App(QObject *parent = nullptr);
    ~App();

    void init();

private:
    bool initDatabase();
    void initLogger();
    void initSettings();
    void initTheme();
    void initWindow();
    void runMaintenance();
    void scheduleMaintenance();

    QTimer *m_maintenanceTimer;
};

#endif // APP_H
