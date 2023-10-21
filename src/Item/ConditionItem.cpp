#include "Item/ConditionItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"

ConditionItem::ConditionItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent)
{
	createUniqueName();
	myPolygon << QPointF(-100, 0) << QPointF(0, 100)
		      << QPointF(100, 0)  << QPointF(0, -100)
		      << QPointF(-100, 0);
	setPolygon(myPolygon);
	// 设置显示在Item上的文本
	setEditText(uniqueName);
}
QPixmap ConditionItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void ConditionItem::debug()
{
	qDebug() << "ConditionItem::debug() ... ";
	qDebug() << "ConditionItem::boundingRect() " << boundingRect();
	qDebug() << "ConditionItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "ConditionItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "ConditionItem::uuid " << QString::fromStdString(uuid);
}
void ConditionItem::ItemXOP(Source& sources, Dest& dests)
{

}
void ConditionItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("Condition");
	setObjectName(uniqueName);
}
void ConditionItem::setSourceFrom(const std::string& xName,
	const QString& yItemId, const std::string& yName)
{

}
REGISTER_CLASS(Condition);