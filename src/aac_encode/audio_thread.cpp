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
    Audio_EncodecSpec in;
    in.filename="test.pcm";
    in.sampleRate = 44100;
    in.sampleFmt=AV_SAMPLE_FMT_S16;
    in.chLayout=AV_CH_LAYOUT_STEREO;
    FFmepg::aacEncode(in,"out.aac");
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


