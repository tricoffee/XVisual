#ifndef ComboBoxDelegate_H
#define ComboBoxDelegate_H
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QComboBox>
#include <QPainter>
#include "TableData.h"
// Custom delegate for QComboBox in the second column
class ComboBoxDelegate : public QStyledItemDelegate
{
public:
	ComboBoxDelegate(const QList<TableData>& variables, QObject* parent = nullptr);
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
	//QComboBoxά��������
	QList<TableData> variables;
	//����һ����ΪcomboModel��QStandardItemModelָ�룬 ά����QComboBox�����б�����
	QStandardItemModel* comboModel;
};
#endif //ComboBoxDelegate_H

