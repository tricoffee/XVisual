#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QTextCursor>
#include <QtDebug>
#include "MainWindow/diagramscene.h"
#include "ItemWidget/XTextEdit.h"
#include <mutex>
#include "GlobalStorage/GlobalStorage.h"
#include "GlobalStorage/HandleGlobalStorage.h"
#include "MainWindow/GraphicsWidget.h"
#include "ItemBase/XBaseItem.h"
#include "Core/Runtime/VarBag.h"
#include "Common/StrUtils.h"
#include "ItemWidget/DiagramProxyWidget.h"
#include "ItemWidget/NodeStateOverlay.h"

namespace XVisual {

DiagramScene::DiagramScene(QMenu* itemMenu, QObject* parent): QGraphicsScene(parent)
{
	myItemMenu = itemMenu;
	lineItem = nullptr;
	myItemColor = Qt::white;
	myTextColor = Qt::black;
	myLineColor = Qt::black;
	graphicsWidget = qobject_cast<GraphicsWidget*>(parent);
}

void DiagramScene::setLineColor(const QColor& color)
{
	myLineColor = color;
	if (isThisType(XArrow::Type))
	{
		XArrow* item = qgraphicsitem_cast<XArrow*>(selectedItems().first());
		item->setColor(myLineColor);
		update();
	}
}

void DiagramScene::setTextColor(const QColor& color)
{
	myTextColor = color;
	if (isThisType(XBaseItem::Type))
	{
		XBaseItem* item = qgraphicsitem_cast<XBaseItem*>(selectedItems().first());
		if (item)
		{
			item->setEditColor(myTextColor);
		}
	}
}

void DiagramScene::setItemColor(const QColor& color)
{
	myItemColor = color;
	if (isThisType(XBaseItem::Type))
	{
		XBaseItem* item = qgraphicsitem_cast<XBaseItem*>(selectedItems().first());
		if (item)
		{
			item->setBrush(myItemColor);
		}
	}
}

void DiagramScene::setFont(const QFont& font)
{
	myFont = font;
	if (isThisType(XBaseItem::Type))
	{
		XBaseItem* item = qgraphicsitem_cast<XBaseItem*>(selectedItems().first());
		if (item)
		{
			item->setEditFont(myFont);
		}
	}
}

void DiagramScene::setView(QWidget* mView)
{
	xView = mView;
}

void DiagramScene::setDiagramName(const QString& name)
{
	diagramName = name;
}

void DiagramScene::setDiagramType(DiagramType type)
{
	diagramType = type;
}

void DiagramScene::setDiagramState(DiagramState state)
{
	diagramState = state;
}

void DiagramScene::addItemByJson(ColleagueData data)
{

	XLOG_INFO("data.itemPos.first = " + std::to_string(data.itemPos.first) + ", data.itemPos.second = " + std::to_string(data.itemPos.second), CURRENT_THREAD_ID);

	std::string itemClassStr = stripSuffix(data.itemClassName, "Item");
	item = ItemRegistry::createObject(itemClassStr, graphicsWidget, myItemMenu, nullptr);
	QPointF currentPos(data.itemPos.first, data.itemPos.second);
	item->setPos(currentPos);
	//Unused, item->setColleagueType(data.itemColleagueType);
	item->setUuid(data.itemColleagueId);
	item->setUniqueName(data.itemUniqueName);


	item->setEditText(QString::fromStdString(data.itemUniqueName));


	item->setBrush(myItemColor);
	std::string itemId = item->getUuid();
	addItem(item);
	//Unused, item->getXHandle()->setColleagueType(colleagueType);
	item->getXHandle()->setUuid(data.colleagueId);
	item->getXHandle()->setUniqueName(data.uniqueName);
	item->getXHandle()->setOuterParam(data.outerParamUMap);
	item->getXHandle()->setInnerParam(data.innerParam);

	//Done, set sourceFrom for item->getXHandle()
	for (const auto& pair : data.sourceFromUMap)
	{
		XLOG_INFO("== === ===  pair.first " + pair.first + " === === ===", CURRENT_THREAD_ID);

		item->getXHandle()->setSourceFrom(pair.first, pair.second);
	}

	{
		std::lock_guard<std::mutex> lock(itemMapMutex);
		globalItemMap[itemId] = item;
	}// 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		// 创建标识为itemId的节点
		xGraph.push_back(std::make_shared<GraphNode>(itemId));
	}// 作用域结束时，lock_guard 会自动解锁互斥锁xGraphMutex


