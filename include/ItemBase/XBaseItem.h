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
#include "XVariable/SourceFrom.h"

class DiagramProxyWidget;
class ItemWidget;
class GraphicsWidget;
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
		XBaseItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
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
		virtual void initItemOperands();
		virtual void ItemXOP();
		Source& getSources();
		Dest& getDests();
		const QString& getUniqueName();
		void setSourceFrom(const std::string& xVariableName, const SourceFrom& sourceFrom);
		void loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom);
	public slots:
			void TextEditFocusOutSlot(QString mText);
	protected:
		// The default is true, indicating that the source of information(i.e. the input of the node) comes from the outside world. 
		bool isSourceFromOutside = true; 
		void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
		QPolygonF myPolygon;
		DiagramProxyWidget* proxyWidget;
		ItemWidget* xitemWidget;
		void createUuid();
		std::string uuid;
		virtual void createUniqueName();
		QString uniqueName;
		Source sources;
		Dest dests;
    private:
		QMenu* myContextMenu;
		QList<XArrow*> arrows;
		QString text;
	signals:
		void showImageSignal(const std::string& filename, const cv::Mat& image, XBaseItem* item);
};

#endif //XBaseItem_H