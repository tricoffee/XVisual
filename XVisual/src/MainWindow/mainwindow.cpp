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
#include "MainWindow/Diagramscene.h"
#include "MainWindow/Mainwindow.h"
#include "Common/constants.h"
#include "MainWindow/GraphicsWidget.h"
#include "MainWindow/SideWidget.h"
#include "MainWindow/ImagePageWidget.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "CJSON/cJson.h"
#include <fstream>
#include <set>
#include <any>
#include "Common/JsonFIleUtils.h"
#include "Common/XParser.h"
#include "Common/TypeClassifier.h"
#include "GlobalStorage/GlobalVariable.h"
#include "WorkSpaceDialog/WorkSpaceDialog.h"

#include "Common/ErrorCode.h"
#include <QFile>
#include "Common/FileUtils.h"
#include "Common/JsonFileUtils.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)

void MainWindow::checkAndCreateDirectory(const QString& path)
{
	QDir dir(path);

	// ����ļ����Ƿ����
	if (dir.exists())
	{
		XLOG_INFO("MainWindow::checkAndCreateDirectory, " + path.toStdString() + "Existed. ", CURRENT_THREAD_ID);
	}
	else
	{
		XLOG_INFO("MainWindow::checkAndCreateDirectory, No Found, " + path.toStdString() + ", Creating ... ", CURRENT_THREAD_ID);

		// �����ļ���
		if (dir.mkpath("."))
		{
			XLOG_INFO("MainWindow::checkAndCreateDirectory, Created " + path.toStdString() + " Successfully ! ", CURRENT_THREAD_ID);
		}
		else
		{
			XLOG_INFO("MainWindow::checkAndCreateDirectory, Created " + path.toStdString() + " Failed ! ", CURRENT_THREAD_ID);
		}
	}
}

void MainWindow::makeDefaultSettings()
{
	/* ���Ĭ�� settingsFilePath ������, �򴴽�һ�� settings.json */
	// create JSON object
	QJsonObject workspaceObject;
	workspaceObject["DefaultWorkSpace"] = QDir::toNativeSeparators(workspaceData.defaultWorkSpace);
	workspaceObject["CustomWorkSpace"] = workspaceData.customWorkSpace;
	workspaceObject["EnableCustom"] = workspaceData.enableCustom;

	QJsonObject rootObject;
	rootObject["WorkSpace"] = workspaceObject;

	// create JSON document
	QJsonDocument jsonDoc(rootObject);

	// �� JSON �ĵ�ת��Ϊ�ַ���
	QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);

	// ��� JSON �ַ���������̨
	XLOG_INFO("MainWindow::makeDefaultSettings, jsonString = " + jsonString.toStdString(), CURRENT_THREAD_ID);
	XLOG_INFO("MainWindow::makeDefaultSettings, settingsFilePath_ = " + settingsFilePath.toStdString(), CURRENT_THREAD_ID);

	// �� JSON �ַ���д���ļ�
	QFile file(settingsFilePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		//qWarning("MainWindow::makeDefaultSettings, Cannot open \"settings.json\" ");
		XLOG_INFO("MainWindow::makeDefaultSettings, Cannot open " + settingsFilePath.toStdString(), CURRENT_THREAD_ID);
		// Done, emit error
		m_lastError = XVisual::ErrorCode::OpenJsonFileError;
		// ��������źţ�֪ͨ������
		emit errorOccurred(m_lastError);
		return;
	}

	file.write(jsonString.toUtf8());
	file.close();
}

