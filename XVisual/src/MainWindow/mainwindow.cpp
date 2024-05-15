// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "GlobalStorage/GlobalStorage.h"
#include "XGraph/XGraph.h"

#include <QtWidgets>
#include <QString>
#include <string>
#include <QSplitter>

#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include "HandleBase/XBaseHandle.h"
#include "MainWindow/diagramscene.h"
#include "MainWindow/mainwindow.h"
#include "Common/constants.h"
#include "MainWindow/GraphicsWidget.h"
#include "MainWindow/SideWidget.h"
#include "MainWindow/ImagePageWidget.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "cJson.h"
#include <fstream>
#include <set>
#include <any>
#include "Common/JsonFIleUtils.h"
#include "Common/XParser.h"
#include "Common/TypeClassifier.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)

MainWindow::MainWindow()
{
	createActions();
	createMenus();
    graphicsWidget = new GraphicsWidget(itemMenu);
	createToolbars();
	createToolBox();
	inintActionConnection();

	sideWidget = new SideWidget();

	// 创建水平方向splitter
	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	// 设置QSplitter分割线的样式表
	splitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
    // 将toolBox添加到splitter
	splitter->addWidget(toolBox);
	// 将graphicsWidget添加到splitter
	splitter->addWidget(graphicsWidget);
	// 将sideWidget添加到splitter
	splitter->addWidget(sideWidget);
	//设置拉伸比例
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 1);
	splitter->setStretchFactor(2, 0);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(splitter);
	// 设置layout中小控件之间的距离, 即设置内边距
	layout->setSpacing(0);
	// 设置layout的外边距
	layout->setContentsMargins(0, 0, 0, 0);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	setCentralWidget(widget);
	setWindowTitle(tr("Diagramscene"));
	setUnifiedTitleAndToolBarOnMac(true);

	connect(graphicsWidget, &GraphicsWidget::showImageInTabSignal, 
		sideWidget,&SideWidget::showImageInTabSlot);
}

void MainWindow::backgroundButtonGroupClicked(QAbstractButton* button)
{
	button->setChecked(false);
	const int id = backgroundButtonGroup->id(button);
	graphicsWidget->setBackgroundChange(id);
}

void MainWindow::buttonGroupClicked(QAbstractButton* button)
{
	button->setChecked(false);
	const int id = buttonGroup->id(button);
	graphicsWidget->setDiagramState(DiagramState::Insert);
	graphicsWidget->setDiagramType(DiagramType::Item);
	graphicsWidget->setDiagramName(idnames[id]);
}

void MainWindow::linePointerButtonClicked(bool checked)
{
	linePointerButton->setChecked(false);
	graphicsWidget->setDiagramState(DiagramState::Insert);
	graphicsWidget->setDiagramType(DiagramType::Line);
}

void MainWindow::runButtonClicked(bool checked)
{
	XLOG_INFO("void MainWindow::runButtonClicked to do executeXGraph", CURRENT_THREAD_ID);
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		// 遍历xGraph的每个元素
		//for(auto it = xGraph.begin(); it != xGraph.end(); ++it) 
		//{
		//	// 获取当前节点node
		//	std::shared_ptr<GraphNode> node = *it;
		//	// 获取当前节点的nodeId
		//	std::string nodeId = node->nodeId;
		//	XLOG_INFO("void MainWindow::runButtonClicked, nodeId = "+nodeId, CURRENT_THREAD_ID);
		//}

		try 
		{
			XGraph::executeXGraph<XBaseItem>(globalItemMap, xGraph);
		}
		catch (UmapKeyNoFoundException& e)
		{
			//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
			XLOG_INFO("====== void MainWindow::runButtonClicked, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
			//throw UmapKeyNoFoundException(e.what());
			// 捕获异常并记录错误码
			m_lastError = XVisual::ErrorCode::UmapKeyNoFound;
			// 发射错误信号，通知错误发生
			emit errorOccurred(m_lastError);
		}

	}// 作用域结束时，lock_guard 会自动解锁互斥锁xGraphMutex
	XLOG_INFO("void MainWindow::runButtonClicked xGraph is executed.", CURRENT_THREAD_ID);
}


