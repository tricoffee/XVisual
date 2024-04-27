#include "Common/ZoomableGraphicsView.h"
ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
{
	scene = new QGraphicsScene(this);
	setScene(scene);
	//QString filePath = "C:/NDev/CPPDev/XVisual/SEB2023.bmp";
	//cv::Mat inputImage = cv::imread(filePath.toStdString());
	//initLoad(inputImage);
	// ���ù�����
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
// ������ʾͼ��
void ZoomableGraphicsView::centerImage()
{
	// ��ȡ������ͼ����ľ���
	QRectF sceneRect = scene->sceneRect();
	QRectF itemRect = pixmapItem->sceneBoundingRect();

	// �������λ��
	qreal xOffset = (sceneRect.width() - itemRect.width()) / 2.0;
	qreal yOffset = (sceneRect.height() - itemRect.height()) / 2.0;

	// ����ͼ�����λ��
	pixmapItem->setPos(sceneRect.x() + xOffset, sceneRect.y() + yOffset);
}
void ZoomableGraphicsView::initLoad(cv::Mat inputImage)
{
	// ��ȡͼ��Ļ�����Ϣ
	imageWidth = inputImage.cols;
	imageHeight = inputImage.rows;
	// ��ճ���
	scene->clear();
	// viewport geometry
	int viewWidth = viewport()->geometry().width();
	int viewHeight = viewport()->geometry().height();
	// ��������ͼ�����ű���
	qreal hScaleFactor = qMin(1.0, static_cast<qreal>(viewWidth) / static_cast<qreal>(imageWidth));
	qreal vScaleFactor = qMin(1.0, static_cast<qreal>(viewHeight) / static_cast<qreal>(imageHeight));
	qDebug() << "hScaleFactor =  " << hScaleFactor;
	qDebug() << "vScaleFactor =  " << vScaleFactor;
	// �����������ű���ΪscaleFactor
	qreal scaleFactor = qMin(hScaleFactor, vScaleFactor);
	//
	QPixmap pixmap = cvMatToQPixmap(inputImage);
	pixmapItem = new QGraphicsPixmapItem(pixmap);
	pixmapItem->setPos(0, 0);
	scene->addItem(pixmapItem);
	// ����ͼ����
	QTransform transform;
	transform.scale(scaleFactor, scaleFactor);
	setTransform(transform);
	// ������ʾͼ��
	centerImage();
}
QPixmap ZoomableGraphicsView::cvMatToQPixmap(const cv::Mat& inMat)
{
	switch (inMat.type())
	{
		// 8-bit, 4 channel
		case CV_8UC4:
		{
			QImage image(inMat.data,
				inMat.cols, inMat.rows,
				static_cast<int>(inMat.step),
				QImage::Format_ARGB32);

			return QPixmap::fromImage(image);
		}
		// 8-bit, 3 channel
		case CV_8UC3:
		{
			QImage image(inMat.data,
				inMat.cols, inMat.rows,
				static_cast<int>(inMat.step),
				QImage::Format_RGB888);

			return QPixmap::fromImage(image.rgbSwapped());
		}
		// 8-bit, 1 channel
		case CV_8UC1:
		{
			QImage image(inMat.data,
				inMat.cols, inMat.rows,
				static_cast<int>(inMat.step),
				QImage::Format_Grayscale8);

			return QPixmap::fromImage(image);
		}
		default:
			qWarning() << "cvMatToQPixmap() - cv::Mat image type not handled in switch:" << inMat.type();
			break;
	}
	return QPixmap();
}
void ZoomableGraphicsView::wheelEvent(QWheelEvent* event)
{
	if (event->angleDelta().y() > 0)
	{
		scale(1.1, 1.1);
	}
	else
	{
		scale(1 / 1.1, 1 / 1.1);
	}
}
void ZoomableGraphicsView::mousePressEvent(QMouseEvent* event)
{
	// ��¼��갴�µĳ�ʼλ��
	lastMousePos = event->pos();
}
void ZoomableGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	// ��ȡ���λ��
	currentMousePos = event->position().toPoint();
}
void ZoomableGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	// ��������ƶ��ľ���
	delta = mapToScene(currentMousePos) - mapToScene(lastMousePos);
	// ����lastMousePos
	lastMousePos = currentMousePos;
	qDebug() << " scene->sceneRect() = " << scene->sceneRect();
	QRectF newRect = scene->sceneRect().translated(-delta);
	// Set the scene's sceneRect to the new rectangle
	scene->setSceneRect(newRect);
}

