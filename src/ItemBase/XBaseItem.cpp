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
#include "Exception/UmapKeyNoFoundException.h"
#include "Common/StrUtils.h"

// 现版本, 新增一个父类(基类)Colleague, sources和dests的实例化转由在其父类(基类)Colleague实现
XBaseItem::XBaseItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: QGraphicsPolygonItem(parent), myContextMenu(contextMenu)
{
	std::string classNameStr = "XBase";
	QString classNameQStr = QString::fromStdString(classNameStr);
	createUniqueName(classNameQStr);
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
	connect(this, &XBaseItem::showImageSignal, gWidget, &GraphicsWidget::showImageSlot);


	// 创建xHandle
	//xHandle = HandleRegistry::createObject(classNameStr);

	//std::string delimiter = "_";
	//std::string uuidStr2 = extractSubstrAfterDelimiter(uuid, delimiter);
	//const std::string handleUuid = "Handle_" + uuidStr2;
	//xHandle->setUuid(handleUuid);

	//xHandle->setUuid("Handle_" + uuid);

	//xHandle->setUuidConsistentWithItem(uuid);

	//initParams();
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
void XBaseItem::createUniqueName(const QString& classNameQStr)
{
	uniqueName = ItemManager::instance().getUniqueItemName(classNameQStr);
}
const QString& XBaseItem::getUniqueName()
{
	return uniqueName;
}
void XBaseItem::setColleagueType()
{
	colleagueType = ColleagueType::HandleType;
}
void XBaseItem::createUuid()
{
	QUuid id = QUuid::createUuid();
	uuid = "Item_" + id.toString().toStdString();
}
// initParams()初始化参数，因为纯虚函数无法创建对象，所以在此用虚函数而不是纯虚函数，而XBaseItem不需要做具体的逻辑, 具体逻辑在子类中实现
void XBaseItem::initParams()
{

}
// xOperate()执行计算逻辑，因为纯虚函数无法创建对象，所以在此用虚函数而不是纯虚函数，而XBaseItem不需要做具体的逻辑, 具体逻辑在子类中实现
void XBaseItem::xOperate()
{

}
/*
从sourceFrom查找sources里面的每个参数的来源并且初始化该参数
*/
void XBaseItem::initOperands()
{
	xHandle->initOperands();
}
REGISTER_ITEM(XBase);