MainWindow::MainWindow(const QString& mProjectRootDir, QWidget* parent) : QMainWindow(parent)
{
	// default settingsFilePath
	settingsFilePath = mProjectRootDir + "/settings.json";

	workspaceData.customWorkSpace = ""; // QString 
	workspaceData.defaultWorkSpace = mProjectRootDir + "/WorkSpace"; // QString 
	workspaceData.enableCustom = 0; // int 

	// if settingsFilePath_ exists, then read WorkSpaceData from this json file, and update \"data\"
	if (isJsonFile(settingsFilePath.toStdString()))
	{
		SettingsReader& reader = SettingsReader::getInstance();
		reader.getData(settingsFilePath, workspaceData);
		if (workspaceData.enableCustom) // data.enableCustom == 1
		{
			// update globalWorkSpaceDir
			globalWorkSpaceDir = workspaceData.customWorkSpace.toStdString();
		}
		else // data.enableCustom == 0
		{
			// update globalWorkSpaceDir
			globalWorkSpaceDir = workspaceData.defaultWorkSpace.toStdString();
		}
	}
	else
	{
		// update globalWorkSpaceDir
		globalWorkSpaceDir = workspaceData.defaultWorkSpace.toStdString();
		checkAndCreateDirectory(workspaceData.defaultWorkSpace);
		makeDefaultSettings();
	}
	createActions();
	createMenus();
    graphicsWidget = new GraphicsWidget(itemMenu);
	createToolbars();
	createToolBox();
	inintActionConnection();

	sideWidget = new SideWidget();

	// ����ˮƽ����splitter
	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	// ����QSplitter�ָ��ߵ���ʽ��
	splitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
    // ��toolBox��ӵ�splitter
	splitter->addWidget(toolBox);
	// ��graphicsWidget��ӵ�splitter
	splitter->addWidget(graphicsWidget);
	// ��sideWidget��ӵ�splitter
	splitter->addWidget(sideWidget);
	//�����������
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 1);
	splitter->setStretchFactor(2, 0);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(splitter);
	// ����layout��С�ؼ�֮��ľ���, �������ڱ߾�
	layout->setSpacing(0);
	// ����layout����߾�
	layout->setContentsMargins(0, 0, 0, 0);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	setCentralWidget(widget);
	setWindowTitle(tr("XVisual"));
	setUnifiedTitleAndToolBarOnMac(true);

	connect(graphicsWidget, &GraphicsWidget::showImageInTabSignal, 
		sideWidget,&SideWidget::showImageInTabSlot);
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
		// ����xGraph��ÿ��Ԫ��
		//for(auto it = xGraph.begin(); it != xGraph.end(); ++it) 
		//{
		//	// ��ȡ��ǰ�ڵ�node
		//	std::shared_ptr<GraphNode> node = *it;
		//	// ��ȡ��ǰ�ڵ��nodeId
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
			// �����쳣����¼������
			m_lastError = XVisual::ErrorCode::UmapKeyNoFound;
			// ��������źţ�֪ͨ������
			emit errorOccurred(m_lastError);
		}

	}// ���������ʱ��lock_guard ���Զ�����������xGraphMutex
	XLOG_INFO("void MainWindow::runButtonClicked xGraph is executed.", CURRENT_THREAD_ID);
}


