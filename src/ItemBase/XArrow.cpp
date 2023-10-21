// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"

#include <QPainter>
#include <QPen>
#include <QtMath>
#include "MainWindow/MainWindow.h"

XArrow::XArrow(XBaseItem* startItem, XBaseItem* endItem, QGraphicsItem* parent)
	: QGraphicsLineItem(parent), myStartItem(startItem), myEndItem(endItem)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

XArrow::XArrow(QWidget* widget, XBaseItem* startItem, XBaseItem* endItem, QGraphicsItem* parent)
	:QGraphicsLineItem(parent), myStartItem(startItem), myEndItem(endItem)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	// 连接双击信号到槽函数
	MainWindow* mWidget = static_cast<MainWindow*>(widget);
	connect(this, &XArrow::showTableViewSingle,
		mWidget, &MainWindow::showTableViewSlot);
}

QRectF XArrow::boundingRect() const
{
	qreal extra = (pen().width() + 20) / 2.0;

	return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
		line().p2().y() - line().p1().y()))
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}

QPainterPath XArrow::shape() const
{
	QPainterPath path = QGraphicsLineItem::shape();
	path.addPolygon(arrowHead);
	return path;
}

void XArrow::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
	QWidget*)
{
	if (myStartItem->collidesWithItem(myEndItem))
		return;

	QPen myPen = pen();
	myPen.setColor(myColor);
	qreal arrowSize = 20;
	painter->setPen(myPen);
	painter->setBrush(myColor);
	QLineF centerLine(myStartItem->pos(), myEndItem->pos());
	QPolygonF endPolygon = myEndItem->polygon();
	QPointF p1 = endPolygon.first() + myEndItem->pos();
	QPointF intersectPoint;
	for (int i = 1; i < endPolygon.count(); ++i)
	{
		QPointF p2 = endPolygon.at(i) + myEndItem->pos();
		QLineF polyLine = QLineF(p1, p2);
		QLineF::IntersectionType intersectionType =
			polyLine.intersects(centerLine, &intersectPoint);
		if (intersectionType == QLineF::BoundedIntersection)
			break;
		p1 = p2;
	}
	setLine(QLineF(myStartItem->pos(), intersectPoint));
	double angle = std::atan2(line().dy(), line().dx());
	QPointF arrowP1 = line().p2() - QPointF(cos(angle + M_PI / 6) * arrowSize,
		sin(angle + M_PI / 6) * arrowSize);
	QPointF arrowP2 = line().p2() + QPointF(-cos(M_PI / 6 - angle) * arrowSize,
		+sin(M_PI / 6 - angle) * arrowSize);
	arrowHead.clear();
	arrowHead << line().p2() << arrowP1 << arrowP2;
	painter->drawLine(line());
	painter->drawPolygon(arrowHead);
	if (isSelected())
	{
		painter->setPen(QPen(myColor, 1, Qt::DashLine));
		QLineF myLine = line();
		myLine.translate(0, 4.0);
		painter->drawLine(myLine);
		myLine.translate(0, -8.0);
		painter->drawLine(myLine);
	}
}

void XArrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// 当双击XArrow时，更改XArrow的颜色为红色
	myColor = Qt::red;
	qDebug() << "void XArrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) ";
	qDebug() << "yUuid = " << QString::fromStdString(myStartItem->getUuid());
	qDebug() << "xUuid = " << QString::fromStdString(myEndItem->getUuid());
	std::string yItemId = const_cast<std::string&>(myStartItem->getUuid());
	std::string xItemId = const_cast<std::string&>(myEndItem->getUuid());
	QString yItemName = const_cast<QString&>(myStartItem->getUniqueName());
	QString xItemName = const_cast<QString&>(myEndItem->getUniqueName());
	Dest& dest = myStartItem->getDests();
	Source& source = myEndItem->getSources();
	std::vector<std::string> yNames = ACQUIRE_NAMES(dest);
	std::vector<std::string> xNames = ACQUIRE_NAMES(source);
	int yNum = yNames.size();
	int xNum = xNames.size();
	qDebug() << "yNum = " << yNum;
	qDebug() << "xNum = " << xNum;
	QList<TableData> variablesY;
	QList<TableData> variablesX;
	for (int j = 0; j < yNum; ++j)
	{
		std::string yName = yNames[j];
		std::string yTypeName = (*GET_MEMBER_TYPE_STR(dest, yName)).name();
		TableData yData{ yItemId,yName,yTypeName };
		variablesY << yData;
	}
	for (int i = 0; i < xNum; ++i)
	{
		std::string xName = xNames[i];
		std::string xTypeName = (*GET_MEMBER_TYPE_STR(source, xName)).name();
		//std::any xValue = GET_MEMBER_STR(dest, xName);
		TableData xData{ xItemId,xName,xTypeName };
		variablesX << xData;
	}
	emit showTableViewSingle(xItemName,yItemName,this,variablesX, variablesY);
	return QGraphicsLineItem::mouseDoubleClickEvent(event);
}

void XArrow::changeLineArrowColorSlots(const QColor& color)
{
	myColor = color;
}