#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void setupLayout();
    void loadSettings();
    void saveSettings();

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_contentLayout;
    QLabel *m_placeholderLabel;
};

#endif // MAINWINDOW_H
