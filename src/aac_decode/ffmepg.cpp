#include "ffmepg.h"
#include <QFile>
#include <QDebug>
#define IN_DATA_SIZE 20480
#define REFILL_THRESH 4096

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
static int decode(AVCodecContext* ctx,AVPacket* pkt,
                  AVFrame* frame,QFile& outFile)
{
    int ret=avcodec_send_packet(ctx,pkt);
    if(ret<0)
    {
        ERROR_BUF(ret);
        qDebug()<<"avcodec_send_packet failed:"<<buffer;
        return ret;
    }

    while(1)
    {
        ret=avcodec_receive_frame(ctx,frame);
        if(ret==AVERROR(EAGAIN)||ret==AVERROR_EOF)
        {
            return 0;
        }
        else if(ret<0)
        {
            ERROR_BUF(ret);
            qDebug()<<"avcodec_receive_frame failed:"<<buffer;
            return ret;
        }
        outFile.write((char*)frame->data[0],frame->linesize[0]);
    }
}

void FFmepg::aacDecode(const char *inFilename, AudioDecodeSpec &out)
{
    int ret=0;
    int inLen=0;
    int inEnd=0;
    char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    char *inData = inDataArray;

    QFile inFile(inFilename);
    QFile outFile(out.filename);

    // 解码器
    AVCodec *codec = nullptr;
    // 上下文
    AVCodecContext *ctx = nullptr;
    AVCodecParserContext* parserCtx=nullptr;

    // 存放解码前的数据
    AVPacket *pkt = nullptr;
    // 存放解码后的数据
    AVFrame *frame = nullptr;

    // 获取解码器
    codec = avcodec_find_decoder_by_name("libfdk_aac");
    if (!codec) {
        qDebug() << "decoder libfdk_aac not found";
        return;
    }

    // 初始化解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        qDebug() << "av_parser_init error";
        return;
    }

    // 创建上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        goto end;
    }

    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "av_packet_alloc error";
        goto end;
    }

    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }


    // 打开解码器
    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_open2 error" << buffer;
        goto end;
    }

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error:" << inFilename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error:" << out.filename;
        goto end;
    }


    inLen=inFile.read(inData,IN_DATA_SIZE);
    while(inLen>0)
    {
        ret=av_parser_parse2(parserCtx,ctx,
                             &pkt->data,&pkt->size,
                             (uint8_t*)inData,inLen,
                              AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if(ret<0)
        {
            ERROR_BUF(ret);
            qDebug()<<"av_parser_parse2 failed:"<<buffer;
            goto end;
        }
        inData+=ret;
        inLen-=ret;

        if(pkt->size>0&&decode(ctx,pkt,frame,outFile)<0)
        {
            goto end;
        }

        if(inLen<REFILL_THRESH&&!inEnd)
        {
            memmove(inDataArray,inData,inLen);
            inData=inDataArray;

            int len=inFile.read(inData+inLen,IN_DATA_SIZE-inLen);
            if(len>0)
            {
                inLen+=len;
            }
            else
            {
                inEnd=1;
            }
        }

    }
    decode(ctx, nullptr, frame, outFile);


    // 设置输出参数
    out.sampleRate = ctx->sample_rate;
    out.sampleFmt = ctx->sample_fmt;
    out.chLayout = ctx->channel_layout;


end:

    inFile.close();
    outFile.close();
    av_frame_free(&frame);
    av_packet_free(&pkt);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);


}
