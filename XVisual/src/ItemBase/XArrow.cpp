#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include <QPainter>
#include <QPen>
#include <QtMath>
#include "MainWindow/MainWindow.h"
#include "MainWindow/GraphicsWidget.h"
#include "HandleBase/XBaseHandle.h"
#include "Core/Runtime/VarBag.h"
#include <QStandardItemModel>

namespace XVisual {

	XArrow::XArrow(XBaseItem* startItem, XBaseItem* endItem, QGraphicsItem* parent)
		: QGraphicsLineItem(parent), myStartItem(startItem), myEndItem(endItem)
	{
		setFlag(QGraphicsItem::ItemIsSelectable, true);
		setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	}

	XArrow::XArrow(QWidget* widget, XBaseItem* startItem, XBaseItem* endItem, QGraphicsItem* parent)
		:QGraphicsLineItem(parent), myStartItem(startItem), myEndItem(endItem)
	{
		setFlag(QGraphicsItem::ItemIsSelectable, true);
		setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		// 连接双击信号到槽函数
		GraphicsWidget* mWidget = static_cast<GraphicsWidget*>(widget);
		connect(this, &XArrow::showTableViewSingle,
			mWidget, &GraphicsWidget::showTableViewSlot);
	}

	QRectF XArrow::boundingRect() const
	{
		qreal extra = (pen().width() + 20) / 2.0;

		return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
			line().p2().y() - line().p1().y()))
			.normalized()
			.adjusted(-extra, -extra, extra, extra);
	}

	QPainterPath XArrow::shape() const
	{
		QPainterPath path = QGraphicsLineItem::shape();
		path.addPolygon(arrowHead);
		return path;
	}

	QStandardItemModel*& XArrow::getLatestTableViewModel()
	{
		return latestTableViewModel;
	}

	void XArrow::setLatestTableViewModel(QStandardItemModel* newModel)
	{
		latestTableViewModel = newModel;
	}

	void XArrow::setDefaultYforVariablesX(std::unordered_map<std::string, TableData> mDefaultValues)
	{
		defaultValues = mDefaultValues;
	}

	std::unordered_map<std::string, TableData>& XArrow::getDefaultYforVariablesX()
	{
		return defaultValues;
	}

	void XArrow::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
		QWidget*)
	{
		if (myStartItem->collidesWithItem(myEndItem))
			return;

		QPen myPen = pen();
		myPen.setColor(myColor);
		qreal arrowSize = 20;
		painter->setPen(myPen);
		painter->setBrush(myColor);
		QLineF centerLine(myStartItem->pos(), myEndItem->pos());
		QPolygonF endPolygon = myEndItem->polygon();
		QPointF p1 = endPolygon.first() + myEndItem->pos();
		QPointF intersectPoint;
		for (int i = 1; i < endPolygon.count(); ++i)
		{
			QPointF p2 = endPolygon.at(i) + myEndItem->pos();
			QLineF polyLine = QLineF(p1, p2);
			QLineF::IntersectionType intersectionType =
				polyLine.intersects(centerLine, &intersectPoint);
			if (intersectionType == QLineF::BoundedIntersection)
				break;
			p1 = p2;
		}
		setLine(QLineF(myStartItem->pos(), intersectPoint));
		double angle = std::atan2(line().dy(), line().dx());
		QPointF arrowP1 = line().p2() - QPointF(cos(angle + M_PI / 6) * arrowSize,
			sin(angle + M_PI / 6) * arrowSize);
		QPointF arrowP2 = line().p2() + QPointF(-cos(M_PI / 6 - angle) * arrowSize,
			+sin(M_PI / 6 - angle) * arrowSize);
		arrowHead.clear();
		arrowHead << line().p2() << arrowP1 << arrowP2;
		painter->drawLine(line());
		painter->drawPolygon(arrowHead);
		if (isSelected())
		{
			painter->setPen(QPen(myColor, 1, Qt::DashLine));
			QLineF myLine = line();
			myLine.translate(0, 4.0);
			painter->drawLine(myLine);
			myLine.translate(0, -8.0);
			painter->drawLine(myLine);
		}
	}

	void XArrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
	{
		// 当双击XArrow时，更改XArrow的颜色为红色
		myColor = Qt::red;
		//qDebug() << "void XArrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) ";
		//qDebug() << "yUuid = " << QString::fromStdString(myStartItem->getUuid());
		//qDebug() << "xUuid = " << QString::fromStdString(myEndItem->getUuid());
		XLOG_INFO("void XArrow::mouseDoubleClickEvent ...", CURRENT_THREAD_ID);
		XLOG_INFO("yUuid = " + myStartItem->getUuid(), CURRENT_THREAD_ID);
		XLOG_INFO("xUuid = " + myEndItem->getUuid(), CURRENT_THREAD_ID);
		std::string yItemId = const_cast<std::string&>(myStartItem->getUuid());
		std::string xItemId = const_cast<std::string&>(myEndItem->getUuid());

		QString yItemName = QString::fromStdString(myStartItem->getUniqueName());
		QString xItemName = QString::fromStdString(myEndItem->getUniqueName());

		XBaseHandle* myStartHandle = myStartItem->getXHandle();
		XBaseHandle* myEndHandle = myEndItem->getXHandle();
		VarBag& dests = myStartHandle->getDests();
		VarBag& sources = myEndHandle->getSources();
		std::vector<std::string> yNames_raw = ACQUIRE_NAMES(dests);
		std::vector<std::string> xNames_raw = ACQUIRE_NAMES(sources);
		int yNum_raw = yNames_raw.size();
		int xNum_raw = xNames_raw.size();

		XLOG_INFO("yNames_raw.size() = " + std::to_string(yNum_raw), CURRENT_THREAD_ID);
		XLOG_INFO("xNames_raw.size() = " + std::to_string(xNum_raw), CURRENT_THREAD_ID);

		// 按照typeid过滤掉不匹配的xName和yName

		std::unordered_map<std::string, bool> x_matched;
		for (const auto& x_name : xNames_raw)
		{
			x_matched[x_name] = false;
		}

		std::unordered_map<std::string, bool> y_matched;
		for (const auto& y_name : yNames_raw)
		{
			y_matched[y_name] = false;
		}

		for (int p = 0; p < xNum_raw; ++p)
		{
			std::string x_name = xNames_raw[p];
			std::string xType_Name = (*GET_MEMBER_TYPE_STR(sources, x_name)).name();
			for (int q = 0; q < yNum_raw; ++q)
			{
				std::string y_name = yNames_raw[q];
				std::string yType_Name = (*GET_MEMBER_TYPE_STR(dests, y_name)).name();
				if (xType_Name == yType_Name)
				{
					x_matched[x_name] = true;
					y_matched[y_name] = true;
				}
			}
		}

		// 获取 x_matched 里面不为 false 的 key，并另存到一个 std::vector<std::string>
		std::vector<std::string> xNames;

		// 遍历 unordered_map 并检查值
		for (const auto& pair : x_matched)
		{
			if (pair.second)
			{
				xNames.push_back(pair.first);
			}
		}

		// 获取 y_matched 里面不为 false 的 key，并另存到一个 std::vector<std::string>
		std::vector<std::string> yNames;

		// 遍历 unordered_map 并检查值
		for (const auto& pair : y_matched)
		{
			if (pair.second)
			{
				yNames.push_back(pair.first);
			}
		}

		int yNum = yNames.size();
		int xNum = xNames.size();

		XLOG_INFO("yNames.size() = " + std::to_string(yNum), CURRENT_THREAD_ID);
		XLOG_INFO("xNames.size() = " + std::to_string(xNum), CURRENT_THREAD_ID);

		QList<TableData> variablesY;
		QList<TableData> variablesX;
		for (int j = 0; j < yNum; ++j)
		{
			std::string yName = yNames[j];
			if (IS_MEMBER_FROM_OUTSIDE_STR(dests, yName))
			{
				//do nothing
			}
			else
			{
				std::string yTypeName = (*GET_MEMBER_TYPE_STR(dests, yName)).name();
				TableData yData{ yItemId,yName,yTypeName };
				variablesY << yData;
				XLOG_INFO("yName = " + yName, CURRENT_THREAD_ID);
				XLOG_INFO("yTypeName = " + yTypeName, CURRENT_THREAD_ID);
			}
		}

		// 追加一个 unSelectedData 表示未选中的
		if (variablesY.size() > 0)
		{
			TableData unSelectedData = { TableData_UnSelectedItemId, TableData_UnSelectedName, TableData_UnSelectedType };
			variablesY << unSelectedData;
		}

		for (int i = 0; i < xNum; ++i)
		{
			std::string xName = xNames[i];
			if (IS_MEMBER_FROM_OUTSIDE_STR(sources, xName))
			{
				//do nothing
			}
			else
			{
				std::string xTypeName = (*GET_MEMBER_TYPE_STR(sources, xName)).name();
				TableData xData{ xItemId,xName,xTypeName };
				variablesX << xData;
				XLOG_INFO("xName = " + xName, CURRENT_THREAD_ID);
				XLOG_INFO("xTypeName = " + xTypeName, CURRENT_THREAD_ID);
			}
		}
		emit showTableViewSingle(xItemName, yItemName, this, variablesX, variablesY);


		return QGraphicsLineItem::mouseDoubleClickEvent(event);
	}

	void XArrow::changeLineArrowColorSlots(const QColor& color)
	{
		myColor = color;
	}

} // namespace XVisual
