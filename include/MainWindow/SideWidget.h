#ifndef SIDEWIDGET_H
#define SIDEWIDGET_H
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFontMetrics>
#include <QFrame>
#include "MainWindow/VariableWidget.h"
#include "Common/OpenCVHeaders.h"
class ImagePageWidget;
class NamesWidget;
class VariableWidget;
class XBaseItem;
class SideWidget : public QWidget
{
	Q_OBJECT
	public:
		SideWidget(QWidget* parent = nullptr);
		ImagePageWidget* getImagePageWidget();
	public slots:
		void showImageInTabSlot(const std::string& filename, const cv::Mat& image, XBaseItem* item);
    private slots:
		void on_foldButton_clicked();
		void tabAddSlot(const QString& itemName);
		void tabRemoveSlot(const QString& itemName);
    private:
		// btnWidget is a QWidget which holds a QHBoxLayout for foldButton
		QPushButton* foldButton;
		QWidget* btnWidget;
		QHBoxLayout* btnLayout;

		// imagePageWidget is a QWidget which holds a QHBoxLayout for upWidget
		ImagePageWidget* imagePageWidget;
		QHBoxLayout* upLayout;
		QWidget* upWidget;

		/*
		bottomWidget是SideWidget底部的widget其遵循水平布局bottomLayout, 
	    hSplitter被添加到bottomLayout, hSplitter包含namesWidget, variableWidget
		*/
		QSplitter* hSplitter;
	    NamesWidget* namesWidget;
		VariableWidget* variableWidget;
		QHBoxLayout* bottomLayout;
		QWidget* bottomWidget;

		/*
		rightWidget是btnWidget右侧的widget遵循垂直布局rightLayout, 
		vSplitter被添加到rightLayout, vSplitter包含upWidget, bottomWidget
		*/
		QSplitter* vSplitter;
		QVBoxLayout* rightLayout;
		QWidget* rightWidget;

		bool unfold;
		QHBoxLayout* sideLayout;
	

};

#endif // SIDEWIDGET_H
