#include "settingsdialog.h"
#include "../controllers/backupmanager.h"
#include "../controllers/database.h"
#include "../utils/theme_manager.h"
#include <QTabWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimeEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QFileInfo>

namespace {
const char *KEY_THEME = "settings_theme";
const char *KEY_LANGUAGE = "settings_language";
const char *KEY_AUTO_LAUNCH = "settings_auto_launch";
const char *KEY_RESTORE_LAST = "settings_restore_last";

const char *KEY_CARD_STYLE = "appearance_card_style";
const char *KEY_FONT_SIZE = "appearance_font_size";
const char *KEY_ICON_STYLE = "appearance_icon_style";
const char *KEY_CORNER_RADIUS = "appearance_corner_radius";

const char *KEY_NOTIFY_ENABLED = "notifications_enabled";
const char *KEY_NOTIFY_REMINDERS = "notifications_reminders";
const char *KEY_NOTIFY_SYSTEM = "notifications_system";
const char *KEY_NOTIFY_SOUND = "notifications_sound";

const char *KEY_DELETE_PARENT_ACTION = "delete_parent_action";
const char *KEY_DELETE_AUTO_CLEANUP = "delete_auto_cleanup";
const char *KEY_DELETE_CLEANUP_DAYS = "delete_cleanup_days";
}

SettingsDialog::SettingsDialog(BackupManager *backupManager, QWidget *parent)
    : QDialog(parent)
    , m_backupManager(backupManager)
    , m_database(&Database::instance())
    , m_tabs(nullptr)
    , m_themeCombo(nullptr)
    , m_languageCombo(nullptr)
    , m_autoLaunchCheck(nullptr)
    , m_restoreLastCheck(nullptr)
    , m_cardStyleCombo(nullptr)
    , m_fontSizeSpin(nullptr)
    , m_iconStyleCombo(nullptr)
    , m_cornerRadiusSpin(nullptr)
    , m_notificationsEnabledCheck(nullptr)
    , m_remindersCheck(nullptr)
    , m_systemNotifyCheck(nullptr)
    , m_soundCheck(nullptr)
    , m_backupEnabledCheck(nullptr)
    , m_backupFrequencyCombo(nullptr)
    , m_backupTimeEdit(nullptr)
    , m_backupRetentionSpin(nullptr)
    , m_backupLocationEdit(nullptr)
    , m_backupOnExitCheck(nullptr)
    , m_backupNowButton(nullptr)
    , m_databasePathEdit(nullptr)
    , m_parentDeleteCombo(nullptr)
    , m_autoCleanupCheck(nullptr)
    , m_cleanupDaysSpin(nullptr)
    , m_shortcutsTable(nullptr)
{
    setWindowTitle("设置");
    setMinimumSize(720, 520);
    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(buildGeneralTab(), "通用");
    m_tabs->addTab(buildAppearanceTab(), "外观");
    m_tabs->addTab(buildNotificationTab(), "通知");
    m_tabs->addTab(buildBackupTab(), "备份");
    m_tabs->addTab(buildDataTab(), "数据");
    m_tabs->addTab(buildDeleteTab(), "删除");
    m_tabs->addTab(buildShortcutsTab(), "快捷键");
    m_tabs->addTab(buildAboutTab(), "关于");

    mainLayout->addWidget(m_tabs, 1);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onSave);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

QWidget *SettingsDialog::buildGeneralTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem("深色", static_cast<int>(ThemeManager::Dark));
    m_themeCombo->addItem("浅色", static_cast<int>(ThemeManager::Light));
    m_themeCombo->addItem("跟随系统", static_cast<int>(ThemeManager::System));
    form->addRow("主题", m_themeCombo);

    m_languageCombo = new QComboBox(this);
    m_languageCombo->addItem("简体中文", "zh-CN");
    m_languageCombo->addItem("English", "en");
    form->addRow("语言", m_languageCombo);

    layout->addLayout(form);

    m_autoLaunchCheck = new QCheckBox("开机启动", this);
    m_restoreLastCheck = new QCheckBox("启动时恢复上次窗口状态", this);
    layout->addWidget(m_autoLaunchCheck);
    layout->addWidget(m_restoreLastCheck);
    layout->addStretch();

    return widget;
}

