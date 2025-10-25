#include "commonpage.h"
#include "ui_commonpage.h"
#include "listitembox.h"

CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage)
{
    ui->setupUi(this);
    ui->pageMusicList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(ui->playAllBtn,&QPushButton::clicked,this,[=](){
        //播放所有按钮点击之后，通知MyMusic播放当前PageType标记的page页面中的所有歌曲
        emit playAll(pageType);
    });

    connect(ui->pageMusicList,&QListWidget::doubleClicked,this,[=](const QModelIndex& index){
        //鼠标双击后，发射信号告诉MyMusic；播放this页面中被双击的歌曲
        emit playMusicByIndex(this,index.row());
    });
}

CommonPage::~CommonPage()
{
    delete ui;
}

void CommonPage::setMusicListType(PageType pageType)
{
    this->pageType = pageType;
}

void CommonPage::setCommonPageUI(const QString &text, const QString &imagePath)
{
    //设置标题
    ui->pageTitle->setText(text);

    //设置封面
    ui->musicImageLabel->setPixmap(QPixmap(imagePath));

    ui->musicImageLabel->setScaledContents(true);

    //测试
//    ListItemBox* listItemBox = new ListItemBox(this);
//    QListWidgetItem* item = new QListWidgetItem(ui->pageMusicList);
//    item->setSizeHint(QSize(ui->pageMusicList->width(),45));
    //    ui->pageMusicList->setItemWidget(item,listItemBox);
}

void CommonPage::addMusicToMusicPage(MusicList &musicList)
{
    //musicOfPage和界面显示无关
    musicOfPage.clear();

    for(auto music:musicList)
    {
        switch (pageType)
        {
        case LIKE_PAGE:
            if(music.getIsLike())
            {
                musicOfPage.push_back(music.getMusicId());
            }
            break;
        case LOCAL_PAGE:
            musicOfPage.push_back(music.getMusicId());
            break;
        case HISTORY_PAGE:
            if(music.getIsHistory())
            {
                musicOfPage.push_back(music.getMusicId());
            }
            break;
        default:
            qDebug()<<"暂不支持";
        }
    }
}

//该方法负责将歌曲信息更新到界面
void CommonPage::reFresh(MusicList &musicList)
{
    //将QListWidget之前已经添加的内容清空
    ui->pageMusicList->clear();

    //添加新的歌曲
    addMusicToMusicPage(musicList);
    //遍历歌单，将歌单中的歌曲显示到界面
    for(auto musicId:musicOfPage)
    {
        auto it = musicList.findMusicByMusicId(musicId);
        if(it == musicList.end())
        {
            continue;
        }

        //将Music的歌曲名称、作者、专辑名称更新到界面
        ListItemBox* listItemBox = new ListItemBox(this);
        listItemBox->setMusicName(it->getMusicName());
        listItemBox->setMusicSinger(it->getMusicSinger());
        listItemBox->setMusicAlbum(it->getMusicAlbum());
        listItemBox->setLikeMusic(it->getIsLike());

        QListWidgetItem* item = new QListWidgetItem(ui->pageMusicList);
        item->setSizeHint(QSize(listItemBox->width(),listItemBox->height()));
        ui->pageMusicList->setItemWidget(item,listItemBox);
        connect(listItemBox,&ListItemBox::setIsLike,this,[=](bool isLike){
            //更新歌曲状态
            //需要通知MyMusic，让其中的likePage localPage recentPage更新页面歌曲信息
            emit updateLikeMusic(isLike,it->getMusicId());
        });
    }

    //触发窗口重绘paintEvent
    //update
    repaint();//立刻响应paintEvent事件
}

void CommonPage::addMusicToPlaylist(MusicList &musicList, QMediaPlaylist *playList)
{
    for(auto music:musicList)
    {
        switch(pageType)
        {
        case LIKE_PAGE:
            if(music.getIsLike())
            {
                playList->addMedia(music.getMusicUrl());
            }
            break;
        case LOCAL_PAGE:
            playList->addMedia(music.getMusicUrl());
            break;
        case HISTORY_PAGE:
            if(music.getIsHistory())
            {
                playList->addMedia(music.getMusicUrl());
            }
            break;
        default:
            qDebug()<<"未支持页面";
        }
    }
}

void CommonPage::setMusicImage(QPixmap pixmap)
{
    ui->musicImageLabel->setPixmap(pixmap);
    ui->musicImageLabel->setScaledContents(true);
}

QString CommonPage::getMusicIdByIndex(int index)
{
    if(index >= musicOfPage.size())
    {
        qDebug()<<"无此音乐";
        return "";
    }
    return musicOfPage[index];
}
