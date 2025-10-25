#include "btform.h"
#include "ui_btform.h"

#include <QRect>

BtForm::BtForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BtForm)
{
    ui->setupUi(this);
    ui->lineBox->hide();//隐藏

    //line1设置动画效果
    line1Animal = new QPropertyAnimation(ui->line1,"geometry",this);
    line1Animal->setDuration(1500);
    line1Animal->setKeyValueAt(0,QRect(0,25,2,0));
    line1Animal->setKeyValueAt(0.5,QRect(0,10,2,15));
    line1Animal->setKeyValueAt(1,QRect(0,25,2,0));
    line1Animal->setLoopCount(-1);
    line1Animal->start();

    //line2设置动画效果
    line2Animal = new QPropertyAnimation(ui->line2,"geometry",this);
    line2Animal->setDuration(1500);
    line2Animal->setKeyValueAt(0,QRect(9,25,2,1));
    line2Animal->setKeyValueAt(0.5,QRect(9,10,2,15));
    line2Animal->setKeyValueAt(1,QRect(9,25,2,1));
    line2Animal->setLoopCount(-1);
    line2Animal->start();

    //line3设置动画效果
    line3Animal = new QPropertyAnimation(ui->line3,"geometry",this);
    line3Animal->setDuration(1500);
    line3Animal->setKeyValueAt(0,QRect(17,25,2,1));
    line3Animal->setKeyValueAt(0.5,QRect(17,10,2,15));
    line3Animal->setKeyValueAt(1,QRect(17,25,2,1));
    line3Animal->setLoopCount(-1);
    line3Animal->start();

    //line4设置动画效果
    line4Animal = new QPropertyAnimation(ui->line4,"geometry",this);
    line4Animal->setDuration(1500);
    line4Animal->setKeyValueAt(0,QRect(25,25,2,1));
    line4Animal->setKeyValueAt(0.5,QRect(25,10,2,15));
    line4Animal->setKeyValueAt(1,QRect(25,25,2,1));
    line4Animal->setLoopCount(-1);
    line4Animal->start();
}

BtForm::~BtForm()
{
    delete ui;
}

void BtForm::setIconAndText(const QString &btIcon, const QString &btText, int pageId)
{
    //设置按钮的图标
    ui->btIcon->setPixmap(QPixmap(btIcon));
    //设置按钮的文本信息
    ui->btText->setText(btText);
    //将按钮和page页面进行关联
    this->pageId=pageId;
}

int BtForm::getPageId() const
{
    return pageId;
}

void BtForm::clearBackground()
{
    ui->btStyle->setStyleSheet("#btStyle:hover{background-color: #D8D8D8;}");
}

void BtForm::showAnimal(bool isShow)
{
    if(isShow)
    {
        ui->lineBox->show();
    }
    else
    {
        ui->lineBox->hide();
    }

}

void BtForm::mousePressEvent(QMouseEvent *event)
{
    (void)event;//告诉编译器不要触发敬告
    //1.当按钮按下之后，需要修改其背景颜色，文字变白色
    ui->btStyle->setStyleSheet("#btStyle{background-color: rgb(30,206,154);}*{color:#F6F6F6;}");

    //2.当按钮按下之后，需要切换bodyRight中page页面
    //切换时机；
    //完成页面切换
    //按钮和page页面的对应关系
    emit btClick(pageId);//发送鼠标点击信号
}
