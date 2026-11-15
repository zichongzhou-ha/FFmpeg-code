#include "ffmepg.h"
#include <QFile>
#include <QDebug>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
#define ERROR_BUF(ret) \
    char buffer[1024]; \
    av_strerror(ret,buffer,sizeof buffer);
FFmepg::FFmepg()
{

}
int check_sample_fmt(AVCodec* codec,AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat* p=codec->sample_fmts;
    while(*p!=AV_SAMPLE_FMT_NONE)
    {
        if(*p==sample_fmt)return 1;
        p++;
    }
    return 0;
}
static int encode(AVCodecContext* ctx,
         AVFrame* frame,AVPacket* pkt,QFile &outFile)
{
    int ret=avcodec_send_frame(ctx,frame);
    if(ret<0)
    {
        ERROR_BUF(ret);
        qDebug()<<"avcodec_send_frame failed:"<<buffer;
        return ret;
    }
    while(1)
    {
        ret=avcodec_receive_packet(ctx,pkt);
        if(ret==AVERROR(EAGAIN)||ret==AVERROR_EOF)
        {
            return 0;
        }
        else if(ret<0)
        {
            ERROR_BUF(ret);
            qDebug()<<"avcodec_receive_packet failed:"<<buffer;
            return ret;
        }
        outFile.write((char*)pkt->data,pkt->size);
        av_packet_unref(pkt);
    }
    return 0;
}
void FFmepg::aacEncode(Audio_EncodecSpec &in, const char *outFilename)
{
    AVCodec* codec=nullptr;
    AVCodecContext* ctx=nullptr;
    AVFrame* frame=nullptr;
    AVPacket* pkt=nullptr;
    int ret=0;
    QFile infile(in.filename);
    QFile outfile(outFilename);

    codec=avcodec_find_encoder_by_name("libfdk_aac");
    if(!codec)
    {
        qDebug()<<"avcodec_find_encoder_by_name failed";
        return;
    }
    if(!check_sample_fmt(codec,in.sampleFmt))
    {
        qDebug() << "Encoder does not support sample format"
                     << av_get_sample_fmt_name(in.sampleFmt);
        return;
    }

    ctx=avcodec_alloc_context3(codec);
    if(!ctx)
    {
        qDebug()<<"avcodec_alloc_context3 failed";
        return ;
    }
    ctx->sample_fmt=in.sampleFmt;
    ctx->sample_rate=in.sampleRate;
    ctx->channel_layout=in.chLayout;
    ctx->channels=av_get_channel_layout_nb_channels(in.chLayout);
    ctx->bit_rate=32000;
    ctx->profile=FF_PROFILE_AAC_HE_V2;

    AVDictionary *options = nullptr;
    av_dict_set(&options, "vbr", "1", 0);
    ret = avcodec_open2(ctx, codec, &options);

    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_open2 error" << buffer;
        goto end;
    }


    frame=av_frame_alloc();
    if(!frame)
    {
        qDebug()<<"av_frame_alloc failed failed";
        goto end;
    }
    frame->nb_samples=ctx->frame_size;
    frame->format=ctx->sample_fmt;
    frame->channel_layout=ctx->channel_layout;

    ret=av_frame_get_buffer(frame,0);
    if(ret<0)
    {
        ERROR_BUF(ret);
        qDebug()<<"av_frame_get_buffe failed: "<<buffer;
        goto end;
    }

    pkt=av_packet_alloc();
    if(!pkt)
    {
        qDebug()<<"av_packet_alloc faield ";
        goto end;
    }

    if(!infile.open(QFile::ReadOnly))
    {
        qDebug()<<"infile.open failed";
        goto end;
    }
    if(!outfile.open(QFile::WriteOnly))
    {
        qDebug()<<"outfile.open failed";
        goto end;
    }
    while((ret=infile.read((char*)frame->data[0],\
           frame->linesize[0]))>0)
    {
        if(ret<frame->linesize[0])
        {
            int chs=av_get_channel_layout_nb_channels(frame->channel_layout);
            int bytes=av_get_bytes_per_sample((AVSampleFormat)frame->format);
            frame->nb_samples=ret/(chs*bytes);
        }
        if(encode(ctx,frame,pkt,outfile)<0)
        {
            goto end;
        }
    }
    encode(ctx,nullptr,pkt,outfile);
end:
    infile.close();
    outfile.close();

    // 释放资源
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);

}