	std::string handleId = item->getXHandle()->getUuid();
	{
		std::lock_guard<std::mutex> lock(handleMapMutex);
		globalHandleMap[handleId] = item->getXHandle();
	}// 作用域结束时，lock_guard 会自动解锁互斥锁handleMapMutex

	//将item对应的handleId存储在xHandleGraph
	{
		std::lock_guard<std::mutex> lock(xHandleGraphMutex);
		// 创建标识为itemId的节点
		xHandleGraph.push_back(std::make_shared<GraphNode>(handleId));
	}// 作用域结束时，lock_guard 会自动解锁互斥锁xHandleGraphMutex

	diagramState = DiagramState::Move;
	diagramType = DiagramType::Item;

	XLOG_INFO(" == DiagramScene::addItemByJson == ", CURRENT_THREAD_ID);
}

void DiagramScene::addArrowByJson(const std::pair<std::string, std::set<std::string>>& itemChildren,
	const std::unordered_map<std::string, std::unordered_map<std::string, SourceFrom>>& qItemIdSourceFromUMap)
{
	std::string pItemId = itemChildren.first;
	std::set<std::string> qItemIds = itemChildren.second;

	XBaseItem* pItem = nullptr;
	{
		std::lock_guard<std::mutex> lock(itemMapMutex);
		pItem = globalItemMap[pItemId];
	}// 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex

	XBaseItem* qItem = nullptr;
	for (const auto& qItemId : qItemIds)
	{
		{
			std::lock_guard<std::mutex> lock(itemMapMutex);
			qItem = globalItemMap[qItemId];
		}// 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex


		lineItem = new QGraphicsLineItem(QLineF(pItem->scenePos(), qItem->scenePos()));
		lineItem->setPen(QPen(myLineColor, 2));
		addItem(lineItem);
		diagramState = DiagramState::Move;
		diagramType = DiagramType::Line;

		QPointF pa = lineItem->line().p1();
		QPointF pb = lineItem->line().p2();
		removeItem(lineItem);
		delete lineItem;
	    arrow = new XArrow(graphicsWidget, pItem, qItem); // pItem ---> qItem
		arrow->setColor(myLineColor);
		pItem->addArrow(arrow);
		qItem->addArrow(arrow);
		addItem(arrow);
		//qDebug() << "arrow->setZValue(-9999.99)" << -9999.99;
		QLineF line(pa, pb);
		arrow->setLine(line);
		arrow->setZValue(-9999.99);
		if (!pItem->childs.contains(qItem))
		{
			pItem->childs.emplace_back(qItem);
		}
		if (!qItem->parents.contains(pItem))
		{
			qItem->parents.emplace_back(pItem);
		}
		lineItem = nullptr;
		if (pItem != nullptr)
		{
			XLOG_INFO("DiagramScene::addArrowByJson, pItem != nullptr ...  ", CURRENT_THREAD_ID);
			std::lock_guard<std::mutex> lock(xGraphMutex);
			// 获取节点pItem->getUuid()在xGraph里面的索引pIndex
			int pIndex = XGraph::findNodeIndex(xGraph, pItem->getUuid());
			if (qItem != nullptr)
			{
				XLOG_INFO("DiagramScene::addArrowByJson, endItem != nullptr ...  ", CURRENT_THREAD_ID);
				// 获取节点qItem->getUuid()在xGraph里面的索引qIndex
				int qIndex = XGraph::findNodeIndex(xGraph, qItem->getUuid());
				// 把xGraph[qIndex]设置为xGraph[pIndex]的邻节点, 添加边, 标识为pItemId的节点---》指向---》标识为qItemId的节点(即getUuid()的返回值)的节点, 这个逻辑应该紧跟连线动作释放之后
				xGraph[pIndex]->neighbors.push_back(xGraph[qIndex]);
			}
		}//作用域结束时，lock_guard会自动解锁互斥锁xGraphMutex

		// sourceFromUMap 维护 qItemId 里面每个变量的 sourceFrom
		std::unordered_map<std::string, SourceFrom> sourceFromUMap;
		if (qItemIdSourceFromUMap.find(qItemId) != qItemIdSourceFromUMap.end())
		{
			sourceFromUMap = qItemIdSourceFromUMap.at(qItemId);
		}

		XBaseHandle* qHandle = qItem->getXHandle();
		VarBag& qS = qHandle->getSources();
		std::vector<std::string> qSourceNames = ACQUIRE_NAMES(qS);
		std::unordered_map<std::string, TableData> defaultValuesUMap;
		for (const auto& qSourceName : qSourceNames)
		{
			//IS_MEMBER_FROM_OUTSIDE_STR为true,主要针对需要从流程外部获取的值(比如imagePath,savedModelPath)
			if (IS_MEMBER_FROM_OUTSIDE_STR(qS, qSourceName))
			{
				//do nothing
			}
			else
			{
				XLOG_INFO("DiagramScene::addArrowByJson, uniqueName " + qHandle->getUniqueName(), CURRENT_THREAD_ID);

				//Done, 从 qItemId 的 sourceFromUMap 解析出 qSourceName 的 TableData, 并且 yItemId == pItem->getXHandle()->getUuid() 时, 更新 defaultValuesUMap
				if(sourceFromUMap.find(qSourceName) != sourceFromUMap.end())
				{
					SourceFrom sourceFrom = sourceFromUMap.at(qSourceName);
					std::string yItemId = sourceFrom.itemId; // 这里yItemId实际上是Handle的Id
					std::string yName = sourceFrom.variableName;
					XLOG_INFO("DiagramScene::addArrowByJson, yItemId " + yItemId, CURRENT_THREAD_ID);
					XLOG_INFO("DiagramScene::addArrowByJson, pItemId " + pItem->getXHandle()->getUuid(), CURRENT_THREAD_ID);
					if (yItemId == pItem->getXHandle()->getUuid())
					{
						XBaseHandle* pHandle = pItem->getXHandle();
						VarBag& pD = pHandle->getDests();
						std::string yTypeName = (*GET_MEMBER_TYPE_STR(pD, yName)).name();
						TableData yData{ yItemId,yName,yTypeName };
						defaultValuesUMap[qSourceName] = yData;
					}
				}
			}
		}

		XLOG_INFO("DiagramScene::addArrowByJson, defaultValuesUMap.size() = " + std::to_string(defaultValuesUMap.size()), CURRENT_THREAD_ID);

		// Done, 重写 XArrow::setDefaultYforVariablesX 方法，将参数调整为 std::unordered_map<std::string, TableData>
		// Done, 相应的 XArrow::getDefaultYforVariablesX 方法也要重写

		arrow->setDefaultYforVariablesX(defaultValuesUMap);

	} // end for "for (const auto& qItemId : qItemIds)"

}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

	if (mouseEvent->button() != Qt::LeftButton)
		return;

	if (DiagramType::Item == diagramType && DiagramState::Insert == diagramState)
	{
		std::string itemclass = diagramName.toStdString();
		// 新增参数 GraphicsWidget* graphicsWidget
		item = ItemRegistry::createObject(itemclass, graphicsWidget, myItemMenu, nullptr);
		item->setPos(mouseEvent->scenePos());
		item->setBrush(myItemColor);
		// item->debug();
		std::string itemId = item->getUuid();
		addItem(item);


		{
			std::lock_guard<std::mutex> lock(itemMapMutex); 
			globalItemMap[itemId] = item;
		}// 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex
		{
			std::lock_guard<std::mutex> lock(xGraphMutex);
			// 创建标识为itemId的节点
			xGraph.push_back(std::make_shared<GraphNode>(itemId));
		}// 作用域结束时，lock_guard 会自动解锁互斥锁xGraphMutex


		std::string handleId = item->getXHandle()->getUuid();
		{
			std::lock_guard<std::mutex> lock(handleMapMutex);
			globalHandleMap[handleId] = item->getXHandle();

			// 使用范围循环遍历unordered_map
			for (const auto& pair : globalHandleMap)
			{
				// std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
				std::string keyStr = pair.first;
				std::string uuidStr = pair.second->getUuid();
				XLOG_INFO("globalHandleMap pair, key = " + keyStr + "uuid = " + uuidStr, CURRENT_THREAD_ID);
			}
		}// 作用域结束时，lock_guard 会自动解锁互斥锁handleMapMutex

		//将item对应的handleId存储在xHandleGraph
		{
			std::lock_guard<std::mutex> lock(xHandleGraphMutex);
			// 创建标识为itemId的节点
			xHandleGraph.push_back(std::make_shared<GraphNode>(handleId));
		}// 作用域结束时，lock_guard 会自动解锁互斥锁xHandleGraphMutex

		diagramState = DiagramState::Move;
		diagramType = DiagramType::Item;
		QGraphicsScene::mousePressEvent(mouseEvent);
	}
	else if (DiagramType::Line == diagramType && DiagramState::Insert == diagramState)
	{

		lineItem = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
		lineItem->setPen(QPen(myLineColor, 2));
		//lineItem->setZValue(9999.99);
		addItem(lineItem);
		diagramState = DiagramState::Move;
		diagramType = DiagramType::Line;
		QGraphicsScene::mousePressEvent(mouseEvent);

	}
	else
	{
		QGraphicsScene::mousePressEvent(mouseEvent);
	}
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	if (DiagramState::Move == diagramState && DiagramType::Line == diagramType && lineItem != nullptr)
	{
		QLineF newLine(lineItem->line().p1(), mouseEvent->scenePos());
		lineItem->setLine(newLine);
		//lineItem->setZValue(9999.99);
		diagramState = DiagramState::Move;
		diagramType = DiagramType::Line;
		//这里采用	mouseEvent->ignore();而不是QGraphicsScene::mouseMoveEvent(mouseEvent);其目的是
		//为了避免以拖拽方式绘制两个item之间的链接线时造成item的位移
		mouseEvent->ignore();
	}
	else if (DiagramState::Move == diagramState && DiagramType::Item == diagramType && item != nullptr)
	{
		diagramState = DiagramState::Move;
		diagramType = DiagramType::Item;
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
	else
	{
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}

}

