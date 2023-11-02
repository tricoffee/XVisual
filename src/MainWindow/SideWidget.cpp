#include "MainWindow/SideWidget.h"

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
    connect(foldButton, &QPushButton::clicked, this, &SideWidget::on_button1_clicked);
    //��foldButton��ӵ�btnLayout����
    btnLayout = new QHBoxLayout();
    // �ƶ�foldButton���Ҳ�
    btnLayout->addStretch();
    // ʹ�� Qt::AlignRight ���Ҷ��� foldButton
    btnLayout->addWidget(foldButton, 0, Qt::AlignRight);
    //btnLayout->addWidget(foldButton);
    // ����btnLayout��С�ؼ�֮��ľ���
    btnLayout->setSpacing(0);
    // ����foldLayout����߾�
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
    // ��ȡ��ť�ı��Ŀ��ΪtextWidth
    QFontMetrics fontMetrics(foldButton->font());
    int textWidth = fontMetrics.horizontalAdvance(foldButton->text());
    // ����btnWidget�������Ϊ��ť�ı��Ŀ��+35
    btnWidget->setMaximumWidth(textWidth + 35);
    // ��ʾ��foldWidget��buttonS1
    buttonS1 = new QPushButton("buttonS1");
    // ��ʾ��foldWidget��buttonS2
    buttonS2 = new QPushButton("buttonS2");
    // ����foldWidget, ����һ�����Ա��۵���չ����widget, ������ʾ������Ҫ����
    foldWidget = new QWidget();
    // ��ʼʱ����foldWidget
    foldWidget->hide();
    foldLayout = new QHBoxLayout();
    // ����foldLayout��С�ؼ�֮��ľ���
    foldLayout->setSpacing(0);
    // ����foldLayout����߾�
    foldLayout->setContentsMargins(0, 0, 0, 0);
    // ��buttonS1��ӵ�foldLayout
    foldLayout->addWidget(buttonS1);
    // ��buttonS2��ӵ�foldLayout
    foldLayout->addWidget(buttonS2);
    /*
    foldWidget->setStyleSheet("padding: 0px;");��һ����ʽ��(CSS, Cascading Style Sheets)�����ã�
    ��ָ����С�ؼ�foldWidget���ڱ߾ࡣ
    �ڱ߾���С�ؼ�������С�ؼ��߽�֮��Ŀռ䡣
    ���ڱ߾�����Ϊ"0px"��Ϊ��ȷ����ťС�ؼ������ݽ����ڱ߽��ϡ�
    */
    foldWidget->setStyleSheet("padding: 0px;");
    //��foldLayout����ΪfoldWidget�Ĳ���, QHBoxLayout����
    foldWidget->setLayout(foldLayout);
    //foldWidget�ĳ�ʼ״̬Ϊ����
    foldWidget->hide();
    // ����foldButton�Ĵ�С����
    foldButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // ����btnWidget�Ĵ�С����
    btnWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // ����һ����ΪsideLayout��QHBoxLayout
    sideLayout = new QHBoxLayout();
    // ��btnWidget��ӵ�sideLayout
    sideLayout->addWidget(btnWidget);
    // ��foldWidget��ӵ�sideLayout
    sideLayout->addWidget(foldWidget);
    // ��sideLayout����Ϊ���Ҷ���
    sideLayout->setAlignment(Qt::AlignRight);
    // ����sideLayout��С�ؼ�֮��ľ���
    sideLayout->setSpacing(0);
    // ����sideLayout����߾�
    sideLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(sideLayout);
}

void SideWidget::on_button1_clicked()
{
    qDebug() << "void SideWidget::on_button1_clicked() ";
    if (unfold)
    {
        foldWidget->setStyleSheet("background-color: lightgreen");
        foldWidget->show();
        foldButton->setText("Hide\n>>");
        unfold = false;
        qDebug() << "foldWidget->isVisible() " << foldWidget->isVisible();
    }
    else
    {
        foldLayout->setContentsMargins(0, 0, 0, 0); // ����foldLayout����߾�
        foldWidget->hide();
        foldButton->setText("Show\n<<");
        unfold = true;
        qDebug() << "foldWidget->isVisible() " << foldWidget->isVisible();
    }
}
