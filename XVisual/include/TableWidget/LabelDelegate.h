#ifndef LabelDelegate_H
#define LabelDelegate_H
#include <QStyledItemDelegate>
#include <QLabel>
#include <QPainter>

// Custom delegate for QLabel in the first column
namespace XVisual {

class LabelDelegate : public QStyledItemDelegate
{
public:
	LabelDelegate(QObject* parent = nullptr);
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // namespace XVisual

#endif //LabelDelegate_H
