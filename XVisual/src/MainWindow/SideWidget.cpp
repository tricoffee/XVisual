#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "MainWindow/SideWidget.h"
#include "MainWindow/ImagePageWidget.h"
#include "MainWindow/NamesWidget.h"
#include "MainWindow/VariableWidget.h"
#include "Common/Aux.h"


//#include "Common/ZoomImageLabel.h"


#include "Common/ZoomableGraphicsView.h"
#include "ItemBase/XBaseItem.h"

namespace XVisual {

SideWidget::SideWidget(QWidget* parent) : QWidget(parent), unfold(false)
{
    /*
    foldButton是一个控制foldWidget折叠和展开的按钮。foldButton被添加到QHBoxLayout并显示在btnWidget
    */
    // 样式表定义了按钮在正常状态和按下状态下的外观
    const QString btnStyle = "QPushButton{border-radius:18px; background-position:center; background-color:rgba(245,245,245,250)}"\
        "QPushButton:pressed{background-color:rgba(255,255,255,255);border-style:inset;}";
    foldButton = new QPushButton();
    foldButton->setText("Hide<<");
    // 应用样式表调整foldButton的外观
    foldButton->setStyleSheet(btnStyle);
    // 连接按钮按下信号和它的槽函数
    connect(foldButton, &QPushButton::clicked, this, &SideWidget::on_foldButton_clicked);
    // 将foldButton添加到btnLayout布局
    btnLayout = new QHBoxLayout();
    // 推动foldButton到右侧
    btnLayout->addStretch();
    // 使用Qt::AlignRight来右对齐foldButton
    btnLayout->addWidget(foldButton, 0, Qt::AlignRight);
    // 设置btnLayout中小控件之间的距离
    btnLayout->setSpacing(0);
    // 设置btnLayout的外边距
    btnLayout->setContentsMargins(0, 0, 0, 0);
    // btnWidget is a QWidget which holds a QHBoxLayout for foldButton
    btnWidget = new QWidget();
    btnWidget->setLayout(btnLayout);
    /*
    btnWidget->setStyleSheet("padding: 0px;");是一个样式表(CSS, Cascading Style Sheets)的设置，
    它指定了小控件btnWidget的内边距。
    内边距是小控件内容与小控件边界之间的空间。
    将内边距设置为"0px"是为了确保按钮小控件的内容紧贴在边界上。
    */
    btnWidget->setStyleSheet("padding: 0px; margin: 0px;background-color: lightblue; ");
    // 设置btnWidget的大小策略
    btnWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 获取按钮文本的宽度
    QFontMetrics fontMetrics(foldButton->font());
    // fontMetrics.horizontalAdvance是以像素为单位的字符宽度
    int textWidth = fontMetrics.horizontalAdvance(foldButton->text());
    // 设置foldButton的宽度为textWidth+3
    foldButton->setFixedWidth(textWidth + 3);
    // 设置btnWidget的宽度为textWidth+3
    btnWidget->setFixedWidth(textWidth + 3);
    // fontMetrics.height字符高度
    int textHeight = fontMetrics.height();
    // 设置foldButton的高度为textHeight+12
    foldButton->setFixedHeight(textHeight + 12);
    // 设置foldButton的大小策略
    foldButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 创建一个名为"imagePageWidget"的QWidget用于显示照片
    imagePageWidget = new ImagePageWidget();
    // 创建一个名为"upLayout"的QHBoxLayout
    upLayout = new QHBoxLayout();
    // 设置upLayout中小控件之间的距离
    upLayout->setSpacing(0);
    // 设置upLayout的外边距
    upLayout->setContentsMargins(0, 0, 0, 0);
    // 将imagePageWidget添加到upLayout
    upLayout->addWidget(imagePageWidget);
    // 创建一个名为"upWidget"的QWidget
    upWidget = new QWidget();
    // upWidget的初始状态为隐藏
    upWidget->hide();
    // 给upWidget设置upLaypout布局
    upWidget->setLayout(upLayout);

    namesWidget = new NamesWidget();
    variableWidget = new VariableWidget();
    //当点击NameWidget中某个项的"+"按钮时，发送信号给SideWidget::tabAddSlot
    connect(namesWidget, &NamesWidget::tabAddSignal, this, &SideWidget::tabAddSlot);
    //当点击NameWidget中某个项的"-"按钮时，发送信号给SideWidget::tabRomoveSlot 
    connect(namesWidget, &NamesWidget::tabRemoveSignal, this, &SideWidget::tabRemoveSlot);
    //当在NamesWidget中切换选中项时，ImagePageWidget里面的tabWidget的选中项
    connect(namesWidget, &NamesWidget::itemSelectedSignal, imagePageWidget, &ImagePageWidget::tabSelectedSlot);
    //当在NamesWidget中切换选中项时，VariableWidget的选中项    
    connect(namesWidget, &NamesWidget::itemSelectedSignal, variableWidget, &VariableWidget::nodeSelectedSlot);
	//
    connect(imagePageWidget, &ImagePageWidget::tabChangedSignal, namesWidget, &NamesWidget::itemChangedSlot);
	//
	connect(imagePageWidget, &ImagePageWidget::tabChangedSignal, variableWidget, &VariableWidget::nodeChangedSlot);
    //
    connect(variableWidget, &VariableWidget::nodeToggledSignal, imagePageWidget, &ImagePageWidget::tabToggledSlot);
    //
    connect(variableWidget, &VariableWidget::nodeToggledSignal, namesWidget, &NamesWidget::itemToggledSlot);
    
    
    // 创建水平方向splitter
	hSplitter = new QSplitter(Qt::Horizontal);
	// 设置QSplitter分割线的样式表
    hSplitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
	// 将namesWidget添加到hSplitter
    hSplitter->addWidget(namesWidget);
	// 将variableWidget添加到hSplitter
    hSplitter->addWidget(variableWidget);
	// 设置拉伸比例
    hSplitter->setStretchFactor(0, 1);
    hSplitter->setStretchFactor(1, 1);
    bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(hSplitter);
    // 设置bottomLayout中小控件之间的距离, 即设置内边距
    bottomLayout->setSpacing(0);
    // 设置bottomLayout的外边距
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomWidget = new QWidget();
    bottomWidget->setLayout(bottomLayout);

	// 创建垂直方向splitter
	vSplitter = new QSplitter(Qt::Vertical);
	// 设置QSplitter分割线的样式表
	vSplitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
	// 将upWidget添加到vSplitter
	vSplitter->addWidget(upWidget);
	// 将bottomWidget添加到vSplitter
	vSplitter->addWidget(bottomWidget);
	// 设置拉伸比例
	vSplitter->setStretchFactor(0, 1);
	vSplitter->setStretchFactor(1, 1);
	rightLayout = new QVBoxLayout();
	rightLayout->addWidget(vSplitter);
	// 设置rightLayout中小控件之间的距离, 即设置内边距
	rightLayout->setSpacing(0);
	// 设置rightLayout的外边距
	rightLayout->setContentsMargins(0, 0, 0, 0);
	rightWidget = new QWidget();
	rightWidget->setLayout(rightLayout);

    // 创建一个名为sideLayout的QHBoxLayout
    sideLayout = new QHBoxLayout();
    // 将btnWidget添加到sideLayout
    sideLayout->addWidget(btnWidget);
    // 将rightWidget添加到sideLayout
    sideLayout->addWidget(rightWidget);
    // 将sideLayout设置为向右对齐
    sideLayout->setAlignment(Qt::AlignRight);
    // 设置sideLayout中小控件之间的距离
    sideLayout->setSpacing(0);
    // 设置sideLayout的外边距
    sideLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(sideLayout);
}

void SideWidget::on_foldButton_clicked()
{
	qDebug() << "void SideWidget::on_button1_clicked() ";
	if (unfold)
	{
        // lightgreen
		//upWidget->setStyleSheet("background-color: lightgreen");
        // #8FBC8F, DarkSeaGreen
        //upWidget->setStyleSheet("background-color: #8FBC8F");
        // #9BCD9B 
        //upWidget->setStyleSheet("background-color: #9BCD9B");
        // #9ACD32
        //upWidget->setStyleSheet("background-color: #9ACD32");
        // #CDC5BF
        //upWidget->setStyleSheet("background-color: #CDC5BF");
        // #EEE5DE
        //upWidget->setStyleSheet("background-color: #EEE5DE");
        // #BCD2EE
        upWidget->setStyleSheet("background-color: #BCD2EE");
        upLayout->setContentsMargins(0, 0, 0, 0);
        upWidget->show();
		foldButton->setText("Hide\n>>");
		unfold = false;
		qDebug() << "foldWidget->isVisible() " << upWidget->isVisible();
	}
	else
	{
		upLayout->setContentsMargins(0, 0, 0, 0); 
        upWidget->hide();
		foldButton->setText("Show\n<<");
		unfold = true;
		qDebug() << "foldWidget->isVisible() " << upWidget->isVisible();
	}
}

ImagePageWidget* SideWidget::getImagePageWidget()
{
    return imagePageWidget;
}

void SideWidget::showImageInTabSlot(const std::string& filename, const cv::Mat& image, XBaseItem* item)
{
    XLOG_INFO("SideWidget::showImageInTabSlot ...", CURRENT_THREAD_ID);

    // 弃用代码段
	 // //  创建一个ZoomImageLabel用于显示图像
    // ZoomImageLabel* imageLabel = new ZoomImageLabel();
	// 将CV::Mat转换为QImage
	// QImage qImage = cvMatToQImage(image);
	// // 将QImage转换为QPixmap
	// QPixmap pixmap = QPixmap::fromImage(qImage);
	// // 在QLabel上显示QPixmap
	// imageLabel->setPixmap(pixmap);
    // imageLabel->setScaledContents(true);
    // // 获取tabWidget
    // QTabWidget* tabWidget = imagePageWidget->getTabWidget();
	// // 将QWidget添加为新的标签页
	// int tabIndex = tabWidget->addTab(imageLabel, QString::fromStdString(filename));




    // 创建一个ZoomableGraphicsView用于显示图像
    ZoomableGraphicsView* imageShowWidget = new ZoomableGraphicsView();
    imageShowWidget->initLoad(image);
     // 获取 tabWidget
	QTabWidget* tabWidget = imagePageWidget->getTabWidget();
	// 将 imageShowWidget 添加为新的标签页
    int tabIndex = tabWidget->addTab(imageShowWidget, QString::fromStdString(filename));

	// 使新添加的标签页成为活动页
	tabWidget->setCurrentIndex(tabIndex);

    // 使namesWidget新增一行记录
    namesWidget->addRow(QString::fromStdString(filename));

    const std::string xUuid = item->getUuid();

    XLOG_INFO("SideWidget::showImageInTabSlot  xUuid = "+xUuid, CURRENT_THREAD_ID);

    variableWidget->createTree(item);
}

void SideWidget::tabAddSlot(const QString& itemName)
{
    XLOG_INFO("SideWidget::tabAddSlot  itemName = " + itemName.toStdString(), CURRENT_THREAD_ID);
    QTabWidget* tabWidget = imagePageWidget->getTabWidget();
	int tabIndex = -1;
	// 遍历tabWidget，查找与itemName匹配的tab
	for (int i = 0; i < tabWidget->count(); ++i) 
    {
		if (tabWidget->tabText(i) == itemName) 
        {
			tabIndex = i;
			break;
		}
	}
	if (tabIndex != -1) 
    {
        tabWidget->setCurrentIndex(tabIndex);
	}
	else 
    {
        //TO-DO, Please use the cv:: Mat associated with this itemName 
        //to add a new tab in this tab Widget to display images
        XLOG_INFO("SideWidget::tabAddSlot  TO-DO " + itemName.toStdString(), CURRENT_THREAD_ID);
	}
}

void SideWidget::tabRemoveSlot(const QString& itemName)
{
    XLOG_INFO("SideWidget::tabRemoveSlot  itemName = " + itemName.toStdString(), CURRENT_THREAD_ID);
    QTabWidget* tabWidget = imagePageWidget->getTabWidget();
	int tabIndex = -1;
	// 遍历tabWidget，查找与itemName匹配的tab
	for (int i = 0; i < tabWidget->count(); ++i)
	{
		if (tabWidget->tabText(i) == itemName)
		{
			tabIndex = i;
			break;
		}
	}
	if (tabIndex != -1)
	{
        tabWidget->removeTab(tabIndex);
        XLOG_INFO("SideWidget::tabRemoveSlot  removed " + itemName.toStdString(), CURRENT_THREAD_ID);
	}
	else
	{
        XLOG_INFO("SideWidget::tabRemoveSlot  " + itemName.toStdString()+ " not existed.", CURRENT_THREAD_ID);
	}
}

} // namespace XVisual
