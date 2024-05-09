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
		QString::number里面的参数是itemCount - 1而不是itemCount，是因为该项目采用宏定义注册每个XXXItem
		在编译时会执行一遍每个XXXItem的构造函数，此时itemCount是0；
		这样的话在到scene里面添加XXXItem的时候itemCount则变成1
		*/
		QString uniqueItemName = baseName + QString::number(itemCount-1);
		if (itemCount-1 == 0)
		{
			/*
			如果itemCount - == 0成立则表示实际的uniqueItemName是XXXItem0;
			习惯上显示XXXItem比XXXItem0要好得多;
			所以将uniqueItemName重置为baseName + QString("")
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