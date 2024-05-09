#include "Common/ZoomableGraphicsView.h"
ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
{
	scene = new QGraphicsScene(this);
	setScene(scene);
	//QString filePath = "C:/NDev/CPPDev/XVisual/SEB2023.bmp";
	//cv::Mat inputImage = cv::imread(filePath.toStdString());
	//initLoad(inputImage);
	// 禁用滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
// 居中显示图像
void ZoomableGraphicsView::centerImage()
{
	// 获取场景和图像项的矩形
	QRectF sceneRect = scene->sceneRect();
	QRectF itemRect = pixmapItem->sceneBoundingRect();

	// 计算居中位置
	qreal xOffset = (sceneRect.width() - itemRect.width()) / 2.0;
	qreal yOffset = (sceneRect.height() - itemRect.height()) / 2.0;

	// 设置图像项的位置
	pixmapItem->setPos(sceneRect.x() + xOffset, sceneRect.y() + yOffset);
}
void ZoomableGraphicsView::initLoad(cv::Mat inputImage)
{
	// 获取图像的基本信息
	imageWidth = inputImage.cols;
	imageHeight = inputImage.rows;
	// 清空场景
	scene->clear();
	// viewport geometry
	int viewWidth = viewport()->geometry().width();
	int viewHeight = viewport()->geometry().height();
	// 计算缩略图的缩放比例
	qreal hScaleFactor = qMin(1.0, static_cast<qreal>(viewWidth) / static_cast<qreal>(imageWidth));
	qreal vScaleFactor = qMin(1.0, static_cast<qreal>(viewHeight) / static_cast<qreal>(imageHeight));
	qDebug() << "hScaleFactor =  " << hScaleFactor;
	qDebug() << "vScaleFactor =  " << vScaleFactor;
	// 设置整体缩放比例为scaleFactor
	qreal scaleFactor = qMin(hScaleFactor, vScaleFactor);
	//
	QPixmap pixmap = cvMatToQPixmap(inputImage);
	pixmapItem = new QGraphicsPixmapItem(pixmap);
	pixmapItem->setPos(0, 0);
	scene->addItem(pixmapItem);
	// 缩放图像项
	QTransform transform;
	transform.scale(scaleFactor, scaleFactor);
	setTransform(transform);
	// 居中显示图像
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
	// 记录鼠标按下的初始位置
	lastMousePos = event->pos();
}
void ZoomableGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	// 获取鼠标位置
	currentMousePos = event->position().toPoint();
}
void ZoomableGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	// 计算鼠标移动的距离
	delta = mapToScene(currentMousePos) - mapToScene(lastMousePos);
	// 更新lastMousePos
	lastMousePos = currentMousePos;
	qDebug() << " scene->sceneRect() = " << scene->sceneRect();
	QRectF newRect = scene->sceneRect().translated(-delta);
	// Set the scene's sceneRect to the new rectangle
	scene->setSceneRect(newRect);
}

