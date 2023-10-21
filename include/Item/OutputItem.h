#ifndef OutputItem_H
#define OutputItem_H

#include "ItemBase/XBaseItem.h"

class OutputItem : public XBaseItem
{
	Q_OBJECT
	public:
		OutputItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void setSourceFrom(const std::string& xName,
			const QString& yItemId, const std::string& yName) override;
		void ItemXOP(Source& sources, Dest& dests) override;
    protected:
		void createUniqueName() override;
};

#endif //OutputItem_H