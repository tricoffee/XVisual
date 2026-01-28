#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/DeletePtrMacro.h"
#include "ParamWidget/ResizeParamWidget.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

namespace XVisual {

ResizeParamWidget::~ResizeParamWidget()
{
	// qDebug() << " === ResizeParamWidget::~ResizeParamWidget() === ";
	XLOG_INFO(" === ResizeParamWidget::~ResizeParamWidget() === ", CURRENT_THREAD_ID);
	SAFE_DELETE(dstWidthLabel);
	SAFE_DELETE(dstWidthLineEdit);
	SAFE_DELETE(dstWidthLayout);
	SAFE_DELETE(dstHeightLabel);
	SAFE_DELETE(dstHeightLineEdit);
	SAFE_DELETE(dstHeightLayout);
	SAFE_DELETE(mLayout);
}

ResizeParamWidget::ResizeParamWidget(QWidget* parent) : PreParamWidget(parent)
{
	//qDebug() << " @@@ ResizeParamWidget::ResizeParamWidget() @@@ ";
	XLOG_INFO(" === @@@ ResizeParamWidget::ResizeParamWidget() @@@ === ", CURRENT_THREAD_ID);
	dstWidthLayout = new QHBoxLayout();
	dstWidthLabel = new QLabel(tr("targetWidth:"));
	dstWidthLineEdit = new QLineEdit;
	dstWidthLineEdit->setText("256");
	dstWidthLayout->addWidget(dstWidthLabel);
	dstWidthLayout->addWidget(dstWidthLineEdit);
	dstWidthLayout->setContentsMargins(0, 0, 0, 0); // 设置外边距为0

	dstHeightLayout = new QHBoxLayout();
	dstHeightLabel = new QLabel(tr("targetHeight:"));
	dstHeightLineEdit = new QLineEdit;
	dstHeightLineEdit->setText("256");
	dstHeightLayout->addWidget(dstHeightLabel);
	dstHeightLayout->addWidget(dstHeightLineEdit);
	dstHeightLayout->setContentsMargins(0, 0, 0, 0); // 设置外边距为0

	mLayout = new QVBoxLayout();
	mLayout->addLayout(dstWidthLayout);
	mLayout->addLayout(dstHeightLayout);
	// 设置布局的对齐方式为居中
	mLayout->setAlignment(Qt::AlignCenter);
	mLayout->setContentsMargins(0, 0, 0, 0); // 设置外边距为0
	setLayout(mLayout);
}


void ResizeParamWidget::getPreParam(PreParam& params)
{
	std::string preTagStr = "Resize";
	params.preTag = preTagStr;
	params.params["targetWidth"] = dstWidthLineEdit->text().toInt();
	params.params["targetHeight"] = dstHeightLineEdit->text().toInt();
}

void ResizeParamWidget::setPreParam(const PreParam& params)
{
	std::string preTagStr = params.preTag;
	if ("Resize" == preTagStr)
	{
		XLOG_INFO(" === @@@ ResizeParamWidget::setPreParam @@@ ===, PreTag :" + preTagStr, CURRENT_THREAD_ID);
		std::unordered_map<std::string, std::any> imagePreParams = std::any_cast<std::unordered_map<std::string, std::any>>(params.params);
		int width = std::any_cast<int>(imagePreParams["targetWidth"]);
		int height = std::any_cast<int>(imagePreParams["targetHeight"]);
		dstWidthLineEdit->setText(QString::number(width));
		dstHeightLineEdit->setText(QString::number(height));
	}
	else
	{
		XLOG_INFO(" === @@@ ResizeParamWidget::setPreParam @@@ ===, UnknownPreTag :" + preTagStr, CURRENT_THREAD_ID);
	}
}

REGISTER_ParamWidget(Resize);

} // namespace XVisual
