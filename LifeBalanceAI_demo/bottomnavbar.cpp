#include "bottomnavbar.h"
#include "designtokens.h"
#include "platformlayoutpolicy.h"

#ifndef Q_OS_ANDROID
#include <QGraphicsDropShadowEffect>
#endif
#include <QEasingCurve>
#include <QFont>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QTimer>

BottomNavBar::BottomNavBar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void BottomNavBar::setupUi()
{
#ifdef Q_OS_ANDROID
    setFixedHeight(LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavContentHeight());
#else
    setFixedHeight(66);
#endif
    setObjectName(QStringLiteral("bottomNavBar"));
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
    setStyleSheet(QStringLiteral(
        "#bottomNavBar{background:%1;border-top:1px solid %2;}")
        .arg(DesignTokens::bgCard(), DesignTokens::borderLight()));

#ifndef Q_OS_ANDROID
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(14);
    shadow->setOffset(0, -3);
    shadow->setColor(QColor(26, 26, 26, 18));
    setGraphicsEffect(shadow);
#endif

    m_activePill = new QWidget(this);
    m_activePill->setStyleSheet(QStringLiteral("background:transparent;border-radius:18px;"));
    m_activePill->hide();

    m_layout = new QHBoxLayout(this);
#ifdef Q_OS_ANDROID
    m_layout->setContentsMargins(8, 3, 8, 6);
    m_layout->setSpacing(4);
#else
    m_layout->setContentsMargins(8, 6, 8, 8);
    m_layout->setSpacing(4);
#endif

    for (int i = 0; i < 4; ++i)
        m_items.append(addNavItem(i));

    QTimer::singleShot(0, this, [this]() {
        if (m_activePill)
            m_activePill->hide();
        updateActiveState(m_currentIndex);
    });

#ifdef Q_OS_ANDROID
    setBottomSafeAreaInset(LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomSafeAreaInset());
#endif
}

void BottomNavBar::setBottomSafeAreaInset(int inset)
{
#ifdef Q_OS_ANDROID
    const int normalizedInset = qMax(0, inset);
    const int desiredHeight = LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavContentHeight()
                              + normalizedInset;
    if (m_bottomSafeAreaInset == normalizedInset && height() == desiredHeight)
        return;

    m_bottomSafeAreaInset = normalizedInset;
    setFixedHeight(desiredHeight);
    if (m_layout)
        m_layout->setContentsMargins(8, 3, 8, 6 + m_bottomSafeAreaInset);
    updateGeometry();
    updateActiveState(m_currentIndex);
#else
    Q_UNUSED(inset);
#endif
}

BottomNavBar::NavItem BottomNavBar::addNavItem(int index)
{
    NavItem item;
    static const QString labels[] = {
        QString::fromUtf8("\u9996\u9875"),
        QString::fromUtf8("\u5206\u6790"),
        QString::fromUtf8("\u5468\u62a5"),
        QString::fromUtf8("\u6211\u7684")
    };

    item.container = new QWidget(this);
    item.container->setObjectName(QStringLiteral("navItemContainer%1").arg(index));
#ifdef Q_OS_ANDROID
    item.container->setMinimumHeight(40);
    item.container->setMaximumHeight(40);
#else
    item.container->setMinimumHeight(50);
    item.container->setMaximumHeight(50);
#endif
    item.container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *vbox = new QVBoxLayout(item.container);
    vbox->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_ANDROID
    vbox->setSpacing(2);
#else
    vbox->setSpacing(3);
#endif
    vbox->setAlignment(Qt::AlignCenter);

    item.textLabel = new QLabel(labels[index], item.container);
    item.textLabel->setAlignment(Qt::AlignCenter);
    QFont font = item.textLabel->font();
    const QStringList families = QFontDatabase::families();
    for (const QString &candidate : {
             QStringLiteral("MiSans Medium"),
             QStringLiteral("MiSans"),
             QStringLiteral("MiSans-Regular"),
             QStringLiteral("Noto Sans SC"),
             QStringLiteral("Microsoft YaHei UI")}) {
        if (families.contains(candidate, Qt::CaseInsensitive)) {
            font.setFamily(candidate);
            break;
        }
    }
#ifdef Q_OS_ANDROID
    font.setPointSize(10);
#else
    font.setPointSize(12);
#endif
    font.setWeight(QFont::Medium);
    item.textLabel->setFont(font);
    vbox->addWidget(item.textLabel, 0, Qt::AlignCenter);

    item.indicator = new QWidget(item.container);
    item.indicator->setFixedSize(16, 2);
    item.indicator->setStyleSheet(QStringLiteral("background:%1;border-radius:1px;").arg(DesignTokens::accent()));
    item.indicator->raise();

    item.button = new QPushButton(item.container);
    item.button->setFlat(true);
    item.button->setCursor(Qt::PointingHandCursor);
    item.button->setStyleSheet(QStringLiteral("QPushButton{background:transparent;border:none;}"));
    item.button->setGeometry(item.container->rect());
    item.button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    item.button->hide();

    connect(item.button, &QPushButton::clicked, this, [this, index]() {
        setCurrentIndex(index);
        emit currentChanged(index);
    });

    m_layout->addWidget(item.container, 1);
    return item;
}

