#include "lrcpage.h"
#include "ui_lrcpage.h"
#include <QFile>
#include <QDebug>

LrcPage::LrcPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LrcPage)
{
    ui->setupUi(this);
    //设置窗口无标题栏
    setWindowFlag(Qt::FramelessWindowHint);

    animal = new QPropertyAnimation(this,"geometry",this);
    animal->setDuration(500);
    animal->setStartValue(QRect(10,10,width(),height()));
    animal->setEndValue(QRect(10,10+height(),width(),height()));

    //下拉按钮关联clicked
    connect(ui->hideBtn,&QPushButton::clicked,this,[=](){
        animal->start();
    });

    //当动画结束之后，窗口需要隐藏起来
    connect(animal,&QPropertyAnimation::finished,this,[=](){
        hide();
    });

    ui->hideBtn->setIcon(QIcon(":/images/xiala.png"));

    showLrcWordLine(-1);
}

LrcPage::~LrcPage()
{
    delete ui;
}

bool LrcPage::parseLrcFile(const QString &lrcFilePath)
{
    //1.打开文件
    QFile file(lrcFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"打开Lrc文件: "<<lrcFilePath<<"失败!!!";
        return false;
    }

    //将上一首歌的lrc歌词清空
    lrcWordLines.clear();

    //一次性读取一行lrc歌词
    while(!file.atEnd())
    {
        QString lrclineWord = file.readLine(1024);

        //解析歌词
        int start = 0, end = 0;
        end = lrclineWord.indexOf(']',start);
        QString lrcTime = lrclineWord.mid(start, end-start+1);
        QString lrcWord = lrclineWord.mid(end+1,lrclineWord.size()-end-1-1);

        //2.在时间中解析出分:秒.毫秒
        //解析分：
        qint64 lineTime = 0;
        start = 1;
        end = lrcTime.indexOf(':',start);
        lineTime += lrcTime.mid(start,end-start).toInt()*60*1000;//解析分并将其转化为毫秒

        //解析秒
        start = end+1;
        end = lrcTime.indexOf('.',start);
        lineTime += lrcTime.mid(start,end-start).toInt()*1000;//解析秒并将其转化为毫秒

        //解析毫秒
        start = end+1;
        end = lrcTime.indexOf('.',start);
        lineTime += lrcTime.mid(start,end-start).toInt();//解析毫秒

        //3.将该行次保存
        lrcWordLines.push_back(LrcWordLine(lineTime,lrcWord));
    }

    //测试
//    for(auto e : lrcWordLines)
//    {
//        qDebug()<<e.lrcTime<<":"<<e.lrcText;
//    }

    return true;
}

void LrcPage::showLrcWordLine(qint64 time)
{
    //1.根据当前所播放歌曲的时间来获取歌曲在QVector中的索引
    int index = getLrcWordLineIndex(time);
    //2.更新当前行的前三行 当前播放行  后三行
    if(index == -1)
    {
        ui->line1->setText("");
        ui->line2->setText("");
        ui->line3->setText("");
        ui->lineCenter->setText("当前歌曲暂无歌词");
        ui->line4->setText("");
        ui->line5->setText("");
        ui->line6->setText("");
    }
    else
    {
        ui->line1->setText(getLrcWordByIndex(index-3));
        ui->line2->setText(getLrcWordByIndex(index-2));
        ui->line3->setText(getLrcWordByIndex(index-1));
        ui->lineCenter->setText(getLrcWordByIndex(index));
        ui->line4->setText(getLrcWordByIndex(index+1));
        ui->line5->setText(getLrcWordByIndex(index+2));
        ui->line6->setText(getLrcWordByIndex(index+3));
    }
}

int LrcPage::getLrcWordLineIndex(qint64 time)
{
    //将time和QVector中保存的LrcWordLine中的time进行对比

    //当前歌曲没有lrc歌词文件
    if(lrcWordLines.isEmpty())
    {
        return -1;
    }

    if(time <= lrcWordLines[0].lrcTime)
    {
        return 0;
    }

    //对比
    for(int i = 1; i < lrcWordLines.size();++i)
    {
        if(time > lrcWordLines[i-1].lrcTime && time < lrcWordLines[i].lrcTime)
        {
            //第i-1行还没有播放完
            return i-1;
        }
    }
    //到最后一行播放完，歌曲结束，但是还有收尾的音乐
    //让歌词界面显示最后一行歌词
    return lrcWordLines.size()-1;
}

QString LrcPage::getLrcWordByIndex(int index)
{
    if(index < 0 || index >= lrcWordLines.size())
    {
        return "";
    }

    return lrcWordLines[index].lrcText;
}
