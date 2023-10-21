#ifndef InputItem_H
#define InputItem_H

#include "ItemBase/XBaseItem.h"

class InputItem : public XBaseItem
{
	Q_OBJECT
	public:
		InputItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void setSourceFrom(const std::string& xName,
			const QString& yItemId, const std::string& yName) override;
		void ItemXOP(Source& sources, Dest& dests) override;
    protected:
		void createUniqueName() override;
};

#endif //InputItem_H