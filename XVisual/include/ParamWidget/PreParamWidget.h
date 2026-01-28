#ifndef PreParamWidget_H
#define PreParamWidget_H

#include <iostream>
#include <unordered_map>
#include <any>

#include <QWidget>

#include "ParamWidgetFactory.h"
#include "Common/PreParam.h"

namespace XVisual {

class PreParamWidget : public QWidget
{
	Q_OBJECT
	public:
		PreParamWidget(QWidget* parent = nullptr);
		virtual ~PreParamWidget();
		virtual void getPreParam(PreParam& params);
		virtual void setPreParam(const PreParam& params);
};

} // namespace XVisual

#endif // PreParamWidget_H
