#ifndef PreParamWidget_H
#define PreParamWidget_H

#include <QWidget>
#include <unordered_map>
#include <any>
#include <iostream>
#include "ParamWidgetFactory.h"
#include "Common/PreParam.h"

class PreParamWidget : public QWidget
{
	Q_OBJECT
	public:
		PreParamWidget(QWidget* parent = nullptr);
		virtual ~PreParamWidget();
		virtual void getPreParam(PreParam& params);
		virtual void setPreParam(const PreParam& params);
};

#endif // PreParamWidget_H
