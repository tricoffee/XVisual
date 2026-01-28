#ifndef XBaseItem_H
#define XBaseItem_H

#include <vector>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPixmap>
#include <QGraphicsSceneContextMenuEvent>

#include "ItemFactory.h"
#include "ItemWidget/XTextEdit.h"
#include "XVariable/Source.h"
#include "XVariable/Dest.h"
#include "XVariable/XMacro.h"
#include "XVariable/SourceFrom.h"
#include "HandleBase/Colleague.h"

namespace XVisual {

class DiagramProxyWidget;
class ItemWidget;
class GraphicsWidget;
class XArrow;
class XBaseHandle;

class XBaseItem : public QObject, public QGraphicsPolygonItem, public Colleague
{
	Q_OBJECT
	public:
		virtual ~XBaseItem() 
		{

		}
		enum { Type = UserType + 1 };
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
		QPixmap image();
		int type() const override { return Type; }
		void debug();
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
		virtual void initParams();
		virtual void initOperands();
		virtual void xOperate();
	public slots:
			void TextEditFocusOutSlot(QString mText);
	protected:
		XBaseHandle* xHandle;
		void setColleagueType() override;
		void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
		void set_polygon();
		QPolygonF myPolygon; // this maintains the shape of XBaseItem or it's subclass
		DiagramProxyWidget* proxyWidget;
		ItemWidget* xitemWidget;
		void createUniqueName(const std::string& classNameStr) override;
		void createUuid() override;
		void setClassName(const std::string& classNameStr) override;
		void configItem(const std::string& classNameStr);
    private:
		QMenu* myContextMenu;
		QList<XArrow*> arrows;
		QString text;
	signals:
		void showImageSignal(const std::string& filename, const cv::Mat& image, XBaseItem* item);
};

} // namespace XVisual

#endif //XBaseItem_H
