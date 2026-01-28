#ifndef ImagePreItem_H
#define ImagePreItem_H

#include "ItemBase/XBaseItem.h"
//#include "Common/DebugConfigMacro.h"
#include "Common/PreParam.h"

namespace XVisual {

class ImagePreItem : public XBaseItem
{
	Q_OBJECT
	public:
		ImagePreItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:
	#ifdef DEBUG_PRINT
		void printPreParam(const PreParam& mParam);
	#endif
};

} // namespace XVisual

#endif //ImagePreItem_H