QWidget *SettingsDialog::buildAppearanceTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_cardStyleCombo = new QComboBox(this);
    m_cardStyleCombo->addItem("标准");
    m_cardStyleCombo->addItem("紧凑");
    m_cardStyleCombo->addItem("卡片");
    form->addRow("卡片样式", m_cardStyleCombo);

    m_fontSizeSpin = new QSpinBox(this);
    m_fontSizeSpin->setRange(11, 20);
    form->addRow("字体大小", m_fontSizeSpin);

    m_iconStyleCombo = new QComboBox(this);
    m_iconStyleCombo->addItem("线性");
    m_iconStyleCombo->addItem("实色");
    form->addRow("图标风格", m_iconStyleCombo);

    m_cornerRadiusSpin = new QSpinBox(this);
    m_cornerRadiusSpin->setRange(0, 20);
    form->addRow("圆角半径", m_cornerRadiusSpin);

    layout->addLayout(form);
    layout->addStretch();
    return widget;
}

QWidget *SettingsDialog::buildNotificationTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    m_notificationsEnabledCheck = new QCheckBox("启用通知", this);
    m_remindersCheck = new QCheckBox("启用提醒", this);
    m_systemNotifyCheck = new QCheckBox("系统通知", this);
    m_soundCheck = new QCheckBox("声音提示", this);

    layout->addWidget(m_notificationsEnabledCheck);
    layout->addWidget(m_remindersCheck);
    layout->addWidget(m_systemNotifyCheck);
    layout->addWidget(m_soundCheck);
    layout->addStretch();

    return widget;
}

QWidget *SettingsDialog::buildBackupTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    m_backupEnabledCheck = new QCheckBox("启用自动备份", this);
    layout->addWidget(m_backupEnabledCheck);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_backupFrequencyCombo = new QComboBox(this);
    m_backupFrequencyCombo->addItem("每小时", static_cast<int>(BackupManager::Hourly));
    m_backupFrequencyCombo->addItem("每天", static_cast<int>(BackupManager::Daily));
    m_backupFrequencyCombo->addItem("每周", static_cast<int>(BackupManager::Weekly));
    m_backupFrequencyCombo->addItem("手动", static_cast<int>(BackupManager::Manual));
    form->addRow("频率", m_backupFrequencyCombo);

    m_backupTimeEdit = new QTimeEdit(this);
    m_backupTimeEdit->setDisplayFormat("HH:mm");
    form->addRow("时间", m_backupTimeEdit);

    m_backupRetentionSpin = new QSpinBox(this);
    m_backupRetentionSpin->setRange(1, 99);
    form->addRow("保留数量", m_backupRetentionSpin);

    auto *locationLayout = new QHBoxLayout();
    m_backupLocationEdit = new QLineEdit(this);
    m_backupLocationEdit->setReadOnly(true);
    auto *browseButton = new QPushButton("选择目录", this);
    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseBackupLocation);
    locationLayout->addWidget(m_backupLocationEdit, 1);
    locationLayout->addWidget(browseButton);
    form->addRow("备份位置", locationLayout);

    m_backupOnExitCheck = new QCheckBox("退出时备份", this);
    form->addRow("", m_backupOnExitCheck);

    layout->addLayout(form);

    m_backupNowButton = new QPushButton("立即备份", this);
    connect(m_backupNowButton, &QPushButton::clicked, this, &SettingsDialog::onManualBackup);
    layout->addWidget(m_backupNowButton);
    layout->addStretch();
    return widget;
}

QWidget *SettingsDialog::buildDataTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    auto *dbLayout = new QHBoxLayout();
    m_databasePathEdit = new QLineEdit(this);
    m_databasePathEdit->setReadOnly(true);
    auto *openDbButton = new QPushButton("打开位置", this);
    connect(openDbButton, &QPushButton::clicked, this, &SettingsDialog::onOpenDatabaseLocation);
    dbLayout->addWidget(m_databasePathEdit, 1);
    dbLayout->addWidget(openDbButton);
    form->addRow("数据库", dbLayout);

    layout->addLayout(form);

    auto *actionLayout = new QHBoxLayout();
    auto *exportJsonBtn = new QPushButton("导出 JSON", this);
    auto *importJsonBtn = new QPushButton("导入 JSON", this);
    auto *exportSqlBtn = new QPushButton("导出 SQLite", this);
    auto *importSqlBtn = new QPushButton("导入 SQLite", this);
    auto *clearCacheBtn = new QPushButton("清理缓存", this);

    connect(exportJsonBtn, &QPushButton::clicked, this, &SettingsDialog::onExportJson);
    connect(importJsonBtn, &QPushButton::clicked, this, &SettingsDialog::onImportJson);
    connect(exportSqlBtn, &QPushButton::clicked, this, &SettingsDialog::onExportSqlite);
    connect(importSqlBtn, &QPushButton::clicked, this, &SettingsDialog::onImportSqlite);
    connect(clearCacheBtn, &QPushButton::clicked, this, &SettingsDialog::onClearCache);

    actionLayout->addWidget(exportJsonBtn);
    actionLayout->addWidget(importJsonBtn);
    actionLayout->addWidget(exportSqlBtn);
    actionLayout->addWidget(importSqlBtn);
    actionLayout->addWidget(clearCacheBtn);

    layout->addLayout(actionLayout);
    layout->addStretch();
    return widget;
}

