#ifndef ConditionItem_H
#define ConditionItem_H

#include "ItemBase/XBaseItem.h"

class ConditionItem : public XBaseItem
{
	Q_OBJECT
	public:
		ConditionItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void setSourceFrom(const std::string& xName,
			const QString& yItemId, const std::string& yName) override;
		void ItemXOP(Source& sources, Dest& dests) override;
    protected:
		void createUniqueName() override;
};

#endif //ConditionItem_H