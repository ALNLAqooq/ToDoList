#ifndef CONTENT_AREA_H
#define CONTENT_AREA_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class ContentArea : public QWidget
{
    Q_OBJECT

public:
    explicit ContentArea(QWidget *parent = nullptr);
    ~ContentArea();

    void setCurrentGroup(const QString &group);
    QString getCurrentGroup() const;

private:
    void setupUI();

    QVBoxLayout *m_mainLayout;
    QLabel *m_groupLabel;
    QLabel *m_placeholderLabel;
    QString m_currentGroup;
};

#endif 
