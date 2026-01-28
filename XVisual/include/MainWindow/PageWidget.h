#ifndef PageWidget_H
#define PageWidget_H

#include <QWidget>
#include <QVBoxLayout>

namespace XVisual {

class PageWidget : public QWidget
{
	Q_OBJECT
	public:
		PageWidget(QWidget* parent = nullptr);
		void addWidgetsToLayout(QList<QWidget*> widgetList);
	private:
		QVBoxLayout* vLayout;
};

} // namespace XVisual

#endif // PageWidget_H

