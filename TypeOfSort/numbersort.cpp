#include "numbersort.h"
#include "ui_numbersort.h"
#include <QMessageBox>
#include <QString>
#include <QKeyEvent>
#include <QDebug>
#include <QVector>

NumberSort::NumberSort(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NumberSort)
{
    ui->setupUi(this);
}

NumberSort::~NumberSort()
{
    delete ui;
}


void NumberSort::on_SortArray_clicked()
{
    QString buff;
    QString check;
    QVector<int> array ;

    // Creating File:"output.txt"
    QFile Input("input.txt");
    if(!Input.open(QIODevice::ReadOnly|QIODevice::Text|QIODevice::Truncate)){
        ui->statusBar->showMessage("Файл 'output.txt' не был создан.");
    }
    QTextStream fromin(&Input);





}

void NumberSort::on_CreatArray_clicked()
{

   bool ok;
   int buff;
   QString str;
   QString Number = ui->lineEdit->text();
   N = Number.toInt(&ok,10);

   // Cheaking of N - amount of numbers
   if(!ok){
       ui->statusBar->showMessage("Ошибка! Введено не число");
       return;
   }else if(N <= 0){
           ui->statusBar->showMessage("Число N должно быть больше 0");
           return;
   }
   ui->statusBar->clearMessage();

   //Creating File:"input.txt"

   QFile Input("input.txt");
   if(!Input.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
       ui->statusBar->showMessage("Файл 'input.txt' не был создан.");
   }

   // Changing of stream
   QTextStream writestream(&Input);

   // Creating and writing random number of 1 to 1 million
   for(int i = 0; i < N; i++){
       buff = rand()%1000000 + 1;
       str.setNum(buff);
       str += " ";
       writestream << str;
   }

   // Show that work is done
   str = "";
   str += QString::number(N) + " чисел было сгенерированно. Вы можете посмотреть файл по пусти" +
           "'C:/QtProjects/Qt-Project/build-TypeOfSort-Desktop_Qt_5_12_2_MSVC2017_32bit-Debug' ";

   QMessageBox::information(this,"Info",str);

   Input.close();
}

