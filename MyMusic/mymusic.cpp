#include "mymusic.h"
#include "ui_mymusic.h"
#include "recbox.h"
#include "recboxitem.h"
#include "lrcpage.h"

#include <QIcon>
#include <QGraphicsDropShadowEffect>
#include <QJsonObject>
#include <QJsonArray>
#include <QChar>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>

MyMusic::MyMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyMusic)
    , currentIndex(-1)
{
    ui->setupUi(this);

    initUi();
    this->setMouseTracking(true);//设置鼠标追踪
    initSqlite();
    initPlayer();
    initMusicList();

    connectSignalAndSlots();
}

MyMusic::~MyMusic()
{
    delete ui;
}

void MyMusic::initUi()
{
    //设置没有标题栏
    this->setWindowFlag(Qt::FramelessWindowHint,true);
    //设置窗口背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);
    //设置窗口图标
    this->setWindowIcon(QIcon(":/images/yyyuyin.png"));
    //将最大化按钮禁止掉
    ui->max->setEnabled(false);

    //添加系统托盘
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/yyyuyin.png"));
    trayIcon->show();//通过菜单让系统托盘退出


    //给托盘添加菜单
    QMenu* trayMenu = new QMenu();
    trayMenu->addAction("显示",this,&QWidget::showNormal);
    trayMenu->addAction("退出",this,&MyMusic::onMyMusicQuit);
    trayIcon->setContextMenu(trayMenu);

    //给窗口设置阴影效果
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0,0);//设置阴影偏移
    shadowEffect->setColor("#000000");//设置阴影颜色为--黑色
    shadowEffect->setBlurRadius(10);//设置阴影的模糊半径
    this->setGraphicsEffect(shadowEffect);

    //设置BtForm图标&文本信息
    ui->Rec->setIconAndText(":/images/rec.png","推荐",0);
    ui->audio->setIconAndText(":/images/radio.png","电台",1);
    ui->music->setIconAndText(":/images/music.png","音乐馆",2);
    ui->like->setIconAndText(":/images/like.png","我喜欢",3);
    ui->local->setIconAndText(":/images/local.png","本地下载",4);
    ui->recent->setIconAndText(":/images/recent.png","最近播放",5);

    //将localPage设置为当前页面
    ui->stackedWidget->setCurrentIndex(4);
    currentPage = ui->localPage;

    //让本地下载的BtForm动画默认显示
    ui->local->showAnimal(true);

    //初始化推荐页面，设置随机数的种子
    srand(time(NULL));
    ui->recMusicBox->initRecBoxUI(randomPicture(),1);
    ui->supportMusicBox->initRecBoxUI(randomPicture(),2);

    //设置commonPage的信息
//    ui->likePage->setMusicListType(PageType::LIKE_PAGE);
    ui->likePage->setCommonPageUI("我喜欢",":/images/ilikebg.png");

//    ui->localPage->setMusicListType(PageType::LOCAL_PAGE);
    ui->localPage->setCommonPageUI("本地音乐",":/images/yyyuyin.png");

//    ui->recentPage->setMusicListType(PageType::HISTORY_PAGE);
    ui->recentPage->setCommonPageUI("最近播放",":/images/recentbg.png");

    //播放控制区按钮图标设定
    ui->play->setIcon(QIcon(":/images/play_2.png"));
    ui->playMode->setIcon(QIcon(":/images/shuffle_2.png"));

    //创建音量调节窗口对象并挂在对象树
    volumeTool = new VolumeTool(this);


    //实例化LrcWord对象
    lrcPage = new LrcPage(this);
    lrcPage->setGeometry(10,10,lrcPage->width(),lrcPage->height());
    lrcPage->hide();
    //初始化上移动画对象
    lrcPageAnimal = new QPropertyAnimation(lrcPage,"geometry",this);
    lrcPageAnimal->setDuration(500);
    lrcPageAnimal->setStartValue(QRect(10,10+lrcPage->height(),lrcPage->width(),lrcPage->height()));
    lrcPageAnimal->setEndValue(QRect(10,10,lrcPage->width(),lrcPage->height()));
}

