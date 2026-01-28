#ifndef XARROW_H
#define XARROW_H

#include <QGraphicsLineItem>
#include <QStandardItemModel>
#include "TableWidget/TableData.h"

namespace XVisual {

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
	QStandardItemModel*& getLatestTableViewModel();
	void setLatestTableViewModel(QStandardItemModel* newModel);
	void setDefaultYforVariablesX(std::unordered_map<std::string, TableData> mDefaultValues);
	std::unordered_map<std::string, TableData>& getDefaultYforVariablesX();
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
	QStandardItemModel* latestTableViewModel = nullptr;
	std::unordered_map<std::string, TableData> defaultValues;
};

} // namespace XVisual

#endif // XARROW_H
