#ifndef AUDIO_THREAD_H
#define AUDIO_THREAD_H

#include <QObject>
#include <QThread>
#include<QDebug>
#include<QFile>
extern "C"
{
#include<libavcodec/avcodec.h>
#include<libavdevice/avdevice.h>
#include<libavutil/avutil.h>
#include<libavcodec/avcodec.h>
}
class Audio_thread : public QThread
{
    Q_OBJECT
private:
    void run() override;
    bool _stop;
public:
    explicit Audio_thread(QObject *parent = nullptr);
    void setstop(bool stop);
    ~Audio_thread();
};

#endif // AUDIO_THREAD_H
