#include "ffmpegs.h"
#include<QFile>
#include<QDebug>
FFmpegs::FFmpegs()
{

}

void FFmpegs::pcm2wav(WAVHeader &header, const char *pcmFilename, const char *wavFilename)
{
    header.blockAlign=(header.bitsPerSample*header.numChannels)>>3;
     header.byteRate=header.blockAlign*header.sampleRate;

    QFile pcmfile(pcmFilename);
    if(!pcmfile.open(QFile::ReadOnly))
    {
        qDebug()<<u8"打开文件失败";
        return ;
    }
    header.dataChunkDataSize=pcmfile.size();
     header.riffChunkDataSize=header.dataChunkDataSize
             +sizeof(WAVHeader)-8;
    QFile wavfile(wavFilename);
    if(!wavfile.open(QFile::WriteOnly))
    {
        qDebug()<<u8"打开文件失败";
        pcmfile.close();
        return ;
    }

    wavfile.write((const char*)&header,sizeof(WAVHeader));

    char buf[1024];
    int size;
    while((size=pcmfile.read(buf,sizeof(buf)))>0)
    {
        wavfile.write(buf,size);
    }
    pcmfile.close();
    wavfile.close();
}
