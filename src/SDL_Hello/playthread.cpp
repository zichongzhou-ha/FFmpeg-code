#include "playthread.h"
#include <QDebug>
#include "SDL.h"
char* bufferData;
int bufferLen;
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
    if(bufferLen<=0)return ;//文件没准备好
    len=(len>bufferLen)?bufferLen:len;

    SDL_MixAudio(stream,(Uint8 *)bufferData,len,SDL_MIX_MAXVOLUME);
    bufferData+=len;
    bufferLen-=len;
}
void PlayThread::run()
{
    if(SDL_Init(SDL_INIT_AUDIO))
    {
        qDebug()<<"SDL_Init error"<<SDL_GetError();
        return ;
    }
    SDL_AudioSpec spec;
    spec.freq=SAMPLE_RATE;//采样率
    spec.channels=CHANNELS;//声道数
    spec.format=AUDIO_S16LSB;//采样大小
    spec.callback=pull_audio_data;
    spec.samples=1024;//决定缓冲区大小
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
        bufferLen=file.read(data,BUFFER_SIZE);
        if(bufferLen<=0)break;

        bufferData=data;
        while(bufferLen>0)
        {
            SDL_Delay(1);
        }
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

