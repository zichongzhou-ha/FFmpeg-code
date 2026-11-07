#include "audio_thread.h"
#define errlog(ret)  \
        char buf[1024]; \
        av_strerror(ret,buf,sizeof(buf));

void Audio_thread::run()
{
    int len=0;
    const char* inFilename="test.pcm";
    QFile inFile(inFilename);
    const char* outFilename="out.pcm";
    QFile outFile(outFilename);
    int64_t in_ch_layout=AV_CH_LAYOUT_STEREO;
    AVSampleFormat in_sample_fmt=AV_SAMPLE_FMT_S16;
    int in_sample_rate=44100;

    uint8_t **inData=nullptr;
    int inLinesize=0;
    int inChs=av_get_channel_layout_nb_channels(in_ch_layout);
    int inSamples=1024;
    int inBytesPerSample=inChs*av_get_bytes_per_sample(in_sample_fmt);

    int64_t out_ch_layout=AV_CH_LAYOUT_STEREO;
    AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_FLT;
    int out_sample_rate=48000;

    uint8_t **outData=nullptr;
    int outLinesize=0;
    int outChs=av_get_channel_layout_nb_channels(out_ch_layout);
//    int outSamples=1024;
    int outSamples=av_rescale_rnd(out_sample_rate,inSamples,
          in_sample_rate,AV_ROUND_UP);
    int outBytesPerSample=outChs*av_get_bytes_per_sample(out_sample_fmt);
    SwrContext* ctx=swr_alloc_set_opts(NULL,out_ch_layout,out_sample_fmt,out_sample_rate
                                       ,in_ch_layout,in_sample_fmt,in_sample_rate,
                                       0,NULL);
    if(!ctx)
    {
        qDebug()<<"swr_alloc_set_opts failed ";
    }

    int ret=swr_init(ctx);
    if(ret<0)
    {
        errlog(ret);
        qDebug()<<"swr_init failed:"<<buf;
        swr_free(&ctx);
        goto end;
    }


    ret=av_samples_alloc_array_and_samples(
              &inData,&inLinesize,inChs,
                inSamples,in_sample_fmt,1);
    if(ret<0)
    {
        errlog(ret);
        qDebug()<<"av_samples_alloc_array_and_samples:"<<buf;
        goto end;
    }



    ret=av_samples_alloc_array_and_samples(
              &outData,&outLinesize,outChs,
                outSamples,out_sample_fmt,1);
    if(ret<0)
    {
        errlog(ret);
        qDebug()<<"av_samples_alloc_array_and_samples:"<<buf;
        goto end;
    }


    if(!inFile.open(QFile::ReadOnly))
    {
        qDebug()<<"file open error: "<<inFilename;
        goto end;

    }

    if(!outFile.open(QFile::WriteOnly))
    {
        qDebug()<<"file open error: "<<inFilename;
        goto end;

    }


    while((len=inFile.read((char*)inData[0],inLinesize))>0)
    {
        inSamples=len/inBytesPerSample;
        ret=swr_convert(ctx,outData,outSamples,//这里有疑问
                        (const uint8_t**) inData,inSamples);
        if(ret<0)
        {
            errlog(ret);
            qDebug()<<"swr_convert failed:"<<buf;
            goto end;

        }
        outFile.write((char*)outData[0],ret*outBytesPerSample);
    }

    while((ret=swr_convert(ctx,outData,outSamples,//这里有疑问
                           nullptr,0))>0)
    {
        outFile.write((char*)outData[0],ret*outBytesPerSample);
    }
end:
    if(inData)
    {
        av_free(&inData[0]);
    }
    if(outData)
    {
        av_free(&outData[0]);
    }
    av_freep(&inData);
    av_freep(&outData);
    swr_free(&ctx);
    inFile.close();
    outFile.close();
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


