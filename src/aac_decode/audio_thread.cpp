#include "audio_thread.h"
#include "ffmepg.h"
#include <QDebug>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
#define ERROR_BUF(ret) \
    char buffer[1024]; \
    av_strerror(ret,errbuf,sizeof buffer);




void Audio_thread::run()
{
    AudioDecodeSpec out;
    out.filename = "out.pcm";
    FFmepg::aacDecode("test.aac",out);
    // 44100
    qDebug() << out.sampleRate;
    // s16
    qDebug() << av_get_sample_fmt_name(out.sampleFmt);
    // 2
    qDebug() << av_get_channel_layout_nb_channels(out.chLayout);

}
















Audio_thread::Audio_thread(QObject *parent):QThread(parent)
{

}

Audio_thread::~Audio_thread()
{
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"free";

}


