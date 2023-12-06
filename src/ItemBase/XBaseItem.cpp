// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "Common/LoggerInstance.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <mutex>
#include "Common/XThreadMacro.h"
#include "ItemBase/XBaseItem.h"
#include "ItemBase/XArrow.h"
#include "ItemWidget/XTextEdit.h"
#include "ItemWidget/ItemWidget.h"
#include "ItemWidget/DiagramProxyWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "GlobalStorage/GlobalStorage.h"
#include "XGraph/XGraph.h"
#include "MainWindow/GraphicsWidget.h"

XBaseItem::XBaseItem(QMenu* contextMenu,QGraphicsItem* parent)
	: QGraphicsPolygonItem(parent), myContextMenu(contextMenu), sources(Source::getInstance()), dests(Dest::getInstance())
{
	QString uniqueName = ItemManager::instance().getUniqueItemName("XBase");
	setObjectName(uniqueName);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	// 创建Item的Uuid
	createUuid();
	// 给Item初始化一个代理
	initProxy();
	// 给代理设置一个widget，并且setPos
	setWidget();
	// Item接收其代理维护的QWidget对象xitemWidget里面的一个QTextEdit发过来的信号
	connect(xitemWidget->getEdit(), &XTextEdit::TextEditFocusOutSignal,
		this, &XBaseItem::TextEditFocusOutSlot);
}
XBaseItem::XBaseItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: QGraphicsPolygonItem(parent), myContextMenu(contextMenu), sources(Source::getInstance()), dests(Dest::getInstance())
{
	QString uniqueName = ItemManager::instance().getUniqueItemName("XBase");
	setObjectName(uniqueName);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	// 创建Item的Uuid
	createUuid();
	// 给Item初始化一个代理
	initProxy();
	// 给代理设置一个widget，并且setPos
	setWidget();
	// Item接收其代理维护的QWidget对象xitemWidget里面的一个QTextEdit发过来的信号
	connect(xitemWidget->getEdit(), &XTextEdit::TextEditFocusOutSignal,
		this, &XBaseItem::TextEditFocusOutSlot);
	// 连接发送showImage信号到槽函数
	//GraphicsWidget* gWidget = static_cast<GraphicsWidget*>(widget);
	connect(this, &XBaseItem::showImageSignal, gWidget, &GraphicsWidget::showImageSlot);
}
void XBaseItem::removeArrow(XArrow* arrow)
{
	arrows.removeAll(arrow);
}
void XBaseItem::removeArrows()
{
	// need a copy here since removeArrow() will
	// modify the arrows container
	const auto arrowsCopy = arrows;
	for (XArrow* arrow : arrowsCopy)
	{
		arrow->getStartItem()->removeArrow(arrow);
		arrow->getEndItem()->removeArrow(arrow);
		scene()->removeItem(arrow);
		delete arrow;
	}
}
void XBaseItem::addArrow(XArrow* arrow)
{
	arrows.append(arrow);
}
QPixmap XBaseItem::image()
{
	QPixmap pixmap(250, 250);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(QPen(Qt::black, 8));
	painter.translate(125, 125);
	painter.drawPolyline(myPolygon);
	return pixmap;
}
void XBaseItem::TextEditFocusOutSlot(QString mText)
{
	text = mText;
	qDebug() << "void XBaseItem::TextEditFocusOutSlot(QString mText)" << text;
}
void XBaseItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	myContextMenu->exec(event->screenPos());
}
void XBaseItem::debug()
{
	qDebug() << "XBaseItem::debug() ... ";
	qDebug() << "XBaseItem::boundingRect() " << boundingRect();
	qDebug() << "XBaseItem::uuid " << QString::fromStdString(uuid);
}
void XBaseItem::setEditText(QString mText)
{
	xitemWidget->getEdit()->setText(mText);
	qDebug() << "void XBaseItem::setEditText(QString mText)" << mText;
}
void XBaseItem::setEditFont(const QFont& font)
{
	xitemWidget->getEdit()->setFont(font);
	qDebug() << "void XBaseItem::setEditFont(const QFont& font)" << font;
}
void XBaseItem::setEditColor(const QColor& color)
{
	xitemWidget->getEdit()->setTextColor(color);
	qDebug() << "void XBaseItem::setEditColor(const QFont& color)" << color;
}
void XBaseItem::paint(QPainter* painter, 
	const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QGraphicsPolygonItem::paint(painter,option,widget);
}
qreal XBaseItem::boundingWidth()
{
	return boundingRect().width();
}
qreal XBaseItem::boundingHeight()
{
	return boundingRect().height();
}
void XBaseItem::initProxy()
{
	proxyWidget = new DiagramProxyWidget(this);
	xitemWidget = new ItemWidget();
}
void XBaseItem::setWidget()
{
	xitemWidget->resize(XBaseItem::boundingWidth()/2, XBaseItem::boundingHeight()/2);
	proxyWidget->setWidget(xitemWidget);
	proxyWidget->setPos(-xitemWidget->width() / 2, -xitemWidget->height() / 2);
}
void XBaseItem::createUuid()
{
	QUuid id = QUuid::createUuid();
	uuid = id.toString().toStdString();
}
const std::string& XBaseItem::getUuid() const
{
	return uuid;
}
void XBaseItem::createUniqueName()
{

}
const QString& XBaseItem::getUniqueName()
{
	return uniqueName;
}
Source& XBaseItem::getSources()
{
	//qDebug() << "Source& XBaseItem::getSources() ";
	return sources;
}
Dest& XBaseItem::getDests()
{
	//qDebug() << "Dest& XBaseItem::getDests() ";
	return dests;
}
void XBaseItem::initParameters()
{

}
void XBaseItem::ItemXOP()
{

}
/*
从sourceFrom查找sources里面的每个参数的来源并且初始化该参数
*/
void XBaseItem::initItemOperands()
{
	XLOG_INFO("XBaseItem::initItemOperands ...... ", CURRENT_THREAD_ID);
	// To-DO, XBaseItem再添加一个成员变量, isSourceFromOutside, 条件isSourceFromOutside == true为真时, 直接返回true
	if (isSourceFromOutside == true)
	{
		return;
	}
	std::vector<std::string> xVaribleNames = ACQUIRE_NAMES(sources);
	int num = xVaribleNames.size();
	XLOG_INFO("XBaseItem::initItemOperands,xVaribleNames.size() = "+ std::to_string(num), CURRENT_THREAD_ID);
	for (const auto& xName : xVaribleNames) 
	{
		XLOG_INFO("XBaseItem::initItemOperands: " + xName, CURRENT_THREAD_ID);
		SourceFrom sourceFrom;
		loadSourceFrom(xName, sourceFrom);
		std::string yItemId = sourceFrom.itemId;
		std::string yName = sourceFrom.variableName;
		XBaseItem* yItem = nullptr;
		{
			std::lock_guard<std::mutex> lock(itemMapMutex);
			yItem = globalItemMap[yItemId];
		} // 作用域结束时，lock_guard 会自动解锁互斥锁itemMapMutex
		// To-DO, XBaseItem再添加一个成员变量, isSourceFromOutside, 并且if条件修改为yItem != nullptr && isSourceFromOutside == false
		if (yItem != nullptr && isSourceFromOutside == false)
		{
			auto yValue = GET_MEMBER_STR(yItem->getDests(), yName);
			REGISTER_MEMBER_STR(sources, xName, yValue);
		}
	}
}
/*
设置某个变量的sourceFrom，让我们知道它从哪里来
*/
void XBaseItem::setSourceFrom(const std::string& xVariableName,const SourceFrom& sourceFrom)
{
	// To-DO, XBaseItem再添加一个成员变量, isSourceFromOutside, 条件isSourceFromOutside == true为真时, 直接返回true
	if (isSourceFromOutside == true)
	{
		return;
	}
	SET_SOURCEFROM_STR(sources, xVariableName, sourceFrom);
}
/*
按名字查找某个变量的sourceFrom，在执行计算图前初始化参数时会调用这个函数
*/
void XBaseItem::loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom)
{
	// To-DO, XBaseItem再添加一个成员变量, isSourceFromOutside, 条件isSourceFromOutside == true为真时, 直接返回true
	if (isSourceFromOutside == true)
	{
		return;
	}
	sourceFrom = GET_SOURCEFROM_STR(sources, xVariableName);
}
REGISTER_CLASS(XBase);
