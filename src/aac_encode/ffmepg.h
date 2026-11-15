#ifndef FFMEPG_H
#define FFMEPG_H
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
typedef  struct{
    const char* filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} Audio_EncodecSpec;

class FFmepg
{
public:
    FFmepg();
    static void aacEncode(Audio_EncodecSpec& in,const char* outFilename);
};

#endif // FFMEPG_H
