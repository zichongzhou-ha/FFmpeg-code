#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTime>

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

        connect(thread,&Audio_thread::timeChanged,
                [this](unsigned long long ms){
            QTime time(0,0,0,0);
            QString timeText=time.addMSecs(ms).toString("mm:ss.z");

            ui->label->setText(timeText);
        });
    }
    else
    {
//        thread->setstop(true);
        thread->requestInterruption();//相当于stop那套机制
        thread=nullptr;
        ui->AudioButton->setText(u8"开始录音");
    }


}
