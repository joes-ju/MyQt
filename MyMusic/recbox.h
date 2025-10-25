#ifndef RECBOX_H
#define RECBOX_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class RecBox;
}

class RecBox : public QWidget
{
    Q_OBJECT

public:
    explicit RecBox(QWidget *parent = nullptr);
    ~RecBox();

    //初始化RecBoxItem
    void initRecBoxUI(QJsonArray data,int row);

private slots:
    void on_btUp_clicked();

    void on_btDown_clicked();

private:
    void createRecBoxItem();

private:
    Ui::RecBox *ui;

    int row;//行,记录当前recBox实际总行数
    int column;//列，记录当前RecBox实际每行有几个元素

    int currentIndex;//标记当前显示第几组图片
    int count;//标记图片总的组数；
    QJsonArray imageList;//保存界面上的图片，里面实际为key、value键值对
};

#endif // RECBOX_H
