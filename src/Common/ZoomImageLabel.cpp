#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QTabWidget>
#include <QWheelEvent>
#include "Common/OpenCVHeaders.h"
#include "Common/ZoomImageLabel.h"

ZoomImageLabel::ZoomImageLabel(QWidget* parent):QLabel(parent), scaleFactor(1.0)
{
	setScaledContents(true);
}
void ZoomImageLabel::setPixmap(const QPixmap& pixmap)
{
	originalPixmap = pixmap;
	QLabel::setPixmap(originalPixmap.scaled(originalPixmap.size() * scaleFactor));
}
void ZoomImageLabel::wheelEvent(QWheelEvent* event)
{
	// 滚轮滚动的角度
	int delta = event->angleDelta().y();

	// 调整缩放因子
	scaleFactor += delta > 0 ? 0.1 : -0.1;

	// 限制缩放因子的范围
	scaleFactor = qMax(0.1, qMin(scaleFactor, 3.0));

	// 根据缩放因子调整QPixmap的大小
	QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor);

	// 在QLabel上显示调整后的QPixmap
	QLabel::setPixmap(scaledPixmap);
}

