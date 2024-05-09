#include "TableWidget/ComboBoxDelegate.h"
#include <QString>
// Custom delegate for QComboBox in the second column
ComboBoxDelegate::ComboBoxDelegate(const QList<TableData>& variables, QObject* parent)
	: QStyledItemDelegate(parent), variables(variables)
{
	// ��ʼ��ά��QComboBox�����б����ݵ�comboModel������ָ�������б������Ԫ�ظ���Ϊvariables.size()
	comboModel = new QStandardItemModel(variables.size(), 1);
	for (int i = 0; i < variables.size(); ++i)
	{
		TableData var = variables[i];
		QString nameType = QString::fromStdString(var.name + "\n" + var.type);
		QStandardItem* item = new QStandardItem(nameType);
		// ���´�����QStandardItem����item��ӵ�comboModel�ĵ�i�е�0��
		comboModel->setItem(i, 0, item);
	}
}
QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// ����һ����Ϊeditor��QComboBox����ָ�룬������ʾcomboModelά���������б�����
	QComboBox* editor = new QComboBox(parent);
	// ��comboModel����Ϊeditor��Model
	editor->setModel(comboModel);
	// maxWidthά�������б��е��ı�������ֵ
	int maxWidth = 0;
	// maxHeightά�������б��е��ı��߶����ֵ
	int maxHeight = 0;
	//���������б��������ʱ��ʹ�� sizeHintForIndex��ȷ��ÿ����Ĵ�С
	for (int i = 0; i < comboModel->rowCount(); ++i)
	{
		//������ͼview()��sizeHintForIndex�������ú��������ض���Ĵ�С��ʾ��
		//��С��ʾ��һ�������ԵĴ�С������ָ����ͼ��λ����ض���
		QSize itemSize = editor->view()->sizeHintForIndex(comboModel->index(i, 0));
		// update maxWidth
		maxWidth = qMax(maxWidth, itemSize.width());
		// update maxHeight
		maxHeight = qMax(maxHeight, itemSize.height());
	}
	// ����QComboBox����С��ȣ�����Ӧ�����������
	editor->view()->setMinimumWidth(maxWidth);
	// ����QComboBox����С�߶ȣ�����Ӧ�����������
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
	// ��������Ӵ�
	boldFont.setBold(true);
	painter->setFont(boldFont);
	// �����ı�
	painter->drawText(option.rect, Qt::AlignCenter, text);
}