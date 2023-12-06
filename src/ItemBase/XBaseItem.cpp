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
	// ����Item��Uuid
	createUuid();
	// ��Item��ʼ��һ������
	initProxy();
	// ����������һ��widget������setPos
	setWidget();
	// Item���������ά����QWidget����xitemWidget�����һ��QTextEdit���������ź�
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
	// ����Item��Uuid
	createUuid();
	// ��Item��ʼ��һ������
	initProxy();
	// ����������һ��widget������setPos
	setWidget();
	// Item���������ά����QWidget����xitemWidget�����һ��QTextEdit���������ź�
	connect(xitemWidget->getEdit(), &XTextEdit::TextEditFocusOutSignal,
		this, &XBaseItem::TextEditFocusOutSlot);
	// ���ӷ���showImage�źŵ��ۺ���
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
��sourceFrom����sources�����ÿ����������Դ���ҳ�ʼ���ò���
*/
void XBaseItem::initItemOperands()
{
	XLOG_INFO("XBaseItem::initItemOperands ...... ", CURRENT_THREAD_ID);
	// To-DO, XBaseItem�����һ����Ա����, isSourceFromOutside, ����isSourceFromOutside == trueΪ��ʱ, ֱ�ӷ���true
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
		} // ���������ʱ��lock_guard ���Զ�����������itemMapMutex
		// To-DO, XBaseItem�����һ����Ա����, isSourceFromOutside, ����if�����޸�ΪyItem != nullptr && isSourceFromOutside == false
		if (yItem != nullptr && isSourceFromOutside == false)
		{
			auto yValue = GET_MEMBER_STR(yItem->getDests(), yName);
			REGISTER_MEMBER_STR(sources, xName, yValue);
		}
	}
}
/*
����ĳ��������sourceFrom��������֪������������
*/
void XBaseItem::setSourceFrom(const std::string& xVariableName,const SourceFrom& sourceFrom)
{
	// To-DO, XBaseItem�����һ����Ա����, isSourceFromOutside, ����isSourceFromOutside == trueΪ��ʱ, ֱ�ӷ���true
	if (isSourceFromOutside == true)
	{
		return;
	}
	SET_SOURCEFROM_STR(sources, xVariableName, sourceFrom);
}
/*
�����ֲ���ĳ��������sourceFrom����ִ�м���ͼǰ��ʼ������ʱ������������
*/
void XBaseItem::loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom)
{
	// To-DO, XBaseItem�����һ����Ա����, isSourceFromOutside, ����isSourceFromOutside == trueΪ��ʱ, ֱ�ӷ���true
	if (isSourceFromOutside == true)
	{
		return;
	}
	sourceFrom = GET_SOURCEFROM_STR(sources, xVariableName);
}
REGISTER_CLASS(XBase);
