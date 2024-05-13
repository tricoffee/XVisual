// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
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
#include "Common/StrUtils.h"

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

void DiagramScene::addArrowByJson(const std::pair<std::string, std::set<std::string>>& itemChildren)
{
	std::string pItemId = itemChildren.first;
	std::set<std::string> qItemIds = itemChildren.second;

	XBaseItem* startItem = nullptr;
	{
		std::lock_guard<std::mutex> lock(itemMapMutex);
		startItem = globalItemMap[pItemId];
	}// 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex

	XBaseItem* endItem = nullptr;
	for (const auto& qItemId : qItemIds)
	{
		{
			std::lock_guard<std::mutex> lock(itemMapMutex);
			endItem = globalItemMap[qItemId];
		}// 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex


		lineItem = new QGraphicsLineItem(QLineF(startItem->scenePos(), endItem->scenePos()));
		lineItem->setPen(QPen(myLineColor, 2));
		addItem(lineItem);
		diagramState = DiagramState::Move;
		diagramType = DiagramType::Line;

		QPointF pa = lineItem->line().p1();
		QPointF pb = lineItem->line().p2();
		removeItem(lineItem);
		delete lineItem;
	    arrow = new XArrow(graphicsWidget, startItem, endItem);
		arrow->setColor(myLineColor);
		startItem->addArrow(arrow);
		endItem->addArrow(arrow);
		addItem(arrow);
		//qDebug() << "arrow->setZValue(-9999.99)" << -9999.99;
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
		lineItem = nullptr;
		if (startItem != nullptr)
		{
			XLOG_INFO("DiagramScene::addArrowByJson, startItem != nullptr ...  ", CURRENT_THREAD_ID);
			std::lock_guard<std::mutex> lock(xGraphMutex);
			// 获取节点startItem->getUuid()在xGraph里面的索引startIndex
			int startIndex = XGraph::findNodeIndex(xGraph, startItem->getUuid());
			if (endItem != nullptr)
			{
				XLOG_INFO("DiagramScene::addArrowByJson, endItem != nullptr ...  ", CURRENT_THREAD_ID);
				// 获取节点endItem->getUuid()在xGraph里面的索引endIndex
				int endIndex = XGraph::findNodeIndex(xGraph, endItem->getUuid());
				// 把xGraph[endIndex]设置为xGraph[startIndex]的邻节点, 添加边, 标识为yItemId的节点---》指向---》标识为xItemId的节点(即getUuid()的返回值)的节点, 这个逻辑应该紧跟连线动作释放之后
				xGraph[startIndex]->neighbors.push_back(xGraph[endIndex]);
			}
		}//作用域结束时，lock_guard会自动解锁互斥锁xGraphMutex


		XBaseHandle* endHandle = endItem->getXHandle();
		Source& endS = endHandle->getSources();
		std::vector<std::string> endSourceNames = ACQUIRE_NAMES(endS);
		std::unordered_map<std::string, TableData> defaultValuesUMap;
		for (const auto& endSourceName : endSourceNames)
		{
			SourceFrom sourceFrom;
			endHandle->loadSourceFrom(endSourceName, sourceFrom);
			// Done, 从sourceFrom解析出TableData, 并更新defaultValuesUMap
			std::string yItemId = pItemId; // pItem ---> qItem
			std::string yName = sourceFrom.variableName;
			XBaseHandle* startHandle = startItem->getXHandle();
			Dest& startD = startHandle->getDests();
			std::string yTypeName = (*GET_MEMBER_TYPE_STR(startD, yName)).name();
			TableData yData{ yItemId,yName,yTypeName };
			defaultValuesUMap[endSourceName] = yData;
		}


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
				QList<QGraphicsItem*> startItems = items(lineItem->line().p1());
				if (startItems.count() && startItems.first() == lineItem)
				{
					startItems.removeFirst();
				}
				QPointF pb = lineItem->line().p2();
				QList<QGraphicsItem*> endItems = items(lineItem->line().p2());
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
