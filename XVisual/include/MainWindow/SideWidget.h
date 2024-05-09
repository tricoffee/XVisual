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
		bottomWidget��SideWidget�ײ���widget����ѭˮƽ����bottomLayout, 
	    hSplitter����ӵ�bottomLayout, hSplitter����namesWidget, variableWidget
		*/
		QSplitter* hSplitter;
	    NamesWidget* namesWidget;
		VariableWidget* variableWidget;
		QHBoxLayout* bottomLayout;
		QWidget* bottomWidget;

		/*
		rightWidget��btnWidget�Ҳ��widget��ѭ��ֱ����rightLayout, 
		vSplitter����ӵ�rightLayout, vSplitter����upWidget, bottomWidget
		*/
		QSplitter* vSplitter;
		QVBoxLayout* rightLayout;
		QWidget* rightWidget;

		bool unfold;
		QHBoxLayout* sideLayout;
	

};

#endif // SIDEWIDGET_H
