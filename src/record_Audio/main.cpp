#include "mainwindow.h"
#include<iostream>
#include<QDebug>
#include <QApplication>




//c语言
//    printf("c:hello world");
//c++
//    std::cout<<"c++:"<<"hello world";
//qt
//    qDebug()<<"qt：hello world";
//FFmpeg:TRACE<DEBUG<INFO<WARNING<ERROR<FATAL<QUIET(所有都不打印）
//    av_log_set_level(AV_LOG_DEBUG);
//    av_log(nullptr,AV_LOG_ERROR,"AV_LOG_ERROR");
//    av_log(nullptr,AV_LOG_FATAL,"AV_LOG_FATAL");
//    av_log(nullptr,AV_LOG_INFO,"AV_LOG_INFO");
//    fflush(stdout);
//    fflush(stderr);
extern "C"
{
#include<libavdevice/avdevice.h>
}
int main(int argc, char *argv[])
{
    avdevice_register_all();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
