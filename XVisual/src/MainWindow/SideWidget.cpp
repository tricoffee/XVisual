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

SideWidget::SideWidget(QWidget* parent) : QWidget(parent), unfold(false)
{
    /*
    foldButton��һ������foldWidget�۵���չ���İ�ť��foldButton����ӵ�QHBoxLayout����ʾ��btnWidget
    */
    // ��ʽ�����˰�ť������״̬�Ͱ���״̬�µ����
    const QString btnStyle = "QPushButton{border-radius:18px; background-position:center; background-color:rgba(245,245,245,250)}"\
        "QPushButton:pressed{background-color:rgba(255,255,255,255);border-style:inset;}";
    foldButton = new QPushButton();
    foldButton->setText("Hide<<");
    // Ӧ����ʽ�����foldButton�����
    foldButton->setStyleSheet(btnStyle);
    // ���Ӱ�ť�����źź����Ĳۺ���
    connect(foldButton, &QPushButton::clicked, this, &SideWidget::on_foldButton_clicked);
    // ��foldButton��ӵ�btnLayout����
    btnLayout = new QHBoxLayout();
    // �ƶ�foldButton���Ҳ�
    btnLayout->addStretch();
    // ʹ��Qt::AlignRight���Ҷ���foldButton
    btnLayout->addWidget(foldButton, 0, Qt::AlignRight);
    // ����btnLayout��С�ؼ�֮��ľ���
    btnLayout->setSpacing(0);
    // ����btnLayout����߾�
    btnLayout->setContentsMargins(0, 0, 0, 0);
    // btnWidget is a QWidget which holds a QHBoxLayout for foldButton
    btnWidget = new QWidget();
    btnWidget->setLayout(btnLayout);
    /*
    btnWidget->setStyleSheet("padding: 0px;");��һ����ʽ��(CSS, Cascading Style Sheets)�����ã�
    ��ָ����С�ؼ�btnWidget���ڱ߾ࡣ
    �ڱ߾���С�ؼ�������С�ؼ��߽�֮��Ŀռ䡣
    ���ڱ߾�����Ϊ"0px"��Ϊ��ȷ����ťС�ؼ������ݽ����ڱ߽��ϡ�
    */
    btnWidget->setStyleSheet("padding: 0px; margin: 0px;background-color: lightblue; ");
    // ����btnWidget�Ĵ�С����
    btnWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // ��ȡ��ť�ı��Ŀ��
    QFontMetrics fontMetrics(foldButton->font());
    // fontMetrics.horizontalAdvance��������Ϊ��λ���ַ����
    int textWidth = fontMetrics.horizontalAdvance(foldButton->text());
    // ����foldButton�Ŀ��ΪtextWidth+3
    foldButton->setFixedWidth(textWidth + 3);
    // ����btnWidget�Ŀ��ΪtextWidth+3
    btnWidget->setFixedWidth(textWidth + 3);
    // fontMetrics.height�ַ��߶�
    int textHeight = fontMetrics.height();
    // ����foldButton�ĸ߶�ΪtextHeight+12
    foldButton->setFixedHeight(textHeight + 12);
    // ����foldButton�Ĵ�С����
    foldButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // ����һ����Ϊ"imagePageWidget"��QWidget������ʾ��Ƭ
    imagePageWidget = new ImagePageWidget();
    // ����һ����Ϊ"upLayout"��QHBoxLayout
    upLayout = new QHBoxLayout();
    // ����upLayout��С�ؼ�֮��ľ���
    upLayout->setSpacing(0);
    // ����upLayout����߾�
    upLayout->setContentsMargins(0, 0, 0, 0);
    // ��imagePageWidget��ӵ�upLayout
    upLayout->addWidget(imagePageWidget);
    // ����һ����Ϊ"upWidget"��QWidget
    upWidget = new QWidget();
    // upWidget�ĳ�ʼ״̬Ϊ����
    upWidget->hide();
    // ��upWidget����upLaypout����
    upWidget->setLayout(upLayout);

    namesWidget = new NamesWidget();
    variableWidget = new VariableWidget();
    //�����NameWidget��ĳ�����"+"��ťʱ�������źŸ�SideWidget::tabAddSlot
    connect(namesWidget, &NamesWidget::tabAddSignal, this, &SideWidget::tabAddSlot);
    //�����NameWidget��ĳ�����"-"��ťʱ�������źŸ�SideWidget::tabRomoveSlot 
    connect(namesWidget, &NamesWidget::tabRemoveSignal, this, &SideWidget::tabRemoveSlot);
    //����NamesWidget���л�ѡ����ʱ��ImagePageWidget�����tabWidget��ѡ����
    connect(namesWidget, &NamesWidget::itemSelectedSignal, imagePageWidget, &ImagePageWidget::tabSelectedSlot);
    //����NamesWidget���л�ѡ����ʱ��VariableWidget��ѡ����    
    connect(namesWidget, &NamesWidget::itemSelectedSignal, variableWidget, &VariableWidget::nodeSelectedSlot);
	//
    connect(imagePageWidget, &ImagePageWidget::tabChangedSignal, namesWidget, &NamesWidget::itemChangedSlot);
	//
	connect(imagePageWidget, &ImagePageWidget::tabChangedSignal, variableWidget, &VariableWidget::nodeChangedSlot);
    //
    connect(variableWidget, &VariableWidget::nodeToggledSignal, imagePageWidget, &ImagePageWidget::tabToggledSlot);
    //
    connect(variableWidget, &VariableWidget::nodeToggledSignal, namesWidget, &NamesWidget::itemToggledSlot);
    
    
    // ����ˮƽ����splitter
	hSplitter = new QSplitter(Qt::Horizontal);
	// ����QSplitter�ָ��ߵ���ʽ��
    hSplitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
	// ��namesWidget��ӵ�hSplitter
    hSplitter->addWidget(namesWidget);
	// ��variableWidget��ӵ�hSplitter
    hSplitter->addWidget(variableWidget);
	// �����������
    hSplitter->setStretchFactor(0, 1);
    hSplitter->setStretchFactor(1, 1);
    bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(hSplitter);
    // ����bottomLayout��С�ؼ�֮��ľ���, �������ڱ߾�
    bottomLayout->setSpacing(0);
    // ����bottomLayout����߾�
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomWidget = new QWidget();
    bottomWidget->setLayout(bottomLayout);

	// ������ֱ����splitter
	vSplitter = new QSplitter(Qt::Vertical);
	// ����QSplitter�ָ��ߵ���ʽ��
	vSplitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
	// ��upWidget��ӵ�vSplitter
	vSplitter->addWidget(upWidget);
	// ��bottomWidget��ӵ�vSplitter
	vSplitter->addWidget(bottomWidget);
	// �����������
	vSplitter->setStretchFactor(0, 1);
	vSplitter->setStretchFactor(1, 1);
	rightLayout = new QVBoxLayout();
	rightLayout->addWidget(vSplitter);
	// ����rightLayout��С�ؼ�֮��ľ���, �������ڱ߾�
	rightLayout->setSpacing(0);
	// ����rightLayout����߾�
	rightLayout->setContentsMargins(0, 0, 0, 0);
	rightWidget = new QWidget();
	rightWidget->setLayout(rightLayout);

    // ����һ����ΪsideLayout��QHBoxLayout
    sideLayout = new QHBoxLayout();
    // ��btnWidget��ӵ�sideLayout
    sideLayout->addWidget(btnWidget);
    // ��rightWidget��ӵ�sideLayout
    sideLayout->addWidget(rightWidget);
    // ��sideLayout����Ϊ���Ҷ���
    sideLayout->setAlignment(Qt::AlignRight);
    // ����sideLayout��С�ؼ�֮��ľ���
    sideLayout->setSpacing(0);
    // ����sideLayout����߾�
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

    // ���ô����
	 // //  ����һ��ZoomImageLabel������ʾͼ��
    // ZoomImageLabel* imageLabel = new ZoomImageLabel();
	// ��CV::Matת��ΪQImage
	// QImage qImage = cvMatToQImage(image);
	// // ��QImageת��ΪQPixmap
	// QPixmap pixmap = QPixmap::fromImage(qImage);
	// // ��QLabel����ʾQPixmap
	// imageLabel->setPixmap(pixmap);
    // imageLabel->setScaledContents(true);
    // // ��ȡtabWidget
    // QTabWidget* tabWidget = imagePageWidget->getTabWidget();
	// // ��QWidget���Ϊ�µı�ǩҳ
	// int tabIndex = tabWidget->addTab(imageLabel, QString::fromStdString(filename));




    // ����һ��ZoomableGraphicsView������ʾͼ��
    ZoomableGraphicsView* imageShowWidget = new ZoomableGraphicsView();
    imageShowWidget->initLoad(image);
     // ��ȡ tabWidget
	QTabWidget* tabWidget = imagePageWidget->getTabWidget();
	// �� imageShowWidget ���Ϊ�µı�ǩҳ
    int tabIndex = tabWidget->addTab(imageShowWidget, QString::fromStdString(filename));

	// ʹ����ӵı�ǩҳ��Ϊ�ҳ
	tabWidget->setCurrentIndex(tabIndex);

    // ʹnamesWidget����һ�м�¼
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
	// ����tabWidget��������itemNameƥ���tab
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
	// ����tabWidget��������itemNameƥ���tab
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
