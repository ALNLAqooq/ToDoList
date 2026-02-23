#ifndef STYLE_UTILS_H
#define STYLE_UTILS_H

#include <QString>

namespace StyleUtils {
inline QString buildCornerRadiusStyle(int radius)
{
    return QString(
        "QLineEdit, QTextEdit, QPlainTextEdit, QComboBox, QDateEdit, QTimeEdit, QDateTimeEdit, "
        "QSpinBox, QDoubleSpinBox, QPushButton, QToolButton, QGroupBox, QMenu, QListWidget, "
        "QTableWidget, QTreeView, QTabBar::tab { border-radius: %1px; }\n"
        "TaskCardWidget { border-radius: %1px; }\n"
    ).arg(radius);
}
}

#endif // STYLE_UTILS_H