void MyMusic::initSqlite()
{
    //1.进行数据库驱动加载
    sqlite = QSqlDatabase::addDatabase("QSQLITE");

    //2.设置数据库名称
    sqlite.setDatabaseName("MyMusic.db");

    //3.打开
    if(!sqlite.open())
    {
        QMessageBox::critical(this,"MyMusic","数据库打开失败!!!");
        return;
    }
    qDebug()<<"MyMusic数据库连接成功";

    //4.创建表
    QString sql = "CREATE TABLE IF NOT EXISTS MusicInfo(\
                          id INTEGER PRIMARY KEY AUTOINCREMENT,\
                          musicId varchar(50) UNIQUE,\
                          musicName varchar(50),\
                          musicSinger varchar(50), \
                          albumName varchar(50),\
                          musicUrl varchar(256),\
                          duration BIGINT,\
                          isLike INTEGER,\
                          isHistory INTEGER)";
    QSqlQuery query;
    if(!query.exec(sql))
    {
        QMessageBox::critical(this,"MuMusic","初始化错误");
        return;
    }

    qDebug()<<"MyMusic表创建成功!!!";
}

void MyMusic::initMusicList()
{
    //从数据库读取歌曲信息
    musicList.readFromDB();

    //更新CommonPage页面
    //设置CommonPage的信息
    ui->likePage->setMusicListType(PageType::LIKE_PAGE);
    ui->likePage->reFresh(musicList);

    ui->localPage->setMusicListType(PageType::LOCAL_PAGE);
    ui->localPage->reFresh(musicList);

    //将localPage中的歌曲添加道媒体播放列表中
    ui->localPage->addMusicToPlaylist(musicList,playList);

    ui->recentPage->setMusicListType(PageType::HISTORY_PAGE);
    ui->recentPage->reFresh(musicList);
}

void MyMusic::initPlayer()
{
    //1.初始化媒体播相关类对象
    player = new QMediaPlayer(this);
    playList = new QMediaPlaylist(this);

    //2.设置默认播放模式
    playList->setPlaybackMode(QMediaPlaylist::Random);

    //3.将播放列表设置到播放媒体对象中
    player->setPlaylist(playList);

    //4.设置默认的音量大小，默认设置成20%
    player->setVolume(20);

    //关联QMediaPlayer信号
    connect(player,&QMediaPlayer::stateChanged,this,&MyMusic::onPlayerStateChanged);
    //关联QMediaPlayer::durationChanged信号
    connect(player,&QMediaPlayer::durationChanged,this,&MyMusic::onDurationChanged);
    //关联QMediaPlayer::positionChanged信号
    connect(player,&QMediaPlayer::positionChanged,this,&MyMusic::onPositionChanged);
    //关联QMediaPlayer::onMetaDataAvailbaleChanged
    connect(player,&QMediaPlayer::metaDataAvailableChanged,this,&MyMusic::onMetaDataAvailableChanged);

    //播放列表的模式发生改变时的信号槽关联
    connect(playList,&QMediaPlaylist::playbackModeChanged,this,&MyMusic::onPlaybackModeChanged);
    //当playlist中播放源发生变化时
    connect(playList,&QMediaPlaylist::currentIndexChanged,this,&MyMusic::onCurrentIndexChanged);
}

//设置随机图片【歌曲的图片】
QJsonArray MyMusic::randomPicture()
{
    //推荐文本 + 推荐图片路径
    QVector<QString> vecImageName;
    vecImageName<<"001.png"<<"003.png"<<"004.png"<<"005.png"<<"006.png"<<"007.png"<<"008.png"<<"009.png"<<"010.png"
                <<"011.png"<<"012.png"<<"013.png"<<"014.png"<<"015.png"<<"016.png"<<"017.png"<<"018.png"<<"019.png"
                <<"020.png"<<"021.png"<<"022.png"<<"023.png"<<"024.png"<<"025.png"<<"026.png"<<"027.png"<<"028.png"
                <<"029.png"<<"030.png"<<"031.png"<<"032.png"<<"033.png"<<"034.png"<<"035.png"<<"036.png"<<"037.png"
                <<"038.png"<<"039.png"<<"040.png";
    std::random_shuffle(vecImageName.begin(),vecImageName.end());

    QJsonArray objArray;
    for(int i = 0; i < vecImageName.size(); i++)
    {
        QJsonObject obj;
        obj.insert("path",":/images/rec/"+vecImageName[i]);

        QString strText = QString("推荐-%1").arg(i,3,10,QChar('0'));
        obj.insert("text",strText);
        objArray.append(obj);
    }
    return objArray;
}

