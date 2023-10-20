#include "TableWidget/ComboBoxDelegate.h"
#include <QString>
// Custom delegate for QComboBox in the second column
ComboBoxDelegate::ComboBoxDelegate(const QList<TableData>& variables, QObject* parent)
	: QStyledItemDelegate(parent), variables(variables)
{
	// 初始化维护QComboBox下拉列表数据的comboModel，并且指明下拉列表里面的元素个数为variables.size()
	comboModel = new QStandardItemModel(variables.size(), 1);
	for (int i = 0; i < variables.size(); ++i)
	{
		TableData var = variables[i];
		QString nameType = QString::fromStdString(var.name + "\n" + var.type);
		QStandardItem* item = new QStandardItem(nameType);
		// 将新创建的QStandardItem对象item添加到comboModel的第i行第0列
		comboModel->setItem(i, 0, item);
	}
}
QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// 创建一个名为editor的QComboBox类型指针，负责显示comboModel维护的下拉列表数据
	QComboBox* editor = new QComboBox(parent);
	// 将comboModel设置为editor的Model
	editor->setModel(comboModel);
	// maxWidth维护下拉列表中的文本宽度最大值
	int maxWidth = 0;
	// maxHeight维护下拉列表中的文本高度最大值
	int maxHeight = 0;
	//迭代下拉列表的所有项时，使用 sizeHintForIndex来确定每个项的大小
	for (int i = 0; i < comboModel->rowCount(); ++i)
	{
		//调用视图view()的sizeHintForIndex函数，该函数返回特定项的大小提示。
		//大小提示是一个建议性的大小，用于指导视图如何绘制特定项
		QSize itemSize = editor->view()->sizeHintForIndex(comboModel->index(i, 0));
		// update maxWidth
		maxWidth = qMax(maxWidth, itemSize.width());
		// update maxHeight
		maxHeight = qMax(maxHeight, itemSize.height());
	}
	// 设置QComboBox的最小宽度，以适应所有项的内容
	editor->view()->setMinimumWidth(maxWidth);
	// 设置QComboBox的最小高度，以适应所有项的内容
	editor->view()->setMinimumHeight(maxHeight);
	return editor;
}
void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QStyledItemDelegate::setEditorData(editor, index);
}
void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QStyledItemDelegate::setModelData(editor, model, index);
}
void ComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QString text = index.data().toString();
	QFont boldFont = painter->font();
	// 设置字体加粗
	boldFont.setBold(true);
	painter->setFont(boldFont);
	// 绘制文本
	painter->drawText(option.rect, Qt::AlignCenter, text);
}