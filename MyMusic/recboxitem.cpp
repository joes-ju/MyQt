#include "recboxitem.h"
#include "ui_recboxitem.h"

RecBoxItem::RecBoxItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBoxItem)
{
    ui->setupUi(this);
    //注意：安装事件拦截器，否则时间拦截不到；拦截事件处理器时，一定要安装事件拦截器
    ui->musicImageBox->installEventFilter(this);
}

RecBoxItem::~RecBoxItem()
{
    delete ui;
}

void RecBoxItem::setRecText(const QString &text)
{
    ui->recBoxItemText->setText(text);
}

void RecBoxItem::setRecImage(const QString &imagePath)
{
    QString style = "background-image: url("+imagePath+");";
    ui->recMusicImage->setStyleSheet(style);
}

bool RecBoxItem::eventFilter(QObject *watched, QEvent *event)
{
    //注意：recItem上有一个按钮，当鼠标放在这个按钮上就开启动画
    if(watched == ui->musicImageBox)
    {
        //拦截鼠标进入事件
        if(QEvent::Enter == event->type())
        {
            //添加图片上移动画
            QPropertyAnimation* animation = new QPropertyAnimation(ui->musicImageBox,"geometry");
            animation->setDuration(150);
            animation->setStartValue(QRect(9,9,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->setEndValue(QRect(9,0,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->start();

            //注意：动画结束的时候会触发finished信号，拦截到该信号，销毁animation
            connect(animation,&QPropertyAnimation::finished,this,[=](){
                delete animation;
            });
        }
        //拦截鼠标离开事件
        else if(QEvent::Leave == event->type())
        {
            //添加图标下移动画
            QPropertyAnimation* animation = new QPropertyAnimation(ui->musicImageBox,"geometry");
            animation->setDuration(150);
            animation->setStartValue(QRect(9,0,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->setEndValue(QRect(9,9,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->start();

            connect(animation,&QPropertyAnimation::finished,this,[=](){
                delete animation;
            });
        }
        return true;
    }
    return QObject::eventFilter(watched,event);
}