void MyMusic::connectSignalAndSlots()
{
    //关联BtForm的信号和处理该信号的槽函数
    connect(ui->Rec,&BtForm::btClick,this,&MyMusic::onBtFormClick);
    connect(ui->audio,&BtForm::btClick,this,&MyMusic::onBtFormClick);
    connect(ui->music,&BtForm::btClick,this,&MyMusic::onBtFormClick);
    connect(ui->like,&BtForm::btClick,this,&MyMusic::onBtFormClick);
    connect(ui->local,&BtForm::btClick,this,&MyMusic::onBtFormClick);
    connect(ui->recent,&BtForm::btClick,this,&MyMusic::onBtFormClick);

    //收藏或不收藏处理
    connect(ui->likePage,&CommonPage::updateLikeMusic,this,&MyMusic::updateLikeMusicAndPage);
    connect(ui->localPage,&CommonPage::updateLikeMusic,this,&MyMusic::updateLikeMusicAndPage);
    connect(ui->recentPage,&CommonPage::updateLikeMusic,this,&MyMusic::updateLikeMusicAndPage);

    //播放控制区的信号和槽函数关联
    connect(ui->play,&QPushButton::clicked,this,&MyMusic::onPlayMusic);
    connect(ui->playUp,&QPushButton::clicked,this,&MyMusic::onPlayUpClicked);
    connect(ui->playDown,&QPushButton::clicked,this,&MyMusic::onPlayDownClicked);
    connect(ui->playMode,&QPushButton::clicked,this,&MyMusic::onPlaybackModeClicked);

    //likePage localPage recentPage都有播放所有按钮
    connect(ui->likePage,&CommonPage::playAll,this,&MyMusic::onPlayAll);
    connect(ui->localPage,&CommonPage::playAll,this,&MyMusic::onPlayAll);
    connect(ui->recentPage,&CommonPage::playAll,this,&MyMusic::onPlayAll);

    //处理likePage localPage recentPage中ListItemBox双击
    connect(ui->likePage,&CommonPage::playMusicByIndex,this,&MyMusic::playMusicByIndex);
    connect(ui->localPage,&CommonPage::playMusicByIndex,this,&MyMusic::playMusicByIndex);
    connect(ui->recentPage,&CommonPage::playMusicByIndex,this,&MyMusic::playMusicByIndex);

    //显示歌词窗口
    connect(ui->lrcWord,&QPushButton::clicked,this,&MyMusic::onLrcWordClicked);

    //处理静音
    connect(volumeTool,&VolumeTool::setMusicMuted,this,&MyMusic::setPlayerMuted);

    //处理音量的大小
    connect(volumeTool,&VolumeTool::setMusicVolume,this,&MyMusic::setPlayerVolume);

    //MusicSlider::setMusicSliderPosition
    connect(ui->processBar,&MusicSlider::setMusicSliderPosition,this,&MyMusic::onMusicSliderChanged);
}

void MyMusic::updateBtnformAnimal()
{
    //获取currentPage在stackWidget中的索引
    int index = ui->stackedWidget->indexOf(currentPage);
    if(-1 == index)
    {
        qDebug()<<"该页面不存在";
        return;
    }

    //获取MyMusic界面上所有的btForm
    //findChildren()方法用于在当前窗口部件的子树中，查找符合指定类型和名称的所有子部件，返回一个包含这些子部件的列表
    QList<BtForm*> btForms = this->findChildren<BtForm*>();
    for(auto btForm:btForms)
    {
        if(btForm->getPageId() == index)
        {
            //将currentPage对应的btForm找到了
            btForm->showAnimal(true);
        }
        else
        {
            btForm->showAnimal(false);
        }
    }
}

