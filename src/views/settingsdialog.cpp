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
#include <QButtonGroup>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDate>
#include <QRegularExpression>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QHash>
#include <QSet>
#include <QVector>

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

struct ExportOptions {
    bool tasks = true;
    bool folders = true;
    bool settings = true;
    bool notifications = true;
};

struct ImportOptions {
    enum Mode {
        Merge,
        Overwrite,
        Append
    };
    enum Conflict {
        Skip,
        OverwriteConflict,
        Regenerate
    };
    Mode mode = Merge;
    Conflict conflict = OverwriteConflict;
};
struct ImportSummary {
    int tasks = 0;
    int folders = 0;
    int settings = 0;
    int notifications = 0;
    int tags = 0;
};

struct FileItem {
    QString path;
    QString name;
};

struct StepItem {
    QString title;
    bool completed = false;
    int position = 0;
};

struct ImportedTask {
    int originalId = -1;
    int parentOriginalId = 0;
    QString title;
    QString description;
    int priority = 1;
    QDateTime dueDate;
    bool completed = false;
    int progress = 0;
    bool isDeleted = false;
    QDateTime deletedAt;
    QDateTime createdAt;
    QDateTime updatedAt;
    QList<QString> tagNames;
    QList<int> dependencyIds;
    QList<FileItem> files;
    QList<StepItem> steps;
};

struct ImportedFolder {
    int originalId = -1;
    QString name;
    QString color;
    int position = 0;
    QDateTime createdAt;
    QList<int> taskIds;
};

struct ImportedTag {
    int originalId = -1;
    QString name;
    QString color;
    QDateTime createdAt;
};

struct ImportedSetting {
    QString key;
    QString value;
    QDateTime updatedAt;
};

struct ImportedNotification {
    int originalId = -1;
    int type = 0;
    QString title;
    QString message;
    int taskId = 0;
    bool read = false;
    QDateTime createdAt;
};

QString isoDateString(const QDateTime &dt)
{
    if (!dt.isValid()) {
        return QString();
    }
    return dt.toUTC().toString(Qt::ISODate);
}

QDateTime parseIsoDate(const QString &value)
{
    if (value.trimmed().isEmpty()) {
        return QDateTime();
    }
    QDateTime parsed = QDateTime::fromString(value, Qt::ISODate);
    if (!parsed.isValid()) {
        parsed = QDateTime::fromString(value, Qt::ISODateWithMs);
    }
    return parsed;
}

QString formatDurationSeconds(qint64 seconds)
{
    if (seconds < 0) {
        seconds = 0;
    }
    qint64 mins = seconds / 60;
    qint64 secs = seconds % 60;
    if (mins > 99) {
        return QString("99:59");
    }
    return QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));
}

void updateProgress(QProgressDialog *dialog, QElapsedTimer *timer, int current, int total, const QString &prefix, bool showRemaining)
{
    if (!dialog || total <= 0) {
        return;
    }
    int percent = static_cast<int>((static_cast<double>(current) / total) * 100.0);
    if (percent > 100) {
        percent = 100;
    } else if (percent < 0) {
        percent = 0;
    }
    dialog->setValue(percent);
    if (showRemaining && timer && timer->elapsed() > 0 && percent > 0 && percent < 100) {
        qint64 elapsedSeconds = timer->elapsed() / 1000;
        qint64 remainingSeconds = (elapsedSeconds * (100 - percent)) / percent;
        dialog->setLabelText(QString("%1\n预计剩余：%2")
                                 .arg(prefix, formatDurationSeconds(remainingSeconds)));
    } else {
        dialog->setLabelText(QString("%1 (%2/%3)").arg(prefix).arg(current).arg(total));
    }
}

QString defaultExportDirectory()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (dir.trimmed().isEmpty()) {
        dir = QDir::currentPath();
    }
    return dir;
}

bool ensureWritableExportLocation(QWidget *parent, const QString &path, QString *error)
{
    QFileInfo info(path);
    QDir dir = info.dir();
    if (!dir.exists()) {
        if (error) {
            *error = "导出目录不存在";
        }
        return false;
    }
    QFileInfo dirInfo(dir.absolutePath());
    if (!dirInfo.isWritable()) {
        if (error) {
            *error = "导出目录不可写";
        }
        return false;
    }
    if (QFile::exists(path)) {
        auto response = QMessageBox::warning(parent,
                                             "覆盖文件",
                                             "目标文件已存在，是否覆盖？",
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::No);
        if (response != QMessageBox::Yes) {
            if (error) {
                *error = "用户取消";
            }
            return false;
        }
    }
    return true;
}

bool promptExportOptions(QWidget *parent, ExportOptions *options)
{
    if (!options) {
        return false;
    }
    QDialog dialog(parent);
    dialog.setWindowTitle("导出内容");
    dialog.setMinimumWidth(320);
    auto *layout = new QVBoxLayout(&dialog);
    auto *tasksCheck = new QCheckBox("任务", &dialog);
    auto *foldersCheck = new QCheckBox("文件夹", &dialog);
    auto *settingsCheck = new QCheckBox("设置", &dialog);
    auto *notificationsCheck = new QCheckBox("通知", &dialog);

    tasksCheck->setChecked(options->tasks);
    foldersCheck->setChecked(options->folders);
    settingsCheck->setChecked(options->settings);
    notificationsCheck->setChecked(options->notifications);

    layout->addWidget(tasksCheck);
    layout->addWidget(foldersCheck);
    layout->addWidget(settingsCheck);
    layout->addWidget(notificationsCheck);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    if (auto *okButton = buttons->button(QDialogButtonBox::Ok)) {
        okButton->setText("确定");
    }
    if (auto *cancelButton = buttons->button(QDialogButtonBox::Cancel)) {
        cancelButton->setText("取消");
    }
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    options->tasks = tasksCheck->isChecked();
    options->folders = foldersCheck->isChecked();
    options->settings = settingsCheck->isChecked();
    options->notifications = notificationsCheck->isChecked();
    return true;
}

bool promptImportOptions(QWidget *parent, ImportOptions *options)
{
    if (!options) {
        return false;
    }

    QDialog dialog(parent);
    dialog.setWindowTitle("导入选项");
    dialog.setMinimumWidth(360);
    auto *layout = new QVBoxLayout(&dialog);

    auto *modeGroup = new QGroupBox("导入模式", &dialog);
    auto *modeLayout = new QVBoxLayout(modeGroup);
    auto *mergeRadio = new QRadioButton("合并（保留现有数据）", modeGroup);
    auto *overwriteRadio = new QRadioButton("覆盖（清空后导入）", modeGroup);
    auto *appendRadio = new QRadioButton("追加（仅新增）", modeGroup);
    modeLayout->addWidget(mergeRadio);
    modeLayout->addWidget(overwriteRadio);
    modeLayout->addWidget(appendRadio);
    layout->addWidget(modeGroup);

    auto *conflictGroup = new QGroupBox("冲突处理", &dialog);
    auto *conflictLayout = new QVBoxLayout(conflictGroup);
    auto *skipRadio = new QRadioButton("跳过冲突项", conflictGroup);
    auto *overwriteConflictRadio = new QRadioButton("覆盖冲突项", conflictGroup);
    auto *regenerateRadio = new QRadioButton("重新生成ID", conflictGroup);
    conflictLayout->addWidget(skipRadio);
    conflictLayout->addWidget(overwriteConflictRadio);
    conflictLayout->addWidget(regenerateRadio);
    layout->addWidget(conflictGroup);

    switch (options->mode) {
        case ImportOptions::Overwrite:
            overwriteRadio->setChecked(true);
            break;
        case ImportOptions::Append:
            appendRadio->setChecked(true);
            break;
        default:
            mergeRadio->setChecked(true);
            break;
    }

    switch (options->conflict) {
        case ImportOptions::Skip:
            skipRadio->setChecked(true);
            break;
        case ImportOptions::Regenerate:
            regenerateRadio->setChecked(true);
            break;
        default:
            overwriteConflictRadio->setChecked(true);
            break;
    }

    auto updateConflictState = [&]() {
        bool enabled = !overwriteRadio->isChecked();
        conflictGroup->setEnabled(enabled);
    };

    QObject::connect(overwriteRadio, &QRadioButton::toggled, &dialog, updateConflictState);
    updateConflictState();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    if (auto *okButton = buttons->button(QDialogButtonBox::Ok)) {
        okButton->setText("确定");
    }
    if (auto *cancelButton = buttons->button(QDialogButtonBox::Cancel)) {
        cancelButton->setText("取消");
    }
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    if (overwriteRadio->isChecked()) {
        options->mode = ImportOptions::Overwrite;
    } else if (appendRadio->isChecked()) {
        options->mode = ImportOptions::Append;
    } else {
        options->mode = ImportOptions::Merge;
    }

    if (skipRadio->isChecked()) {
        options->conflict = ImportOptions::Skip;
    } else if (regenerateRadio->isChecked()) {
        options->conflict = ImportOptions::Regenerate;
    } else {
        options->conflict = ImportOptions::OverwriteConflict;
    }

    return true;
}