QRect BottomNavBar::pillGeometryFor(int index) const
{
    if (index < 0 || index >= m_items.size() || !m_items[index].container)
        return QRect();

    QWidget *container = m_items[index].container;
#ifdef Q_OS_ANDROID
    QPoint topLeft = container->mapTo(const_cast<BottomNavBar *>(this), QPoint(5, 6));
    return QRect(topLeft, QSize(qMax(50, container->width() - 10), 38));
#else
    QPoint topLeft = container->mapTo(const_cast<BottomNavBar *>(this), QPoint(6, 8));
    return QRect(topLeft, QSize(qMax(56, container->width() - 12), 42));
#endif
}

void BottomNavBar::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_items.size())
        return;
    if (index == m_currentIndex) {
        updateActiveState(index);
        return;
    }

    m_currentIndex = index;
    if (m_activePill)
        m_activePill->hide();
    updateActiveState(index);
}

void BottomNavBar::updateActiveState(int index)
{
    for (int i = 0; i < m_items.size(); ++i) {
        bool active = (i == index);
        if (m_items[i].container) {
            const QString selector = QStringLiteral("QWidget#%1").arg(m_items[i].container->objectName());
            m_items[i].container->setStyleSheet(QStringLiteral("%1{background:transparent;border-radius:16px;}").arg(selector));
        }
        m_items[i].textLabel->setStyleSheet(
            QStringLiteral("color:%1;background:transparent;border:none;padding-bottom:0;font-weight:%2;")
                .arg(active ? m_activeColor : m_inactiveColor,
                     active ? QStringLiteral("600") : QStringLiteral("500")));
        m_items[i].indicator->setStyleSheet(
            QStringLiteral("background:%1;border-radius:1px;")
                .arg(active ? DesignTokens::primary() : QStringLiteral("transparent")));
        if (m_items[i].container) {
            const int indicatorWidth = 16;
            m_items[i].indicator->setGeometry(
                (m_items[i].container->width() - indicatorWidth) / 2,
                qMax(0, m_items[i].container->height() - 5),
                indicatorWidth,
                2);
            m_items[i].indicator->raise();
        }
        m_items[i].indicator->setVisible(true);
    }
}

void BottomNavBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event || width() <= 0) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    const int index = qBound(0, (event->position().toPoint().x() * 4) / qMax(1, width()), 3);
    setCurrentIndex(index);
    emit currentChanged(index);
    event->accept();
}

void BottomNavBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    for (auto &item : m_items) {
        if (item.button && item.container)
            item.button->setGeometry(item.container->rect());
    }
    updateActiveState(m_currentIndex);
    if (m_activePill)
        m_activePill->hide();
}
