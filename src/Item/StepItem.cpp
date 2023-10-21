#include "Item/StepItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"

StepItem::StepItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent)
{
	createUniqueName();
	myPolygon << QPointF(-100, -100) << QPointF(100, -100)
		<< QPointF(100, 100) << QPointF(-100, 100)
		<< QPointF(-100, -100);
	setPolygon(myPolygon);
	// 设置显示在Item上的文本
	setEditText(uniqueName);
}
QPixmap StepItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void StepItem::debug()
{
	qDebug() << "StepItem::debug() ... ";
	qDebug() << "StepItem::boundingRect() " << boundingRect();
	qDebug() << "StepItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "StepItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "StepItem::uuid " << QString::fromStdString(uuid);
}
void StepItem::ItemXOP(Source& sources, Dest& dests)
{

}
void StepItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("Step");
	setObjectName(uniqueName);
}
REGISTER_CLASS(Step);