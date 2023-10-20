#include "ItemWidget/DiagramProxyWidget.h"

DiagramProxyWidget::DiagramProxyWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags)
	:QGraphicsProxyWidget(parent)
{

}
QRectF DiagramProxyWidget::boundingRect() const
{
	return QGraphicsProxyWidget::boundingRect();
}