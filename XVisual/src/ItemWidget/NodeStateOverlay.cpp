#include "ItemWidget/NodeStateOverlay.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

// 确保 M_PI 在 Windows 上可用
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
        qDebug() << "NodeStateOverlay::setState: Starting pulse animation";
        startPulseAnimation();
    } else if (state_ != NodeState::Running && oldState == NodeState::Running) {
        qDebug() << "NodeStateOverlay::setState: Stopping pulse animation";
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
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    // 获取目标图元边界
    QRectF targetRect = targetItem_->boundingRect();
    QRectF borderRect = targetRect.adjusted(
        -kBorderPadding, -kBorderPadding, 
        kBorderPadding, kBorderPadding
    );
    
    // 设置画笔
    QColor color = stateColor();
    qreal currentBorderWidth = kBorderWidth;
    
    // 绘制基础边框
    QPen basePen(color, currentBorderWidth, statePenStyle());
    basePen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(basePen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(borderRect, 8.0, 8.0);
    
    // Running 状态时绘制流水灯效果
    if (state_ == NodeState::Running) {
        drawRunningAnimation(painter, borderRect, color);
    }
    
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

void NodeStateOverlay::drawRunningAnimation(QPainter* painter, const QRectF& borderRect, const QColor& baseColor)
{
    // 计算边框周长
    qreal width = borderRect.width();
    qreal height = borderRect.height();
    qreal perimeter = 2 * (width + height);
    
    // 流水灯参数
    const int numDots = 3;           // 亮点数量
    const qreal dotLength = 30.0;    // 每个亮点的长度
    const qreal dotSpacing = perimeter / numDots;  // 亮点间距
    
    // 根据相位计算起始位置
    qreal basePosition = std::fmod(pulsePhase_ / (2 * M_PI) * perimeter, perimeter);
    
    for (int i = 0; i < numDots; ++i) {
        qreal dotPosition = std::fmod(basePosition + i * dotSpacing, perimeter);
        
        // 绘制渐变亮点
        for (int j = 0; j < static_cast<int>(dotLength); j += 3) {
            qreal pos = std::fmod(dotPosition + j, perimeter);
            QPointF point = getPointOnBorder(borderRect, pos, perimeter);
            
            // 渐变透明度：头部最亮，尾部渐暗
            qreal alpha = 1.0 - (static_cast<qreal>(j) / dotLength);
            QColor dotColor = baseColor;
            dotColor.setAlphaF(alpha);
            
            // 渐变大小
            qreal dotSize = 6.0 * alpha + 2.0;
            
            painter->setPen(Qt::NoPen);
            painter->setBrush(dotColor);
            painter->drawEllipse(point, dotSize / 2, dotSize / 2);
        }
    }
}

QPointF NodeStateOverlay::getPointOnBorder(const QRectF& rect, qreal position, qreal perimeter) const
{
    qreal width = rect.width();
    qreal height = rect.height();
    
    // 将位置映射到边框上（顺时针）
    // 上边：0 ~ width
    // 右边：width ~ width + height
    // 下边：width + height ~ 2*width + height
    // 左边：2*width + height ~ perimeter
    
    if (position < width) {
        // 上边（从左到右）
        return QPointF(rect.left() + position, rect.top());
    }
    position -= width;
    
    if (position < height) {
        // 右边（从上到下）
        return QPointF(rect.right(), rect.top() + position);
    }
    position -= height;
    
    if (position < width) {
        // 下边（从右到左）
        return QPointF(rect.right() - position, rect.bottom());
    }
    position -= width;
    
    // 左边（从下到上）
    return QPointF(rect.left(), rect.bottom() - position);
}

void NodeStateOverlay::onPulseTimer()
{
    pulsePhase_ += kPulseSpeed;
    if (pulsePhase_ > 2 * M_PI) {
        pulsePhase_ -= 2 * M_PI;
    }
    qDebug() << "NodeStateOverlay::onPulseTimer: pulsePhase=" << pulsePhase_;
    update();
}

void NodeStateOverlay::startPulseAnimation()
{
    qDebug() << "NodeStateOverlay::startPulseAnimation: pulseTimer_=" << pulseTimer_ 
             << " isActive=" << (pulseTimer_ ? pulseTimer_->isActive() : false);
    if (pulseTimer_ && !pulseTimer_->isActive()) {
        pulsePhase_ = 0.0;
        pulseTimer_->start(kPulseIntervalMs);
        qDebug() << "NodeStateOverlay::startPulseAnimation: Timer started with interval=" << kPulseIntervalMs;
        // 立即触发一次重绘，确保动画开始可见
        update();
    }
}

void NodeStateOverlay::stopPulseAnimation()
{
    if (pulseTimer_ && pulseTimer_->isActive()) {
        pulseTimer_->stop();
        pulsePhase_ = 0.0;
        update();
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
