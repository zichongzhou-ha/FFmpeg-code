#include "playthread.h"
#include <QDebug>
#include "SDL.h"
#define FILE_NAME "out.wav"

typedef  struct{
    int len=0;
    Uint8* data=nullptr;
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

    // WAV中的PCM数据
    Uint8 *data=nullptr;
    // WAV中的PCM数据大小（字节）
    Uint32 len;
    SDL_AudioSpec spec;
    if(!SDL_LoadWAV(FILE_NAME,&spec,&data,&len))
    {
        qDebug()<<"SDL_LoadWAV failed";
        SDL_Quit();
         return ;
    }



    spec.callback=pull_audio_data;
    spec.samples=1024;//决定缓冲区大小


    AudioBuffer buffer;
    buffer.len=len;
    buffer.data=data;

    spec.userdata=&buffer;

    if(SDL_OpenAudio(&spec,nullptr))
    {
        qDebug()<<"SDL_OpenAudio error"<<SDL_GetError();
        SDL_Quit();
        SDL_FreeWAV(data);
        return ;
    }
    SDL_PauseAudio(0);


    while(!isInterruptionRequested())
    {
        if(buffer.len>0)continue;
        int size=spec.channels*SDL_AUDIO_BITSIZE(spec.format)/8;
        int samples=buffer.pulllen/size;
        int ms=samples*1000/spec.freq;
        SDL_Delay(ms);
        break;
    }
    SDL_FreeWAV(data);
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

