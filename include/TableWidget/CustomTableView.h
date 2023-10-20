#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H
#include <QTableView>
#include <QGraphicsLineItem>
class CustomTableView : public QTableView
{
		Q_OBJECT
	public:
		CustomTableView(QWidget* parent = nullptr);
		CustomTableView(QGraphicsLineItem* lineArrow, QWidget* parent = nullptr);
	protected:
		void closeEvent(QCloseEvent* event) override;
signals:
	void changeLineArrowColorSignal(const QColor& color);

};
#endif //CUSTOMTABLEVIEW_H