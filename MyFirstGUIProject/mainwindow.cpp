#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QString>

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


void MainWindow::on_pushButton_clicked()
{
    QString Login = ui->login->text() ;
    QString Password = ui->password->text();
    if( Login == "Гольчевский" && Password == "Сдал все программы"){
        QMessageBox::information(this,"Вход","Вы успешно вошли в Гольчевского");
    }else{
        QMessageBox::warning(this,"Вход","Вас спалил Гольчевский, вы отчислены! Собирайте свои манатки и "
                                         "проваливаете");
    }
}
