#ifndef DiagramProxyWidget_H
#define DiagramProxyWidget_H

#include <QGraphicsProxyWidget>

class DiagramProxyWidget : public QGraphicsProxyWidget
{
	public:
		DiagramProxyWidget(QGraphicsItem* parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
		QRectF boundingRect() const override;
};

#endif //DiagramProxyWidget_H