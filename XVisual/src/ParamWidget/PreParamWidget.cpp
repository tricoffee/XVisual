#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/DeletePtrMacro.h"
#include "ParamWidget/PreParamWidget.h"

namespace XVisual {

PreParamWidget::~PreParamWidget()
{
	//qDebug() << " === PreParamWidget::~PreParamWidget() === ";
	XLOG_INFO(" === PreParamWidget::~PreParamWidget() === ", CURRENT_THREAD_ID);
}

PreParamWidget::PreParamWidget(QWidget* parent) : QWidget(parent)
{
	//qDebug() << " @@@ PreParamWidget::PreParamWidget() @@@ ";
	XLOG_INFO(" @@@ PreParamWidget::PreParamWidget() @@@ ", CURRENT_THREAD_ID);
}

void PreParamWidget::getPreParam(PreParam& params)
{
	// Do nothing;
}

void PreParamWidget::setPreParam(const PreParam& params)
{
	// Do nothing;
}

REGISTER_ParamWidget(Pre);

} // namespace XVisual
