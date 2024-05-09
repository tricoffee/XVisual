#ifndef PageWidget_H
#define PageWidget_H

#include <QWidget>
#include <QVBoxLayout>

class PageWidget : public QWidget
{
	Q_OBJECT
	public:
		PageWidget(QWidget* parent = nullptr);
		void addWidgetsToLayout(QList<QWidget*> widgetList);
	private:
		QVBoxLayout* vLayout;
};

#endif // PageWidget_H

