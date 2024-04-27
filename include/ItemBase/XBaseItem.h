// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef XBaseItem_H
#define XBaseItem_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <vector>
#include "ItemFactory.h"
#include "ItemWidget/XTextEdit.h"
#include "XVariable/Source.h"
#include "XVariable/Dest.h"
#include "XVariable/XMacro.h"
#include "XVariable/SourceFrom.h"
#include "HandleBase/Colleague.h"

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
class XBaseHandle;
class XBaseItem : public QObject, public QGraphicsPolygonItem, public Colleague
{
	Q_OBJECT
	public:
		virtual ~XBaseItem() 
		{
			/*		if (xHandle)
					{
						delete xHandle;
					}*/
		}
		enum { Type = UserType + 1 };
		//XBaseItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		XBaseItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		XBaseHandle* getXHandle()
		{
			return xHandle;
		}
		void setXHandle(XBaseHandle* mXHandle)
		{
			xHandle = mXHandle;
		}
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
		QList<XBaseItem*> parents;
		QList<XBaseItem*> childs;
		const QString& getUniqueName();
		virtual void initParams();
		virtual void initOperands();
		virtual void xOperate();
	public slots:
			void TextEditFocusOutSlot(QString mText);
	protected:
		XBaseHandle* xHandle;
		void setColleagueType() override;
		void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
		QPolygonF myPolygon;
		DiagramProxyWidget* proxyWidget;
		ItemWidget* xitemWidget;
		virtual void createUniqueName(const QString& classNameQStr);
		QString uniqueName;
		void createUuid();
    private:
		QMenu* myContextMenu;
		QList<XArrow*> arrows;
		QString text;
	signals:
		void showImageSignal(const std::string& filename, const cv::Mat& image, XBaseItem* item);
};

#endif //XBaseItem_H