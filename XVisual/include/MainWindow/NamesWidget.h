#ifndef NamesWidget_H
#define NamesWidget_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>

namespace XVisual {

class RowWidget;

class NamesWidget : public QWidget
{
	Q_OBJECT 
    public:
		NamesWidget(QWidget* parent = nullptr);
		void addRow(const QString& text);
    signals:
	    void tabAddSignal(const QString& itemName);
		void tabRemoveSignal(const QString& itemName);
		void itemSelectedSignal(const QString& itemName);
	public slots:
		void itemChangedSlot(const QString& itemName);
		void itemToggledSlot(const QString& itemName);
    private:
		QVBoxLayout* layout;
		QHBoxLayout* rowLayout;
		RowWidget* rowWidget;
		QLabel* titleLabel;
		QLabel* label;
		QPushButton* addButton1;
		QPushButton* deleteButton;
		QString text;
		QListWidget* listWidget;
		QListWidgetItem* widgetItem;
	private slots:
		void tabInsertSlot(const QString& itemName);
		void tabDeleteSlot(const QString& itemName);
	    void onListWidgetItemClicked(QListWidgetItem* item);
};

} // namespace XVisual

#endif // NamesWidget_H