void MainWindow::exportButtonClicked(bool checked)
{
	XLOG_INFO("MainWindow::exportButtonClicked, to do export XGraph", CURRENT_THREAD_ID);

	// handle_parentsάϵÿ��handle�ڵ�ĸ��ڵ�
	std::unordered_map<std::string, std::set<std::string>> handle_parents;
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		// ����xGraph��ÿ��Ԫ��
		for(auto it = xGraph.begin(); it != xGraph.end(); ++it) 
		{
			// ��ȡ��ǰ�ڵ�node
			std::shared_ptr<GraphNode> node = *it;
			// ��ȡ��ǰ�ڵ��nodeId
			std::string pItemId = node->nodeId;
			// ��ȡ��ǰ�ڵ��ӦHandle��nodeId
			std::string pDelimiter = "_";
			std::string pUuidStr2 = extractSubstrAfterDelimiter(pItemId, pDelimiter);
			std::string pHandleId = "Handle_" + pUuidStr2;
			
			for (const auto& neighbor : node->neighbors)
			{
				// ��ȡ��ǰ�ڵ�ĳ���ھӵ�nodeId
				std::string qItemId = neighbor->nodeId;

				//Unused, item_parentsάϵÿ��item�ڵ�ĸ��ڵ�
				//item_parents[qItemId].insert(pItemId);
				
				// ��ȡ��ǰ�ڵ�ĳ���ھӶ�ӦHandle��nodeId
				std::string qDelimiter = "_";
				std::string qUuidStr2 = extractSubstrAfterDelimiter(qItemId, qDelimiter);
				std::string qHandleId = "Handle_" + qUuidStr2;
				handle_parents[qHandleId].insert(pHandleId);
			}
		}
	}// ���������ʱ��lock_guard ���Զ�����������xGraphMutex

	cJSON* cjson_root = NULL;
	/* ����һ��JSON���ݶ���(����ͷ���) */
	cjson_root = cJSON_CreateObject();
	cJSON* cjson_arr = cJSON_CreateArray();
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		for (const auto& eMap: globalItemMap)
		{
			// cjson_arr_elementά��Item�����Ϣ, Handle�����Ϣ, �Լ�innerParam��outerParam
			cJSON* cjson_arr_element = cJSON_CreateObject();

			// ��ȡItem�����Ϣ(Item_ClassName,Item_ColleagueType,Item_ColleagueType,Item_UniqueName)
			XBaseItem* item = eMap.second;
			std::string item_classNameStr = item->getClassName();
			ColleagueType item_colleagueType = item->getColleagueType();
			int item_colleagueTypeIntValue = static_cast<int>(item_colleagueType);
			std::string item_colleagueId = item->getUuid();
			std::string item_uniqueName = item->getUniqueName();

			// д��Item�����Ϣ(Item_ClassName,Item_ColleagueType,Item_ColleagueType,Item_UniqueName)��JSON
			cJSON_AddStringToObject(cjson_arr_element, "Item_ClassName", item_classNameStr.c_str());
			cJSON_AddNumberToObject(cjson_arr_element, "Item_ColleagueType", item_colleagueTypeIntValue);
			cJSON_AddStringToObject(cjson_arr_element, "Item_ColleagueId", item_colleagueId.c_str());
			cJSON_AddStringToObject(cjson_arr_element, "Item_UniqueName", item_uniqueName.c_str());

			// ��ȡItem��λ����Ϣ
			QPointF itemPos = item->scenePos();
			// д��Item��λ����Ϣ��JSON
			cJSON* cjson_itemPos = cJSON_CreateArray();
			cJSON_AddItemToArray(cjson_itemPos, cJSON_CreateNumber(itemPos.x()));
			cJSON_AddItemToArray(cjson_itemPos, cJSON_CreateNumber(itemPos.y()));
			cJSON_AddItemToObject(cjson_arr_element, "Item_Position", cjson_itemPos);

			// ��ȡHandle�������Ϣ(ClassName,ColleagueType,ColleagueType,UniqueName)
			XBaseHandle* handle = eMap.second->getXHandle();
			std::string handle_classNameStr = handle->getClassName();
			ColleagueType handle_colleagueType = handle->getColleagueType();
			int handle_colleagueTypeIntValue = static_cast<int>(handle_colleagueType);
			std::string handle_colleagueId = handle->getUuid();
			std::string handle_uniqueName = handle->getUniqueName();

            // д��Handle�����Ϣ(ClassName,ColleagueType,ColleagueType,UniqueName)��JSON
			cJSON_AddStringToObject(cjson_arr_element, "ClassName", handle_classNameStr.c_str());
			cJSON_AddNumberToObject(cjson_arr_element, "ColleagueType", handle_colleagueTypeIntValue);
			cJSON_AddStringToObject(cjson_arr_element, "ColleagueId", handle_colleagueId.c_str());
			cJSON_AddStringToObject(cjson_arr_element, "UniqueName", handle_uniqueName.c_str());

			// ��ȡsources����ÿ��source����xName��Ӧ��sourceFrom, ������д�뵽JSON
			cJSON* cjson_sourceFrom = cJSON_CreateObject();
			Source& sources = handle->getSources();
			std::vector<std::string> xVaribleNames = ACQUIRE_NAMES(sources);
			for (const auto& xName : xVaribleNames)
			{
				SourceFrom sourceFrom;
				handle->loadSourceFrom(xName, sourceFrom);

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
				// ����˵��SourceFrom��JSON�ļ�����ı�����ʽ
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
	}// ���������ʱ��lock_guard ���Զ�����������xGraphMutex

	cJSON_AddItemToObject(cjson_root, "ClassInfo", cjson_arr);

	/* ��ӡJSON����(��������)���������� */
	char* str = NULL;
	str = cJSON_Print(cjson_root);
	// printf("%s\n", str);

	const std::string file_name = "solutions.json";
	// ���� std::filesystem::path �����ʾ�ļ�·��
	std::filesystem::path file_path = globalWorkSpaceDir;
	file_path /= file_name; // ʹ�� /= ���������·��ƴ��
	std::string solutionJsonFilePath = file_path.string();

	// �� JSON ���ݱ��浽 .json �ļ���
	std::ofstream outFile(solutionJsonFilePath);
	outFile << str;
	outFile.close();

	XLOG_INFO("MainWindow::exportButtonClicked, XGraph export completed", CURRENT_THREAD_ID);
}

