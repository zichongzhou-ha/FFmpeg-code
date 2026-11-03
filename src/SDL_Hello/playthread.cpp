#include "playthread.h"
#include <QDebug>
#include "SDL.h"

typedef  struct{
    int len=0;
    char* data=nullptr;
    int pulllen=0;
} AudioBuffer;

PlayThread::PlayThread(QObject *parent) : QThread(parent)
{
    connect(this,&QThread::finished,
            this,&QThread::deleteLater);

}
//stream是需要我们要填充的pcm的缓冲区，len缓冲区大小(字节）
//bufferData-》stream-》播放缓冲区
void pull_audio_data(void *userdata, Uint8 * stream,int len)//播放缓冲区降到一定阈值才会调用
{
    SDL_memset(stream,0,len);
    AudioBuffer* buffer=(AudioBuffer*)userdata;
    if(buffer->len<=0)return ;//文件没准备好
    buffer->pulllen=(len>buffer->len)?buffer->len:len;

    SDL_MixAudio(stream,(Uint8 *)buffer->data,buffer->pulllen,SDL_MIX_MAXVOLUME);
    buffer->data+=buffer->pulllen;
    buffer->len-=buffer->pulllen;
}
void PlayThread::run()
{
    if(SDL_Init(SDL_INIT_AUDIO))
    {
        qDebug()<<"SDL_Init error"<<SDL_GetError();
        return ;
    }
    AudioBuffer buffer;
    SDL_AudioSpec spec;
    spec.freq=SAMPLE_RATE;//采样率
    spec.channels=CHANNELS;//声道数
    spec.format=AUDIO_S16LSB;//采样大小
    spec.callback=pull_audio_data;
    spec.samples=1024;//决定缓冲区大小
    spec.userdata=&buffer;

    //1024个样本点-》1024frame-》1024*采样大小*声道数/8=4096
    //一个frame=左声道+右声道
    if(SDL_OpenAudio(&spec,nullptr))
    {
        qDebug()<<"SDL_OpenAudio error"<<SDL_GetError();
        SDL_Quit();
        return ;
    }

    QFile file(FILE_NAME);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"file open error"<<FILE_NAME;
        SDL_CloseAudio();
        SDL_Quit();
        return ;
    }
    SDL_PauseAudio(0);

    char data[BUFFER_SIZE];
    while(!isInterruptionRequested())
    {
        if(buffer.len>0)continue;
        buffer.len=file.read(data,BUFFER_SIZE);
        if(buffer.len<=0){
            int samples=buffer.pulllen/BYTES_PER_SAMPLE;
            int ms=samples*1000/SAMPLE_RATE;
            SDL_Delay(ms);
            break;
        }
        buffer.data=data;
    }
    file.close();
    SDL_CloseAudio();
    SDL_Quit();
}

PlayThread::~PlayThread()
{
    disconnect();
    requestInterruption();
    quit();
    wait();
    qDebug()<<this;
}

