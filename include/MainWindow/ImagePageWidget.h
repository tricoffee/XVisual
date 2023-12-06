#ifndef ImagePageWidget_H
#define ImagePageWidget_H

#include <QWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include "Common/OpenCVHeaders.h"

class XBaseItem;
class ImagePageWidget : public QWidget
{
	Q_OBJECT
	public:
		ImagePageWidget(QWidget* parent = nullptr);
		QTabWidget* getTabWidget();
    signals:
	    void tabChangedSignal(const QString& itemName);
	public slots:
		void tabSelectedSlot(const QString& itemName);
		void tabToggledSlot(const QString& itemName);
	private slots:
		void closeTab(int index);
		void tabCurrentSlot(int index);
	private:
		QTabWidget* tabWidget;
		QGridLayout* gridLayout;
};

#endif // ImagePageWidget_H

