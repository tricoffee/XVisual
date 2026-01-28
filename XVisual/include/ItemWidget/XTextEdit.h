#ifndef XTextEdit_H
#define XTextEdit_H

#include<QTextEdit>

namespace XVisual {

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

} // namespace XVisual

#endif //XTextEdit_H
