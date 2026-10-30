#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audio_thread.h"

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


void MainWindow::on_AudioButton_clicked()
{
    Audio_thread* thread=new Audio_thread(this);
    thread->start();
    //为了防止阻塞UI主线程

}
