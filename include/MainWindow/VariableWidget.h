#ifndef VariableWidget_H
#define VariableWidget_H

//#include <QWidget>
//#include <QHBoxLayout>
//#include <QVBoxLayout>
//#include <QLabel>
//#include <QLineEdit>
//#include <QDebug>

//class XBaseItem;
//class VariableWidget : public QWidget
//{
//	Q_OBJECT
//	public:
//		VariableWidget(QWidget* parent = nullptr);
//	private:
//		// idWidget is a QWidget for idLabel and idLineEdit
//		QLabel* idLabel;
//		QLineEdit* idLineEdit;
//		QHBoxLayout* idLayout;
//		QWidget* idWidget;
//		// nameWidget is a QWidget for nameLabel and nameLineEdit
//		QLabel* nameLabel;
//		QLineEdit* nameLineEdit;
//		QHBoxLayout* nameLayout;
//		QWidget* nameWidget;
//		// vLayout is a Vertical Layout which contains idWidget and nameLayout
//	    QVBoxLayout* vLayout;
//
//		
//};

#include <QTreeWidget>
class XBaseItem;
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