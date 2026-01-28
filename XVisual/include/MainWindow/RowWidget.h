#ifndef RowWidget_H
#define RowWidget_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

namespace XVisual {

class RowWidget : public QWidget
{
	Q_OBJECT
	public:
		RowWidget(const QString& xText, QWidget* parent = nullptr);
		QString getText() const;
		QLabel* getLabel();
    signals:
	    void tabInsertSignal(const QString& xText);
    signals:
		void tabDeleteSignal(const QString& xText);
	private:
		const QString text;
		QLabel* label;
		QPushButton* addButton1;
		QPushButton* deleteButton;
		QHBoxLayout* rowLayout;
	private slots:
		void onAddButton1Clicked();
		void onDeleteButtonClicked();

};

} // namespace XVisual

#endif // RowWidget_H