void MainWindow::exportButtonClicked(bool checked)
{
	XLOG_INFO("void MainWindow::exportButtonClicked to do save XGraph", CURRENT_THREAD_ID);

	//Deprecated, item_parents维系每个item节点的父节点
	//std::unordered_map<std::string, std::set<std::string>> item_parents;

	// handle_parents维系每个handle节点的父节点
	std::unordered_map<std::string, std::set<std::string>> handle_parents;
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		// 遍历xGraph的每个元素
		for(auto it = xGraph.begin(); it != xGraph.end(); ++it) 
		{
			// 获取当前节点node
			std::shared_ptr<GraphNode> node = *it;
			// 获取当前节点的nodeId
			std::string pItemId = node->nodeId;
			// 获取当前节点对应Handle的nodeId
			std::string pDelimiter = "_";
			std::string pUuidStr2 = extractSubstrAfterDelimiter(pItemId, pDelimiter);
			std::string pHandleId = "Handle_" + pUuidStr2;
			
			for (const auto& neighbor : node->neighbors)
			{
				// 获取当前节点某个邻居的nodeId
				std::string qItemId = neighbor->nodeId;

				//Unused, item_parents维系每个item节点的父节点
				//item_parents[qItemId].insert(pItemId);
				
				// 获取当前节点某个邻居对应Handle的nodeId
				std::string qDelimiter = "_";
				std::string qUuidStr2 = extractSubstrAfterDelimiter(qItemId, qDelimiter);
				std::string qHandleId = "Handle_" + qUuidStr2;
				handle_parents[qHandleId].insert(pHandleId);
			}
		}
	}// 作用域结束时，lock_guard 会自动解锁互斥锁xGraphMutex

	cJSON* cjson_root = NULL;
	/* 创建一个JSON数据对象(链表头结点) */
	cjson_root = cJSON_CreateObject();
	cJSON* cjson_arr = cJSON_CreateArray();
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		for (const auto& eMap: globalItemMap)
		{
			// cjson_arr_element维护Item相关信息, Handle相关信息, 以及innerParam和outerParam
			cJSON* cjson_arr_element = cJSON_CreateObject();

			// 获取Item相关信息(Item_ClassName,Item_ColleagueType,Item_ColleagueType,Item_UniqueName)
			XBaseItem* item = eMap.second;
			std::string item_classNameStr = item->getClassName();
			ColleagueType item_colleagueType = item->getColleagueType();
			int item_colleagueTypeIntValue = static_cast<int>(item_colleagueType);
			std::string item_colleagueId = item->getUuid();
			std::string item_uniqueName = item->getUniqueName();

			// 写入Item相关信息(Item_ClassName,Item_ColleagueType,Item_ColleagueType,Item_UniqueName)到JSON
			cJSON_AddStringToObject(cjson_arr_element, "Item_ClassName", item_classNameStr.c_str());
			cJSON_AddNumberToObject(cjson_arr_element, "Item_ColleagueType", item_colleagueTypeIntValue);
			cJSON_AddStringToObject(cjson_arr_element, "Item_ColleagueId", item_colleagueId.c_str());
			cJSON_AddStringToObject(cjson_arr_element, "Item_UniqueName", item_uniqueName.c_str());

			// 获取Item的位置信息
			QPointF itemPos = item->scenePos();
			// 写入Item的位置信息到JSON
			cJSON* cjson_itemPos = cJSON_CreateArray();
			cJSON_AddItemToArray(cjson_itemPos, cJSON_CreateNumber(itemPos.x()));
			cJSON_AddItemToArray(cjson_itemPos, cJSON_CreateNumber(itemPos.y()));
			cJSON_AddItemToObject(cjson_arr_element, "Item_Position", cjson_itemPos);

			// 获取Handle的相关信息(ClassName,ColleagueType,ColleagueType,UniqueName)
			XBaseHandle* handle = eMap.second->getXHandle();
			std::string handle_classNameStr = handle->getClassName();
			ColleagueType handle_colleagueType = handle->getColleagueType();
			int handle_colleagueTypeIntValue = static_cast<int>(handle_colleagueType);
			std::string handle_colleagueId = handle->getUuid();
			std::string handle_uniqueName = handle->getUniqueName();

            // 写入Handle相关信息(ClassName,ColleagueType,ColleagueType,UniqueName)到JSON
			cJSON_AddStringToObject(cjson_arr_element, "ClassName", handle_classNameStr.c_str());
			cJSON_AddNumberToObject(cjson_arr_element, "ColleagueType", handle_colleagueTypeIntValue);
			cJSON_AddStringToObject(cjson_arr_element, "ColleagueId", handle_colleagueId.c_str());
			cJSON_AddStringToObject(cjson_arr_element, "UniqueName", handle_uniqueName.c_str());

			// Deprecated
			//std::set<std::string> mParentIdSet = handle_parents[handle_colleagueId];
			//for (const auto& mParentId : mParentIdSet)
			//{
			//}

			// 获取sources里面每个source变量xName对应的sourceFrom, 并将其写入到JSON
			cJSON* cjson_sourceFrom = cJSON_CreateObject();
			Source& sources = handle->getSources();
			std::vector<std::string> xVaribleNames = ACQUIRE_NAMES(sources);
			for (const auto& xName : xVaribleNames)
			{
				SourceFrom sourceFrom;
				handle->loadSourceFrom(xName, sourceFrom);

				// Deprecated, 因为sourceFrom的默认值就是: sourceFrom.itemId = NULL_ITEMID , sourceFrom.variableName = NULL_VariableName
				//if (sourceFrom.itemId != NULL_ITEMID && sourceFrom.variableName != NULL_VariableName)
				//{
				//}

				cJSON* cjson_variableSource = cJSON_CreateObject();
				cJSON_AddStringToObject(cjson_variableSource, "yId", sourceFrom.itemId.c_str());
				cJSON_AddStringToObject(cjson_variableSource, "yName", sourceFrom.variableName.c_str());
				bool isFromOut = false;
				isFromOut = IS_MEMBER_FROM_OUTSIDE_STR(sources, xName);
				if (isFromOut)
				{
					// Done, write outerParam to cjson_variableSource
					// e.g. xName is "imagePath", 		handle->writeOuterParam(cjson_variableSource, "imagePath");
					handle->writeOuterParam(cjson_variableSource, xName);
				}
				/*
				// 举例说明SourceFrom在JSON文件里面的表现形式
				"SourceFrom":      {
				    // e.g. XName is "imagePath", 	cJSON_AddItemToObject(cjson_sourceFrom, "imagePath", cjson_variableSource);
					"imagePath":      {
						"yId":      "",
						"yName":     "",
						"outerParam": "C:\\NDev\\CPPDev\\XVisual\\XVisual\\images\\background1.png"
					},
				   // e.g. XName is "imagePath2", 	cJSON_AddItemToObject(cjson_sourceFrom, "imagePath2", cjson_variableSource);
					"imagePath2":      {
						"yId2":      "",
						"yName2":     "",
						"outerParam2": "C:\\NDev\\CPPDev\\XVisual\\XVisual\\images\\background2.png"
					}
			    }
				*/
				cJSON_AddItemToObject(cjson_sourceFrom, xName.c_str(), cjson_variableSource);
			}
			cJSON_AddItemToObject(cjson_arr_element, "SourceFrom", cjson_sourceFrom);

			// If innerParamUMap is not empty, write innperParam to handle
			const std::unordered_map<std::string, std::any>& innerParamUMap = handle->getInnerParam();
			if (!innerParamUMap.empty())
			{
				cJSON* cjson_innerParam = cJSON_CreateObject();
				handle->writeInnerParam(cjson_innerParam);
				cJSON_AddItemToObject(cjson_arr_element, "InnerParam", cjson_innerParam);
			}

			cJSON_AddItemToArray(cjson_arr, cjson_arr_element);
		}
	}// 作用域结束时，lock_guard 会自动解锁互斥锁xGraphMutex

	cJSON_AddItemToObject(cjson_root, "ClassInfo", cjson_arr);

	/* 打印JSON对象(整条链表)的所有数据 */
	char* str = NULL;
	str = cJSON_Print(cjson_root);
	// printf("%s\n", str);

	// 将 JSON 数据保存到 .json 文件中
	std::ofstream outFile("C:\\NDev\\CPPDev\\XVisual\\XVisual\\logs\\output.json");
	outFile << str;
	outFile.close();

	XLOG_INFO("Exporting XGraph is executed.", CURRENT_THREAD_ID);
}

