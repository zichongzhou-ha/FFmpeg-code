#include<QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playthread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void show_Version()
{
    SDL_version version;
    SDL_VERSION(&version);
    qDebug()<<version.major<<version.minor<<version.patch;
}

void MainWindow::on_pushButton_clicked()
{
    PlayThread* playThread=new PlayThread(this);
    playThread->start();
}