void MainWindow::loadButtonClicked(bool checked)
{
	XLOG_INFO("void MainWindow::loadButtonClicked to do export XGraph", CURRENT_THREAD_ID);
	QString defaultPath = latestJsonPath.isEmpty() ? QDir::homePath() : latestJsonPath;
	QString jsonPath = QFileDialog::getOpenFileName(nullptr, "Select JSON", defaultPath, "JSON FILE (*.json)");
	if (!jsonPath.isEmpty())
	{
		// ���浱ǰѡ�е��ļ�·��
		latestJsonPath = jsonPath;
		XLOG_INFO("MainWindow::loadButtonClicked, jsonPath is NOT empty ..., jsonPath is " + latestJsonPath.toStdString() , CURRENT_THREAD_ID);
		
		XParser& jsonParser = XParser::getInstance();
		jsonParser.initParser(latestJsonPath.toStdString());

		//Done, ��solution.json�ĸ�Ŀ¼��Ϊ��ǰ��globalWorkSpace,ͬʱ����settings.json��CustomWorkSpace�Լ���EnableCustom����Ϊ1
		std::string parentPathStr;
		getParentPathStr(jsonPath.toStdString(), parentPathStr);

		if (parentPathStr != workspaceData.defaultWorkSpace.toStdString())
		{
			SettingsReader& reader = SettingsReader::getInstance();
			workspaceData.customWorkSpace = QString::fromStdString(parentPathStr);
			workspaceData.enableCustom = 1;
		    reader.updateData(settingsFilePath, workspaceData);
			// update globalWorkSpaceDir
			globalWorkSpaceDir = workspaceData.customWorkSpace.toStdString();
		}

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

			// �������е�������
			mScene->clear();

			int arrSize = cJSON_GetArraySize(classInfoArr);
			XLOG_INFO("== === ===  classInfoArr.size() " + std::to_string(arrSize) + " === === ===", CURRENT_THREAD_ID);


			//item_childrean άϵÿ��item�ڵ�ĺ��ӽڵ�
			std::unordered_map<std::string, std::set<std::string>> itemChildrenLinks;

			//άϵqItemId��ÿ��sourceFrom
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
					// ���� SourceFrom ��ÿ���ڵ�
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
		QString errorInfo = "Your selected jsonPath " + jsonPath + "is empty ...";
		QMessageBox::warning(this, "Error", errorInfo);
	}


	XLOG_INFO("Loading XGraph is executed.", CURRENT_THREAD_ID);
}

void MainWindow::currentFontChanged(const QFont&)
{
	handleFontChanged();
}

void MainWindow::fontSizeChanged(const QString&)
{
	handleFontChanged();
}

void MainWindow::sceneScaleChanged(const QString& scale)
{
	graphicsWidget->setSceneScaleChange(scale);
}

