#include "volumetool.h"
#include "ui_volumetool.h"

#include <QPainter>
#include <QPoint>
#include <QGraphicsDropShadowEffect>

VolumeTool::VolumeTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeTool),
    isMuted(false),
    volumeRatio(20)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup|Qt::FramelessWindowHint|Qt::NoDropShadowWindowHint);
    //在windows上，设置透明效果后，窗口需要加上Qt::FramelessWindowHint格式，否则没有控件位置的背景是黑色的；
    //由于默认窗口有阴影，因此还需要将窗口的原有阴影去掉，窗口需要加上Qt::NoDropShadowWindowHint
    setAttribute(Qt::WA_TranslucentBackground);

    //窗口增加自定义阴影效果
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0,0);
    shadowEffect->setColor("#646464");
    shadowEffect->setBlurRadius(10);
    setGraphicsEffect(shadowEffect);

    //给按钮设置图标
    ui->silenceBtn->setIcon(QIcon(":/images/volumn.png"));
    //音量大小，默认是20
    ui->volumeRadio->setText("20%");

    //设置out_line尺寸
    QRect rect = ui->outLine->geometry();
    ui->outLine->setGeometry(rect.x(),180-36+25,rect.width(),36);
    //移动按钮位置
    ui->sliderBtn->move(ui->sliderBtn->x(),ui->outLine->y()-ui->sliderBtn->height()/2);


    connect(ui->silenceBtn,&QPushButton::clicked,this,&VolumeTool::onSilenceBtnClicked);
    ui->volumeBox->installEventFilter(this);
}

VolumeTool::~VolumeTool()
{
    delete ui;
}

void VolumeTool::paintEvent(QPaintEvent *event)
{
    (void)event;

    //绘制volumeTool界面的三角形
    QPainter painter(this);
    //1.设置画笔
    painter.setPen(Qt::NoPen);//没有画笔时，画出来的图形就没有边框和轮廓线
    //2.设置画刷
    painter.setBrush(QBrush(Qt::white));
    //3.绘制三角形
    QPolygon polygon;
    QPoint a(10,300);
    QPoint b(10+80,300);
    QPoint c(10+40,300+20);
    polygon.append(a);
    polygon.append(b);
    polygon.append(c);
    painter.drawPolygon(polygon);
}

bool VolumeTool::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->volumeBox == watched)
    {
        //事件发生在volumeBox控件上
        if(event->type() == QEvent::MouseButtonPress)
        {
            //鼠标按下事件
            calcVolume();
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            //鼠标释放事件
            emit setMusicVolume(volumeRatio);
        }
        else if(event->type() == QEvent::MouseMove)
        {
            //鼠标移动事件
            calcVolume();
            emit setMusicVolume(volumeRatio);
        }
        return true;
    }
    return QObject::eventFilter(watched,event);
}

void VolumeTool::calcVolume()
{
    //获取鼠标点击时的y坐标
    int height = ui->volumeBox->mapFromGlobal(QCursor().pos()).y();

    //验证height的合法性：[25,205]
    height = height < 25?25:height;
    height = height > 205?205:height;

    //更新outLine大小
    ui->outLine->setGeometry(ui->outLine->x(),height,ui->outLine->width(),205-height);

    //更新sliderBtn的位置
    //sliderBtn实际也是矩形，设置了圆角效果之后，呈现出来是圆
    //移动sliderBtn实际就是重新找其矩形左上角的位置，左上角位置就是outLine左上角
    ui->sliderBtn->move(ui->sliderBtn->x(),ui->outLine->y() - ui->sliderBtn->height()/2);

    //计算音量大小
    volumeRatio = (int)((ui->outLine->height()/(float)180) * 100);

    //更新音量百分比
    ui->volumeRadio->setText(QString::number(volumeRatio) + "%");
}

void VolumeTool::onSilenceBtnClicked()
{
    isMuted = !isMuted;

    //媒体静音设置，发射信号让MyMusic处理
    //静音图标切换
    if(isMuted)
    {
        ui->silenceBtn->setIcon(QIcon(":/images/silent.png"));
    }
    else
    {
        ui->silenceBtn->setIcon(QIcon(":/images/volumn.png"));
    }

    emit setMusicMuted(isMuted);
}
