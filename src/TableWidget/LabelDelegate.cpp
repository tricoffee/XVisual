#include "TableWidget/LabelDelegate.h"
// Custom delegate for QLabel in the first column
LabelDelegate::LabelDelegate(QObject* parent) : QStyledItemDelegate(parent) {}
QWidget* LabelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Disable editing for the first column
	if (index.column() == 0)
	{
		// Return nullptr to prevent editing
		return nullptr;
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}
void LabelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// ����ǵ�һ�У�����һ��QLabel����ʾ�ı�
	if (index.column() == 0)
	{
		QString text = index.data().toString();
		QFont boldFont = painter->font();
		// ��������Ӵ�
		boldFont.setBold(true);
		painter->setFont(boldFont);
		// �����ı�
		painter->drawText(option.rect, Qt::AlignCenter, text);
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
}