/*
����fontColorToolButton�ĵ����˵���ĳһ��QActionʱ,
slot����MainWindow::textColorChanged��Ӧ, fontColorToolButton��ͼ��(Icon)����Ӧ�仯,
ͬʱ����scene��text�ı���ɫ����Ӧ�仯
*/
void MainWindow::textColorChanged()
{
	textAction = qobject_cast<QAction*>(sender());
	fontColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::TextPointer,
		qvariant_cast<QColor>(textAction->data())));
	//ȥ��ԭ����textButtonTriggered();ֱ�Ӱ�textButtonTriggered()�������һ���ù��������£�
	//��δ����ʾ���ó���scene��text�ı���ɫ
	QColor color = qvariant_cast<QColor>(textAction->data());
	graphicsWidget->setItemTextColor(color);
}

/*
����fillColorToolButton�ĵ����˵���ĳһ��QActionʱ,
slot����MainWindow::itemColorChanged��Ӧ, fillColorToolButton��ͼ��(Icon)����Ӧ�仯,
ͬʱ����scene��item����ɫ����Ӧ�仯
*/
void MainWindow::itemColorChanged()
{
	fillAction = qobject_cast<QAction*>(sender());
	fillColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::FloodFill,
		qvariant_cast<QColor>(fillAction->data())));
	//ȥ��ԭ����fillButtonTriggered();ֱ�Ӱ�fillButtonTriggered�������һ���ù���������:
	//��δ����ʾ���ó���scene��item����ɫ
	QColor color = qvariant_cast<QColor>(fillAction->data());
	graphicsWidget->setSceneItemColor(color);
}

/*
����lineColorToolButton�ĵ����˵���ĳһ��QActionʱ,
slot����MainWindow::lineColorChanged��Ӧ, lineColorToolButton��ͼ��(Icon)����Ӧ�仯,
ͬʱ����scene��line����ɫ����Ӧ�仯
*/
void MainWindow::lineColorChanged()
{
	lineAction = qobject_cast<QAction*>(sender());
	lineColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::LineColor,
		qvariant_cast<QColor>(lineAction->data())));
	//ȥ��ԭ����lineButtonTriggered();ֱ�Ӱ�lineButtonTriggered�������һ���ù���������:
	//��δ����ʾ���ó���scene��line����ɫ
	QColor color = qvariant_cast<QColor>(lineAction->data());
	graphicsWidget->setSceneLineColor(color);
}

void MainWindow::handleFontChanged()
{
	QFont font = fontCombo->currentFont();
	font.setPointSize(fontSizeCombo->currentText().toInt());
	font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
	font.setItalic(italicAction->isChecked());
	font.setUnderline(underlineAction->isChecked());
	graphicsWidget->setFont(font);
}


void MainWindow::handleWorkspaceChanged()
{
	WorkSpaceDialog dialog(workspaceData, this);
	if (dialog.exec() == QDialog::Accepted)
	{
		QString workspacePath = dialog.getWorkSpacePath();
		bool isCustom = dialog.isCustomWorkSpace();

		QDir workspaceQDir(workspacePath);
		// Check if the folder really exists
		if (!workspaceQDir.exists())
		{
			QString errorInfo = "Your selected WorkSpace " + workspacePath + " does not exist";
			QMessageBox::warning(this, "Error", errorInfo);
			return;
		}
		else
		{

			if (isCustom)
			{
				workspaceData.customWorkSpace = workspacePath;
				workspaceData.enableCustom = 1;
			}
			else
			{
				workspaceData.enableCustom = 0;
			}
			// Synchronize updates to settings.json
			SettingsReader& reader = SettingsReader::getInstance();
			m_lastError = reader.updateData(settingsFilePath, workspaceData);

			if (m_lastError != XVisual::ErrorCode::Success)
			{
				// ��������źţ�֪ͨ������
				emit errorOccurred(m_lastError);
				return;
			}

	        // update globalWorkSpaceDir 
			globalWorkSpaceDir = workspacePath.toStdString();
			// xLog info 
			XLOG_INFO(" MainWindow::handleWorkspaceChanged, WorkSpace has been set as : " + workspacePath.toStdString(), CURRENT_THREAD_ID);
			// Display confirmation message
			QMessageBox::information(this, "Set WorkSapce", "WorkSpace has been set as : " + workspacePath);

		}

	}
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About XVisual"),
		tr("<b>XVisual</b> is a platform for visually building computer vision tasks, written in Qt-C++"));
}

