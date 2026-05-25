#include "loadingoverlay.h"
#include "designtokens.h"
#include "motionhelper.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

LoadingOverlay::LoadingOverlay(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("loadingOverlay"));
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(false);
    setVisible(false);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(24, 24, 24, 24);
    outer->addStretch();

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("loadingOverlayCard"));
    card->setMaximumWidth(340);
    card->setMinimumHeight(116);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(22, 20, 22, 20);
    cardLayout->setSpacing(12);

    auto *dotRow = new QWidget(card);
    auto *dotLayout = new QHBoxLayout(dotRow);
    dotLayout->setContentsMargins(0, 0, 0, 0);
    dotLayout->setSpacing(8);
    dotLayout->addStretch();
    for (int i = 0; i < 3; ++i) {
        auto *dot = new QLabel(QString::fromUtf8("\u25CF"), dotRow);
        dot->setObjectName(QStringLiteral("loadingDot"));
        dot->setAlignment(Qt::AlignCenter);
        dot->setFixedSize(18, 18);
        m_dots.append(dot);
        dotLayout->addWidget(dot);
    }
    dotLayout->addStretch();
    cardLayout->addWidget(dotRow);

    m_messageLabel = new QLabel(card);
    m_messageLabel->setObjectName(QStringLiteral("loadingOverlayMessage"));
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);
    cardLayout->addWidget(m_messageLabel);

    outer->addWidget(card, 0, Qt::AlignHCenter);
    outer->addStretch();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &LoadingOverlay::updateDots);
    updateDots();
}

void LoadingOverlay::showLoading(const QString &message)
{
    if (parentWidget())
        setGeometry(parentWidget()->rect());

    m_messageLabel->setText(message.isEmpty()
                                ? QString::fromUtf8("\u6b63\u5728\u8f7b\u8f7b\u6574\u7406\u4e2d...")
                                : message);
    m_frame = 0;
    updateDots();
    show();
    raise();
    m_timer->start(220);
    MotionHelper::fadeRiseIn(this, 6, DesignTokens::MotionBase);
}

void LoadingOverlay::hideLoading()
{
    m_timer->stop();
    hide();
}

void LoadingOverlay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (parentWidget())
        setGeometry(parentWidget()->rect());
}

void LoadingOverlay::updateDots()
{
    for (int i = 0; i < m_dots.size(); ++i) {
        QLabel *dot = m_dots.at(i);
        const bool active = (i == (m_frame % qMax(1, m_dots.size())));
        dot->setProperty("active", active);
        dot->style()->unpolish(dot);
        dot->style()->polish(dot);
    }
    ++m_frame;
}
