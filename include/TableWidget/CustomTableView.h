#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H
#include <QTableView>
#include <QGraphicsLineItem>
class XBaseItem;
class CustomTableView : public QTableView
{
	Q_OBJECT
	public:
		CustomTableView(QWidget* parent = nullptr);
		CustomTableView(QGraphicsLineItem* lineArrow, QWidget* parent = nullptr);
	protected:
		void closeEvent(QCloseEvent* event) override;
    private:
		XBaseItem* yItem;
		XBaseItem* xItem;
	    QString yItemId;
	    QString xItemId;
		signals:
			void changeLineArrowColorSignal(const QColor& color);

};
#endif //CUSTOMTABLEVIEW_H