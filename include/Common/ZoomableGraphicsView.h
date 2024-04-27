#ifndef ZoomaleGraphicsView_H
#define ZoomaleGraphicsView_H

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
class ZoomableGraphicsView : public QGraphicsView
{
	public:
		ZoomableGraphicsView(QWidget* parent = nullptr);
		void initLoad(cv::Mat image);
		QPixmap cvMatToQPixmap(const cv::Mat& inMat);
	protected:
		void wheelEvent(QWheelEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
	private:
		void centerImage();
		QGraphicsScene* scene;
		QGraphicsPixmapItem* pixmapItem;
		//cv::Mat inputImage; // 弃用该成员变量
		int imageWidth;
		int imageHeight;
		QPoint lastMousePos; // 用于存储上一次鼠标位置的变量
		QPoint currentMousePos; // 用于存储当前鼠标位置的变量
		QPointF delta;
};

#endif //ZoomaleGraphicsView_H
