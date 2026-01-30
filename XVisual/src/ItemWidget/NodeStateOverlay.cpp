#include "ItemWidget/NodeStateOverlay.h"

#include <QPainter>
#include <QGraphicsScene>
#include <cmath>

namespace XVisual {

// 状态颜色定义
static const QColor kColorPending(128, 128, 128);    // 灰色
static const QColor kColorReady(74, 144, 217);       // 蓝色
static const QColor kColorRunning(255, 165, 0);      // 橙色
static const QColor kColorCompleted(40, 167, 69);    // 绿色
static const QColor kColorFailed(220, 53, 69);       // 红色
static const QColor kColorSkipped(128, 128, 128);    // 灰色
static const QColor kColorCanceled(128, 128, 128);   // 灰色

NodeStateOverlay::NodeStateOverlay(QGraphicsItem* targetItem, QObject* parent)
    : QGraphicsObject()
    , targetItem_(targetItem)
{
    Q_UNUSED(parent);
    
    // 设置为目标图元的子对象，这样会随目标移动
    if (targetItem_) {
        setParentItem(targetItem_);
        // 设置 Z 值略高于父对象，确保覆盖层在上面
        setZValue(targetItem_->zValue() + 0.5);
    }
    
    // 不接受鼠标事件，让事件透传到下面的图元
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    
    // 创建脉冲动画定时器
    pulseTimer_ = new QTimer(this);
    connect(pulseTimer_, &QTimer::timeout, this, &NodeStateOverlay::onPulseTimer);
}

NodeStateOverlay::~NodeStateOverlay()
{
    stopPulseAnimation();
}

void NodeStateOverlay::setState(NodeState state)
{
    if (state_ == state)
        return;
    
    NodeState oldState = state_;
    state_ = state;
    
    // 管理脉冲动画
    if (state_ == NodeState::Running && oldState != NodeState::Running) {
        startPulseAnimation();
    } else if (state_ != NodeState::Running && oldState == NodeState::Running) {
        stopPulseAnimation();
    }
    
    // 触发重绘
    update();
}

void NodeStateOverlay::setShowBadge(bool show)
{
    if (showBadge_ == show)
        return;
    showBadge_ = show;
    update();
}

QRectF NodeStateOverlay::boundingRect() const
{
    if (!targetItem_)
        return QRectF();
    
    // 边界包含目标图元的边界 + 边框宽度 + 间距
    QRectF targetRect = targetItem_->boundingRect();
    qreal margin = kBorderPadding + kBorderWidth;
    return targetRect.adjusted(-margin, -margin, margin, margin);
}

void NodeStateOverlay::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (!targetItem_)
        return;
    
    painter->save();
    
    // 获取目标图元边界
    QRectF targetRect = targetItem_->boundingRect();
    QRectF borderRect = targetRect.adjusted(
        -kBorderPadding, -kBorderPadding, 
        kBorderPadding, kBorderPadding
    );
    
    // 设置画笔
    QColor color = stateColor();
    
    // Running 状态时应用脉冲效果
    if (state_ == NodeState::Running) {
        // 脉冲效果：透明度和边框宽度随相位变化
        qreal pulse = 0.5 + 0.5 * std::sin(pulsePhase_);  // 0.0 ~ 1.0
        color.setAlphaF(0.5 + 0.5 * pulse);  // 0.5 ~ 1.0
    }
    
    QPen pen(color, kBorderWidth, statePenStyle());
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    
    // 绘制圆角矩形边框
    painter->drawRoundedRect(borderRect, 8.0, 8.0);
    
    // 绘制角标
    if (showBadge_) {
        QString icon = stateIcon();
        if (!icon.isEmpty()) {
            // 角标位置：右上角
            QRectF badgeRect(
                borderRect.right() - kBadgeSize + 4,
                borderRect.top() - 4,
                kBadgeSize, kBadgeSize
            );
            
            // 绘制角标背景圆
            painter->setPen(Qt::NoPen);
            painter->setBrush(color);
            painter->drawEllipse(badgeRect);
            
            // 绘制角标文字
            painter->setPen(Qt::white);
            QFont font = painter->font();
            font.setBold(true);
            font.setPixelSize(static_cast<int>(kBadgeSize * 0.7));
            painter->setFont(font);
            painter->drawText(badgeRect, Qt::AlignCenter, icon);
        }
    }
    
    painter->restore();
}

void NodeStateOverlay::onPulseTimer()
{
    pulsePhase_ += kPulseSpeed;
    if (pulsePhase_ > 2 * M_PI) {
        pulsePhase_ -= 2 * M_PI;
    }
    update();
}

void NodeStateOverlay::startPulseAnimation()
{
    if (!pulseTimer_->isActive()) {
        pulsePhase_ = 0.0;
        pulseTimer_->start(kPulseIntervalMs);
    }
}

void NodeStateOverlay::stopPulseAnimation()
{
    if (pulseTimer_->isActive()) {
        pulseTimer_->stop();
        pulsePhase_ = 0.0;
    }
}

void NodeStateOverlay::updatePosition()
{
    // 由于是 targetItem_ 的子对象，位置会自动跟随
    // 只需要触发重绘以更新边界
    prepareGeometryChange();
    update();
}

QColor NodeStateOverlay::stateColor() const
{
    switch (state_) {
        case NodeState::Pending:   return kColorPending;
        case NodeState::Ready:     return kColorReady;
        case NodeState::Running:   return kColorRunning;
        case NodeState::Completed: return kColorCompleted;
        case NodeState::Failed:    return kColorFailed;
        case NodeState::Skipped:   return kColorSkipped;
        case NodeState::Canceled:  return kColorCanceled;
        default:                   return kColorPending;
    }
}

Qt::PenStyle NodeStateOverlay::statePenStyle() const
{
    switch (state_) {
        case NodeState::Skipped:
        case NodeState::Canceled:
            return Qt::DashLine;
        default:
            return Qt::SolidLine;
    }
}

QString NodeStateOverlay::stateIcon() const
{
    switch (state_) {
        case NodeState::Completed: return QStringLiteral("✓");
        case NodeState::Failed:    return QStringLiteral("✗");
        case NodeState::Skipped:   return QStringLiteral("—");
        default:                   return QString();
    }
}

} // namespace XVisual
