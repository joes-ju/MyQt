#ifndef MUSICSLIDER_H
#define MUSICSLIDER_H

#include <QWidget>
#include <QMouseEvent>
#include <QDebug>

namespace Ui {
class MusicSlider;
}

class MusicSlider : public QWidget
{
    Q_OBJECT

public:
    explicit MusicSlider(QWidget *parent = nullptr);
    ~MusicSlider();

    void setStep(float ratio);
signals:
    void setMusicSliderPosition(float ratio);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);

    void moveSlider();
private:
    Ui::MusicSlider *ui;
    int currentPos;//播放进度
    int maxWidth;//outLine的总宽度
};

#endif // MUSICSLIDER_H
