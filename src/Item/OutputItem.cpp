#include "Item/OutputItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"

OutputItem::OutputItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent)
{
	QString uniqueName = ItemManager::instance().getUniqueItemName("OutputItem");
	setObjectName(uniqueName);
	myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
		<< QPointF(120, 80) << QPointF(70, -80)
		<< QPointF(-120, -80);
	setPolygon(myPolygon);
	// 设置显示在Item上的文本
	setEditText(uniqueName);
}
QPixmap OutputItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void OutputItem::debug()
{
	qDebug() << "OutputItem::debug() ... ";
	qDebug() << "OutputItem::boundingRect() " << boundingRect();
	qDebug() << "OutputItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "OutputItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "OutputItem::uuid " << QString::fromStdString(uuid);
}
void OutputItem::ItemXOP(Source& sources, Dest& dests)
{

}
REGISTER_CLASS(Output);