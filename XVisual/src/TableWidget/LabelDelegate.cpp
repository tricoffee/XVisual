#include "TableWidget/LabelDelegate.h"

namespace XVisual {
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
		// 如果是第一列，创建一个QLabel来显示文本
		if (index.column() == 0)
		{
			QString text = index.data().toString();
			QFont boldFont = painter->font();
			// 设置字体加粗
			boldFont.setBold(true);
			painter->setFont(boldFont);
			// 绘制文本
			painter->drawText(option.rect, Qt::AlignCenter, text);
		}
		else
		{
			QStyledItemDelegate::paint(painter, option, index);
		}
	}
} //namespace XVisual