#ifndef MUSICLIST_H
#define MUSICLIST_H

#include <QUrl>
#include <QList>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "music.h"

typedef QVector<Music>::iterator iterator;
class MusicList
{
public:
    MusicList();

    void addMusicsByUrl(const QList<QUrl>& musicUrls);
    iterator findMusicByMusicId(const QString& musicId);
    iterator begin();
    iterator end();

    void readFromDB();
    void writeToDB();
private:
    QVector<Music> musicList;
    QSet<QString> musicPaths;//为了防止歌曲文件重复加载,hash
};

#endif // MUSICLIST_H
