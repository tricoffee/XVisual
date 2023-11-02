// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QTextCursor>
#include <QtDebug>
#include "MainWindow/diagramscene.h"
#include "ItemWidget/XTextEdit.h"
#include <mutex>
#include "GlobalStorage/GlobalStorage.h"
#include "MainWindow/GraphicsWidget.h"

class XBaseItem;
class ConditionItem;

DiagramScene::DiagramScene(QMenu* itemMenu, QObject* parent)
	: QGraphicsScene(parent)
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

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

	if (mouseEvent->button() != Qt::LeftButton)
		return;

	if (DiagramType::Item == diagramType && DiagramState::Insert == diagramState)
	{
		std::string itemclass = diagramName.toStdString();
		item = ItemRegistry::createObject(itemclass, myItemMenu, nullptr);
		item->setPos(mouseEvent->scenePos());
		item->setBrush(myItemColor);
		item->debug();
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
				if (startItems.count() && endItems.count() &&
					startItems.first()->type() == XBaseItem::Type &&
					endItems.first()->type() == XBaseItem::Type &&
					startItems.first() != endItems.first())
				{
					XBaseItem* startItem = qgraphicsitem_cast<XBaseItem*>(startItems.first());
					XBaseItem* endItem = qgraphicsitem_cast<XBaseItem*>(endItems.first());
					arrow = new XArrow(graphicsWidget, startItem, endItem);
					arrow->setColor(myLineColor);
					startItem->addArrow(arrow);
					endItem->addArrow(arrow);
					addItem(arrow);
					qDebug() << "arrow->setZValue(-9999.99)" << -9999.99;
					QLineF line(pa, pb);
					arrow->setLine(line);
					arrow->setZValue(-9999.99);
					if (!startItem->childs.contains(endItem))
						startItem->childs.emplace_back(endItem);
					if (!endItem->parents.contains(startItem))
						endItem->parents.emplace_back(startItem);
				}
				lineItem = nullptr;
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