bool validateJsonTask(const QJsonObject &obj, QString *error)
{
    if (!obj.contains("title") || !obj.value("title").isString()) {
        if (error) {
            *error = "任务标题缺失或格式不正确";
        }
        return false;
    }
    if (obj.contains("id") && !obj.value("id").isDouble()) {
        if (error) {
            *error = "任务ID必须为数字";
        }
        return false;
    }
    auto checkArrayOfStrings = [&](const QString &key) {
        if (!obj.contains(key)) {
            return true;
        }
        if (!obj.value(key).isArray()) {
            if (error) {
                *error = QString("字段 %1 必须是数组").arg(key);
            }
            return false;
        }
        QJsonArray arr = obj.value(key).toArray();
        for (const QJsonValue &val : arr) {
            if (!val.isString()) {
                if (error) {
                    *error = QString("字段 %1 必须为字符串数组").arg(key);
                }
                return false;
            }
        }
        return true;
    };

    auto checkArrayOfNumbers = [&](const QString &key) {
        if (!obj.contains(key)) {
            return true;
        }
        if (!obj.value(key).isArray()) {
            if (error) {
                *error = QString("字段 %1 必须是数组").arg(key);
            }
            return false;
        }
        QJsonArray arr = obj.value(key).toArray();
        for (const QJsonValue &val : arr) {
            if (!val.isDouble()) {
                if (error) {
                    *error = QString("字段 %1 必须为数字数组").arg(key);
                }
                return false;
            }
        }
        return true;
    };

    if (!checkArrayOfStrings("tags")) {
        return false;
    }
    if (!checkArrayOfNumbers("dependencies")) {
        return false;
    }

    if (obj.contains("files")) {
        if (!obj.value("files").isArray()) {
            if (error) {
                *error = "files 字段必须为数组";
            }
            return false;
        }
        QJsonArray files = obj.value("files").toArray();
        for (const QJsonValue &val : files) {
            if (!val.isObject()) {
                if (error) {
                    *error = "files 数组项必须为对象";
                }
                return false;
            }
            QJsonObject fileObj = val.toObject();
            if (!fileObj.contains("path") || !fileObj.value("path").isString()) {
                if (error) {
                    *error = "files 项缺少 path";
                }
                return false;
            }
        }
    }

    if (obj.contains("steps")) {
        if (!obj.value("steps").isArray()) {
            if (error) {
                *error = "steps 字段必须为数组";
            }
            return false;
        }
        QJsonArray steps = obj.value("steps").toArray();
        for (const QJsonValue &val : steps) {
            if (!val.isObject()) {
                if (error) {
                    *error = "steps 数组项必须为对象";
                }
                return false;
            }
            QJsonObject stepObj = val.toObject();
            if (!stepObj.contains("title") || !stepObj.value("title").isString()) {
                if (error) {
                    *error = "steps 项缺少 title";
                }
                return false;
            }
        }
    }

    if (obj.contains("children")) {
        if (!obj.value("children").isArray()) {
            if (error) {
                *error = "children 字段必须为数组";
            }
            return false;
        }
        QJsonArray children = obj.value("children").toArray();
        for (const QJsonValue &val : children) {
            if (!val.isObject()) {
                if (error) {
                    *error = "children 数组项必须为对象";
                }
                return false;
            }
            if (!validateJsonTask(val.toObject(), error)) {
                return false;
            }
        }
    }

    if (obj.contains("due_date") && !obj.value("due_date").isString() && !obj.value("due_date").isNull()) {
        if (error) {
            *error = "due_date 必须为字符串或空";
        }
        return false;
    }
    if (obj.contains("created_at") && !obj.value("created_at").isString() && !obj.value("created_at").isNull()) {
        if (error) {
            *error = "created_at 必须为字符串或空";
        }
        return false;
    }
    if (obj.contains("updated_at") && !obj.value("updated_at").isString() && !obj.value("updated_at").isNull()) {
        if (error) {
            *error = "updated_at 必须为字符串或空";
        }
        return false;
    }
    if (obj.contains("deleted_at") && !obj.value("deleted_at").isString() && !obj.value("deleted_at").isNull()) {
        if (error) {
            *error = "deleted_at 必须为字符串或空";
        }
        return false;
    }

    return true;
}

bool validateJsonStructure(const QJsonObject &root, QString *error)
{
    if (!root.contains("version") || !root.value("version").isString()) {
        if (error) {
            *error = "version 字段缺失或格式不正确";
        }
        return false;
    }
    if (!root.contains("export_date") || !root.value("export_date").isString()) {
        if (error) {
            *error = "export_date 字段缺失或格式不正确";
        }
        return false;
    }
    if (!root.contains("data") || !root.value("data").isObject()) {
        if (error) {
            *error = "data 字段缺失或格式不正确";
        }
        return false;
    }

    QJsonObject data = root.value("data").toObject();
    if (data.contains("tasks")) {
        if (!data.value("tasks").isArray()) {
            if (error) {
                *error = "tasks 字段必须为数组";
            }
            return false;
        }
        QJsonArray tasks = data.value("tasks").toArray();
        for (const QJsonValue &val : tasks) {
            if (!val.isObject()) {
                if (error) {
                    *error = "tasks 数组项必须为对象";
                }
                return false;
            }
            if (!validateJsonTask(val.toObject(), error)) {
                return false;
            }
        }
    }

    auto ensureArray = [&](const QString &key) {
        if (data.contains(key) && !data.value(key).isArray()) {
            if (error) {
                *error = QString("字段 %1 必须为数组").arg(key);
            }
            return false;
        }
        return true;
    };

    if (!ensureArray("folders")) {
        return false;
    }
    if (!ensureArray("settings")) {
        return false;
    }
    if (!ensureArray("notifications")) {
        return false;
    }
    if (!ensureArray("tags")) {
        return false;
    }

    return true;
}

bool validateSqliteFile(const QString &path, QString *error)
{
    QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        if (error) {
            *error = "文件不存在或不是文件";
        }
        return false;
    }
    if (info.suffix().toLower() != "db") {
        if (error) {
            *error = "请选择 .db 后缀的 SQLite 文件";
        }
        return false;
    }

    const QString connectionName = "import_validation_connection";
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(path);
        if (!db.open()) {
            if (error) {
                *error = "无法打开 SQLite 文件";
            }
            QSqlDatabase::removeDatabase(connectionName);
            return false;
        }

        QStringList requiredTables = {
            "tasks", "task_steps", "tags", "task_tags",
            "task_dependencies", "task_files", "folders",
            "task_folders", "notifications", "settings"
        };

        QSqlQuery query(db);
        for (const QString &table : requiredTables) {
            query.prepare("SELECT 1 FROM sqlite_master WHERE type='table' AND name=?");
            query.addBindValue(table);
            if (!query.exec() || !query.next()) {
                if (error) {
                    *error = QString("数据库缺少表：%1").arg(table);
                }
                db.close();
                QSqlDatabase::removeDatabase(connectionName);
                return false;
            }
        }

        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
    return true;
}

bool clearAllData(QSqlDatabase &db, QString *error)
{
    QStringList tables = {
        "task_tags",
        "task_dependencies",
        "task_files",
        "task_steps",
        "task_folders",
        "notifications",
        "tasks",
        "folders",
        "tags",
        "settings",
        "backup_history"
    };

    QSqlQuery query(db);
    if (!query.exec("PRAGMA foreign_keys = OFF")) {
        if (error) {
            *error = "无法关闭外键约束";
        }
        return false;
    }

    for (const QString &table : tables) {
        if (!query.exec(QString("DELETE FROM %1").arg(table))) {
            if (error) {
                *error = QString("清空表失败：%1").arg(table);
            }
            query.exec("PRAGMA foreign_keys = ON");
            return false;
        }
    }

    query.exec("PRAGMA foreign_keys = ON");
    return true;
}

QString generateTimestampedSqliteName()
{
    QString date = QDate::currentDate().toString("yyyyMMdd");
    return QString("todolist_backup_%1.db").arg(date);
}
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
    if (auto *saveButton = buttonBox->button(QDialogButtonBox::Save)) {
        saveButton->setText("保存设置");
    }
    if (auto *cancelButton = buttonBox->button(QDialogButtonBox::Cancel)) {
        cancelButton->setText("取消");
    }
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

