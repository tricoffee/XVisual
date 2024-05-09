#ifndef ItemManagerQ_H
#define ItemManagerQ_H
#include <QString>
#include <QHash>
class ItemManager
{
public:
	static ItemManager& instance()
	{
		static ItemManager manager;
		return manager;
	}

	QString getUniqueItemName(const QString& baseName)
	{
		int& itemCount = itemCounts[baseName];
		//qDebug() << "itemCount = " << QString::number(itemCount);
		/*
		QString::number����Ĳ�����itemCount - 1������itemCount������Ϊ����Ŀ���ú궨��ע��ÿ��XXXItem
		�ڱ���ʱ��ִ��һ��ÿ��XXXItem�Ĺ��캯������ʱitemCount��0��
		�����Ļ��ڵ�scene�������XXXItem��ʱ��itemCount����1
		*/
		QString uniqueItemName = baseName + QString::number(itemCount-1);
		if (itemCount-1 == 0)
		{
			/*
			���itemCount - == 0�������ʾʵ�ʵ�uniqueItemName��XXXItem0;
			ϰ������ʾXXXItem��XXXItem0Ҫ�õö�;
			���Խ�uniqueItemName����ΪbaseName + QString("")
			*/
			uniqueItemName = baseName + QString("");
		}
		++itemCount;
		return uniqueItemName;
	}

private:
	ItemManager() {}
	QHash<QString, int> itemCounts;
};
#endif // ItemManagerQ_H