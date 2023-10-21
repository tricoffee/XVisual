// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef XARROW_H
#define XARROW_H

#include <QGraphicsLineItem>
#include "TableWidget/TableData.h"

class XBaseItem;
class XArrow : public QObject, public QGraphicsLineItem
{
	Q_OBJECT
public:
	enum { Type = UserType + 2 };
	XArrow(XBaseItem* startItem, XBaseItem* endItem,
		QGraphicsItem* parent = nullptr);
	XArrow(QWidget* widget, XBaseItem* startItem, XBaseItem* endItem,
		QGraphicsItem* parent = nullptr);
	int type() const override { return Type; }
	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void setColor(const QColor& color) { myColor = color; }
	XBaseItem* getStartItem() const { return myStartItem; }
	XBaseItem* getEndItem() const { return myEndItem; }
signals:
	void showTableViewSingle(const QString& xName, const QString& yName,
		XArrow*, QList<TableData>, QList<TableData>);
public slots:
	void changeLineArrowColorSlots(const QColor& color);
protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
		QWidget* widget = nullptr) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
private:
	XBaseItem* myStartItem;
	XBaseItem* myEndItem;
	QPolygonF arrowHead;
	QColor myColor = Qt::black;
};

#endif // XARROW_H
