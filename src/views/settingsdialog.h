#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class BackupManager;
class Database;
class QTabWidget;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QTimeEdit;
class QLineEdit;
class QPushButton;
class QLabel;
class QTableWidget;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(BackupManager *backupManager, QWidget *parent = nullptr);

signals:
    void dataImported();

private slots:
    void onBrowseBackupLocation();
    void onManualBackup();
    void onOpenDatabaseLocation();
    void onExportJson();
    void onImportJson();
    void onExportSqlite();
    void onImportSqlite();
    void onClearCache();
    void onResetShortcuts();
    void onCheckUpdates();
    void onFeedback();
    void onOpenGitHub();
    void onSave();

private:
    void setupUI();
    QWidget *buildGeneralTab();
    QWidget *buildAppearanceTab();
    QWidget *buildNotificationTab();
    QWidget *buildBackupTab();
    QWidget *buildDataTab();
    QWidget *buildDeleteTab();
    QWidget *buildShortcutsTab();
    QWidget *buildAboutTab();

    void loadSettings();
    bool applySettings();
    QString getSetting(const QString &key, const QString &defaultValue) const;
    bool setSetting(const QString &key, const QString &value);

    BackupManager *m_backupManager;
    Database *m_database;

    QTabWidget *m_tabs;

    QComboBox *m_themeCombo;
    QComboBox *m_languageCombo;
    QCheckBox *m_autoLaunchCheck;
    QCheckBox *m_restoreLastCheck;

    QComboBox *m_cardStyleCombo;
    QSpinBox *m_fontSizeSpin;
    QComboBox *m_iconStyleCombo;
    QSpinBox *m_cornerRadiusSpin;

    QCheckBox *m_notificationsEnabledCheck;
    QCheckBox *m_remindersCheck;
    QCheckBox *m_systemNotifyCheck;
    QCheckBox *m_soundCheck;

    QCheckBox *m_backupEnabledCheck;
    QComboBox *m_backupFrequencyCombo;
    QTimeEdit *m_backupTimeEdit;
    QSpinBox *m_backupRetentionSpin;
    QLineEdit *m_backupLocationEdit;
    QCheckBox *m_backupOnExitCheck;
    QPushButton *m_backupNowButton;

    QLineEdit *m_databasePathEdit;

    QComboBox *m_parentDeleteCombo;
    QCheckBox *m_autoCleanupCheck;
    QSpinBox *m_cleanupDaysSpin;

    QTableWidget *m_shortcutsTable;
};

#endif // SETTINGSDIALOG_H
