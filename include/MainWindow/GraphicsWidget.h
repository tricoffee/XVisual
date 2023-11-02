#ifndef GraphicsWidget_H
#define GraphicsWidget_H
#include "Common/constants.h"
#include "TableWidget/TableData.h"
class DiagramScene;
class QGraphicsView;
class XArrow;
class CustomTableView;
#include <QWidget>
class GraphicsWidget : public QWidget
{
	Q_OBJECT
	public:
		GraphicsWidget(QMenu* itemMenu, QWidget* parent = nullptr, 
			Qt::WindowFlags f = Qt::WindowFlags());
		void setBackgroundChange(const int& id);
		void setBackgroundBrushChange(const QBrush& brush);
		void setSceneScaleChange(const QString& scale);
		void setItemTextColor(const QColor& color);
		void setSceneItemColor(const QColor& color);
		void setSceneLineColor(const QColor& color);
		void setSceneFont(const QFont& font);
		void setDiagramName(const QString& name);
		void setDiagramType(DiagramType type);
		void setDiagramState(DiagramState state);
	public slots:
		void bringToFront();
		void sendToBack();
		void deleteItem();
		void showTableViewSlot(const QString& xName, const QString& yName, XArrow* xLineArrow,
			QList<TableData> variablesX, QList<TableData> variablesY);

    private:
		int scene_width;
		int scene_height;
		QMenu* itemMenu;
		DiagramScene* scene;
		QGraphicsView* view;
		CustomTableView* tableView = nullptr;
};

#endif //GraphicsWidget_H
