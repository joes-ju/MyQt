#ifndef MUSIC_H
#define MUSIC_H

#include <QUrl>
#include <QUuid>
#include <QMediaPlayer>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class Music
{
public:
    Music();
    Music(QUrl url);
    void setMusicId(const QString& musicId);
    void setMusicName(const QString& musicName);
    void setMusicSinger(const QString& musicSinger);
    void setMusicAlbum(const QString& musicAlbum);
    void setMusicDuration(qint64 duration);
    void setIsLike(bool isLike);
    void setIsHistory(bool isHistory);
    void setMusicUrl(QUrl musicUrl);

    QString getMusicName()const;
    QString getMusicSinger()const;
    QString getMusicAlbum()const;
    qint64 getMusicDuration()const;
    bool getIsLike()const;
    bool getIsHistory()const;
    QUrl getMusicUrl()const;
    QString getMusicId()const;

    QString getLrcFilePath()const;
    //将歌曲属性写入数据库
    void InsertMusicToDB();

private:
    void parseMediaMetaMusic();//解析

private:
    QString musicId;
    QString musicName;//音乐名称
    QString musicSinger;//音乐歌手
    QString musicAlbum;//音乐专辑
    qint64 duration;//持续时间
    bool isLike;//是否收藏（是否喜欢）
    bool isHistory;//是否为历史播放
    QUrl musicUrl;//音乐QUrl
};

#endif // MUSIC_H
