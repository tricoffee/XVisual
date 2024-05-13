#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/DeletePtrMacro.h"
#include "ParamWidget/ResizePasteParamWidget.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

ResizePasteParamWidget::~ResizePasteParamWidget()
{
	// qDebug() << " === ResizePasteParamWidget::~ResizePasteParamWidget() === ";
	XLOG_INFO(" === ResizePasteParamWidget::~ResizePasteParamWidget() === ", CURRENT_THREAD_ID);
	SAFE_DELETE(dstWidthLabel);
	SAFE_DELETE(dstWidthLineEdit);
	SAFE_DELETE(dstWidthLayout);
	SAFE_DELETE(dstHeightLabel);
	SAFE_DELETE(dstHeightLineEdit);
	SAFE_DELETE(dstHeightLayout);
	SAFE_DELETE(mLayout);
}

ResizePasteParamWidget::ResizePasteParamWidget(QWidget* parent) : PreParamWidget(parent)
{
	//qDebug() << " @@@ ResizePasteParamWidget::ResizePasteParamWidget() @@@ ";
	XLOG_INFO(" === @@@ ResizePasteParamWidget::ResizePasteParamWidget() @@@ === ", CURRENT_THREAD_ID);
	dstWidthLayout = new QHBoxLayout();
	dstWidthLabel = new QLabel(tr("dstWidth:"));
	dstWidthLineEdit = new QLineEdit;
	dstWidthLineEdit->setText("512");
	dstWidthLayout->addWidget(dstWidthLabel);
	dstWidthLayout->addWidget(dstWidthLineEdit);
	dstWidthLayout->setContentsMargins(0, 0, 0, 0); // 设置外边距为0

	dstHeightLayout = new QHBoxLayout();
	dstHeightLabel = new QLabel(tr("dstHeight:"));
	dstHeightLineEdit = new QLineEdit;
	dstHeightLineEdit->setText("512");
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

void ResizePasteParamWidget::getPreParam(PreParam& params)
{
	std::string preTagStr = "ResizePaste";
	params.preTag = preTagStr;
	params.params["dstWidth"] = dstWidthLineEdit->text().toInt();
	params.params["dstHeight"] = dstHeightLineEdit->text().toInt();
}

void ResizePasteParamWidget::setPreParam(const PreParam& params)
{
	std::string preTagStr = params.preTag;
	if ("ResizePaste" == preTagStr)
	{
		XLOG_INFO(" === @@@ ResizePasteParamWidget::setPreParam @@@ ===, PreTag :" + preTagStr, CURRENT_THREAD_ID);
		std::unordered_map<std::string, std::any> imagePreParams = std::any_cast<std::unordered_map<std::string, std::any>>(params.params);
		int width = std::any_cast<int>(imagePreParams["dstWidth"]);
		int height = std::any_cast<int>(imagePreParams["dstHeight"]);
		dstWidthLineEdit->setText(QString::number(width));
		dstHeightLineEdit->setText(QString::number(height));
	}
	else
	{
		XLOG_INFO(" === @@@ ResizePasteParamWidget::setPreParam @@@ ===, UnknownPreTag :" + preTagStr, CURRENT_THREAD_ID);
	}
}

REGISTER_ParamWidget(ResizePaste);