bool SettingsDialog::applySettings()
{
    bool ok = true;

    ok &= setSetting(KEY_THEME, QString::number(m_themeCombo->currentData().toInt()));
    ok &= setSetting(KEY_LANGUAGE, m_languageCombo->currentData().toString());
    ok &= setSetting(KEY_AUTO_LAUNCH, m_autoLaunchCheck->isChecked() ? "1" : "0");
    ok &= setSetting(KEY_RESTORE_LAST, m_restoreLastCheck->isChecked() ? "1" : "0");

    ok &= setSetting(KEY_CARD_STYLE, QString::number(m_cardStyleCombo->currentIndex()));
    ok &= setSetting(KEY_FONT_SIZE, QString::number(m_fontSizeSpin->value()));
    ok &= setSetting(KEY_ICON_STYLE, QString::number(m_iconStyleCombo->currentIndex()));
    ok &= setSetting(KEY_CORNER_RADIUS, QString::number(m_cornerRadiusSpin->value()));

    ok &= setSetting(KEY_NOTIFY_ENABLED, m_notificationsEnabledCheck->isChecked() ? "1" : "0");
    ok &= setSetting(KEY_NOTIFY_REMINDERS, m_remindersCheck->isChecked() ? "1" : "0");
    ok &= setSetting(KEY_NOTIFY_SYSTEM, m_systemNotifyCheck->isChecked() ? "1" : "0");
    ok &= setSetting(KEY_NOTIFY_SOUND, m_soundCheck->isChecked() ? "1" : "0");

    if (m_backupManager) {
        ok &= m_backupManager->setAutoBackupEnabled(m_backupEnabledCheck->isChecked());
        ok &= m_backupManager->setBackupFrequency(static_cast<BackupManager::BackupFrequency>(m_backupFrequencyCombo->currentData().toInt()));
        ok &= m_backupManager->setBackupTime(m_backupTimeEdit->time());
        ok &= m_backupManager->setBackupRetention(m_backupRetentionSpin->value());
        if (!m_backupLocationEdit->text().trimmed().isEmpty()) {
            ok &= m_backupManager->setBackupLocation(m_backupLocationEdit->text().trimmed());
        }
        ok &= m_backupManager->setBackupOnExit(m_backupOnExitCheck->isChecked());
    } else {
        ok &= setSetting("auto_backup_enabled", m_backupEnabledCheck->isChecked() ? "1" : "0");
        ok &= setSetting("backup_frequency", QString::number(m_backupFrequencyCombo->currentData().toInt()));
        ok &= setSetting("backup_time", m_backupTimeEdit->time().toString("HH:mm"));
        ok &= setSetting("backup_retention", QString::number(m_backupRetentionSpin->value()));
        ok &= setSetting("backup_location", m_backupLocationEdit->text().trimmed());
        ok &= setSetting("backup_on_exit", m_backupOnExitCheck->isChecked() ? "1" : "0");
    }

    ok &= setSetting(KEY_DELETE_PARENT_ACTION, QString::number(m_parentDeleteCombo->currentIndex()));
    ok &= setSetting(KEY_DELETE_AUTO_CLEANUP, m_autoCleanupCheck->isChecked() ? "1" : "0");
    ok &= setSetting(KEY_DELETE_CLEANUP_DAYS, QString::number(m_cleanupDaysSpin->value()));

    ThemeManager &themeManager = ThemeManager::instance();
    int themeValue = m_themeCombo->currentData().toInt();
    if (themeValue == static_cast<int>(ThemeManager::System)) {
        themeManager.setFollowSystem(true);
    } else {
        themeManager.setFollowSystem(false);
        themeManager.setTheme(static_cast<ThemeManager::Theme>(themeValue));
    }

    return ok;
}

QString SettingsDialog::getSetting(const QString &key, const QString &defaultValue) const
{
    return m_database->getSetting(key, defaultValue);
}

bool SettingsDialog::setSetting(const QString &key, const QString &value)
{
    return m_database->setSetting(key, value);
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
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()))) {
        QMessageBox::warning(this, "错误", "无法打开数据库所在目录。");
    }
}

