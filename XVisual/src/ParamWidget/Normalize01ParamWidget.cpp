#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/DeletePtrMacro.h"
#include "ParamWidget/Normalize01ParamWidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

Normalize01ParamWidget::~Normalize01ParamWidget()
{
	//qDebug() << " === Normalize01ParamWidget::~Normalize01ParamWidget() === ";
	XLOG_INFO(" === Normalize01ParamWidget::~Normalize01ParamWidget() === ", CURRENT_THREAD_ID);
	SAFE_DELETE(label);
	SAFE_DELETE(mLayout);
}

Normalize01ParamWidget::Normalize01ParamWidget(QWidget* parent) : PreParamWidget(parent)
{
	//qDebug() << " @@@ Normalize01ParamWidget::Normalize01ParamWidget() @@@ ";
	XLOG_INFO(" @@@ Normalize01ParamWidget::Normalize01ParamWidget() @@@ ", CURRENT_THREAD_ID);
	label = new QLabel(tr("convert 0~255 to 0~1"));
	mLayout = new QHBoxLayout();
	mLayout->addWidget(label);
	// ���ò��ֵĶ��뷽ʽΪ����
	mLayout->setAlignment(Qt::AlignLeft);
	mLayout->setContentsMargins(0, 0, 0, 0); // ������߾�Ϊ0
	setLayout(mLayout);
}

void Normalize01ParamWidget::getPreParam(PreParam& params)
{
	float scale = 1 / 255.0;
	std::string preTagStr = "Normalize01";
	params.preTag = preTagStr;
	params.params["scale"] = scale;
}

void Normalize01ParamWidget::setPreParam(const PreParam& params)
{
	std::string preTagStr = params.preTag;
	XLOG_INFO(" === @@@ ResizePasteParamWidget::setPreParam @@@ ===, params.preTag :" + preTagStr, CURRENT_THREAD_ID);

	// Do nothing;
}

REGISTER_ParamWidget(Normalize01);
