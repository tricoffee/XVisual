#ifndef ItemWidget_H
#define ItemWidget_H

#include <QWidget>
#include <QTextEdit>

class XTextEdit;

namespace XVisual {

class ItemWidget : public QWidget
{
	public:
		ItemWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		void initWidget();
		XTextEdit* getEdit();
	protected:
		XTextEdit* edit;
};

} // namespace XVisual

#endif //ItemWidget_H