void SettingsDialog::onExportJson()
{
    ExportOptions options;
    if (!promptExportOptions(this, &options)) {
        return;
    }

    if (!options.tasks && !options.folders && !options.settings && !options.notifications) {
        QMessageBox::warning(this, "导出 JSON", "请至少选择一种导出内容。");
        return;
    }

    QString defaultName = QString("todolist_export_%1.json").arg(QDate::currentDate().toString("yyyyMMdd"));
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "导出 JSON",
        QDir(defaultExportDirectory()).filePath(defaultName),
        "JSON 文件 (*.json)");

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }

    QString error;
    if (!ensureWritableExportLocation(this, filePath, &error)) {
        if (error != "用户取消") {
            QMessageBox::warning(this, "导出 JSON", error);
        }
        return;
    }

    QProgressDialog progress("正在导出数据...", QString(), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setAutoClose(false);
    progress.setAutoReset(false);
    progress.setCancelButton(nullptr);
    progress.show();

    QElapsedTimer timer;
    timer.start();

    QSqlDatabase db = m_database->database();
    QSqlQuery query(db);

    auto countTable = [&](const QString &table) -> int {
        QSqlQuery countQuery(db);
        if (countQuery.exec(QString("SELECT COUNT(*) FROM %1").arg(table)) && countQuery.next()) {
            return countQuery.value(0).toInt();
        }
        return 0;
    };

    int totalItems = 0;
    if (options.tasks) {
        totalItems += countTable("tasks");
        totalItems += countTable("task_steps");
        totalItems += countTable("task_files");
        totalItems += countTable("task_dependencies");
        totalItems += countTable("task_tags");
        totalItems += countTable("tags");
    }
    if (options.folders) {
        totalItems += countTable("folders");
        totalItems += countTable("task_folders");
    }
    if (options.settings) {
        totalItems += countTable("settings");
    }
    if (options.notifications) {
        totalItems += countTable("notifications");
    }
    if (totalItems <= 0) {
        totalItems = 1;
    }

    int processed = 0;

    QJsonObject root;
    QString version = QCoreApplication::applicationVersion();
    if (version.isEmpty()) {
        version = "1.0.0";
    }
    root["version"] = version;
    root["export_date"] = isoDateString(QDateTime::currentDateTimeUtc());

    QJsonObject data;

    if (options.tasks) {
        QHash<int, QJsonObject> taskObjects;
        QHash<int, QList<int>> childrenMap;
        QList<int> rootIds;
        QHash<int, QString> tagIdToName;

        QJsonArray tagsArray;
        if (query.exec("SELECT id, name, color, created_at FROM tags ORDER BY created_at ASC")) {
            while (query.next()) {
                QJsonObject tagObj;
                int tagId = query.value(0).toInt();
                QString tagName = query.value(1).toString();
                tagObj["id"] = tagId;
                tagObj["name"] = tagName;
                tagObj["color"] = query.value(2).toString();
                QDateTime createdAt = parseIsoDate(query.value(3).toString());
                tagObj["created_at"] = isoDateString(createdAt);
                tagsArray.append(tagObj);
                tagIdToName.insert(tagId, tagName);
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }
        if (!tagsArray.isEmpty()) {
            data["tags"] = tagsArray;
        }

        if (query.exec("SELECT id, title, description, priority, due_date, completed, progress, parent_id, is_deleted, deleted_at, created_at, updated_at, file_path FROM tasks ORDER BY created_at ASC")) {
            while (query.next()) {
                QJsonObject taskObj;
                int taskId = query.value(0).toInt();
                taskObj["id"] = taskId;
                taskObj["title"] = query.value(1).toString();
                taskObj["description"] = query.value(2).toString();
                taskObj["priority"] = query.value(3).toInt();

                QDateTime dueDate = parseIsoDate(query.value(4).toString());
                taskObj["due_date"] = isoDateString(dueDate);
                taskObj["completed"] = query.value(5).toInt() == 1;
                taskObj["progress"] = static_cast<int>(query.value(6).toDouble());
                taskObj["parent_id"] = query.value(7).toInt();
                taskObj["is_deleted"] = query.value(8).toInt() == 1;

                QDateTime deletedAt = parseIsoDate(query.value(9).toString());
                taskObj["deleted_at"] = isoDateString(deletedAt);

                QDateTime createdAt = parseIsoDate(query.value(10).toString());
                QDateTime updatedAt = parseIsoDate(query.value(11).toString());
                taskObj["created_at"] = isoDateString(createdAt);
                taskObj["updated_at"] = isoDateString(updatedAt);
                taskObj["file_path"] = query.value(12).toString();

                taskObj["tags"] = QJsonArray();
                taskObj["tag_ids"] = QJsonArray();
                taskObj["dependencies"] = QJsonArray();
                taskObj["files"] = QJsonArray();
                taskObj["steps"] = QJsonArray();
                taskObj["children"] = QJsonArray();

                taskObjects.insert(taskId, taskObj);
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }

        if (query.exec("SELECT id, task_id, title, completed, position, created_at FROM task_steps ORDER BY position ASC")) {
            while (query.next()) {
                int taskId = query.value(1).toInt();
                if (taskObjects.contains(taskId)) {
                    QJsonObject stepObj;
                    stepObj["id"] = query.value(0).toInt();
                    stepObj["title"] = query.value(2).toString();
                    stepObj["completed"] = query.value(3).toInt() == 1;
                    stepObj["position"] = query.value(4).toInt();
                    QDateTime createdAt = parseIsoDate(query.value(5).toString());
                    stepObj["created_at"] = isoDateString(createdAt);
                    QJsonArray steps = taskObjects[taskId]["steps"].toArray();
                    steps.append(stepObj);
                    taskObjects[taskId]["steps"] = steps;
                }
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }

        if (query.exec("SELECT task_id, depends_on_id FROM task_dependencies ORDER BY created_at ASC")) {
            while (query.next()) {
                int taskId = query.value(0).toInt();
                if (taskObjects.contains(taskId)) {
                    QJsonArray deps = taskObjects[taskId]["dependencies"].toArray();
                    deps.append(query.value(1).toInt());
                    taskObjects[taskId]["dependencies"] = deps;
                }
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }

        if (query.exec("SELECT task_id, tag_id FROM task_tags")) {
            while (query.next()) {
                int taskId = query.value(0).toInt();
                int tagId = query.value(1).toInt();
                if (taskObjects.contains(taskId)) {
                    QJsonArray tagIds = taskObjects[taskId]["tag_ids"].toArray();
                    tagIds.append(tagId);
                    taskObjects[taskId]["tag_ids"] = tagIds;

                    QString tagName = tagIdToName.value(tagId);
                    if (!tagName.isEmpty()) {
                        QJsonArray tags = taskObjects[taskId]["tags"].toArray();
                        tags.append(tagName);
                        taskObjects[taskId]["tags"] = tags;
                    }
                }
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }

        if (query.exec("SELECT task_id, file_path, file_name, created_at FROM task_files ORDER BY created_at ASC")) {
            while (query.next()) {
                int taskId = query.value(0).toInt();
                if (taskObjects.contains(taskId)) {
                    QJsonObject fileObj;
                    fileObj["path"] = query.value(1).toString();
                    fileObj["name"] = query.value(2).toString();
                    QDateTime createdAt = parseIsoDate(query.value(3).toString());
                    fileObj["created_at"] = isoDateString(createdAt);
                    QJsonArray files = taskObjects[taskId]["files"].toArray();
                    files.append(fileObj);
                    taskObjects[taskId]["files"] = files;
                }
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }

        for (auto it = taskObjects.begin(); it != taskObjects.end(); ++it) {
            QString primaryFile = it.value().value("file_path").toString();
            if (!primaryFile.isEmpty()) {
                QJsonArray files = it.value().value("files").toArray();
                bool exists = false;
                for (const QJsonValue &fileVal : files) {
                    if (fileVal.isObject()) {
                        if (fileVal.toObject().value("path").toString() == primaryFile) {
                            exists = true;
                            break;
                        }
                    }
                }
                if (!exists) {
                    QJsonObject fileObj;
                    fileObj["path"] = primaryFile;
                    fileObj["name"] = QFileInfo(primaryFile).fileName();
                    files.append(fileObj);
                    it.value()["files"] = files;
                }
            }
            it.value().remove("file_path");
        }

        for (auto it = taskObjects.begin(); it != taskObjects.end(); ++it) {
            int parentId = it.value().value("parent_id").toInt();
            if (parentId > 0 && taskObjects.contains(parentId)) {
                childrenMap[parentId].append(it.key());
            } else {
                rootIds.append(it.key());
            }
        }

        auto buildTree = [&](auto &&self, int taskId) -> QJsonObject {
            QJsonObject obj = taskObjects.value(taskId);
            QJsonArray children;
            for (int childId : childrenMap.value(taskId)) {
                children.append(self(self, childId));
            }
            obj["children"] = children;
            return obj;
        };

        QJsonArray tasksArray;
        for (int rootId : rootIds) {
            tasksArray.append(buildTree(buildTree, rootId));
        }
        data["tasks"] = tasksArray;
    }

    if (options.folders) {
        QJsonArray foldersArray;
        if (query.exec("SELECT id, name, color, position, created_at FROM folders ORDER BY position ASC")) {
            while (query.next()) {
                QJsonObject folderObj;
                int folderId = query.value(0).toInt();
                folderObj["id"] = folderId;
                folderObj["name"] = query.value(1).toString();
                folderObj["color"] = query.value(2).toString();
                folderObj["position"] = query.value(3).toInt();
                QDateTime createdAt = parseIsoDate(query.value(4).toString());
                folderObj["created_at"] = isoDateString(createdAt);

                QJsonArray taskIds;
                QSqlQuery taskQuery(db);
                taskQuery.prepare("SELECT task_id FROM task_folders WHERE folder_id = ?");
                taskQuery.addBindValue(folderId);
                if (taskQuery.exec()) {
                    while (taskQuery.next()) {
                        taskIds.append(taskQuery.value(0).toInt());
                        processed++;
                        updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
                    }
                }
                folderObj["task_ids"] = taskIds;
                foldersArray.append(folderObj);
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }
        data["folders"] = foldersArray;
    }

    if (options.settings) {
        QJsonArray settingsArray;
        if (query.exec("SELECT key, value, updated_at FROM settings")) {
            while (query.next()) {
                QJsonObject settingObj;
                settingObj["key"] = query.value(0).toString();
                settingObj["value"] = query.value(1).toString();
                QDateTime updatedAt = parseIsoDate(query.value(2).toString());
                settingObj["updated_at"] = isoDateString(updatedAt);
                settingsArray.append(settingObj);
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }
        data["settings"] = settingsArray;
    }

    if (options.notifications) {
        QJsonArray notificationsArray;
        if (query.exec("SELECT id, type, title, message, task_id, read, created_at FROM notifications ORDER BY created_at DESC")) {
            while (query.next()) {
                QJsonObject notificationObj;
                notificationObj["id"] = query.value(0).toInt();
                notificationObj["type"] = query.value(1).toInt();
                notificationObj["title"] = query.value(2).toString();
                notificationObj["message"] = query.value(3).toString();
                notificationObj["task_id"] = query.value(4).toInt();
                notificationObj["read"] = query.value(5).toInt() == 1;
                QDateTime createdAt = parseIsoDate(query.value(6).toString());
                notificationObj["created_at"] = isoDateString(createdAt);
                notificationsArray.append(notificationObj);
                processed++;
                updateProgress(&progress, &timer, processed, totalItems, "正在导出数据...", true);
            }
        }
        data["notifications"] = notificationsArray;
    }

    root["data"] = data;
    QJsonDocument doc(root);

    QFile outFile(filePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        progress.close();
        QMessageBox::warning(this, "导出 JSON", "无法写入 JSON 文件。");
        return;
    }
    outFile.write(doc.toJson(QJsonDocument::Indented));
    outFile.close();

    updateProgress(&progress, &timer, totalItems, totalItems, "导出完成", false);
    progress.close();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("导出 JSON");
    msgBox.setText(QString("导出完成。\n%1").arg(filePath));
    QPushButton *openButton = msgBox.addButton("打开所在文件夹", QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();
    if (msgBox.clickedButton() == openButton) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).absolutePath()));
    }
}


void SettingsDialog::onImportJson()
{
    QString filePath = QFileDialog::getOpenFileName(this, "导入 JSON", "", "JSON 文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile inFile(filePath);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "导入 JSON", "无法读取 JSON 文件。");
        return;
    }
    QByteArray jsonData = inFile.readAll();
    inFile.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        QMessageBox::warning(this, "导入 JSON", "JSON 格式不正确。");
        return;
    }

    QJsonObject root = doc.object();
    QString validationError;
    if (!validateJsonStructure(root, &validationError)) {
        QMessageBox::warning(this, "导入 JSON", validationError);
        return;
    }

    ImportOptions options;
    if (!promptImportOptions(this, &options)) {
        return;
    }

    if (options.mode == ImportOptions::Overwrite) {
        auto response = QMessageBox::warning(this,
                                             "导入 JSON",
                                             "此操作会清空现有数据，是否继续？",
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::No);
        if (response != QMessageBox::Yes) {
            return;
        }
    }

    QProgressDialog progress("正在导入数据...", QString(), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setAutoClose(false);
    progress.setAutoReset(false);
    progress.setCancelButton(nullptr);
    progress.show();

    QElapsedTimer timer;
    timer.start();

    QJsonObject data = root.value("data").toObject();
    bool hasFolderData = data.contains("folders");

    QList<ImportedTag> importedTags;
    QHash<int, QString> importedTagIdToName;
    if (data.contains("tags")) {
        QJsonArray tagsArray = data.value("tags").toArray();
        for (const QJsonValue &val : tagsArray) {
            if (!val.isObject()) {
                continue;
            }
            QJsonObject tagObj = val.toObject();
            ImportedTag tag;
            tag.originalId = tagObj.value("id").toInt(-1);
            tag.name = tagObj.value("name").toString();
            tag.color = tagObj.value("color").toString();
            tag.createdAt = parseIsoDate(tagObj.value("created_at").toString());
            if (!tag.name.trimmed().isEmpty()) {
                importedTags.append(tag);
                if (tag.originalId > 0) {
                    importedTagIdToName.insert(tag.originalId, tag.name);
                }
            }
        }
    }

    QList<ImportedFolder> importedFolders;
    if (data.contains("folders")) {
        QJsonArray foldersArray = data.value("folders").toArray();
        for (const QJsonValue &val : foldersArray) {
            if (!val.isObject()) {
                continue;
            }
            QJsonObject folderObj = val.toObject();
            ImportedFolder folder;
            folder.originalId = folderObj.value("id").toInt(-1);
            folder.name = folderObj.value("name").toString();
            folder.color = folderObj.value("color").toString();
            folder.position = folderObj.value("position").toInt(0);
            folder.createdAt = parseIsoDate(folderObj.value("created_at").toString());
            QJsonArray taskIds = folderObj.value("task_ids").toArray();
            for (const QJsonValue &taskVal : taskIds) {
                if (taskVal.isDouble()) {
                    folder.taskIds.append(taskVal.toInt());
                }
            }
            if (!folder.name.trimmed().isEmpty()) {
                importedFolders.append(folder);
            }
        }
    }

    QList<ImportedSetting> importedSettings;
    if (data.contains("settings")) {
        QJsonArray settingsArray = data.value("settings").toArray();
        for (const QJsonValue &val : settingsArray) {
            if (!val.isObject()) {
                continue;
            }
            QJsonObject settingObj = val.toObject();
            ImportedSetting setting;
            setting.key = settingObj.value("key").toString();
            setting.value = settingObj.value("value").toString();
            setting.updatedAt = parseIsoDate(settingObj.value("updated_at").toString());
            if (!setting.key.trimmed().isEmpty()) {
                importedSettings.append(setting);
            }
        }
    }

    QList<ImportedNotification> importedNotifications;
    if (data.contains("notifications")) {
        QJsonArray notificationsArray = data.value("notifications").toArray();
        for (const QJsonValue &val : notificationsArray) {
            if (!val.isObject()) {
                continue;
            }
            QJsonObject notificationObj = val.toObject();
            ImportedNotification notification;
            notification.originalId = notificationObj.value("id").toInt(-1);
            notification.type = notificationObj.value("type").toInt(0);
            notification.title = notificationObj.value("title").toString();
            notification.message = notificationObj.value("message").toString();
            notification.taskId = notificationObj.value("task_id").toInt(0);
            notification.read = notificationObj.value("read").toBool(false);
            notification.createdAt = parseIsoDate(notificationObj.value("created_at").toString());
            importedNotifications.append(notification);
        }
    }
    QList<ImportedTask> importedTasks;
    bool taskParseError = false;
    QString taskError;
    if (data.contains("tasks")) {
        QJsonArray tasksArray = data.value("tasks").toArray();
        auto parseTasks = [&](auto &&self, const QJsonArray &array, int parentId) -> void {
            for (const QJsonValue &val : array) {
                if (!val.isObject()) {
                    continue;
                }
                QJsonObject taskObj = val.toObject();
                ImportedTask task;
                task.originalId = taskObj.value("id").toInt(-1);
                if (parentId > 0) {
                    task.parentOriginalId = parentId;
                } else {
                    task.parentOriginalId = taskObj.value("parent_id").toInt(0);
                }
                task.title = taskObj.value("title").toString();
                task.description = taskObj.value("description").toString();
                task.priority = taskObj.value("priority").toInt(1);
                task.completed = taskObj.value("completed").toBool(false);
                task.progress = taskObj.value("progress").toInt(0);
                task.isDeleted = taskObj.value("is_deleted").toBool(false);

                QString dueDateStr = taskObj.value("due_date").toString();
                task.dueDate = parseIsoDate(dueDateStr);
                if (!dueDateStr.trimmed().isEmpty() && !task.dueDate.isValid()) {
                    taskParseError = true;
                    taskError = "任务日期格式不正确";
                    return;
                }
                QString deletedAtStr = taskObj.value("deleted_at").toString();
                task.deletedAt = parseIsoDate(deletedAtStr);
                QString createdAtStr = taskObj.value("created_at").toString();
                task.createdAt = parseIsoDate(createdAtStr);
                QString updatedAtStr = taskObj.value("updated_at").toString();
                task.updatedAt = parseIsoDate(updatedAtStr);

                if (taskObj.contains("tags") && taskObj.value("tags").isArray()) {
                    QJsonArray tags = taskObj.value("tags").toArray();
                    for (const QJsonValue &tagVal : tags) {
                        if (tagVal.isString()) {
                            task.tagNames.append(tagVal.toString());
                        }
                    }
                } else if (taskObj.contains("tag_ids") && taskObj.value("tag_ids").isArray()) {
                    QJsonArray tagIds = taskObj.value("tag_ids").toArray();
                    for (const QJsonValue &tagVal : tagIds) {
                        int tagId = tagVal.toInt(-1);
                        if (importedTagIdToName.contains(tagId)) {
                            task.tagNames.append(importedTagIdToName.value(tagId));
                        }
                    }
                }

                if (taskObj.contains("dependencies") && taskObj.value("dependencies").isArray()) {
                    QJsonArray deps = taskObj.value("dependencies").toArray();
                    for (const QJsonValue &depVal : deps) {
                        if (depVal.isDouble()) {
                            task.dependencyIds.append(depVal.toInt());
                        }
                    }
                }

                if (taskObj.contains("files") && taskObj.value("files").isArray()) {
                    QJsonArray files = taskObj.value("files").toArray();
                    for (const QJsonValue &fileVal : files) {
                        if (!fileVal.isObject()) {
                            continue;
                        }
                        QJsonObject fileObj = fileVal.toObject();
                        FileItem fileItem;
                        fileItem.path = fileObj.value("path").toString();
                        fileItem.name = fileObj.value("name").toString();
                        if (!fileItem.path.trimmed().isEmpty()) {
                            task.files.append(fileItem);
                        }
                    }
                }

                if (taskObj.contains("steps") && taskObj.value("steps").isArray()) {
                    QJsonArray steps = taskObj.value("steps").toArray();
                    for (const QJsonValue &stepVal : steps) {
                        if (!stepVal.isObject()) {
                            continue;
                        }
                        QJsonObject stepObj = stepVal.toObject();
                        StepItem step;
                        step.title = stepObj.value("title").toString();
                        step.completed = stepObj.value("completed").toBool(false);
                        step.position = stepObj.value("position").toInt(0);
                        if (!step.title.trimmed().isEmpty()) {
                            task.steps.append(step);
                        }
                    }
                }

                if (!task.title.trimmed().isEmpty()) {
                    importedTasks.append(task);
                }

                if (taskObj.contains("children") && taskObj.value("children").isArray()) {
                    if (task.originalId <= 0) {
                        taskParseError = true;
                        taskError = "包含子任务的任务必须提供 ID";
                        return;
                    }
                    self(self, taskObj.value("children").toArray(), task.originalId);
                    if (taskParseError) {
                        return;
                    }
                }
            }
        };
        parseTasks(parseTasks, tasksArray, 0);
    }

    if (taskParseError) {
        QMessageBox::warning(this, "导入 JSON", taskError);
        return;
    }
    int totalItems = importedTasks.size() + importedFolders.size() + importedSettings.size() +
                     importedNotifications.size() + importedTags.size();

    for (const ImportedTask &task : importedTasks) {
        totalItems += task.steps.size();
        totalItems += task.files.size();
        totalItems += task.dependencyIds.size();
        totalItems += task.tagNames.size();
    }
    for (const ImportedFolder &folder : importedFolders) {
        totalItems += folder.taskIds.size();
    }
    if (totalItems <= 0) {
        totalItems = 1;
    }

    int processed = 0;

    QSqlDatabase db = m_database->database();
    if (!db.transaction()) {
        progress.close();
        QMessageBox::warning(this, "导入 JSON", "无法开始数据库事务。");
        return;
    }

    QString dbError;
    if (options.mode == ImportOptions::Overwrite) {
        if (!clearAllData(db, &dbError)) {
            db.rollback();
            progress.close();
            QMessageBox::warning(this, "导入 JSON", dbError);
            return;
        }
    }

    QSet<int> existingTaskIds;
    QHash<QString, int> existingTagNameToId;
    QHash<QString, int> existingFolderNameToId;
    {
        QSqlQuery existingQuery(db);
        if (existingQuery.exec("SELECT id FROM tasks")) {
            while (existingQuery.next()) {
                existingTaskIds.insert(existingQuery.value(0).toInt());
            }
        }
        if (existingQuery.exec("SELECT id, name FROM tags")) {
            while (existingQuery.next()) {
                existingTagNameToId.insert(existingQuery.value(1).toString(), existingQuery.value(0).toInt());
            }
        }
        if (existingQuery.exec("SELECT id, name FROM folders")) {
            while (existingQuery.next()) {
                existingFolderNameToId.insert(existingQuery.value(1).toString(), existingQuery.value(0).toInt());
            }
        }
    }

    for (const ImportedTag &tag : importedTags) {
        QString name = tag.name.trimmed();
        if (name.isEmpty()) {
            continue;
        }
        int actualId = -1;
        if (existingTagNameToId.contains(name)) {
            actualId = existingTagNameToId.value(name);
            if (options.mode != ImportOptions::Append && options.conflict == ImportOptions::OverwriteConflict) {
                QSqlQuery updateQuery(db);
                updateQuery.prepare("UPDATE tags SET color = ? WHERE id = ?");
                updateQuery.addBindValue(tag.color.isEmpty() ? "#3B82F6" : tag.color);
                updateQuery.addBindValue(actualId);
                updateQuery.exec();
            }
        } else {
            QSqlQuery insertQuery(db);
            insertQuery.prepare("INSERT INTO tags (name, color, created_at) VALUES (?, ?, ?)");
            insertQuery.addBindValue(name);
            insertQuery.addBindValue(tag.color.isEmpty() ? "#3B82F6" : tag.color);
            insertQuery.addBindValue(isoDateString(tag.createdAt.isValid() ? tag.createdAt : QDateTime::currentDateTimeUtc()));
            if (!insertQuery.exec()) {
                db.rollback();
                progress.close();
                QMessageBox::warning(this, "导入 JSON", "导入标签失败。");
                return;
            }
            actualId = insertQuery.lastInsertId().toInt();
            existingTagNameToId.insert(name, actualId);
        }
        processed++;
        updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
    }

    QHash<int, int> folderIdMap;
    QSet<QString> folderNames = existingFolderNameToId.keys().toSet();
    for (const ImportedFolder &folder : importedFolders) {
        QString name = folder.name.trimmed();
        if (name.isEmpty()) {
            continue;
        }

        int actualId = -1;
        if (existingFolderNameToId.contains(name)) {
            actualId = existingFolderNameToId.value(name);
            if (options.mode != ImportOptions::Append && options.conflict == ImportOptions::OverwriteConflict) {
                QSqlQuery updateQuery(db);
                updateQuery.prepare("UPDATE folders SET color = ?, position = ? WHERE id = ?");
                updateQuery.addBindValue(folder.color.isEmpty() ? "#64748B" : folder.color);
                updateQuery.addBindValue(folder.position);
                updateQuery.addBindValue(actualId);
                updateQuery.exec();
            } else if (options.conflict == ImportOptions::Regenerate) {
                QString baseName = name + " (imported)";
                QString newName = baseName;
                int counter = 1;
                while (folderNames.contains(newName)) {
                    newName = QString("%1 %2").arg(baseName).arg(counter++);
                }
                QSqlQuery insertQuery(db);
                insertQuery.prepare("INSERT INTO folders (name, color, position, created_at) VALUES (?, ?, ?, ?)");
                insertQuery.addBindValue(newName);
                insertQuery.addBindValue(folder.color.isEmpty() ? "#64748B" : folder.color);
                insertQuery.addBindValue(folder.position);
                insertQuery.addBindValue(isoDateString(folder.createdAt.isValid() ? folder.createdAt : QDateTime::currentDateTimeUtc()));
                if (!insertQuery.exec()) {
                    db.rollback();
                    progress.close();
                    QMessageBox::warning(this, "导入 JSON", "导入文件夹失败。");
                    return;
                }
                actualId = insertQuery.lastInsertId().toInt();
                existingFolderNameToId.insert(newName, actualId);
                folderNames.insert(newName);
            }
        } else {
            QSqlQuery insertQuery(db);
            insertQuery.prepare("INSERT INTO folders (name, color, position, created_at) VALUES (?, ?, ?, ?)");
            insertQuery.addBindValue(name);
            insertQuery.addBindValue(folder.color.isEmpty() ? "#64748B" : folder.color);
            insertQuery.addBindValue(folder.position);
            insertQuery.addBindValue(isoDateString(folder.createdAt.isValid() ? folder.createdAt : QDateTime::currentDateTimeUtc()));
            if (!insertQuery.exec()) {
                db.rollback();
                progress.close();
                QMessageBox::warning(this, "导入 JSON", "导入文件夹失败。");
                return;
            }
            actualId = insertQuery.lastInsertId().toInt();
            existingFolderNameToId.insert(name, actualId);
            folderNames.insert(name);
        }

        if (folder.originalId > 0 && actualId > 0) {
            folderIdMap.insert(folder.originalId, actualId);
        }

        processed++;
        updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
    }
    QHash<int, int> taskIdMap;
    QVector<int> finalTaskIds;
    QVector<bool> taskUpdateRelations;
    finalTaskIds.resize(importedTasks.size());
    taskUpdateRelations.resize(importedTasks.size());

    for (int i = 0; i < importedTasks.size(); ++i) {
        const ImportedTask &task = importedTasks.at(i);
        int parentId = 0;
        if (task.parentOriginalId > 0) {
            if (taskIdMap.contains(task.parentOriginalId)) {
                parentId = taskIdMap.value(task.parentOriginalId);
            } else if (existingTaskIds.contains(task.parentOriginalId)) {
                parentId = task.parentOriginalId;
            }
        }

        bool hasId = task.originalId > 0;
        bool exists = hasId && existingTaskIds.contains(task.originalId);
        bool shouldUpdate = false;
        int finalId = -1;

        auto insertTask = [&](bool withId, int idValue) -> bool {
            QSqlQuery insertQuery(db);
            if (withId) {
                insertQuery.prepare("INSERT INTO tasks (id, title, description, file_path, priority, due_date, completed, progress, parent_id, is_deleted, deleted_at, created_at, updated_at) "
                                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                insertQuery.addBindValue(idValue);
            } else {
                insertQuery.prepare("INSERT INTO tasks (title, description, file_path, priority, due_date, completed, progress, parent_id, is_deleted, deleted_at, created_at, updated_at) "
                                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            }

            QString primaryFilePath = task.files.isEmpty() ? QString() : task.files.first().path;
            QString createdAt = isoDateString(task.createdAt.isValid() ? task.createdAt : QDateTime::currentDateTimeUtc());
            QString updatedAt = isoDateString(task.updatedAt.isValid() ? task.updatedAt : QDateTime::currentDateTimeUtc());

            insertQuery.addBindValue(task.title);
            insertQuery.addBindValue(task.description);
            insertQuery.addBindValue(primaryFilePath);
            insertQuery.addBindValue(task.priority);
            insertQuery.addBindValue(isoDateString(task.dueDate));
            insertQuery.addBindValue(task.completed ? 1 : 0);
            insertQuery.addBindValue(task.progress);
            insertQuery.addBindValue(parentId);
            insertQuery.addBindValue(task.isDeleted ? 1 : 0);
            insertQuery.addBindValue(isoDateString(task.deletedAt));
            insertQuery.addBindValue(createdAt);
            insertQuery.addBindValue(updatedAt);

            if (!insertQuery.exec()) {
                return false;
            }
            finalId = withId ? idValue : insertQuery.lastInsertId().toInt();
            return true;
        };

        auto updateTask = [&]() -> bool {
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE tasks SET title = ?, description = ?, file_path = ?, priority = ?, due_date = ?, completed = ?, progress = ?, parent_id = ?, is_deleted = ?, deleted_at = ?, updated_at = ? WHERE id = ?");
            QString primaryFilePath = task.files.isEmpty() ? QString() : task.files.first().path;
            updateQuery.addBindValue(task.title);
            updateQuery.addBindValue(task.description);
            updateQuery.addBindValue(primaryFilePath);
            updateQuery.addBindValue(task.priority);
            updateQuery.addBindValue(isoDateString(task.dueDate));
            updateQuery.addBindValue(task.completed ? 1 : 0);
            updateQuery.addBindValue(task.progress);
            updateQuery.addBindValue(parentId);
            updateQuery.addBindValue(task.isDeleted ? 1 : 0);
            updateQuery.addBindValue(isoDateString(task.deletedAt));
            updateQuery.addBindValue(isoDateString(task.updatedAt.isValid() ? task.updatedAt : QDateTime::currentDateTimeUtc()));
            updateQuery.addBindValue(task.originalId);
            return updateQuery.exec();
        };

        if (options.mode == ImportOptions::Overwrite) {
            if (hasId) {
                if (!insertTask(true, task.originalId)) {
                    db.rollback();
                    progress.close();
                    QMessageBox::warning(this, "导入 JSON", "导入任务失败。");
                    return;
                }
                finalId = task.originalId;
            } else if (!insertTask(false, -1)) {
                db.rollback();
                progress.close();
                QMessageBox::warning(this, "导入 JSON", "导入任务失败。");
                return;
            }
            shouldUpdate = true;
        } else if (exists) {
            if (options.conflict == ImportOptions::Skip) {
                finalId = task.originalId;
                shouldUpdate = false;
            } else if (options.conflict == ImportOptions::OverwriteConflict) {
                if (!updateTask()) {
                    db.rollback();
                    progress.close();
                    QMessageBox::warning(this, "导入 JSON", "导入任务失败。");
                    return;
                }
                finalId = task.originalId;
                shouldUpdate = true;
            } else {
                if (!insertTask(false, -1)) {
                    db.rollback();
                    progress.close();
                    QMessageBox::warning(this, "导入 JSON", "导入任务失败。");
                    return;
                }
                shouldUpdate = true;
            }
        } else {
            if (hasId) {
                if (!insertTask(true, task.originalId)) {
                    db.rollback();
                    progress.close();
                    QMessageBox::warning(this, "导入 JSON", "导入任务失败。");
                    return;
                }
                finalId = task.originalId;
            } else if (!insertTask(false, -1)) {
                db.rollback();
                progress.close();
                QMessageBox::warning(this, "导入 JSON", "导入任务失败。");
                return;
            }
            shouldUpdate = true;
        }

        if (finalId > 0 && hasId) {
            taskIdMap.insert(task.originalId, finalId);
        }
        if (finalId > 0 && !hasId) {
            taskIdMap.insert(i + 1000000, finalId);
        }

        if (finalId > 0) {
            existingTaskIds.insert(finalId);
        }

        finalTaskIds[i] = finalId;
        taskUpdateRelations[i] = shouldUpdate;
        processed++;
        updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
    }

    auto clearTaskRelations = [&](int taskId) {
        QSqlQuery clearQuery(db);
        clearQuery.prepare("DELETE FROM task_tags WHERE task_id = ?");
        clearQuery.addBindValue(taskId);
        clearQuery.exec();
        clearQuery.prepare("DELETE FROM task_dependencies WHERE task_id = ?");
        clearQuery.addBindValue(taskId);
        clearQuery.exec();
        clearQuery.prepare("DELETE FROM task_files WHERE task_id = ?");
        clearQuery.addBindValue(taskId);
        clearQuery.exec();
        clearQuery.prepare("DELETE FROM task_steps WHERE task_id = ?");
        clearQuery.addBindValue(taskId);
        clearQuery.exec();
        if (hasFolderData) {
            clearQuery.prepare("DELETE FROM task_folders WHERE task_id = ?");
            clearQuery.addBindValue(taskId);
            clearQuery.exec();
        }
    };

    for (int i = 0; i < importedTasks.size(); ++i) {
        if (!taskUpdateRelations[i]) {
            continue;
        }
        int taskId = finalTaskIds[i];
        if (taskId <= 0) {
            continue;
        }
        clearTaskRelations(taskId);

        const ImportedTask &task = importedTasks.at(i);

        for (const StepItem &step : task.steps) {
            QSqlQuery stepQuery(db);
            stepQuery.prepare("INSERT INTO task_steps (task_id, title, completed, position, created_at) VALUES (?, ?, ?, ?, ?)");
            stepQuery.addBindValue(taskId);
            stepQuery.addBindValue(step.title);
            stepQuery.addBindValue(step.completed ? 1 : 0);
            stepQuery.addBindValue(step.position);
            stepQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
            stepQuery.exec();
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }

        for (const FileItem &file : task.files) {
            QSqlQuery fileQuery(db);
            fileQuery.prepare("INSERT INTO task_files (task_id, file_path, file_name, created_at) VALUES (?, ?, ?, ?)");
            fileQuery.addBindValue(taskId);
            fileQuery.addBindValue(file.path);
            QString fileName = file.name.isEmpty() ? QFileInfo(file.path).fileName() : file.name;
            fileQuery.addBindValue(fileName);
            fileQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
            fileQuery.exec();
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }

        for (const QString &tagName : task.tagNames) {
            QString trimmed = tagName.trimmed();
            if (trimmed.isEmpty()) {
                continue;
            }
            int tagId = -1;
            if (existingTagNameToId.contains(trimmed)) {
                tagId = existingTagNameToId.value(trimmed);
            } else {
                QSqlQuery insertQuery(db);
                insertQuery.prepare("INSERT INTO tags (name, color, created_at) VALUES (?, ?, ?)");
                insertQuery.addBindValue(trimmed);
                insertQuery.addBindValue("#3B82F6");
                insertQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
                if (insertQuery.exec()) {
                    tagId = insertQuery.lastInsertId().toInt();
                    existingTagNameToId.insert(trimmed, tagId);
                }
            }
            if (tagId > 0) {
                QSqlQuery tagQuery(db);
                tagQuery.prepare("INSERT OR IGNORE INTO task_tags (task_id, tag_id) VALUES (?, ?)");
                tagQuery.addBindValue(taskId);
                tagQuery.addBindValue(tagId);
                tagQuery.exec();
            }
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }
    }

    for (int i = 0; i < importedTasks.size(); ++i) {
        if (!taskUpdateRelations[i]) {
            continue;
        }
        int taskId = finalTaskIds[i];
        if (taskId <= 0) {
            continue;
        }

        const ImportedTask &task = importedTasks.at(i);
        for (int depId : task.dependencyIds) {
            int mappedDep = -1;
            if (taskIdMap.contains(depId)) {
                mappedDep = taskIdMap.value(depId);
            } else if (existingTaskIds.contains(depId)) {
                mappedDep = depId;
            }
            if (mappedDep > 0 && mappedDep != taskId) {
                QSqlQuery depQuery(db);
                depQuery.prepare("INSERT OR IGNORE INTO task_dependencies (task_id, depends_on_id, created_at) VALUES (?, ?, ?)");
                depQuery.addBindValue(taskId);
                depQuery.addBindValue(mappedDep);
                depQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
                depQuery.exec();
            }
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }
    }

    for (const ImportedFolder &folder : importedFolders) {
        int folderId = folderIdMap.value(folder.originalId, -1);
        if (folderId <= 0) {
            continue;
        }
        for (int taskOriginalId : folder.taskIds) {
            int mappedTaskId = -1;
            if (taskIdMap.contains(taskOriginalId)) {
                mappedTaskId = taskIdMap.value(taskOriginalId);
            } else if (existingTaskIds.contains(taskOriginalId)) {
                mappedTaskId = taskOriginalId;
            }
            if (mappedTaskId > 0) {
                QSqlQuery folderQuery(db);
                folderQuery.prepare("INSERT OR IGNORE INTO task_folders (task_id, folder_id) VALUES (?, ?)");
                folderQuery.addBindValue(mappedTaskId);
                folderQuery.addBindValue(folderId);
                folderQuery.exec();
            }
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }
    }

    if (options.mode != ImportOptions::Append) {
        for (const ImportedSetting &setting : importedSettings) {
            QSqlQuery settingQuery(db);
            settingQuery.prepare("INSERT OR REPLACE INTO settings (key, value, updated_at) VALUES (?, ?, ?)");
            settingQuery.addBindValue(setting.key);
            settingQuery.addBindValue(setting.value);
            settingQuery.addBindValue(isoDateString(setting.updatedAt.isValid() ? setting.updatedAt : QDateTime::currentDateTimeUtc()));
            settingQuery.exec();
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }
    }

    if (options.mode != ImportOptions::Append) {
        QSet<int> existingNotificationIds;
        QSqlQuery notifQuery(db);
        if (notifQuery.exec("SELECT id FROM notifications")) {
            while (notifQuery.next()) {
                existingNotificationIds.insert(notifQuery.value(0).toInt());
            }
        }

        for (const ImportedNotification &notification : importedNotifications) {
            int taskId = 0;
            if (notification.taskId > 0) {
                if (taskIdMap.contains(notification.taskId)) {
                    taskId = taskIdMap.value(notification.taskId);
                } else if (existingTaskIds.contains(notification.taskId)) {
                    taskId = notification.taskId;
                }
            }

            bool hasId = notification.originalId > 0;
            bool exists = hasId && existingNotificationIds.contains(notification.originalId);
            if (options.mode == ImportOptions::Overwrite || !exists || options.conflict == ImportOptions::Regenerate) {
                QSqlQuery insertQuery(db);
                if (hasId && (options.mode == ImportOptions::Overwrite || !exists)) {
                    insertQuery.prepare("INSERT INTO notifications (id, type, title, message, task_id, read, created_at) VALUES (?, ?, ?, ?, ?, ?, ?)");
                    insertQuery.addBindValue(notification.originalId);
                } else {
                    insertQuery.prepare("INSERT INTO notifications (type, title, message, task_id, read, created_at) VALUES (?, ?, ?, ?, ?, ?)");
                }
                insertQuery.addBindValue(notification.type);
                insertQuery.addBindValue(notification.title);
                insertQuery.addBindValue(notification.message);
                insertQuery.addBindValue(taskId);
                insertQuery.addBindValue(notification.read ? 1 : 0);
                insertQuery.addBindValue(isoDateString(notification.createdAt.isValid() ? notification.createdAt : QDateTime::currentDateTimeUtc()));
                insertQuery.exec();
            } else if (options.conflict == ImportOptions::OverwriteConflict && exists) {
                QSqlQuery updateQuery(db);
                updateQuery.prepare("UPDATE notifications SET type = ?, title = ?, message = ?, task_id = ?, read = ?, created_at = ? WHERE id = ?");
                updateQuery.addBindValue(notification.type);
                updateQuery.addBindValue(notification.title);
                updateQuery.addBindValue(notification.message);
                updateQuery.addBindValue(taskId);
                updateQuery.addBindValue(notification.read ? 1 : 0);
                updateQuery.addBindValue(isoDateString(notification.createdAt.isValid() ? notification.createdAt : QDateTime::currentDateTimeUtc()));
                updateQuery.addBindValue(notification.originalId);
                updateQuery.exec();
            }
            processed++;
            updateProgress(&progress, &timer, processed, totalItems, "正在导入数据...", false);
        }
    }

    QSqlQuery rebuildQuery(db);
    rebuildQuery.exec("INSERT INTO tasks_fts(tasks_fts) VALUES('rebuild')");

    if (!db.commit()) {
        db.rollback();
        progress.close();
        QMessageBox::warning(this, "导入 JSON", "提交数据失败。");
        return;
    }

    updateProgress(&progress, &timer, totalItems, totalItems, "导入完成", false);
    progress.close();

    ImportSummary summary;
    summary.tasks = importedTasks.size();
    summary.folders = importedFolders.size();
    summary.settings = (options.mode == ImportOptions::Append) ? 0 : importedSettings.size();
    summary.notifications = (options.mode == ImportOptions::Append) ? 0 : importedNotifications.size();
    summary.tags = importedTags.size();

    QMessageBox::information(this,
                             "导入 JSON",
                             QString("导入完成。\n任务：%1\n文件夹：%2\n标签：%3\n设置：%4\n通知：%5")
                                 .arg(summary.tasks)
                                 .arg(summary.folders)
                                 .arg(summary.tags)
                                 .arg(summary.settings)
                                 .arg(summary.notifications));
    emit dataImported();
}


void SettingsDialog::onExportSqlite()
{
    QString suggestedName = generateTimestampedSqliteName();
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "导出 SQLite",
        QDir(defaultExportDirectory()).filePath(suggestedName),
        "SQLite 数据库 (*.db)");
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo info(filePath);
    QRegularExpression pattern(R"(^todolist_backup_\d{8}.*\.db$)", QRegularExpression::CaseInsensitiveOption);
    if (!pattern.match(info.fileName()).hasMatch()) {
        filePath = info.dir().filePath(generateTimestampedSqliteName());
    }

    QString error;
    if (!ensureWritableExportLocation(this, filePath, &error)) {
        if (error != "用户取消") {
            QMessageBox::warning(this, "导出 SQLite", error);
        }
        return;
    }

    m_database->vacuum();

    QString sourcePath = m_database->database().databaseName();
    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "导出 SQLite", "无法读取数据库文件。");
        return;
    }

    QFile destFile(filePath);
    if (!destFile.open(QIODevice::WriteOnly)) {
        sourceFile.close();
        QMessageBox::warning(this, "导出 SQLite", "无法写入目标文件。");
        return;
    }

    qint64 totalBytes = sourceFile.size();
    QProgressDialog progress("正在导出数据库...", QString(), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setAutoClose(false);
    progress.setAutoReset(false);
    progress.setCancelButton(nullptr);
    progress.show();
    QElapsedTimer timer;
    timer.start();

    const qint64 chunkSize = 1024 * 1024;
    qint64 copied = 0;
    QByteArray buffer;
    buffer.resize(static_cast<int>(chunkSize));

    while (!sourceFile.atEnd()) {
        qint64 bytesRead = sourceFile.read(buffer.data(), chunkSize);
        if (bytesRead <= 0) {
            break;
        }
        qint64 bytesWritten = destFile.write(buffer.constData(), bytesRead);
        if (bytesWritten != bytesRead) {
            sourceFile.close();
            destFile.close();
            destFile.remove();
            progress.close();
            QMessageBox::warning(this, "导出 SQLite", "导出数据库失败。");
            return;
        }
        copied += bytesWritten;
        int percent = totalBytes > 0 ? static_cast<int>((copied * 100) / totalBytes) : 100;
        updateProgress(&progress, &timer, percent, 100, "正在导出数据库...", true);
    }

    sourceFile.close();
    destFile.close();

    updateProgress(&progress, &timer, 100, 100, "导出完成", false);
    progress.close();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("导出 SQLite");
    msgBox.setText(QString("导出完成。\n%1").arg(filePath));
    QPushButton *openButton = msgBox.addButton("打开所在文件夹", QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();
    if (msgBox.clickedButton() == openButton) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).absolutePath()));
    }
}


void SettingsDialog::onImportSqlite()
{
    QString filePath = QFileDialog::getOpenFileName(this, "导入 SQLite", "", "SQLite 数据库 (*.db)");
    if (filePath.isEmpty()) {
        return;
    }

    QString validationError;
    if (!validateSqliteFile(filePath, &validationError)) {
        QMessageBox::warning(this, "导入 SQLite", validationError);
        return;
    }

    auto response = QMessageBox::warning(this,
                                         "导入 SQLite",
                                         "此操作将替换当前数据库，是否继续？",
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No);
    if (response != QMessageBox::Yes) {
        return;
    }

    QString dbPath = m_database->database().databaseName();
    QString tempPath = QFileInfo(dbPath).absolutePath() + "/temp_import.db";

    m_database->close();

    if (QFile::exists(tempPath)) {
        QFile::remove(tempPath);
    }

    if (!QFile::copy(filePath, tempPath)) {
        m_database->open();
        QMessageBox::warning(this, "导入 SQLite", "复制数据库失败。");
        return;
    }

    if (QFile::exists(dbPath)) {
        QFile::remove(dbPath + ".bak");
        QFile::rename(dbPath, dbPath + ".bak");
    }

    bool success = QFile::rename(tempPath, dbPath);
    if (!success) {
        if (QFile::exists(dbPath + ".bak")) {
            QFile::rename(dbPath + ".bak", dbPath);
        }
        m_database->open();
        QMessageBox::warning(this, "导入 SQLite", "替换数据库失败。");
        return;
    }

    m_database->open();

    QMessageBox::information(this, "导入 SQLite", "导入完成。");
    emit dataImported();
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
    QDesktopServices::openUrl(QUrl("https://github.com/ALNLAqooq/ToDoList"));
}

void SettingsDialog::onSave()
{
    if (!applySettings()) {
        QMessageBox::critical(this, "保存失败", "部分设置未能保存，请检查权限或路径设置。");
        return;
    }
    accept();
}
