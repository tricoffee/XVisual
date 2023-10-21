#ifndef CVCropItem_H
#define CVCropItem_H

#include "ItemBase/XBaseItem.h"

class CVCropItem : public XBaseItem
{
	Q_OBJECT
public:
	CVCropItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
	QPixmap image() override;
	void debug();
	void initParameters() override;
	Source& getSources();
	Dest& getDests();
	void setSourceFrom(const std::string& xName,
		const QString& yItemId, const std::string& yName) override;
	void ItemXOP(Source& sources, Dest& dests) override;
protected:
	void createUniqueName() override;
private:
	Source sources;
	Dest dests;
};

#endif //CVCropItem_H

