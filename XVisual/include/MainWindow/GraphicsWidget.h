#ifndef GraphicsWidget_H
#define GraphicsWidget_H
#include <QWidget>
#include "Common/constants.h"
#include "Common/OpenCVHeaders.h"
#include "TableWidget/TableData.h"
#include <QStandardItemModel>

class DiagramScene;
class QGraphicsView;
class XArrow;
class CustomTableView;
class XBaseItem;

class GraphicsWidget : public QWidget
{
	Q_OBJECT
	public:
		GraphicsWidget(QMenu* itemMenu, QWidget* parent = nullptr, 
			Qt::WindowFlags f = Qt::WindowFlags());
		void setBackgroundBrushChange(const QBrush& brush);
		void setSceneScaleChange(const QString& scale);
		void setItemTextColor(const QColor& color);
		void setSceneItemColor(const QColor& color);
		void setSceneLineColor(const QColor& color);
		void setSceneFont(const QFont& font);
		void setDiagramName(const QString& name);
		void setDiagramType(DiagramType type);
		void setDiagramState(DiagramState state);
		DiagramScene*& getScene();
	signals:
		void showImageInTabSignal(const std::string& filename, const cv::Mat& image, XBaseItem* item);
	public slots:
		void bringToFront();
		void sendToBack();
		void deleteItem();
		void showTableViewSlot(const QString& xName, const QString& yName, XArrow* xLineArrow,
			QList<TableData> variablesX, QList<TableData> variablesY);
		void showImageSlot(const std::string& filename, const cv::Mat& image, XBaseItem* item);
    private:
		int scene_width;
		int scene_height;
		QMenu* itemMenu;
		DiagramScene* scene;
		QGraphicsView* view;
		CustomTableView* tableView = nullptr;
};

#endif //GraphicsWidget_H
