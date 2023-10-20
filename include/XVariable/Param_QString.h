#ifndef PARAM_H
#define PARAM_H
#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include <stdexcept>
#include <QDebug>
#include <QString>
#include <QHash>
#include "SourceFrom.h"
class Param
{
	public:
		static Param& getInstance();

		template<typename T>
		void registerMember(const QString& name, const T& value)
		{
			memberMap[name] = QVariant::fromValue(value);
			memberTypes[name] = &QMetaType::type(typeid(T).name());
		}

		template<typename T>
		void registerMember(const QString& name, const T& value, bool from_inner)
		{
			qDebug() << "void registerMember(const QString& name, const T& value, bool from_inner)";
			memberMap[name] = QVariant::fromValue(value);
			memberTypes[name] = &QMetaType::type(typeid(T).name());
			memberFrom[name] = from_inner;
		}

		template<typename T>
		T& getMember(const QString& name)
		{
			try
			{
				return memberMap[name].value<T>();
			}
			catch (const std::out_of_range& e)
			{
				qCritical() << "Member not found: " << name;
				throw std::runtime_error("Member not found: " + name.toStdString());
			}
			catch (const std::bad_any_cast& e)
			{
				qCritical() << "Invalid member type for: " << name;
				throw std::runtime_error("Invalid member type for: " + name.toStdString());
			}
		}

		const QMetaType* getMemberType(const QString& name)
		{
			try
			{
				return memberTypes[name];
			}
			catch (const std::out_of_range& e)
			{
				qCritical() << "Member not found: " << name;
				throw std::runtime_error("Member not found: " + name.toStdString());
			}
		}

		bool isFromInner(const QString& name)
		{
			try
			{
				return memberFrom[name];
			}
			catch (const std::out_of_range& e)
			{
				qWarning() << "Member not found: " << name;
				throw std::runtime_error("Member not found: " + name.toStdString());
			}
		}

		void setSourceFrom(const QString& name, const SourceFrom& sourceFrom)
		{
			sourceMap[name] = sourceFrom;
		}

		SourceFrom getSourceFrom(const QString& name)
		{
			try
			{
				return sourceMap[name];
			}
			catch (const std::out_of_range& e)
			{
				qCritical() << "Member not found: " << name;
				throw std::runtime_error("Member not found: " + name.toStdString());
			}
		}

		QStringList acquireNames()
		{
			QStringList keys;
			qDebug() << "QStringList acquireNames()";
			for (auto it = memberMap.begin(); it != memberMap.end(); ++it)
			{
				keys.push_back(it.key());
			}
			return keys;
		}
	
	protected:
		Param();
		QHash<QString, QVariant> memberMap;
		QHash<QString, const QMetaType*> memberTypes;
		QHash<QString, bool> memberFrom;
		QHash<QString, SourceFrom> sourceMap;
};
#endif //PARAM_H
