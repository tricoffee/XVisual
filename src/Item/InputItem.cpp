#include "Item/InputItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"

InputItem::InputItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent)
{
	createUniqueName();
	QRect rect(-100, -100, 200, 200);
	QPainterPath path;
	int radius = 25;
	int cornersize = 2 * radius;
	path.moveTo(rect.left()+radius,rect.top());
	path.arcTo(rect.left(), rect.top(),cornersize,cornersize, 90,90);
	path.lineTo(rect.left(), rect.bottom() - radius);
	path.arcTo(rect.left(), rect.bottom() - cornersize, 
		cornersize, cornersize, 180, 90);
	path.lineTo(rect.right()-radius, rect.bottom());
	path.arcTo(rect.right() - cornersize, rect.bottom()-cornersize,
		cornersize, cornersize, 270, 90);
	path.lineTo(rect.right(),rect.top()+radius);
	path.arcTo(rect.right()-cornersize,rect.top(),
		cornersize, cornersize, 0, 90);
	myPolygon = path.toFillPolygon();
	setPolygon(myPolygon);
	// 设置显示在Item上的文本
	setEditText(uniqueName);
}
QPixmap InputItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void InputItem::debug()
{
	qDebug() << "InputItem::debug() ... ";
	qDebug() << "InputItem::boundingRect() " << boundingRect();
	qDebug() << "InputItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "InputItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "InputItem::uuid " << QString::fromStdString(uuid);
}
void InputItem::ItemXOP()
{

}
void InputItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("Input");
	setObjectName(uniqueName);
}
REGISTER_CLASS(Input);