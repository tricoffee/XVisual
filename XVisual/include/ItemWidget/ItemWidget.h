#ifndef ItemWidget_H
#define ItemWidget_H

#include <QWidget>
#include <QTextEdit>

class XTextEdit;
class ItemWidget : public QWidget
{
	public:
		ItemWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		void initWidget();
		XTextEdit* getEdit();
	protected:
		XTextEdit* edit;
};

#endif //ItemWidget_H