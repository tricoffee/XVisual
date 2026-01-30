#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPointF>
#include <QGraphicsLineItem>
#include <QFont>
#include <QGraphicsTextItem>
#include <QColor>

#include <unordered_map>
#include <string>

#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include "Common/constants.h"
#include "MainWindow/mainwindow.h"
#include "Common/ColleagueData.h"

#include "XGraph/GraphNode.h"
#include "Core/Executor/NodeState.h"

namespace XVisual {

class DiagramScene : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit DiagramScene(QMenu* itemMenu, QObject* parent = nullptr);
    void setItemMenu(QMenu* itemMenu) { myItemMenu = itemMenu; }
	QFont font() const { return myFont; }
	QColor textColor() const { return myTextColor; }
	QColor itemColor() const { return myItemColor; }
	QColor lineColor() const { return myLineColor; }
	void setTextColor(const QColor& color);
	void setItemColor(const QColor& color);
	void setLineColor(const QColor& color);
	void setFont(const QFont& font);
	void setDiagramType(DiagramType type);
	void setDiagramState(DiagramState state);
	void setDiagramName(const QString& name);
	void setView(QWidget* view);
	void addItemByJson(ColleagueData data);
	void addArrowByJson(const std::pair<std::string, std::set<std::string>>& itemChildren,
		const std::unordered_map<std::string, std::unordered_map<std::string, SourceFrom>>& qItemIdSourceFromUMap);

	/**
	 * PR-4.5: 节点状态可视化
	 */
	void setNodeState(const std::string& nodeId, NodeState state);
	void clearAllNodeStates();
	void resetAllNodeStatesToPending();
	NodeState getNodeState(const std::string& nodeId) const;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
private:
#ifdef DEBUG_DiagramScene
	void findItemIndex(const std::vector<std::shared_ptr<GraphNode>>& graph, QGraphicsItem* iiItem, int iiIndex);
#endif 
	bool isThisType(int type) const;
	DiagramType diagramType;
	DiagramState diagramState;
	QString diagramName;
	QMenu* myItemMenu;
	QPointF startPoint;
	QGraphicsLineItem* lineItem;
	QFont myFont;
	XBaseItem* item;
	QColor myTextColor;
	QColor myItemColor;
	QColor myLineColor;
	XArrow* arrow;
	QWidget* xView;
	GraphicsWidget* graphicsWidget = nullptr;

	// PR-4.5: 节点状态 overlay 管理
	// nodeId -> overlay，overlay 作为对应 Item 的子对象存在
	std::unordered_map<std::string, class NodeStateOverlay*> nodeOverlays_;
	
	// 根据 nodeId 找到对应的 QGraphicsItem
	QGraphicsItem* findItemByNodeId(const std::string& nodeId) const;
	// 获取或创建 overlay
	class NodeStateOverlay* getOrCreateOverlay(const std::string& nodeId);
};

} // namespace XVisual

#endif //DIAGRAMSCENE_H