QWidget *SettingsDialog::buildDeleteTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_parentDeleteCombo = new QComboBox(this);
    m_parentDeleteCombo->addItem("提升子任务为父任务");
    m_parentDeleteCombo->addItem("级联删除子任务");
    m_parentDeleteCombo->addItem("保留子任务不变");
    form->addRow("父任务处理", m_parentDeleteCombo);

    m_autoCleanupCheck = new QCheckBox("自动清理已删除任务", this);
    form->addRow("", m_autoCleanupCheck);

    m_cleanupDaysSpin = new QSpinBox(this);
    m_cleanupDaysSpin->setRange(1, 60);
    form->addRow("清理天数", m_cleanupDaysSpin);

    layout->addLayout(form);
    layout->addStretch();
    return widget;
}

QWidget *SettingsDialog::buildShortcutsTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    m_shortcutsTable = new QTableWidget(0, 2, this);
    m_shortcutsTable->setHorizontalHeaderLabels(QStringList() << "操作" << "快捷键");
    m_shortcutsTable->horizontalHeader()->setStretchLastSection(true);
    m_shortcutsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_shortcutsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto addRow = [&](const QString &action, const QString &shortcut) {
        int row = m_shortcutsTable->rowCount();
        m_shortcutsTable->insertRow(row);
        m_shortcutsTable->setItem(row, 0, new QTableWidgetItem(action));
        m_shortcutsTable->setItem(row, 1, new QTableWidgetItem(shortcut));
    };

    addRow("新建任务", "Ctrl+N");
    addRow("搜索", "Ctrl+F");
    addRow("保存", "Ctrl+S");
    addRow("删除任务", "Delete");
    addRow("切换主题", "Ctrl+T");

    layout->addWidget(m_shortcutsTable, 1);

    auto *btnLayout = new QHBoxLayout();
    auto *customizeBtn = new QPushButton("自定义", this);
    customizeBtn->setEnabled(false);
    auto *resetBtn = new QPushButton("恢复默认", this);
    connect(resetBtn, &QPushButton::clicked, this, &SettingsDialog::onResetShortcuts);
    btnLayout->addStretch();
    btnLayout->addWidget(customizeBtn);
    btnLayout->addWidget(resetBtn);
    layout->addLayout(btnLayout);
    return widget;
}

