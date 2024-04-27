#ifndef LoadImageItem_H
#define LoadImageItem_H

#include "ItemBase/XBaseItem.h"

class LoadImageItem : public XBaseItem
{
    Q_OBJECT
	public:
		//LoadImageItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		// ���� GraphicsWidget* gWidget ����
		LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
    protected:
	   void createUniqueName(const QString& classNameQStr) override;
	   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	private:
		QString lastImagePath; // �����洢�ϴ�ѡ�е��ļ�·����˽�г�Ա����

};
#endif //LoadImageItem_H