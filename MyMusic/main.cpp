#include "mymusic.h"

#include <QApplication>
#include <QPushButton>
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
      QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling,true);
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QSharedMemory sharedMemory("MyMusic");
    //当程序第一次运行时，由于共享内存的空间还没有创建，此处attch一定会失败，即返回false
    //当程序第二次运行时，由于shm的空间已经被前一次的运行申请好，此时第二个实例再去关联就能成功
    //由于只能运行一个实例，让第二个实例直接退出，即让第一个进程结束
    if(sharedMemory.attach())
    {
        QMessageBox::information(nullptr,"MyMusic提示","MyMusic已经在运行了...");
        return 0;
    }
    sharedMemory.create(1);

    MyMusic w;
    w.show();
    return a.exec();
}
