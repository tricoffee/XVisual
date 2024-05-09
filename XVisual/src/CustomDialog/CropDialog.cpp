#include "CustomDialog/CropDialog.h"
CropDialog::CropDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Enter ROI");
	/*
	setModal(true) �����öԻ���Ϊģ̬�Ի������˼��
	ģ̬�Ի�����һ���û����������
	������ʱ��������ֹ�û���Ӧ�ó�����������ֽ��н�����
	ֱ���û��ر�������Ի�������ζ���û��������ȴ�������Ի���
	Ȼ����ܼ�����Ӧ�ó�����н���
	*/
	setModal(true);

	// Create labels and line edits
	QLabel* xLabel = new QLabel("x:");
	QLabel* yLabel = new QLabel("y:");
	QLabel* widthLabel = new QLabel("width:");
	QLabel* heightLabel = new QLabel("height:");

	xEdit = new QLineEdit(this);
	yEdit = new QLineEdit(this);
	widthEdit = new QLineEdit(this);
	heightEdit = new QLineEdit(this);

	// Create OK and Cancel buttons
	QPushButton* okButton = new QPushButton("OK");
	QPushButton* cancelButton = new QPushButton("Cancel");

	connect(okButton, &QPushButton::clicked, this, &CropDialog::accept);
	connect(cancelButton, &QPushButton::clicked, this, &CropDialog::reject);

	// Create a layout
	QFormLayout* layout = new QFormLayout;
	layout->addRow(xLabel, xEdit);
	layout->addRow(yLabel, yEdit);
	layout->addRow(widthLabel, widthEdit);
	layout->addRow(heightLabel, heightEdit);
	layout->addRow(okButton, cancelButton);

	setLayout(layout);
}

int CropDialog::getX() const 
{ 
	return xEdit->text().toInt(); 
}
int CropDialog::getY() const 
{ 
	return yEdit->text().toInt(); 
}
int CropDialog::getWidth() const 
{ 
	return widthEdit->text().toInt(); 
}
int CropDialog::getHeight() const 
{ 
	return heightEdit->text().toInt(); 
}
void CropDialog::setX(int x) 
{
	xEdit->setText(QString::number(x));
}
void CropDialog::setY(int y) 
{
	yEdit->setText(QString::number(y));
}
void CropDialog::setWidth(int width) 
{
	widthEdit->setText(QString::number(width));
}
void CropDialog::setHeight(int height) 
{
	heightEdit->setText(QString::number(height));
}
