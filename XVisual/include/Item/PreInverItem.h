#ifndef PreInverItem_H
#define PreInverItem_H

#include "ItemBase/XBaseItem.h"
//#include "Common/DebugConfigMacro.h"
#include "Common/PreParam.h"

namespace XVisual {

class PreInverItem : public XBaseItem
{
	Q_OBJECT
	public:
		PreInverItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:
	#ifdef DEBUG_PRINT
		void printPreParam(const PreParam& mParam);
	#endif
};

} // namespace XVisual

#endif //PreInverItem_H