void MainWindow::loadButtonClicked(bool checked)
{
	XLOG_INFO("void MainWindow::loadButtonClicked to do export XGraph", CURRENT_THREAD_ID);
	QString defaultPath = latestJsonPath.isEmpty() ? QDir::homePath() : latestJsonPath;
	QString jsonPath = QFileDialog::getOpenFileName(nullptr, "Select JSON", defaultPath, "JSON FILE (*.json)");
	if (!jsonPath.isEmpty())
	{
		// 保存当前选中的文件路径
		latestJsonPath = jsonPath;
		XLOG_INFO("MainWindow::loadButtonClicked, jsonPath is NOT empty ..., jsonPath is " + latestJsonPath.toStdString() , CURRENT_THREAD_ID);
		
		XParser& jsonParser = XParser::getInstance();
		jsonParser.initParser(latestJsonPath.toStdString());
		cJSON* classInfoArr;
		jsonParser.getClassInfoArr(classInfoArr);
		if (!classInfoArr || !cJSON_IsArray(classInfoArr))
		{
			//printf("Failed to get 'ClassInfo' array.\n");
			//flag = false;
			//return XVisual::ErrorCode::ParseClassInfoArrFromJsonFailed;
			XLOG_INFO("XVisual::ErrorCode::ParseClassInfoArrFromJsonFailed", CURRENT_THREAD_ID);
		}
		else
		{

			DiagramScene*& mScene = graphicsWidget->getScene();

			// 清理场景中的所有项
			mScene->clear();

			int arrSize = cJSON_GetArraySize(classInfoArr);
			XLOG_INFO("== === ===  classInfoArr.size() " + std::to_string(arrSize) + " === === ===", CURRENT_THREAD_ID);


			//item_childrean 维系每个item节点的孩子节点
			std::unordered_map<std::string, std::set<std::string>> itemChildrenLinks;

			//维系qItemId的每个sourceFrom
			std::unordered_map<std::string, std::unordered_map<std::string, SourceFrom>> qItemIdSourceFromUMap;

			// Done, iterate XInfer::constructHandle over classInfoArr 
		    // iterate over classInfoArr 
			for (int i = 0; i < cJSON_GetArraySize(classInfoArr); i++)
			{
				ColleagueData data;


				// Deprecated
				//cJSON* classinfoItem = nullptr;
				//parser.getClassInfoItemAt(classinfoItem, i);

				/* get itemClassName, get itemColleagueType, get itemColleagueId for Handle */

				// get itemClassName
				std::string itemClassNameStr = NullClassNameStr;
				cJSON* itemClassName = nullptr;
				jsonParser.getItemClassNameAt(itemClassName, i);
				if (cJSON_IsString(itemClassName) && (itemClassName->valuestring != NULL))
				{
					//printf("ItemClassName: %s\n", itemClassName->valuestring);
					itemClassNameStr = itemClassName->valuestring;
					XLOG_INFO("itemClassName = " + itemClassNameStr, CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseItemClassNameFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseItemClassNameFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.itemClassName = itemClassNameStr; // write itemClassNameStr to the member "itemClassName" of data

				// get itemColleagueType
				ColleagueType itemCType = ColleagueType::NullType;
				cJSON* itemColleagueType = nullptr;
				jsonParser.getItemColleagueTypeAt(itemColleagueType, i);
				if (cJSON_IsNumber(itemColleagueType))
				{
					//printf("ItemColleagueType: %d\n", itemColleagueType->valueint);
					int vItemInt = itemColleagueType->valueint;
				    itemCType = Number2ColleagueType(vItemInt);
					XLOG_INFO("itemColleagueType = " + std::to_string(vItemInt), CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseItemColleagueTypeFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseItemColleagueTypeFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.itemColleagueType = itemCType; // write itemCType to the member "itemColleagueType" of data

				// get itemColleagueId
				std::string itemColleagueIdStr = NullColleagueId;
				cJSON* itemColleagueId = nullptr;
				jsonParser.getItemColleagueIdAt(itemColleagueId, i);
				if (cJSON_IsString(itemColleagueId) && (itemColleagueId->valuestring != NULL))
				{
					//printf("ItemColleagueId: %s\n", itemColleagueId->valuestring);
					itemColleagueIdStr = itemColleagueId->valuestring;
					XLOG_INFO("itemColleagueId = " + itemColleagueIdStr, CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseItemColleagueIdFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseItemColleagueIdFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.itemColleagueId = itemColleagueIdStr; // write itemColleagueIdStr to the member "itemColleagueId" of data

				// get itemUniqueName
				std::string itemUniqueNameStr = NullUniqueName;
				cJSON* itemUniqueName = nullptr;
				jsonParser.getItemUniqueNameAt(itemUniqueName, i);
				if (cJSON_IsString(itemUniqueName) && (itemUniqueName->valuestring != NULL))
				{
					//printf("ItemUniqueName: %s\n", itemUniqueName->valuestring);
					itemUniqueNameStr = itemUniqueName->valuestring;
					XLOG_INFO("itemUniqueName = " + itemUniqueNameStr, CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseItemUniqueNameFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseItemUniqueNameFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.itemUniqueName = itemUniqueNameStr; // write itemUniqueNameStr to the member "itemUniqueName" of data


				// getItemPosition
				cJSON* itemPos = nullptr;
				jsonParser.getItemPosition(itemPos, i);
				std::pair<int,int> itemPoistion(0, 0);
				if (cJSON_IsArray(itemPos))
				{
					int posX = cJSON_GetArrayItem(itemPos, 0)->valueint;
					int posY = cJSON_GetArrayItem(itemPos, 1)->valueint;
					itemPoistion = std::pair<int,int>(posX, posY);
				}
				data.itemPos = itemPoistion; // write itemPoistion to the member "itemPos" of data


				/* get className, get colleagueType, get colleagueId for Handle */

				// get className
				std::string classNameStr = NullClassNameStr;
				cJSON* className = nullptr;
				jsonParser.getClassNameAt(className, i);
				if (cJSON_IsString(className) && (className->valuestring != NULL))
				{
					//printf("ClassName: %s\n", className->valuestring);
					classNameStr = className->valuestring;
					XLOG_INFO("className = " + classNameStr, CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseClassNameFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseClassNameFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.className = classNameStr; // write classNameStr to the member "className" of data

				// get colleagueType
				ColleagueType cType = ColleagueType::NullType;
				cJSON* colleagueType = nullptr;
				jsonParser.getColleagueTypeAt(colleagueType, i);
				if (cJSON_IsNumber(colleagueType))
				{
					//printf("ColleagueType: %d\n", colleagueType->valueint);
					int vInt = colleagueType->valueint;
					cType = Number2ColleagueType(vInt);
					XLOG_INFO("colleagueType = " + std::to_string(vInt), CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseColleagueTypeFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseColleagueTypeFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.colleagueType = cType; // write cType to the member "colleagueType" of data

				// get colleagueId
				std::string colleagueIdStr = NullColleagueId;
				cJSON* colleagueId = nullptr;
				jsonParser.getColleagueIdAt(colleagueId, i);
				if (cJSON_IsString(colleagueId) && (colleagueId->valuestring != NULL))
				{
					//printf("ColleagueId: %s\n", colleagueId->valuestring);
					colleagueIdStr = colleagueId->valuestring;
					XLOG_INFO("colleagueId = " + colleagueIdStr, CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseColleagueIdFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseColleagueIdFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.colleagueId = colleagueIdStr; // write colleagueIdStr to the member "colleagueId" of data

				// get uniqueName
				std::string uniqueNameStr = NullUniqueName;
				cJSON* uniqueName = nullptr;
				jsonParser.getUniqueNameAt(uniqueName, i);
				if (cJSON_IsString(uniqueName) && (uniqueName->valuestring != NULL))
				{
					//printf("UniqueName: %s\n", uniqueName->valuestring);
					uniqueNameStr = uniqueName->valuestring;
					XLOG_INFO("UniqueName = " + uniqueNameStr, CURRENT_THREAD_ID);
				}
				else
				{
					//return XVisual::ErrorCode::ParseUniqueNameFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseUniqueNameFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.uniqueName = uniqueNameStr; // write uniqueNameStr to the member "uniqueName" of data



				// get sourceFromUMap
				data.sourceFromUMap.clear();
				cJSON* sourceFromPtr = nullptr;
				jsonParser.getSourceFromAt(sourceFromPtr, i);
				if (cJSON_IsObject(sourceFromPtr))
				{
					// 遍历 SourceFrom 的每个节点
					cJSON* source_node = NULL;
					cJSON_ArrayForEach(source_node, sourceFromPtr)
					{
						const char* source_node_name = source_node->string;
						//printf("source_node_name = %s\n", source_node_name);
						cJSON* source_obj = cJSON_GetObjectItem(sourceFromPtr, source_node_name);
						if (source_obj)
						{
							cJSON* yId = cJSON_GetObjectItemCaseSensitive(source_obj, "yId");
							cJSON* yName = cJSON_GetObjectItemCaseSensitive(source_obj, "yName");
							cJSON* yOuterParam = cJSON_GetObjectItemCaseSensitive(source_obj, "outerParam");

							data.outerParamUMap[source_node_name] = yOuterParam;

							if (cJSON_IsString(yId) && cJSON_IsString(yName))
							{
								//printf("SourceFrom (Name): %s, yId=%s, yName=%s\n", source_node_name, yId->valuestring, yName->valuestring);
								SourceFrom sourceFrom;
								sourceFrom.itemId = yId->valuestring;
								sourceFrom.variableName = yName->valuestring;
								data.sourceFromUMap[source_node_name] = sourceFrom;
								if (!sourceFrom.itemId.empty())
								{
									std::string substr2 = stripPrefix(sourceFrom.itemId, "Handle_");
									std::string parentItemId = "Item_" + substr2;
									XLOG_INFO("Item_\"+ substr2\" = " + parentItemId, CURRENT_THREAD_ID);
									// parentItemId ---> data.itemColleagueId (this is custom item id)
									itemChildrenLinks[parentItemId].insert(data.itemColleagueId);
								}
							}
							else
							{
								//return  XVisual::ErrorCode::ParseSourceFromJsonFailed;
								XLOG_INFO("XVisual::ErrorCode::ParseSourceFromJsonFailed", CURRENT_THREAD_ID);
							}

						} // end for "source_obj is NOT NULL"
					} // end for "cJSON_ArrayForEach(source_node, sourceFromPtr)"
				} // end for "get SourceFromUMap"

				cJSON* innerParam = nullptr;
				jsonParser.getInnerParamAt(innerParam, i);
				if (CHECK_NULLPTR(innerParam))
				{
					// return XVisual::ErrorCode::ParseInnerParamFromJsonFailed;
					XLOG_INFO("XVisual::ErrorCode::ParseInnerParamFromJsonFailed", CURRENT_THREAD_ID);
				}
				data.innerParam = innerParam;
				mScene->addItemByJson(data);

				// update qItemIdSourceFromUMap
				qItemIdSourceFromUMap[data.itemColleagueId] = data.sourceFromUMap;

			} // end  for "for (int i = 0; i < cJSON_GetArraySize(classInfoArr); i++"


			for (const auto& eMap : itemChildrenLinks)
			{
				std::string pItemId = eMap.first;
				std::set<std::string> qItemId_set = eMap.second;
				mScene->addArrowByJson(eMap, qItemIdSourceFromUMap);
			}

		}
	}
	else
	{
		XLOG_INFO("MainWindow::loadButtonClicked, jsonPath is empty ...", CURRENT_THREAD_ID);
	}


	XLOG_INFO("Loading XGraph is executed.", CURRENT_THREAD_ID);
}

void MainWindow::currentFontChanged(const QFont&)
{
	handleFontChange();
}

void MainWindow::fontSizeChanged(const QString&)
{
	handleFontChange();
}

void MainWindow::sceneScaleChanged(const QString& scale)
{
	graphicsWidget->setSceneScaleChange(scale);
}

/*
单击fontColorToolButton的弹出菜单的某一个QAction时,
slot函数MainWindow::textColorChanged响应, fontColorToolButton的图标(Icon)有相应变化,
同时场景scene中text的背景色有相应变化
*/
void MainWindow::textColorChanged()
{
	textAction = qobject_cast<QAction*>(sender());
	fontColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::TextPointer,
		qvariant_cast<QColor>(textAction->data())));
	//去掉原来的textButtonTriggered();直接把textButtonTriggered()里面的这一句拿过来，如下：
	//这段代码表示设置场景scene中text的背景色
	QColor color = qvariant_cast<QColor>(textAction->data());
	graphicsWidget->setItemTextColor(color);
}

/*
单击fillColorToolButton的弹出菜单的某一个QAction时,
slot函数MainWindow::itemColorChanged响应, fillColorToolButton的图标(Icon)有相应变化,
同时场景scene中item的颜色有相应变化
*/
void MainWindow::itemColorChanged()
{
	fillAction = qobject_cast<QAction*>(sender());
	fillColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::FloodFill,
		qvariant_cast<QColor>(fillAction->data())));
	//去掉原来的fillButtonTriggered();直接把fillButtonTriggered里面的这一句拿过来，如下:
	//这段代码表示设置场景scene中item的颜色
	QColor color = qvariant_cast<QColor>(fillAction->data());
	graphicsWidget->setSceneItemColor(color);
}

/*
单击lineColorToolButton的弹出菜单的某一个QAction时,
slot函数MainWindow::lineColorChanged响应, lineColorToolButton的图标(Icon)有相应变化,
同时场景scene中line的颜色有相应变化
*/
void MainWindow::lineColorChanged()
{
	lineAction = qobject_cast<QAction*>(sender());
	lineColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::LineColor,
		qvariant_cast<QColor>(lineAction->data())));
	//去掉原来的lineButtonTriggered();直接把lineButtonTriggered里面的这一句拿过来，如下:
	//这段代码表示设置场景scene中line的颜色
	QColor color = qvariant_cast<QColor>(lineAction->data());
	graphicsWidget->setSceneLineColor(color);
}

void MainWindow::handleFontChange()
{
	QFont font = fontCombo->currentFont();
	font.setPointSize(fontSizeCombo->currentText().toInt());
	font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
	font.setItalic(italicAction->isChecked());
	font.setUnderline(underlineAction->isChecked());
	graphicsWidget->setFont(font);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Diagram Scene"),
		tr("The <b>Diagram Scene</b> example shows "
			"use of the graphics framework."));
}

void MainWindow::createToolBox()
{
	buttonGroup = new QButtonGroup(this);
	buttonGroup->setExclusive(false);
	connect(buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
		this, &MainWindow::buttonGroupClicked);

	QGridLayout* layout = new QGridLayout;

	//layout->addWidget(createCellWidget(tr("Condition")), 0, 0);
	//layout->addWidget(createCellWidget(tr("Step")), 0, 1);
	//layout->addWidget(createCellWidget(tr("Input")), 1, 0);
	//layout->addWidget(createCellWidget(tr("Output")), 1, 1);
	//layout->addWidget(createCellWidget(tr("LoadImage")), 2, 0);
	//layout->addWidget(createCellWidget(tr("CVCrop")), 2, 1);

	layout->addWidget(createCellWidget(tr("LoadImage")), 0, 0);
	layout->addWidget(createCellWidget(tr("CVCrop")), 0, 1);
	layout->addWidget(createCellWidget(tr("ImagePre")), 1, 0);
	layout->addWidget(createCellWidget(tr("PreInver")), 1, 1);
	layout->addWidget(createCellWidget(tr("TFDetect")), 2, 0);
	layout->addWidget(createCellWidget(tr("RevertBox")), 2, 1);
	layout->addWidget(createCellWidget(tr("DrawBox")), 3, 0);

	layout->setRowStretch(10, 10);
	layout->setColumnStretch(2, 10);

	QWidget* itemWidget = new QWidget;
	itemWidget->setLayout(layout);

	backgroundButtonGroup = new QButtonGroup(this);
	connect(backgroundButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
		this, &MainWindow::backgroundButtonGroupClicked);

	QGridLayout* backgroundLayout = new QGridLayout;

	backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"), BackGroundType::BlueGrid,
		ImageSources::Background1), 0, 0);
	backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"), BackGroundType::WhiteGrid,
		ImageSources::Background2), 0, 1);
	backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"), BackGroundType::GrayGrid,
		ImageSources::Background3), 1, 0);
	backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"), BackGroundType::NoGrid,
		ImageSources::Background4), 1, 1);


	backgroundLayout->setRowStretch(2, 10);
	backgroundLayout->setColumnStretch(2, 10);

	QWidget* backgroundWidget = new QWidget;
	backgroundWidget->setLayout(backgroundLayout);

	toolBox = new QToolBox;
	toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	toolBox->setMinimumWidth(itemWidget->sizeHint().width());
	toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));
	toolBox->addItem(backgroundWidget, tr("Backgrounds"));

}

