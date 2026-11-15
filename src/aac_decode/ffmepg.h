#ifndef FFMEPG_H
#define FFMEPG_H
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} AudioDecodeSpec;
class FFmepg
{
public:
    FFmepg();

    static void aacDecode(const char* inFilename,
                          AudioDecodeSpec &out);
};

#endif // FFMEPG_H
