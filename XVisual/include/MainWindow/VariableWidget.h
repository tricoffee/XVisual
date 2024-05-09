#ifndef VariableWidget_H
#define VariableWidget_H

#include <QTreeWidget>

class XBaseItem;
class XBaseHandle;
class VariableWidget : public QWidget
{
	Q_OBJECT
	public:
		VariableWidget(QWidget* parent = nullptr);
		void createTree(XBaseItem* xItem);
	signals:
		void nodeToggledSignal(const QString& itemName);
    public slots:
	    void nodeSelectedSlot(const QString& itemName);
		void nodeChangedSlot(const QString& itemName);
	private slots:
		void nodeSelectionChanged();
    private:
		QTreeWidget* treeWidget;
		QTreeWidgetItem* sourcesNode;
		QTreeWidgetItem* destsNode;
		QTreeWidgetItem* itemNode;
};

#endif // VariableWidget_H