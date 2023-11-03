#include "MainWindow/SideWidget.h"

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
    connect(foldButton, &QPushButton::clicked, this, &SideWidget::on_button1_clicked);
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


    // 显示在foldWidget的buttonS1
    buttonS1 = new QPushButton("buttonS1");
    // 显示在foldWidget的buttonS2
    buttonS2 = new QPushButton("buttonS2");
    // 创建foldWidget, 这是一个可以被折叠或展开的widget, 用于显示其它重要内容
    foldWidget = new QWidget();
    // foldWidget的初始状态为隐藏
    foldWidget->hide();
    // 创建一个名为foldLayout为QHBoxLayout
    foldLayout = new QHBoxLayout();
    // 设置foldLayout中小控件之间的距离
    foldLayout->setSpacing(0);
    // 设置foldLayout的外边距
    foldLayout->setContentsMargins(0, 0, 0, 0);
    // 将buttonS1添加到foldLayout
    foldLayout->addWidget(buttonS1);
    // 将buttonS2添加到foldLayout
    foldLayout->addWidget(buttonS2);
    /*
    foldWidget->setStyleSheet("padding: 0px;");是一个样式表(CSS, Cascading Style Sheets)的设置，
    它指定了小控件foldWidget的内边距。
    内边距是小控件内容与小控件边界之间的空间。
    将内边距设置为"0px"是为了确保按钮小控件的内容紧贴在边界上。
    */
    foldWidget->setStyleSheet("padding: 0px;");
    //将foldLayout设置为foldWidget的布局(QHBoxLayout布局)
    foldWidget->setLayout(foldLayout);
    // 设置foldButton的大小策略
    foldButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // 设置btnWidget的大小策略
    btnWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // 创建一个名为sideLayout的QHBoxLayout
    sideLayout = new QHBoxLayout();
    // 将btnWidget添加到sideLayout
    sideLayout->addWidget(btnWidget);
    // 将foldWidget添加到sideLayout
    sideLayout->addWidget(foldWidget);
    // 将sideLayout设置为向右对齐
    sideLayout->setAlignment(Qt::AlignRight);
    // 设置sideLayout中小控件之间的距离
    sideLayout->setSpacing(0);
    // 设置sideLayout的外边距
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
        foldLayout->setContentsMargins(0, 0, 0, 0); // 设置foldLayout的外边距
        foldWidget->hide();
        foldButton->setText("Show\n<<");
        unfold = true;
        qDebug() << "foldWidget->isVisible() " << foldWidget->isVisible();
    }
}
