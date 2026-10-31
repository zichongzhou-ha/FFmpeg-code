#include "audio_thread.h"

void Audio_thread::run()
{
#ifdef Q_OS_WIN
    const char* fmtName="dshow";
#else
    const char* fmtName="avfoundation";
#endif
    AVInputFormat* fmt=av_find_input_format(fmtName);
    if(!fmt)
    {
//        qDebug()<<"获取输入格式对象失败";
        qDebug()<<"Failed to obtain the input format object";
        return ;
    }
    AVFormatContext* ctx=nullptr;
    const char* dev = "audio=@device_cm_{33D9A762-90C8-11D0-BD43-00A0C911CE86}\\wave_{3C9C50C2-462F-4574-BA06-B5FAB447BA74}";
    int ret=avformat_open_input(&ctx,dev,fmt,nullptr);
    if(ret!=0)
    {
        char buf[1024];
        av_strerror(ret,buf,sizeof buf);
        qDebug()<<"open failed"<<buf;
        return ;
    }
    const char* url="test.pcm";
    QFile file(url);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug()<<u8"打开文件失败";
        avformat_close_input(&ctx);
        return ;
    }
    AVPacket pkt;
    av_init_packet(&pkt);
    while(!isInterruptionRequested()&& av_read_frame(ctx, &pkt)==0)
    {
        file.write((const char*)pkt.data, pkt.size);
    }

    file.close();
    avformat_close_input(&ctx);

    qDebug()<<"thread finish";
}

Audio_thread::Audio_thread(QObject *parent) : QThread(parent),_stop(false)
{
    connect(this,&Audio_thread::finished,
            this,&Audio_thread::deleteLater);
    //线程结束会发送结束信号，然后自己去释放
}

void Audio_thread::setstop(bool stop)
{
    _stop=stop;
}

Audio_thread::~Audio_thread()
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