#ifdef DEBUG_DiagramScene
void DiagramScene::findItemIndex(const std::vector<std::shared_ptr<GraphNode>>& graph, QGraphicsItem* iiItem, int iiIndex)
{
	for (const auto& node : graph)
	{
		if (globalItemMap[node->nodeId] == iiItem)
		{
			XBaseItem* xiiItem = dynamic_cast<XBaseItem*>(iiItem);
			XLOG_INFO("DiagramScene::findItemIndex, iiIndex = " + std::to_string(iiIndex) + "xiiItem->getUniqueName() = " + xiiItem->getUniqueName(), CURRENT_THREAD_ID);
			break;
		}
	}
}
#endif 

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	if (Qt::RightButton == mouseEvent->button())
	{
		if (DiagramType::Line == diagramType)
		{
			diagramState = DiagramState::Release;
			diagramType = DiagramType::None;
			if (lineItem != nullptr)
			{
				QPointF pa = lineItem->line().p1();
				QList<QGraphicsItem*> startItems_beforeFilter = items(lineItem->line().p1());
				//XLOG_INFO("DiagramScene::mouseReleaseEvent, startItems_beforeFilter.count() = " + std::to_string(startItems_beforeFilter.count()), CURRENT_THREAD_ID);
				QList<QGraphicsItem*> startItems;
				for (QGraphicsItem* kkItem : startItems_beforeFilter)
				{
					if (!qgraphicsitem_cast<DiagramProxyWidget*>(kkItem))
					{
						startItems.append(kkItem);
					}
				}

#ifdef DEBUG_DiagramScene
				int iiIndex = 0;
				for (const auto& iiItem: startItems)
				{
					std::string type_str = std::to_string(iiItem->Type);
					XLOG_INFO("DiagramScene::mouseReleaseEvent, type_str = " + type_str, CURRENT_THREAD_ID);
					if (iiItem == nullptr)
					{
						XLOG_INFO("DiagramScene::mouseReleaseEvent, iiItem == nullptr ", CURRENT_THREAD_ID);
					}
					else
					{
						XLOG_INFO("DiagramScene::mouseReleaseEvent, iiItem != nullptr ", CURRENT_THREAD_ID);
						iiIndex += 1;
						XLOG_INFO("DiagramScene::mouseReleaseEvent, iiIndex = " + std::to_string(iiIndex), CURRENT_THREAD_ID);
						if (iiItem == lineItem)
						{
							XLOG_INFO("DiagramScene::mouseReleaseEvent, iiIndex = " + std::to_string(iiIndex) + " iiItem == lineItem ", CURRENT_THREAD_ID);
						}
						findItemIndex(xGraph, iiItem, iiIndex);

						//try
						//{
						//	XBaseItem* xxItem = dynamic_cast<XBaseItem*>(iiItem);
						//	XLOG_INFO("DiagramScene::mouseReleaseEvent, xxItem->getClassName() = " + xxItem->getClassName(), CURRENT_THREAD_ID);
						//}
						//catch (std::bad_cast& e)
						//{
						//	//std::cerr << "Caught bad_cast exception: " << e.what() << std::endl;
						//	XLOG_INFO("DiagramScene::mouseReleaseEvent, std::bad_cast ", CURRENT_THREAD_ID);
						//}

						XLOG_INFO("DiagramScene::mouseReleaseEvent, typeid(iiItem).name() = " + std::string(typeid(iiItem).name()), CURRENT_THREAD_ID);

					}
					
				}
#endif 

				if (startItems.count() && startItems.first() == lineItem)
				{
					startItems.removeFirst();
				}
				QPointF pb = lineItem->line().p2();
				QList<QGraphicsItem*> endItems_beforeFilter = items(lineItem->line().p2());
				//XLOG_INFO("DiagramScene::mouseReleaseEvent, endItems_beforeFilter.count() = " + std::to_string(endItems_beforeFilter.count()), CURRENT_THREAD_ID);
				QList<QGraphicsItem*> endItems;
				for (QGraphicsItem* kkItem : endItems_beforeFilter)
				{
					if (!qgraphicsitem_cast<DiagramProxyWidget*>(kkItem))
					{
						endItems.append(kkItem);
					}
				}
				if (endItems.count() && endItems.first() == lineItem)
				{
					endItems.removeFirst();
				}
				removeItem(lineItem);
				delete lineItem;
				XBaseItem* startItem = nullptr;
				XBaseItem* endItem = nullptr;
				if (startItems.count() && endItems.count() &&
					startItems.first()->type() == XBaseItem::Type &&
					endItems.first()->type() == XBaseItem::Type &&
					startItems.first() != endItems.first())
				{
					startItem = qgraphicsitem_cast<XBaseItem*>(startItems.first());
					endItem = qgraphicsitem_cast<XBaseItem*>(endItems.first());
					arrow = new XArrow(graphicsWidget, startItem, endItem);
					arrow->setColor(myLineColor);
					startItem->addArrow(arrow);
					endItem->addArrow(arrow);
					addItem(arrow);
					// qDebug() << "arrow->setZValue(-9999.99)" << -9999.99;
					QLineF line(pa, pb);
					arrow->setLine(line);
					arrow->setZValue(-9999.99);
					if (!startItem->childs.contains(endItem))
					{
						startItem->childs.emplace_back(endItem);
					}
					if (!endItem->parents.contains(startItem))
					{
						endItem->parents.emplace_back(startItem);
					}
				}
				XLOG_INFO("DiagramScene::mouseReleaseEvent, DiagramType::Line == diagramType ...  ", CURRENT_THREAD_ID);
				lineItem = nullptr;
				if (startItem != nullptr)
				{
					XLOG_INFO("DiagramScene::mouseReleaseEvent, startItem != nullptr ...  ", CURRENT_THREAD_ID);
					std::lock_guard<std::mutex> lock(xGraphMutex);
					// 获取节点startItem->getUuid()在xGraph里面的索引startIndex
					int startIndex = XGraph::findNodeIndex(xGraph, startItem->getUuid());
					if (endItem != nullptr)
					{
						XLOG_INFO("DiagramScene::mouseReleaseEvent, endItem != nullptr ...  ", CURRENT_THREAD_ID);
						// 获取节点endItem->getUuid()在xGraph里面的索引endIndex
						int endIndex = XGraph::findNodeIndex(xGraph, endItem->getUuid());
						// 把xGraph[endIndex]设置为xGraph[startIndex]的邻节点, 添加边, 标识为yItemId的节点---》指向---》标识为xItemId的节点(即getUuid()的返回值)的节点, 这个逻辑应该紧跟连线动作释放之后
						xGraph[startIndex]->neighbors.push_back(xGraph[endIndex]);
					}
				}//作用域结束时，lock_guard会自动解锁互斥锁xGraphMutex
			}
		}
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
	}
	else if (Qt::LeftButton == mouseEvent->button())
	{
		if (DiagramType::Line != diagramType)
		{
			diagramState = DiagramState::Release;
			diagramType = DiagramType::None;
		}
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
	}
	else
	{
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
	}
}

