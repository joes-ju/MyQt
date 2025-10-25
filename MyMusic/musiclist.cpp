#include "musiclist.h"
#include <QMimeDatabase>
//#include <QSqlQuery>
MusicList::MusicList()
{

}

void MusicList::addMusicsByUrl(const QList<QUrl> &musicUrls)
{
    //将所有的音乐放置到musicList
    for(auto e: musicUrls)
    {
        //文件重复过滤
        //如果该文件已经在musicList中，就不再添加；
        //过滤方式：将当前要添加到musicList中的文件的路径与musicList中已经存在的歌曲文件路径对比，入宫相同则歌曲已存在，如果不同说明还没有添加过，就要添加
        //时间复杂度O(n)
        #if 0
        auto it = begin();
        for(;it != end();++it)
        {
            if(it->getMusicUrl() == e)
            {
                break;
            }
        }

        if(it != end())
        {
            continue;
        }
        #endif

        //查找：最快的方式就是哈希
        QString musicPath = e.toLocalFile();
        if(musicPaths.contains(musicPath))
            continue;

        //歌曲不存在
        musicPaths.insert(musicPath);

        //如果musicUrl是一个有效的歌曲文件，再将其添加到歌曲列表中
        //检测歌曲文件的MIME类型
        QMimeDatabase mimeDB;
        QMimeType mimeType = mimeDB.mimeTypeForFile(e.toLocalFile());
        QString mime = mimeType.name();

        //mime和audio/mpeg:mp3  audio/flac:无损音频  audio/wav
        if(mime == "audio/mpeg" || mime == "audio/flac" || mime == "audio/wav")
        {
            Music music(e);
            musicList.push_back(music);
        }
    }
}

iterator MusicList::findMusicByMusicId(const QString &musicId)
{
    for(auto it = begin(); it != end(); ++it)
    {
        if(it->getMusicId() == musicId)
        {
            return it;
        }
    }
    return end();
}

iterator MusicList::begin()
{
    return musicList.begin();
}

iterator MusicList::end()
{
    return musicList.end();
}

void MusicList::readFromDB()
{
    QSqlQuery query;
    query.prepare("select musicId, musicName, musicSinger, albumName, musicUrl, duration, isLike, isHistory from MusicInfo");
    if(!query.exec())
    {
        qDebug()<<"数据库查询失败: "<<query.lastError().text();
        return;
    }


    while(query.next())
    {
        Music music;
        music.setMusicId(query.value(0).toString());
        music.setMusicName(query.value(1).toString());
        music.setMusicSinger(query.value(2).toString());
        music.setMusicAlbum(query.value(3).toString());
        music.setMusicUrl(QUrl::fromLocalFile(query.value(4).toString()));
        music.setMusicDuration(query.value(5).toLongLong());
        music.setIsLike(query.value(6).toBool());
        music.setIsHistory(query.value(7).toBool());
        musicList.push_back(music);

        //恢复musicPaths，目的防止相同目录下的歌曲被重复加载
        musicPaths.insert(music.getMusicUrl().toLocalFile());
    }
}

void MusicList::writeToDB()
{
    for(auto music: musicList)
    {
        //获取music对象之后，将music的所有属性写入到数据库
        //具体写入单个music对象的操作也就将其封装成一个函数
        music.InsertMusicToDB();
    }
}
