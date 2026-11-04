#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    if(!thread)
    {
        thread=new Audio_thread(this);
        thread->start();
        //为了防止阻塞UI主线程
        ui->AudioButton->setText(u8"结束录音");
    }
    else
    {
//        thread->setstop(true);
        thread->requestInterruption();//相当于stop那套机制
        thread=nullptr;
        ui->AudioButton->setText(u8"开始录音");
    }


}
