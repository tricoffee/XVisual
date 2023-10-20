// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef XBaseItem_H
#define XBaseItem_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <vector>
#include "QUuid.h"
#include "ItemFactory.h"
#include "ItemWidget/XTextEdit.h"
#include "XVariable/Source.h"
#include "XVariable/Dest.h"
#include "XVariable/XMacro.h"

class DiagramProxyWidget;
class ItemWidget;
QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE
class XArrow;
class XBaseItem : public QObject, public QGraphicsPolygonItem
{
	Q_OBJECT
	public:
		enum { Type = UserType + 1 };
		XBaseItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		void removeArrow(XArrow* arrow);
		void removeArrows();
		QPolygonF polygon() const { return myPolygon; }
		void addArrow(XArrow* arrow);
		virtual QPixmap image();
		int type() const override { return Type; }
		virtual void debug();
		void setEditText(QString text);
		void setEditFont(const QFont& font);
		void setEditColor(const QColor& color);
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
			QWidget* widget = nullptr) override;
		void initProxy();
		void setWidget();
		qreal boundingWidth();
		qreal boundingHeight();
		const std::string& getUuid() const;
		QList<XBaseItem*> parents;
		QList<XBaseItem*> childs;
		virtual void initParameters();
		virtual void ItemXOP(Source& sources, Dest& dests);
		virtual void setSourceFrom();
		virtual Source& getSources();
		virtual Dest& getDests();
	public slots:
			void TextEditFocusOutSlot(QString mText);
	protected:
		void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
		QPolygonF myPolygon;
		DiagramProxyWidget* proxyWidget;
		ItemWidget* xitemWidget;
		void createUuid();
		std::string uuid;
    private:
		QMenu* myContextMenu;
		QList<XArrow*> arrows;
		QString text;
		Source sources;
		Dest dests;
};

#endif //XBaseItem_H