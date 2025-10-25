#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPropertyAnimation>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "musiclist.h"
#include "volumetool.h"
#include "commonpage.h"
#include "lrcpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MyMusic; }
QT_END_NAMESPACE

class MyMusic : public QWidget
{
    Q_OBJECT

public:
    MyMusic(QWidget *parent = nullptr);
    ~MyMusic();

    void initUi();
    void initSqlite();
    void initMusicList();
    void initPlayer();
    //设置随机图片【歌曲的图片】
    QJsonArray randomPicture();

    void connectSignalAndSlots();
    void updateBtnformAnimal();
    void onMyMusicQuit();//响应菜单栏的退出
protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);

private slots:
    void on_quit_clicked();
    void onBtFormClick(int pageId);
    void updateLikeMusicAndPage(bool isLike,const QString& musicId);

    void on_volume_clicked();

    void on_addLocal_clicked();

    void onLrcWordClicked();

    //播放控制区的槽函数 播放按钮：歌曲在播放和暂停之间切换
    void onPlayMusic();
    //上一曲
    void onPlayUpClicked();
    //下一曲
    void onPlayDownClicked();
    //播放模式设置
    void onPlaybackModeClicked();
    //播放所有按钮发射信号对应的槽函数
    void onPlayAll(PageType pageType);
    void playAllMusicOfCommonPage(CommonPage* page,int index);
    void playMusicByIndex(CommonPage* page,int index);

    //QMediaPlayer中stateChanged信号对应的槽函数
    void onPlayerStateChanged();
    void onPlaybackModeChanged(QMediaPlaylist::PlaybackMode playbackMode);
    void onCurrentIndexChanged(int index);//playlist中播放源发生改变，index表示正在播放的媒体的索引

    void onDurationChanged(qint64 duration);//媒体切换，播放时间也会改变
    void onPositionChanged(qint64 position);//播放进度发生改变
    void onMusicSliderChanged(float ratio);//进度条改变需要播放时间跟着修改
    void onMetaDataAvailableChanged(bool available);//元数据发生改变时



    //VolumeTool类中setMusicMuted信号对应的槽函数
    void setPlayerMuted(bool isMuted);

    //VolumeTool类中setMusicVolume信号对应的槽函数
    void setPlayerVolume(int volume);

    void on_skin_clicked();

    void on_min_clicked();

    void on_max_clicked();

private:
    Ui::MyMusic *ui;
    QPoint dragPossition;//记录光标相对于窗口标题栏的相对距离

    VolumeTool* volumeTool;
    QSqlDatabase sqlite;

    LrcPage* lrcPage;
    QPropertyAnimation* lrcPageAnimal;

    MusicList musicList;//用来组织歌曲文件

    QMediaPlayer* player;//专门用来播放控制
    QMediaPlaylist* playList;//专门用来管理播放源，包含一些播放模式设置等；
    CommonPage* currentPage;//记录当前正在播放的commonPage页面
    int currentIndex;//记录当前正在播放歌曲在媒体列表中的索引

    qint64 totalTime;//记录媒体源的总时间
    bool isDrag;
    //当鼠标在非BtForm上点击时，会将isDrag设置为true；
    //当鼠标在BtForm上单击时，将isDrag设置为false;
    //当鼠标移动时，若isDrag为true，再移动窗口
};
#endif // WIDGET_H
