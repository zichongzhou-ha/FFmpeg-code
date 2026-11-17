#include "video_thread.h"
extern "C"{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
}
#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

void showSpec(AVFormatContext* ctx)
{
    AVStream* stream=ctx->streams[0];
    AVCodecParameters* params=stream->codecpar;
    qDebug()<<params->channels;
    qDebug()<<params->sample_rate;
    qDebug()<<params->format;
    //每一个样本的一个声道占用多少字节
    qDebug()<<av_get_bytes_per_sample((AVSampleFormat)params->format);
}
void Video_thread::run()
{
#ifdef Q_OS_WIN
    const char* fmtName="dshow";
#else
    const char* fmtName="avfoundation";
#endif
    AVInputFormat* fmt=av_find_input_format(fmtName);
    if(!fmt)
    {
        qDebug()<<"Failed to obtain the input format object";
        return ;
    }
    AVFormatContext* ctx=nullptr;
    const char* dev = "video=Integrated Camera";
    AVDictionary *options = nullptr;
    av_dict_set(&options, "video_size", "1280x720", 0);
    av_dict_set(&options, "pixel_format", "yuyv422", 0);
    av_dict_set(&options, "framerate", "10", 0);

    int ret=avformat_open_input(&ctx,dev,fmt,nullptr);
    if(ret<0)
    {
        ERROR_BUF(ret);
        qDebug() << "avformat_open_input error" << errbuf;
        return;
    }
    const char* url="out.yuv";
    QFile file(url);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug()<<u8"打开文件失败";
        avformat_close_input(&ctx);
        return ;
    }
    AVCodecParameters *params = ctx->streams[0]->codecpar;
    int imageSize = av_image_get_buffer_size(
                        (AVPixelFormat) params->format,
                        params->width, params->height,
                        1);
    AVPacket* pkt=av_packet_alloc();
    while(!isInterruptionRequested()&& av_read_frame(ctx, pkt)==0)
    {
        if(ret==0)
        {
            file.write((const char*)pkt->data, pkt->size);
//            file.write((const char *) pkt->data, imageSize);
            /*
             这里要使用imageSize，而不是pkt->size。
             pkt->size有可能比imageSize大（比如在Mac平台），
             使用pkt->size会导致写入一些多余数据到YUV文件中，
             进而导致YUV内容无法正常播放
            */
            av_packet_unref(pkt);
        }
        else if(ret==AVERROR(EAGAIN))
        {
            continue;
        }
        else {
            ERROR_BUF(ret);
            qDebug() << "av_read_frame error" << errbuf;
            break;
        }

    }

    // 释放资源
    av_packet_free(&pkt);

    // 关闭文件
    file.close();

    // 关闭设备
    avformat_close_input(&ctx);

}

Video_thread::Video_thread(QObject *parent) : QThread(parent)
{
    connect(this,&Video_thread::finished,
            this,&Video_thread::deleteLater);
    //线程结束会发送结束信号，然后自己去释放
}


Video_thread::~Video_thread()
{
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"free";
    //这里逻辑是这样的：如果通过按键结束录音，子线程跑完，会自己发信号然后触发槽函数去释放内存
    //但是，如果不点结束录音，直接结束窗口，那这个子线程还在跑也没有释放，如果将子线程作为窗口子对象
    //窗口结束子线程还在跑却被释放内存这样也会有问题，所以这里析构函数也加了requestInterruption();
    //窗口释放子线程时，会给子线程的flag置数，等待子线程跑完后释放内存。
    //不过这里现在的代码逻辑肯定还有问题：子线程和主线程之间的。
}


