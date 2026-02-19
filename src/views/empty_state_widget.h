#ifndef EMPTY_STATE_WIDGET_H
#define EMPTY_STATE_WIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;

class EmptyStateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmptyStateWidget(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setActionText(const QString &text);
    void setActionVisible(bool visible);

signals:
    void actionTriggered();

private:
    QLabel *m_titleLabel;
    QLabel *m_descriptionLabel;
    QPushButton *m_actionButton;
};

#endif // EMPTY_STATE_WIDGET_H
