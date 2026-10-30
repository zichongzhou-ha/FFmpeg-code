#include "mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    qDebug()<< u8"MainWindow被创建";
    setWindowTitle("主窗口");

}

MainWindow::~MainWindow()
{
//    qDebug()<< u8"MainWindow被销毁";
}

