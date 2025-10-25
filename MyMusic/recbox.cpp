#include "recbox.h"
#include "ui_recbox.h"
#include "recboxitem.h"
#include <QJsonObject>

RecBox::RecBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBox),
    row(1),
    column(4)
{
    ui->setupUi(this);

}

RecBox::~RecBox()
{
    delete ui;
}

void RecBox::initRecBoxUI(QJsonArray data, int row)
{
    //如果是两行，说明当前RecBox是主界面上的supplyMusicsBox
    if(row == 2)
    {
        this->row = row;
        column = 8;
    }
    else
    {
        //否则：只有一行，为主界面上recMusicBox
        ui->recListDown->hide();
    }

    imageList = data;//图片保存起来
    currentIndex = 0;
    count = imageList.size()/column;

    //在RecBox控件中添加RecBoxItem
    createRecBoxItem();
}

void RecBox::createRecBoxItem()
{
    //删除RecBox内部之前的元素
    QList<RecBoxItem*> recUpList = ui->recListUp->findChildren<RecBoxItem*>();
    for(auto e: recUpList)
    {
        ui->recListUpHLayout->removeWidget(e);
        delete e;
    }

    QList<RecBoxItem*> recDownList = ui->recListDown->findChildren<RecBoxItem*>();
    for(auto e : recDownList)
    {
        ui->recListDownHLayout->removeWidget(e);
        delete e;
    }

    //创建RecBoxItem对象,往RecBox中添加
    int index = 0;
    for(int i = currentIndex*column; i < column + currentIndex*column;++i)
    {
        RecBoxItem* item = new RecBoxItem();

        //设置音乐图片与对应文本
        QJsonObject obj = imageList[i].toObject();
        item->setRecText(obj.value("text").toString());
        item->setRecImage(obj.value("path").toString());

        if(index >= column/2 && row == 2)
        {
            ui->recListDownHLayout->addWidget(item);
        }
        else
        {
            ui->recListUpHLayout->addWidget(item);
        }
        index++;
    }
}

void RecBox::on_btUp_clicked()
{
    //点击之后，需要显示前一组图片，如果已经是第0组图片，让其显示最后一组
    currentIndex--;
    if(currentIndex < 0)
    {
        currentIndex = count-1;
    }
    createRecBoxItem();
}

void RecBox::on_btDown_clicked()
{
    //点击之后，需要显示下一组图片，如果已经是最后一组图片，则显示第0组
    currentIndex++;
    if(currentIndex >= count)
    {
        currentIndex = 0;
    }
    createRecBoxItem();
}