void MyMusic::onMyMusicQuit()
{
    //歌曲写入数据库
    musicList.writeToDB();

    //断开与SQLite的连接
    sqlite.close();

    //关闭窗口
    close();
}

void MyMusic::mouseMoveEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->buttons() && isDrag)
    {
        move(event->globalPos()-dragPossition);
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void MyMusic::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isDrag = true;
        //获取鼠标相对于电脑屏幕左上角的全局坐标
        dragPossition = event->globalPos()-geometry().topLeft();
        return;
    }
    QWidget::mousePressEvent(event);
}


void MyMusic::on_quit_clicked()
{ 
    hide();
    //真正的退出由系统托盘退出
}

void MyMusic::onBtFormClick(int pageId)
{
    //1.获取当前页面所有btForm按钮类型的对象
    QList<BtForm*> btFormList = this->findChildren<BtForm*>();

    //2.遍历所有对象，如果不是当前id的按钮，则把之前设置的背景颜色清除掉
    for(auto btForm:btFormList)
    {
        if(btForm->getPageId() != pageId)
        {
            btForm->clearBackground();
        }
    }

    //3.设置当前栈空间显示页面
    ui->stackedWidget->setCurrentIndex(pageId);
    qDebug()<<"切换页面"<<pageId;
    isDrag = false;
}

void MyMusic::updateLikeMusicAndPage(bool isLike, const QString &musicId)
{
    //1.修改状态
    auto it = musicList.findMusicByMusicId(musicId);
    if(it != musicList.end())
    {
        it->setIsLike(isLike);
    }
    //2.更新Page页面的歌曲列表
    ui->likePage->reFresh(musicList);
    ui->localPage->reFresh(musicList);
    ui->recentPage->reFresh(musicList);
}

void MyMusic::on_volume_clicked()
{
    //1.获取该按钮在左上角的图标
    QPoint point = ui->volume->mapToGlobal(QPoint(0,0));//获取ui->volume控件的left-top坐标，并转换为基于屏幕的全局坐标
    //2.计算volume窗口的左上角位置
    //即，让该窗口显示在鼠标点击的正上方
    QPoint volumeLeftTop = point - QPoint(volumeTool->width()/2, volumeTool->height());

    //微调窗口位置
    volumeLeftTop.setY(volumeLeftTop.y()+30);
    volumeLeftTop.setX(volumeLeftTop.x()+15);

    //3.移动窗口
    volumeTool->move(volumeLeftTop);

    //4.将窗口显示出来
    volumeTool->show();
}

void MyMusic::on_addLocal_clicked()
{
    //1.创建一个文件对话框
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle("添加本地音乐");

    //设置文件对话框默认打开的路径
    QDir dir(QDir::current());
    dir.cdUp();
    QString projectPath = dir.path();
    projectPath += "/MyMusic/musics";
    fileDialog.setDirectory(projectPath);

    //2.创建一个打开格式的文件对话框

    //3.设置一次打开多个文件
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    //4.通过文件的MIME类型来过滤
    QStringList mimeTypeFilters;
    mimeTypeFilters<<"application/octet-stream";
    fileDialog.setMimeTypeFilters(mimeTypeFilters);

    if(QDialog::Accepted == fileDialog.exec())
    {
        //获取选中的文件 --> 当点击打开后，将指定目录下所选择的文件的路径拿到
        QList<QUrl> fileUrls = fileDialog.selectedUrls();
        //fileUrls：内部存放的是刚刚选中的文件的url路径
        //需要将文件信息填充到本地下载
        //将所有音乐添加到音乐列表中，MusicList
        //musicList中管理的是解析之后的Music对象
        musicList.addMusicsByUrl(fileUrls);//将所有拿到的歌曲文件添加到歌曲列表

        ui->stackedWidget->setCurrentIndex(4);
        ui->localPage->reFresh(musicList);
        //将localPage中的歌曲添加到媒体播放器列表中
        ui->localPage->addMusicToPlaylist(musicList,playList);
    }

}

//显示窗口并开启动画
void MyMusic::onLrcWordClicked()
{
    lrcPage->show();
    lrcPageAnimal->start();
}