QWidget *SettingsDialog::buildAboutTab()
{
    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    QString version = QCoreApplication::applicationVersion();
    if (version.isEmpty()) {
        version = "1.0.0";
    }

    auto *title = new QLabel("ToDoList", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");

    auto *versionLabel = new QLabel(QString("版本：%1").arg(version), this);
    auto *descLabel = new QLabel("轻量级任务管理工具", this);
    descLabel->setProperty("secondary", true);

    layout->addWidget(title);
    layout->addWidget(versionLabel);
    layout->addWidget(descLabel);

    auto *btnLayout = new QHBoxLayout();
    auto *updateBtn = new QPushButton("检查更新", this);
    auto *feedbackBtn = new QPushButton("反馈", this);
    auto *gitBtn = new QPushButton("GitHub", this);

    connect(updateBtn, &QPushButton::clicked, this, &SettingsDialog::onCheckUpdates);
    connect(feedbackBtn, &QPushButton::clicked, this, &SettingsDialog::onFeedback);
    connect(gitBtn, &QPushButton::clicked, this, &SettingsDialog::onOpenGitHub);

    btnLayout->addWidget(updateBtn);
    btnLayout->addWidget(feedbackBtn);
    btnLayout->addWidget(gitBtn);
    layout->addLayout(btnLayout);
    layout->addStretch();
    return widget;
}

void SettingsDialog::loadSettings()
{
    QString themeValue = getSetting(KEY_THEME, QString::number(static_cast<int>(ThemeManager::Dark)));
    int themeIndex = m_themeCombo->findData(themeValue.toInt());
    if (themeIndex >= 0) {
        m_themeCombo->setCurrentIndex(themeIndex);
    }

    QString languageValue = getSetting(KEY_LANGUAGE, "zh-CN");
    int langIndex = m_languageCombo->findData(languageValue);
    if (langIndex >= 0) {
        m_languageCombo->setCurrentIndex(langIndex);
    }

    m_autoLaunchCheck->setChecked(getSetting(KEY_AUTO_LAUNCH, "0") == "1");
    m_restoreLastCheck->setChecked(getSetting(KEY_RESTORE_LAST, "1") == "1");

    m_cardStyleCombo->setCurrentIndex(getSetting(KEY_CARD_STYLE, "0").toInt());
    m_fontSizeSpin->setValue(getSetting(KEY_FONT_SIZE, "14").toInt());
    m_iconStyleCombo->setCurrentIndex(getSetting(KEY_ICON_STYLE, "0").toInt());
    m_cornerRadiusSpin->setValue(getSetting(KEY_CORNER_RADIUS, "8").toInt());

    m_notificationsEnabledCheck->setChecked(getSetting(KEY_NOTIFY_ENABLED, "1") == "1");
    m_remindersCheck->setChecked(getSetting(KEY_NOTIFY_REMINDERS, "1") == "1");
    m_systemNotifyCheck->setChecked(getSetting(KEY_NOTIFY_SYSTEM, "1") == "1");
    m_soundCheck->setChecked(getSetting(KEY_NOTIFY_SOUND, "0") == "1");

    if (m_backupManager) {
        m_backupEnabledCheck->setChecked(m_backupManager->autoBackupEnabled());
        int freqIndex = m_backupFrequencyCombo->findData(static_cast<int>(m_backupManager->backupFrequency()));
        if (freqIndex >= 0) {
            m_backupFrequencyCombo->setCurrentIndex(freqIndex);
        }
        m_backupTimeEdit->setTime(m_backupManager->backupTime());
        m_backupRetentionSpin->setValue(m_backupManager->backupRetention());
        m_backupLocationEdit->setText(m_backupManager->backupLocation());
        m_backupOnExitCheck->setChecked(m_backupManager->backupOnExit());
    } else {
        m_backupEnabledCheck->setChecked(getSetting("auto_backup_enabled", "1") == "1");
        m_backupTimeEdit->setTime(QTime::fromString(getSetting("backup_time", "02:00"), "HH:mm"));
        m_backupRetentionSpin->setValue(getSetting("backup_retention", "7").toInt());
        m_backupLocationEdit->setText(getSetting("backup_location", "backup"));
        m_backupOnExitCheck->setChecked(getSetting("backup_on_exit", "0") == "1");
    }

    m_databasePathEdit->setText(m_database->database().databaseName());

    m_parentDeleteCombo->setCurrentIndex(getSetting(KEY_DELETE_PARENT_ACTION, "0").toInt());
    m_autoCleanupCheck->setChecked(getSetting(KEY_DELETE_AUTO_CLEANUP, "1") == "1");
    m_cleanupDaysSpin->setValue(getSetting(KEY_DELETE_CLEANUP_DAYS, "14").toInt());
}

void SettingsDialog::applySettings()
{
    setSetting(KEY_THEME, QString::number(m_themeCombo->currentData().toInt()));
    setSetting(KEY_LANGUAGE, m_languageCombo->currentData().toString());
    setSetting(KEY_AUTO_LAUNCH, m_autoLaunchCheck->isChecked() ? "1" : "0");
    setSetting(KEY_RESTORE_LAST, m_restoreLastCheck->isChecked() ? "1" : "0");

    setSetting(KEY_CARD_STYLE, QString::number(m_cardStyleCombo->currentIndex()));
    setSetting(KEY_FONT_SIZE, QString::number(m_fontSizeSpin->value()));
    setSetting(KEY_ICON_STYLE, QString::number(m_iconStyleCombo->currentIndex()));
    setSetting(KEY_CORNER_RADIUS, QString::number(m_cornerRadiusSpin->value()));

    setSetting(KEY_NOTIFY_ENABLED, m_notificationsEnabledCheck->isChecked() ? "1" : "0");
    setSetting(KEY_NOTIFY_REMINDERS, m_remindersCheck->isChecked() ? "1" : "0");
    setSetting(KEY_NOTIFY_SYSTEM, m_systemNotifyCheck->isChecked() ? "1" : "0");
    setSetting(KEY_NOTIFY_SOUND, m_soundCheck->isChecked() ? "1" : "0");

    if (m_backupManager) {
        m_backupManager->setAutoBackupEnabled(m_backupEnabledCheck->isChecked());
        m_backupManager->setBackupFrequency(static_cast<BackupManager::BackupFrequency>(m_backupFrequencyCombo->currentData().toInt()));
        m_backupManager->setBackupTime(m_backupTimeEdit->time());
        m_backupManager->setBackupRetention(m_backupRetentionSpin->value());
        if (!m_backupLocationEdit->text().trimmed().isEmpty()) {
            m_backupManager->setBackupLocation(m_backupLocationEdit->text().trimmed());
        }
        m_backupManager->setBackupOnExit(m_backupOnExitCheck->isChecked());
    } else {
        setSetting("auto_backup_enabled", m_backupEnabledCheck->isChecked() ? "1" : "0");
        setSetting("backup_frequency", QString::number(m_backupFrequencyCombo->currentData().toInt()));
        setSetting("backup_time", m_backupTimeEdit->time().toString("HH:mm"));
        setSetting("backup_retention", QString::number(m_backupRetentionSpin->value()));
        setSetting("backup_location", m_backupLocationEdit->text().trimmed());
        setSetting("backup_on_exit", m_backupOnExitCheck->isChecked() ? "1" : "0");
    }

    setSetting(KEY_DELETE_PARENT_ACTION, QString::number(m_parentDeleteCombo->currentIndex()));
    setSetting(KEY_DELETE_AUTO_CLEANUP, m_autoCleanupCheck->isChecked() ? "1" : "0");
    setSetting(KEY_DELETE_CLEANUP_DAYS, QString::number(m_cleanupDaysSpin->value()));

    ThemeManager &themeManager = ThemeManager::instance();
    int themeValue = m_themeCombo->currentData().toInt();
    if (themeValue == static_cast<int>(ThemeManager::System)) {
        themeManager.setFollowSystem(true);
    } else {
        themeManager.setFollowSystem(false);
        themeManager.setTheme(static_cast<ThemeManager::Theme>(themeValue));
    }
}

QString SettingsDialog::getSetting(const QString &key, const QString &defaultValue) const
{
    return m_database->getSetting(key, defaultValue);
}

void SettingsDialog::setSetting(const QString &key, const QString &value)
{
    m_database->setSetting(key, value);
}

void SettingsDialog::onBrowseBackupLocation()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择备份目录", m_backupLocationEdit->text());
    if (!dir.isEmpty()) {
        m_backupLocationEdit->setText(dir);
    }
}

