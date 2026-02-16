#include "content_area.h"
#include "../utils/logger.h"

ContentArea::ContentArea(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_groupLabel(nullptr)
    , m_placeholderLabel(nullptr)
    , m_currentGroup("所有任务")
{
    setupUI();
    LOG_INFO("ContentArea", "Content area widget created");
}

ContentArea::~ContentArea()
{
}

void ContentArea::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);

    m_groupLabel = new QLabel("所有任务", this);
    m_groupLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0F172A;");

    m_placeholderLabel = new QLabel("暂无任务。点击 + 按钮添加新任务。", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: #94A3B8; padding: 50px;");

    m_mainLayout->addWidget(m_groupLabel);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_placeholderLabel);
    m_mainLayout->addStretch();

    LOG_INFO("ContentArea", "Content area UI setup complete");
}

void ContentArea::setCurrentGroup(const QString &group)
{
    m_currentGroup = group;
    m_groupLabel->setText(group);
    LOG_INFO("ContentArea", QString("Current group set to: %1").arg(group));
}

QString ContentArea::getCurrentGroup() const
{
    return m_currentGroup;
}