void MainWindow::createToolBox()
{
	buttonGroup = new QButtonGroup(this);
	buttonGroup->setExclusive(false);
	connect(buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
		this, &MainWindow::buttonGroupClicked);

	// itemWidget1
	QGridLayout* layout1 = new QGridLayout;
	layout1->addWidget(createCellWidget(tr("LoadImage")), 0, 0);
	layout1->addWidget(createCellWidget(tr("CVCrop")), 0, 1);
	layout1->addWidget(createCellWidget(tr("ImagePre")), 1, 0);
	layout1->addWidget(createCellWidget(tr("PreInver")), 1, 1);
	layout1->addWidget(createCellWidget(tr("RevertBox")), 2, 0);
	layout1->addWidget(createCellWidget(tr("DrawBox")), 2, 1);
	layout1->setRowStretch(10, 10);
	layout1->setColumnStretch(2, 10);
	QWidget* itemWidget1 = new QWidget;
	itemWidget1->setLayout(layout1);

	// itemWidget2
	QGridLayout* layout2 = new QGridLayout;
	layout2->addWidget(createCellWidget(tr("TFDetect")), 0, 0);
	layout2->setRowStretch(10, 10);
	layout2->setColumnStretch(2, 10);
	QWidget* itemWidget2 = new QWidget;
	itemWidget2->setLayout(layout2);

	toolBox = new QToolBox;
	toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	toolBox->setMinimumWidth(itemWidget1->sizeHint().width());
	toolBox->addItem(itemWidget1, tr("Image Processing"));
	toolBox->addItem(itemWidget2, tr("AI Model"));

}

void MainWindow::inintActionConnection()
{
	connect(toFrontAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::bringToFront);
	connect(sendBackAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::sendToBack);
	connect(deleteAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::deleteItem);
	connect(exitAction, &QAction::triggered, this, &QWidget::close);
	connect(boldAction, &QAction::triggered, this, &MainWindow::handleFontChanged);
	connect(italicAction, &QAction::triggered, this, &MainWindow::handleFontChanged);
	connect(underlineAction, &QAction::triggered, this, &MainWindow::handleFontChanged);
	connect(workspaceAction, &QAction::triggered, this, &MainWindow::handleWorkspaceChanged);
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

	workspaceAction = new QAction(tr("WorkSpace"));
	
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

	settingsMenu = menuBar()->addMenu(tr("&Settings"));
	settingsMenu->addAction(workspaceAction);

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

QWidget* MainWindow::createCellWidget(const QString& text)
{
	idnames[itemtype] = text;
	std::string itemclass = text.toStdString();
	XBaseItem  *item = ItemRegistry::createObject(itemclass, graphicsWidget, itemMenu, nullptr);
	QIcon icon(item->image());
	// ������ item ������Ϊ�˻��imageͼ��, ���������ɾ��
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
	// ToolButton��ͼ���С42x43, ���Ͻ�(4,0), ���½�(46,43)
	painter.drawPixmap(target, image, source);
	//��ɫ���С50x20, ���Ͻ�(0,60), ���½�(50,80)
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
	// ���Ӵ������źŵ��ۺ�������
	QObject::connect(this, &MainWindow::errorOccurred, [&](XVisual::ErrorCode errorCode) 
	{
		std::string errorCodeStr = XVisual::errorCodeToStr(errorCode);
		std::string warningStr = "\"" + errorCodeStr + "\" occurred. Program cannot continue.";
		QString warningQStr = QString::fromStdString(warningStr);
		// ��ʾ����Ի���
		QMessageBox::critical(nullptr, "Error", warningQStr);

		// ���� QMessageBox::question
		// QMessageBox::Yes �� QMessageBox::No ������ťͬʱ��ʾ
		// ��ʾ�û���ֹ����
		QMessageBox::StandardButton button = QMessageBox::question(nullptr, "Error", "Terminate program?",
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// ����û�ѡ����ֹ�������˳�Ӧ��
		if (button == QMessageBox::Yes) 
		{
			qApp->quit();
		}

	});
}