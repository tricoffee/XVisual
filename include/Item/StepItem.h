#ifndef StepItem_H
#define StepItem_H

#include "ItemBase/XBaseItem.h"

class StepItem : public XBaseItem
{
	Q_OBJECT
	public:
		StepItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void setSourceFrom(const std::string& xName,
			const QString& yItemId, const std::string& yName) override;
		void ItemXOP(Source& sources, Dest& dests) override;
	protected:
		void createUniqueName() override;
};

#endif //StepItem_H