//播放和暂停
void MyMusic::onPlayMusic()
{
    if(QMediaPlayer::PlayingState == player->state())
    {
        //播放中点击，应该暂停
        player->pause();
//        ui->play->setIcon(QIcon(":/images/play3.png"));
    }
    else if(QMediaPlayer::PausedState == player->state())
    {
        //暂停状态中点击按钮，应该继续播放
        player->play();
//        ui->play->setIcon(QIcon(":/images/play_on.png"));
    }
    else if(QMediaPlayer::StoppedState == player->state())
    {
        //刚开始默认为停止状态，当点击播放按钮时，直接播放即可
        player->play();
//        ui->play->setIcon(QIcon(":/images/play_on.png"));
    }
    else
    {
        qDebug()<<player->errorString();
    }
}

void MyMusic::onPlayUpClicked()
{
    playList->previous();
}

void MyMusic::onPlayDownClicked()
{
    playList->next();
}

void MyMusic::onPlaybackModeClicked()
{
    //1.根据当前播放模式，切换到下一个播放模式
    //2.设置ToolTip提示
    //列表循环--->随机播放--->单曲循环
    //当状态发生改变的时候，还需要修改按钮上的图标
    if(playList->playbackMode() == QMediaPlaylist::Loop)
    {
        //随机播放
        playList->setPlaybackMode(QMediaPlaylist::Random);
        ui->playMode->setToolTip("随机播放");
    }
    else if(playList->playbackMode() == QMediaPlaylist::Random)
    {
        //单曲循环
        playList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
        ui->playMode->setToolTip("单曲循环");
    }
    else if(playList->playbackMode() == QMediaPlaylist::CurrentItemOnce)
    {
        //列表循环
        playList->setPlaybackMode(QMediaPlaylist::Loop);
        ui->playMode->setToolTip("列表循环");
    }
    else
    {
        //暂不支持
        qDebug()<<"暂不支持";
    }
}

void MyMusic::onPlayAll(PageType pageType)
{
    qDebug()<<"播放所有";
    //播放page对应的CommonPage页面中的所有歌曲，默认从第0首开始播放
    //通过pageType找到对应的CommonPage页面
    CommonPage* page = ui->localPage;
    switch(pageType)
    {
    case PageType::LIKE_PAGE:
        page = ui->likePage;
        break;
    case PageType::LOCAL_PAGE:
        page = ui->localPage;
        break;
    case PageType::HISTORY_PAGE:
        page = ui->recentPage;
        break;
    default:
        qDebug()<<"暂不支持的页面";
    }

    //page中记录的就是要播放的页面
    playAllMusicOfCommonPage(page,0);
}

void MyMusic::playAllMusicOfCommonPage(CommonPage *page, int index)
{
    (void)index;
    currentPage = page;//当前页面设置一下
    updateBtnformAnimal();
    //清空之前playlist中歌曲
    playList->clear();

    //添加当前要播放的page页面中的所有歌曲
    page->addMusicToPlaylist(musicList,playList);

    //播放默认从第0首开始播放
    playList->setCurrentIndex(0);

    //播放
    player->play();
}

void MyMusic::playMusicByIndex(CommonPage *page, int index)
{
    playAllMusicOfCommonPage(page,index);
}

void MyMusic::onPlayerStateChanged()
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        ui->play->setIcon(QIcon(":/images/play_on.png"));
    }
    else
    {
        ui->play->setIcon(QIcon(":/images/play3.png"));
    }
}

void MyMusic::onPlaybackModeChanged(QMediaPlaylist::PlaybackMode playbackMode)
{
    if(playbackMode == QMediaPlaylist::Loop)
    {
        ui->playMode->setIcon(QIcon(":/images/list_play.png"));
    }
    else if(playbackMode == QMediaPlaylist::Random)
    {
        ui->playMode->setIcon(QIcon(":/images/shuffle_2.png"));
    }
    else if(playbackMode == QMediaPlaylist::CurrentItemOnce)
    {
        ui->playMode->setIcon(QIcon(":/images/single_play.png"));
    }
    else
    {
        qDebug()<<"暂不支持";
    }
}

