#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>
#include <QThread>
#include<QFile>
#include<QtDebug>
#define SDL_MAIN_HANDLED
#include "SDL.h"
#define FILE_NAME "D:/qt/AV/test/test.pcm"
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 16
#define CHANNELS 2
#define SAMPLES 1024
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE*CHANNELS)/8)
#define BUFFER_SIZE (SAMPLES*BYTES_PER_SAMPLE)
class PlayThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    void run() override;
private:
    ~PlayThread();
signals:

};

#endif // PLAYTHREAD_H
