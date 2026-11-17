#ifndef AUDIO_THREAD_H
#define AUDIO_THREAD_H

#include <QObject>
#include <QThread>
#include<QDebug>
#include<QFile>


class Video_thread : public QThread
{
    Q_OBJECT
private:
    void run() override;
public:
    explicit Video_thread(QObject *parent = nullptr);
    ~Video_thread();
};

#endif // AUDIO_THREAD_H
