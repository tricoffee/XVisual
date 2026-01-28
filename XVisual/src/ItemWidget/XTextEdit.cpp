#include "ItemWidget/XTextEdit.h"

namespace XVisual {

	XTextEdit::XTextEdit(QWidget* parent)
		:QTextEdit(parent)
	{
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		//setStyleSheet("QTextEdit:focus {background:white; border:1px groove lightgray; border-radius:2px}");
		setStyleSheet("background:transparent;border:none");
		setFocus();
		//qDebug() << "XTextEdit::XTextEdit(QWidget * parent):QTextEdit(parent)";
	}
	XTextEdit::~XTextEdit()
	{

	}
	void XTextEdit::focusInEvent(QFocusEvent* e)
	{
		qDebug() << " XTextEdit::focusInEvent(QFocusEvent * e)";
		return QTextEdit::focusInEvent(e);
	}
	void XTextEdit::focusOutEvent(QFocusEvent* e)
	{
		qDebug() << " XTextEdit::focusOutEvent(QFocusEvent * e)";
		emit TextEditFocusOutSignal(toPlainText());
		return QTextEdit::focusOutEvent(e);
	}
} // namespace XVisual 