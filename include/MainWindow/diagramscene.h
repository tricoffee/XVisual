// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>

#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include "Item/ConditionItem.h"
#include "Common/constants.h"
#include "MainWindow/mainwindow.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

class DiagramScene : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit DiagramScene(QMenu* itemMenu, QObject* parent = nullptr);
	QFont font() const { return myFont; }
	QColor textColor() const { return myTextColor; }
	QColor itemColor() const { return myItemColor; }
	QColor lineColor() const { return myLineColor; }
	void setTextColor(const QColor& color);
	void setItemColor(const QColor& color);
	void setLineColor(const QColor& color);
	void setFont(const QFont& font);
	void setDiagramType(DiagramType type);
	void setDiagramState(DiagramState state);
	void setDiagramName(const QString& name);
	void setView(QWidget* view);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
private:
	bool isThisType(int type) const;
	DiagramType diagramType;
	DiagramState diagramState;
	QString diagramName;
	QMenu* myItemMenu;
	QPointF startPoint;
	QGraphicsLineItem* lineItem;
	QFont myFont;
	XBaseItem* item;
	QColor myTextColor;
	QColor myItemColor;
	QColor myLineColor;
	XArrow* arrow;
	QWidget* xView;
	QWidget* mainWindow = nullptr;
};

#endif //DIAGRAMSCENE_H