void MainWindow::inintActionConnection()
{
	connect(toFrontAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::bringToFront);
	connect(sendBackAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::sendToBack);
	connect(deleteAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::deleteItem);
	connect(exitAction, &QAction::triggered, this, &QWidget::close);
	connect(boldAction, &QAction::triggered, this, &MainWindow::handleFontChange);
	connect(italicAction, &QAction::triggered, this, &MainWindow::handleFontChange);
	connect(underlineAction, &QAction::triggered, this, &MainWindow::handleFontChange);
	connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createActions()
{
	toFrontAction = new QAction(QIcon(ImageSources::BringFront),
		tr("Bring to &Front"), this);
	toFrontAction->setShortcut(tr("Ctrl+F"));
	toFrontAction->setStatusTip(tr("Bring item to front"));
	
	sendBackAction = new QAction(QIcon(ImageSources::SendBack), tr("Send to &Back"), this);
	sendBackAction->setShortcut(tr("Ctrl+T"));
	sendBackAction->setStatusTip(tr("Send item to back"));
	
	deleteAction = new QAction(QIcon(ImageSources::Delete), tr("&Delete"), this);
	deleteAction->setShortcut(tr("Delete"));
	deleteAction->setStatusTip(tr("Delete item from diagram"));
	
	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setStatusTip(tr("Quit Scenediagram example"));
	
	boldAction = new QAction(QIcon(ImageSources::Bold), tr("Bold"), this);
	boldAction->setCheckable(true);
	boldAction->setShortcut(tr("Ctrl+B"));
	
	italicAction = new QAction(QIcon(ImageSources::Italic), tr("Italic"), this);
	italicAction->setCheckable(true);
	italicAction->setShortcut(tr("Ctrl+I"));
	
	underlineAction = new QAction(QIcon(ImageSources::Underline), tr("Underline"), this);
	underlineAction->setCheckable(true);
	underlineAction->setShortcut(tr("Ctrl+U"));
	
	aboutAction = new QAction(tr("A&bout"), this);
	aboutAction->setShortcut(tr("F1"));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(exitAction);

	itemMenu = menuBar()->addMenu(tr("&Item"));
	itemMenu->addAction(deleteAction);
	itemMenu->addSeparator();
	itemMenu->addAction(toFrontAction);
	itemMenu->addAction(sendBackAction);

	aboutMenu = menuBar()->addMenu(tr("&Help"));
	aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars()
{

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(deleteAction);
	editToolBar->addAction(toFrontAction);
	editToolBar->addAction(sendBackAction);

	fontCombo = new QFontComboBox();
	connect(fontCombo, &QFontComboBox::currentFontChanged,
		this, &MainWindow::currentFontChanged);

	fontSizeCombo = new QComboBox;
	fontSizeCombo->setEditable(true);
	for (int i = 8; i < 30; i = i + 2)
		fontSizeCombo->addItem(QString().setNum(i));
	fontSizeCombo->setCurrentIndex(5);
	QIntValidator* validator = new QIntValidator(2, 64, this);
	fontSizeCombo->setValidator(validator);
	connect(fontSizeCombo, &QComboBox::currentTextChanged,
		this, &MainWindow::fontSizeChanged);

	fontColorToolButton = new QToolButton;
	fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
	textAction = fontColorToolButton->menu()->defaultAction();
	fontColorToolButton->setIcon(createColorToolButtonIcon(ImageSources::TextPointer, Qt::black));
	fontColorToolButton->setAutoFillBackground(true);

	fillColorToolButton = new QToolButton;
	fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
	fillAction = fillColorToolButton->menu()->defaultAction();
	fillColorToolButton->setIcon(createColorToolButtonIcon(ImageSources::FloodFill, Qt::white));

	lineColorToolButton = new QToolButton;
	lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
	lineAction = lineColorToolButton->menu()->defaultAction();
	lineColorToolButton->setIcon(createColorToolButtonIcon(ImageSources::LineColor, Qt::black));
	textToolBar = addToolBar(tr("Font"));
	textToolBar->addWidget(fontCombo);
	textToolBar->addWidget(fontSizeCombo);
	textToolBar->addAction(boldAction);
	textToolBar->addAction(italicAction);
	textToolBar->addAction(underlineAction);

	colorToolBar = addToolBar(tr("Color"));
	colorToolBar->addWidget(fontColorToolButton);
	colorToolBar->addWidget(fillColorToolButton);
	colorToolBar->addWidget(lineColorToolButton);

	linePointerButton = new QToolButton;
	linePointerButton->setCheckable(true);
	linePointerButton->setChecked(false);
	linePointerButton->setIcon(QIcon(ImageSources::LinePointer));
	connect(linePointerButton, &QAbstractButton::toggled, this, &MainWindow::linePointerButtonClicked);

	runButton = new QToolButton;
	runButton->setCheckable(true);
	runButton->setChecked(false);
	runButton->setIcon(QIcon(ImageSources::RunButton));
	connect(runButton, &QAbstractButton::toggled, this, &MainWindow::runButtonClicked);

	exportButton = new QToolButton;
	exportButton->setCheckable(true);
	exportButton->setChecked(false);
	exportButton->setIcon(QIcon(ImageSources::ExportButton));
	connect(exportButton, &QAbstractButton::toggled, this, &MainWindow::exportButtonClicked);

	loadButton = new QToolButton;
	loadButton->setCheckable(true);
	loadButton->setChecked(false);
	loadButton->setIcon(QIcon(ImageSources::LoadButton));
	connect(loadButton, &QAbstractButton::toggled, this, &MainWindow::loadButtonClicked);

	sceneScaleCombo = new QComboBox;
	QStringList scales;
	scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
	sceneScaleCombo->addItems(scales);
	sceneScaleCombo->setCurrentIndex(2);
	connect(sceneScaleCombo, &QComboBox::currentTextChanged,
		this, &MainWindow::sceneScaleChanged);

	sceneScaleBar = addToolBar(tr("Scale"));
	sceneScaleBar->addWidget(sceneScaleCombo);

	pointerToolBar = addToolBar(tr("linePointer"));
	pointerToolBar->addWidget(linePointerButton);

	runButtonToolBar = addToolBar(tr("Run"));
	runButtonToolBar->addWidget(runButton);

	exportButtonToolBar = addToolBar(tr("Export"));
	exportButtonToolBar->addWidget(exportButton);

	loadButtonToolBar = addToolBar(tr("Load"));
	loadButtonToolBar->addWidget(loadButton);
}


//BackGroundType
QWidget* MainWindow::createBackgroundCellWidget(const QString& text,
	int type, const QString& image)
{
	QToolButton* button = new QToolButton;
	button->setIcon(QIcon(image));
	button->setIconSize(QSize(50, 50));
	button->setCheckable(true);
	backgroundButtonGroup->addButton(button, type);

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(button, 0, 0, Qt::AlignHCenter);
	layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	return widget;
}

QWidget* MainWindow::createCellWidget(const QString& text)
{
	idnames[itemtype] = text;
	std::string itemclass = text.toStdString();
	XBaseItem  *item = ItemRegistry::createObject(itemclass, graphicsWidget, itemMenu, nullptr);
	QIcon icon(item->image());
	// 在这里 item 仅仅是为了获得image图形, 所以用完就删除
	// The item here is only for obtaining image graphics, so it will be deleted after used
	delete item;
	item = nullptr;

	QToolButton* button = new QToolButton;
	button->setIcon(icon);
	button->setIconSize(QSize(50, 50));
	button->setCheckable(true);
	buttonGroup->addButton(button, itemtype);

	itemtype += 1;

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(button, 0, 0, Qt::AlignHCenter);
	layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	return widget;
}

QMenu* MainWindow::createColorMenu(const char* slot, QColor defaultColor)
{
	QList<QColor> colors;
	colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
	QStringList names;
	names << tr("black") << tr("white") << tr("red") << tr("blue")
		<< tr("yellow");

	QMenu* colorMenu = new QMenu(this);
	for (int i = 0; i < colors.count(); ++i)
	{
		QAction* action = new QAction(names.at(i), this);
		action->setData(colors.at(i));
		action->setIcon(createColorIcon(colors.at(i)));
		connect(action, SIGNAL(triggered()), this, slot);
		colorMenu->addAction(action);
		if (defaultColor == colors.at(i))
		{
			colorMenu->setDefaultAction(action);
		}
	}
	return colorMenu;
}

QIcon MainWindow::createColorToolButtonIcon(const QString& imageFile, QColor color)
{
	QPixmap pixmap(50, 80);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	// Draw icon centred horizontally on button.
	QRect target(4, 0, 42, 43);
	QPixmap image(imageFile);
	QRect source(0, 0, 42, 43);
	// ToolButton的图标大小42x43, 左上角(4,0), 右下角(46,43)
	painter.drawPixmap(target, image, source);
	//颜色块大小50x20, 左上角(0,60), 右下角(50,80)
	painter.fillRect(QRect(0, 60, 50, 20), color);
	return QIcon(pixmap);
}

QIcon MainWindow::createColorIcon(QColor color)
{
	QPixmap pixmap(20, 20);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 20, 20), color);
	return QIcon(pixmap);
}

