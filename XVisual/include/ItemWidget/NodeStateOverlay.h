#ifndef XVISUAL_ITEMWIDGET_NODESTATEOVERLAY_H
#define XVISUAL_ITEMWIDGET_NODESTATEOVERLAY_H

#include <QGraphicsItem>
#include <QTimer>
#include <QColor>

#include "Core/Executor/NodeState.h"

namespace XVisual {

/**
 * NodeStateOverlay: 节点状态可视化覆盖层
 * 
 * 用于在节点图元上绘制状态边框和角标，不修改原有节点的 paint 逻辑。
 * 
 * 视觉效果:
 * - Pending:   灰色实线边框
 * - Ready:     蓝色实线边框
 * - Running:   橙色实线边框 + 脉冲动画
 * - Completed: 绿色实线边框 + ✓ 角标
 * - Failed:    红色实线边框 + ✗ 角标
 * - Skipped:   灰色虚线边框 + — 角标
 * - Canceled:  灰色虚线边框
 */
class NodeStateOverlay : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit NodeStateOverlay(QGraphicsItem* targetItem, QObject* parent = nullptr);
    ~NodeStateOverlay() override;

    /**
     * 设置节点状态
     */
    void setState(NodeState state);
    NodeState state() const { return state_; }

    /**
     * 关联的目标图元
     */
    QGraphicsItem* targetItem() const { return targetItem_; }

    /**
     * 设置是否显示角标
     */
    void setShowBadge(bool show);
    bool showBadge() const { return showBadge_; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private slots:
    void onPulseTimer();

private:
    void startPulseAnimation();
    void stopPulseAnimation();
    void updatePosition();

    QColor stateColor() const;
    Qt::PenStyle statePenStyle() const;
    QString stateIcon() const;

    // 流水灯动画辅助方法
    void drawRunningAnimation(QPainter* painter, const QRectF& borderRect, const QColor& baseColor);
    QPointF getPointOnBorder(const QRectF& rect, qreal position, qreal perimeter) const;

    QGraphicsItem* targetItem_ = nullptr;
    NodeState state_ = NodeState::Pending;
    bool showBadge_ = true;

    // 脉冲动画相关
    QTimer* pulseTimer_ = nullptr;
    qreal pulsePhase_ = 0.0;
    static constexpr qreal kPulseSpeed = 0.1;  // 每次定时器触发的相位增量
    static constexpr int kPulseIntervalMs = 50;  // 定时器间隔

    // 边框样式
    static constexpr qreal kBorderWidth = 3.0;
    static constexpr qreal kBorderPadding = 4.0;  // 边框与目标图元的间距
    static constexpr qreal kBadgeSize = 16.0;     // 角标大小
};

} // namespace XVisual

#endif // XVISUAL_ITEMWIDGET_NODESTATEOVERLAY_H
