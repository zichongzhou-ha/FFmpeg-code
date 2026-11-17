#include<QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playthread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _playthread(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    _playthread=new PlayThread(this);
    _playthread->start();


}
