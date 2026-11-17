#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>
#include <QThread>
#include<QFile>
#include<QtDebug>
#define SDL_MAIN_HANDLED
#include "SDL.h"

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