bool DiagramScene::isThisType(int type) const
{
	const QList<QGraphicsItem*> items = selectedItems();
	const auto cb = [type](const QGraphicsItem* item) { return item->type() == type; };
	return std::find_if(items.begin(), items.end(), cb) != items.end();
}

// ============================================================================
// PR-4.5: 节点状态可视化
// ============================================================================

QGraphicsItem* DiagramScene::findItemByNodeId(const std::string& nodeId) const
{
	std::lock_guard<std::mutex> lock(itemMapMutex);
	auto it = globalItemMap.find(nodeId);
	if (it != globalItemMap.end())
		return it->second;
	return nullptr;
}

NodeStateOverlay* DiagramScene::getOrCreateOverlay(const std::string& nodeId)
{
	// 检查是否已存在
	auto it = nodeOverlays_.find(nodeId);
	if (it != nodeOverlays_.end())
		return it->second;

	// 找到对应的图元
	QGraphicsItem* targetItem = findItemByNodeId(nodeId);
	if (!targetItem)
		return nullptr;

	// 创建新的 overlay
	NodeStateOverlay* overlay = new NodeStateOverlay(targetItem);
	nodeOverlays_[nodeId] = overlay;
	
	return overlay;
}

void DiagramScene::setNodeState(const std::string& nodeId, NodeState state)
{
	NodeStateOverlay* overlay = getOrCreateOverlay(nodeId);
	if (overlay)
	{
		overlay->setState(state);
		XLOG_INFO("DiagramScene::setNodeState: nodeId=" + nodeId + 
		          " state=" + std::to_string(static_cast<int>(state)), CURRENT_THREAD_ID);
	}
}

void DiagramScene::clearAllNodeStates()
{
	// 删除所有 overlay
	for (auto& [nodeId, overlay] : nodeOverlays_)
	{
		if (overlay)
		{
			// overlay 是目标 item 的子对象，删除时会自动从场景中移除
			delete overlay;
		}
	}
	nodeOverlays_.clear();
}

void DiagramScene::resetAllNodeStatesToPending()
{
	// 将所有现有 overlay 重置为 Pending 状态
	for (auto& [nodeId, overlay] : nodeOverlays_)
	{
		if (overlay)
		{
			overlay->setState(NodeState::Pending);
		}
	}
}

NodeState DiagramScene::getNodeState(const std::string& nodeId) const
{
	auto it = nodeOverlays_.find(nodeId);
	if (it != nodeOverlays_.end() && it->second)
		return it->second->state();
	return NodeState::Pending;
}

} // namespace XVisual
