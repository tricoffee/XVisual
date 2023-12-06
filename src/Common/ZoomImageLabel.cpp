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
	// ���ֹ����ĽǶ�
	int delta = event->angleDelta().y();

	// ������������
	scaleFactor += delta > 0 ? 0.1 : -0.1;

	// �����������ӵķ�Χ
	scaleFactor = qMax(0.1, qMin(scaleFactor, 3.0));

	// �����������ӵ���QPixmap�Ĵ�С
	QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor);

	// ��QLabel����ʾ�������QPixmap
	QLabel::setPixmap(scaledPixmap);
}

