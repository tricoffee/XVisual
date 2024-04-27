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
#include "MainWindow/GraphicsWidget.h"
#include "ItemBase/XBaseItem.h"

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

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

	if (mouseEvent->button() != Qt::LeftButton)
		return;

	if (DiagramType::Item == diagramType && DiagramState::Insert == diagramState)
	{
		std::string itemclass = diagramName.toStdString();
		// ���� GraphicsWidget* graphicsWidget
		item = ItemRegistry::createObject(itemclass, graphicsWidget, myItemMenu, nullptr);
		item->setPos(mouseEvent->scenePos());
		item->setBrush(myItemColor);
		item->debug();
		std::string itemId = item->getUuid();
		addItem(item);



		{
			std::lock_guard<std::mutex> lock(itemMapMutex); 
			globalItemMap[itemId] = item;
		}// ���������ʱ��lock_guard ���Զ�����������itemMapMutex
		{
			std::lock_guard<std::mutex> lock(xGraphMutex);
			// ������ʶΪitemId�Ľڵ�
			xGraph.push_back(std::make_shared<GraphNode>(itemId));
		}// ���������ʱ��lock_guard ���Զ�����������xGraphMutex



		// XBaseHandle* handle = item->getXHandle();
		std::string handleId = item->getXHandle()->getUuid();
		{
			std::lock_guard<std::mutex> lock(handleMapMutex);
			globalHandleMap[handleId] = item->getXHandle();

			// ʹ�÷�Χѭ������unordered_map
			for (const auto& pair : globalHandleMap)
			{
				// std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
				std::string keyStr = pair.first;
				std::string uuidStr = pair.second->getUuid();
				XLOG_INFO("globalHandleMap pair, key = " + keyStr + "uuid = " + uuidStr, CURRENT_THREAD_ID);
			}
		}// ���������ʱ��lock_guard ���Զ�����������handleMapMutex

		//��item��Ӧ��handleId�洢��xHandleGraph
		{
			std::lock_guard<std::mutex> lock(xHandleGraphMutex);
			// ������ʶΪitemId�Ľڵ�
			xHandleGraph.push_back(std::make_shared<GraphNode>(handleId));
		}// ���������ʱ��lock_guard ���Զ�����������xHandleGraphMutex

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
		//�������	mouseEvent->ignore();������QGraphicsScene::mouseMoveEvent(mouseEvent);��Ŀ����
		//Ϊ�˱�������ק��ʽ��������item֮���������ʱ���item��λ��
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
					qDebug() << "arrow->setZValue(-9999.99)" << -9999.99;
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
					// ��ȡ�ڵ�startItem->getUuid()��xGraph���������startIndex
					int startIndex = XGraph::findNodeIndex(xGraph, startItem->getUuid());
					if (endItem != nullptr)
					{
						XLOG_INFO("DiagramScene::mouseReleaseEvent, endItem != nullptr ...  ", CURRENT_THREAD_ID);
						// ��ȡ�ڵ�endItem->getUuid()��xGraph���������endIndex
						int endIndex = XGraph::findNodeIndex(xGraph, endItem->getUuid());
						// ��xGraph[endIndex]����ΪxGraph[startIndex]���ڽڵ�, ��ӱ�, ��ʶΪyItemId�Ľڵ�---��ָ��---����ʶΪxItemId�Ľڵ�(��getUuid()�ķ���ֵ)�Ľڵ�, ����߼�Ӧ�ý������߶����ͷ�֮��
						xGraph[startIndex]->neighbors.push_back(xGraph[endIndex]);
					}
				}//���������ʱ��lock_guard���Զ�����������xGraphMutex
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
