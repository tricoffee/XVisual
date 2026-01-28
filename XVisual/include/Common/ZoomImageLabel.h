#ifndef ZoomImageLabel_H
#define ZoomImageLabel_H

#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QTabWidget>
#include <QWheelEvent>
#include "Common/OpenCVHeaders.h"

namespace XVisual {

class ZoomImageLabel : public QLabel
{
	Q_OBJECT
	public:
		ZoomImageLabel(QWidget* parent = nullptr);
		void setPixmap(const QPixmap& pixmap);
	protected:
		void wheelEvent(QWheelEvent* event) override;
	private:
		QPixmap originalPixmap;
		double scaleFactor;
};

} // namespace XVisual

#endif // ZoomImageLabel_H
