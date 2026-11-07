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
     thread=new Audio_thread(this);
     thread->start();
}
