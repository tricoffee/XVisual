#ifndef XTextEdit_H
#define XTextEdit_H

#include<QTextEdit>

class XTextEdit : public QTextEdit
{
    Q_OBJECT
	public:
		XTextEdit(QWidget* parent = nullptr);
		~XTextEdit();
	signals:
		void TextEditFocusOutSignal(QString text);
	protected:
		void focusInEvent(QFocusEvent* e) override;
		void focusOutEvent(QFocusEvent* e) override;
};

#endif //XTextEdit_H