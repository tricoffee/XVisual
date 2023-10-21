// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "ItemBase/XBaseItem.h"
#include "ItemBase/XArrow.h"
#include "ItemWidget/XTextEdit.h"
#include "ItemWidget/ItemWidget.h"
#include "ItemWidget/DiagramProxyWidget.h"
#include "GlobalStorage/ItemManager.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

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
	qDebug() << "Source& XBaseItem::getSources() ";
	return sources;
}
Dest& XBaseItem::getDests()
{
	qDebug() << "Dest& XBaseItem::getDests() ";
	return dests;
}
void XBaseItem::initParameters()
{

}
void XBaseItem::ItemXOP(Source& sources, Dest& dests)
{

}
void XBaseItem::setSourceFrom(const std::string& xName,
	const std::string& yItemId, const std::string& yName)
{
	SourceFrom sourceFrom;
	sourceFrom.itemId = yItemId;
	sourceFrom.name = yName;
	SET_SOURCEFROM_STR(sources, xName, sourceFrom);

}
void XBaseItem::loadSourceFrom(const std::string& xName)
{
	GET_SOURCEFROM_STR(sources, xName);
}
REGISTER_CLASS(XBase);
