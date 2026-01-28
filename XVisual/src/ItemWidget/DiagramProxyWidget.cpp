#include "ItemWidget/DiagramProxyWidget.h"

namespace XVisual {

	DiagramProxyWidget::DiagramProxyWidget(QGraphicsItem* parent, Qt::WindowFlags wFlags)
		:QGraphicsProxyWidget(parent)
	{

	}
	QRectF DiagramProxyWidget::boundingRect() const
	{
		return QGraphicsProxyWidget::boundingRect();
	}

} // namespace XVisual 