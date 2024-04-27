#include "Item/OutputItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "MainWindow/GraphicsWidget.h"

OutputItem::OutputItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent)
{
	createUniqueName();
	myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
		<< QPointF(120, 80) << QPointF(70, -80)
		<< QPointF(-120, -80);
	setPolygon(myPolygon);
	// 设置显示在Item上的文本
	setEditText(uniqueName);
}
OutputItem::OutputItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{
	createUniqueName();
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
void OutputItem::ItemXOP()
{

}
void OutputItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("Output");
	setObjectName(uniqueName);
}
REGISTER_ITEM(Output);