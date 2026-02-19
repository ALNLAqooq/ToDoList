#include "empty_state_widget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>

EmptyStateWidget::EmptyStateWidget(QWidget *parent)
    : QWidget(parent)
    , m_titleLabel(nullptr)
    , m_descriptionLabel(nullptr)
    , m_actionButton(nullptr)
{
    setObjectName("emptyStateWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignCenter);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("emptyStateTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setWordWrap(true);

    m_descriptionLabel = new QLabel(this);
    m_descriptionLabel->setObjectName("emptyStateDescription");
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setWordWrap(true);

    m_actionButton = new QPushButton(this);
    m_actionButton->setObjectName("emptyStateAction");
    m_actionButton->setVisible(false);
    connect(m_actionButton, &QPushButton::clicked, this, &EmptyStateWidget::actionTriggered);

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_descriptionLabel);
    layout->addWidget(m_actionButton, 0, Qt::AlignCenter);
}

void EmptyStateWidget::setTitle(const QString &title)
{
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void EmptyStateWidget::setDescription(const QString &description)
{
    if (m_descriptionLabel) {
        m_descriptionLabel->setText(description);
    }
}

void EmptyStateWidget::setActionText(const QString &text)
{
    if (m_actionButton) {
        m_actionButton->setText(text);
    }
}

void EmptyStateWidget::setActionVisible(bool visible)
{
    if (m_actionButton) {
        m_actionButton->setVisible(visible);
    }
}