void MyMusic::onCurrentIndexChanged(int index)
{
    currentIndex = index;
    //由于CommonPage页面中的歌曲和正在播放的播放列表中的歌曲的先后次序是相同的
    //知道歌曲在playlist中的索引之后，直接到CommonPage中获取
    //注意：playlist中的歌曲就是根据CommonPage中的musicOfPage加载的

    QString musicId = currentPage->getMusicIdByIndex(index);

    auto it = musicList.findMusicByMusicId(musicId);
    if(it != musicList.end())
    {
        it->setIsHistory(true);
    }

    ui->recentPage->reFresh(musicList);
}

void MyMusic::onDurationChanged(qint64 duration)
{
    totalTime = duration;
    //duration为歌曲的总时长，单位为毫秒
    //需要将整型的总时长转化为min:sec
    //分：duration/1000/60;
    //秒：duration/1000%60;
    ui->totalTime->setText(QString("%1:%2").arg(duration/1000/60,2,10,QChar('0'))
                           .arg(duration/1000%60,2,10,QChar('0')));
}

void MyMusic::onPositionChanged(qint64 position)
{
    //更新当前播放时间
    ui->currentTime->setText(QString("%1:%2").arg(position/1000/60,2,10,QChar('0'))
                                              .arg(position/1000%60,2,10,QChar('0')));

    //更新进度条的位置
    ui->processBar->setStep(position/(float)totalTime);
    //在歌词界面同步显示歌词
    if(currentIndex >= 0)
    {
        lrcPage->showLrcWordLine(position);
    }
}

void MyMusic::onMusicSliderChanged(float ratio)
{
    //根据进度条与总宽度的比率，更新当前播放时间
    qint64 duration = totalTime*ratio;
    ui->currentTime->setText(QString("%1:%2").arg(duration/1000/60,2,10,QChar('0'))
                                              .arg(duration/1000%60,2,10,QChar('0')));

    //时间修改之后，播放时间也需要修改
    player->setPosition(duration);
}

void MyMusic::onMetaDataAvailableChanged(bool available)
{
    (void)available;
    //歌曲名成、歌曲作者直接道Music对象中获取
    //此时，需要知道媒体源在播放列表中的索引
    QString musicId = currentPage->getMusicIdByIndex(currentIndex);
    auto it = musicList.findMusicByMusicId(musicId);

    QString musicName("未知歌曲");
    QString musicSinger("歌手未知");
    if(it != musicList.end())
    {
        musicName = it->getMusicName();
        musicSinger = it->getMusicSinger();
    }

    ui->musicName->setText(musicName);
    ui->musicSinger->setText(musicSinger);

    //获取封面，通过元数据来获取
    QVariant coverimage = player->metaData("ThumbnailImage");
    if(coverimage.isValid())
    {
        QImage image = coverimage.value<QImage>();
        ui->musicCover->setPixmap(QPixmap::fromImage(image));
        currentPage->setMusicImage(QPixmap::fromImage(image));
    }
    else
    {
        qDebug()<<"歌曲没有封面";
        //可以设置默认图片
        QString path = ":/images/rec/001.png";
        ui->musicCover->setPixmap(path);
        currentPage->setMusicImage(path);
    }
    ui->musicCover->setScaledContents(true);

    //解析歌曲的LRC歌词
    if(it != musicList.end())
    {
        //获取lrc文件的路径
        QString lrcPath = it->getLrcFilePath();

        //解析lrc文件
        lrcPage->parseLrcFile(lrcPath);
    }
}

void MyMusic::setPlayerMuted(bool isMuted)
{
    player->setMuted(isMuted);
}

void MyMusic::setPlayerVolume(int volume)
{
    player->setVolume(volume);
}

void MyMusic::on_skin_clicked()
{
    //换肤，类似于给窗口更换背景颜色，或更换背景图片
    QMessageBox::information(this,"温馨提示","工程师正在紧急开发......");
}

void MyMusic::on_min_clicked()
{
    //窗口最小化
    showMinimized();
}

void MyMusic::on_max_clicked()
{
    //最大化显示
    showMaximized();
}