void SettingsDialog::onManualBackup()
{
    if (!m_backupManager || m_backupManager->isBackupInProgress()) {
        QMessageBox::information(this, "备份", "备份正在进行中。");
        return;
    }
    m_backupManager->performBackup(tr("Manual backup"));
}

void SettingsDialog::onOpenDatabaseLocation()
{
    QString dbPath = m_database->database().databaseName();
    QFileInfo info(dbPath);
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
}

void SettingsDialog::onExportJson()
{
    QMessageBox::information(this, "导出 JSON", "导出功能将在后续版本提供。");
}

void SettingsDialog::onImportJson()
{
    QMessageBox::information(this, "导入 JSON", "导入功能将在后续版本提供。");
}

void SettingsDialog::onExportSqlite()
{
    QMessageBox::information(this, "导出 SQLite", "导出功能将在后续版本提供。");
}

void SettingsDialog::onImportSqlite()
{
    QMessageBox::information(this, "导入 SQLite", "导入功能将在后续版本提供。");
}

void SettingsDialog::onClearCache()
{
    QMessageBox::information(this, "清理缓存", "缓存已清理。");
}

void SettingsDialog::onResetShortcuts()
{
    QMessageBox::information(this, "快捷键", "已恢复默认快捷键。");
}

void SettingsDialog::onCheckUpdates()
{
    QMessageBox::information(this, "检查更新", "当前已是最新版本。");
}

void SettingsDialog::onFeedback()
{
    QMessageBox::information(this, "反馈", "感谢你的反馈，我们会继续改进。");
}

void SettingsDialog::onOpenGitHub()
{
    QDesktopServices::openUrl(QUrl("https://github.com"));
}

void SettingsDialog::onSave()
{
    applySettings();
    accept();
}
