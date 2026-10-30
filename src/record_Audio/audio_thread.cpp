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
    int count=50;
    AVPacket pkt;
    av_init_packet(&pkt);
    while(count-- > 0 && av_read_frame(ctx, &pkt)==0)
    {
        file.write((const char*)pkt.data, pkt.size);
    }

    file.close();
    avformat_close_input(&ctx);
}

Audio_thread::Audio_thread(QObject *parent) : QThread(parent)
{

}

Audio_thread::~Audio_thread()
{
    qDebug()<<this<<"hahahaha";
}
