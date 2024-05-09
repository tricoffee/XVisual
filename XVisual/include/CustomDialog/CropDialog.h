#ifndef CropDialog_H
#define CropDialog_H
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class CropDialog : public QDialog
{
	Q_OBJECT
	public:
		CropDialog(QWidget* parent = nullptr);
		int getX() const;
		int getY() const;
		int getWidth() const;
		int getHeight() const;
		void setX(int x);
		void setY(int y);
		void setWidth(int width);
		void setHeight(int height);

	private:
		QLineEdit* xEdit;
		QLineEdit* yEdit;
		QLineEdit* widthEdit;
		QLineEdit* heightEdit;
};

#endif // CropDialog_H