void MainWindow::initSceneBackground()
{
	const int TILE_SIZE = 50;
	QPixmap bg(TILE_SIZE, TILE_SIZE);
	bg.fill(Qt::transparent);
	QPainter p(&bg);
	p.setBrush(QBrush(Qt::gray));
	p.drawRect(0, 0, TILE_SIZE, TILE_SIZE);
	graphicsWidget->setBackgroundBrushChange(QBrush(bg));
}

void MainWindow::listenForError()
{
	// 连接错误发生信号到槽函数处理
	QObject::connect(this, &MainWindow::errorOccurred, [&](XVisual::ErrorCode errorCode) 
	{
		std::string errorCodeStr = XVisual::errorCodeToStr(errorCode);
		std::string warningStr = "\"" + errorCodeStr + "\" occurred. Program cannot continue.";
		QString warningQStr = QString::fromStdString(warningStr);
		// 显示警告对话框
		QMessageBox::critical(nullptr, "Error", warningQStr);

		//// 采用 QMessageBox::information
		//// 提示用户终止程序
		//QMessageBox::StandardButton button = QMessageBox::information(nullptr, "Error", "Terminate program?",
		//QMessageBox::Yes, QMessageBox::NoButton);
		//// 如果用户选择终止程序，则退出应用
		//if (button == QMessageBox::Yes) 
		//{
		//	qApp->quit();
		//}


		// 采用 QMessageBox::question
		// QMessageBox::Yes 和 QMessageBox::No 两个按钮同时显示
		// 提示用户终止程序
		QMessageBox::StandardButton button = QMessageBox::question(nullptr, "Error", "Terminate program?",
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 如果用户选择终止程序，则退出应用
		if (button == QMessageBox::Yes) 
		{
			qApp->quit();
		}


		//// 采用 QMessageBox::warning
		//// 提示用户终止程序
		// QMessageBox::StandardButton button = QMessageBox::warning(nullptr, "Error", "Terminate program?");
		//// 如果用户选择终止程序，则退出应用
		//if (button == QMessageBox::Ok) 
		//{
		//	qApp->quit();
		//}


	});
}