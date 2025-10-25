#ifndef VOLUMETOOL_H
#define VOLUMETOOL_H

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class VolumeTool;
}

class VolumeTool : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeTool(QWidget *parent = nullptr);
    ~VolumeTool();

    void paintEvent(QPaintEvent* event);
    bool eventFilter(QObject* watched,QEvent* event);
    void calcVolume();
    void onSilenceBtnClicked();

signals:
    void setMusicMuted(bool);//设置是否静音的信号
    void setMusicVolume(int volume);//设置音量大小
private:
    Ui::VolumeTool *ui;
    bool isMuted;//是否静音
    int volumeRatio;//标记音量的大小
};

#endif // VOLUMETOOL_H
