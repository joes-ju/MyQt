#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "musiclist.h"

namespace Ui {
class CommonPage;
}

enum PageType{
    LIKE_PAGE,
    LOCAL_PAGE,
    HISTORY_PAGE
};

class CommonPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommonPage(QWidget *parent = nullptr);
    ~CommonPage();

    void setMusicListType(PageType pageType);
    void setCommonPageUI(const QString& text,const QString& imagePath);
    void addMusicToMusicPage(MusicList& musicList);
    void reFresh(MusicList& musicList);//更新

    //将歌曲添加到播放列表中
    void addMusicToPlaylist(MusicList& musicList,QMediaPlaylist* playList);
    void setMusicImage(QPixmap pixmap);
    QString getMusicIdByIndex(int index);
signals:
    void updateLikeMusic(bool isLike,const QString& musicId);
    void playAll(PageType);
    void playMusicByIndex(CommonPage*, int);
private:
    Ui::CommonPage *ui;
    PageType pageType;//保存该页面的类别说明
    QVector<QString> musicOfPage;//保存pageType对应的页面的歌曲ID
};

#endif // COMMONPAGE_H
