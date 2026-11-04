#include "mainwindow.h"
#include<iostream>
#include<QDebug>
#include <QApplication>

#include "ffmpegs.h"


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
    WAVHeader header;
    header.riffChunkDataSize=11522048-8;
    header.sampleRate=44100;
    header.bitsPerSample=16;
    header.numChannels=2;
    header.blockAlign=(header.bitsPerSample*header.numChannels)>>3;
     header.byteRate=header.blockAlign*header.sampleRate;
    header.dataChunkDataSize=11522048;
//     FFmpegs::pcm2wav(header,"test.pcm","out.wav");
    FFmpegs::pcm2wav(header,"D:/qt/AV/test/test.pcm","out.wav");
    avdevice_register_all();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
