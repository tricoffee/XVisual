#ifndef SIDEWIDGET_H
#define SIDEWIDGET_H
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFontMetrics>
class SideWidget : public QWidget
{
	Q_OBJECT
	public:
		SideWidget(QWidget* parent = nullptr);
	public slots:
		void on_button1_clicked();
	private:
		// btnWidget is a QWidget which holds a QHBoxLayout for foldButton
		QPushButton* foldButton;
		QWidget* btnWidget;
		QHBoxLayout* btnLayout;
		// foldWidget is a QWidget which holds a QHBoxLayout
		QPushButton* buttonS1;
		QPushButton* buttonS2;
		QWidget* foldWidget;
		QHBoxLayout* foldLayout;

		bool unfold;
		QHBoxLayout* sideLayout;
};

#endif // SIDEWIDGET_H
