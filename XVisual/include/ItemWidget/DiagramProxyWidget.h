#ifndef DiagramProxyWidget_H
#define DiagramProxyWidget_H

#include <QGraphicsProxyWidget>

namespace XVisual {

class DiagramProxyWidget : public QGraphicsProxyWidget
{
	public:
		DiagramProxyWidget(QGraphicsItem* parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
		QRectF boundingRect() const override;
};

} // namespace XVisual

#endif //DiagramProxyWidget_H
