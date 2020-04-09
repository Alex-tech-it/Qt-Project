#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMap>
#include <QHash>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
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


void MainWindow::on_ButtonAnswer_clicked(){
    /* Call in an offer */
    QString answer = ui->EditAnswer->text();

    /* Empty offer check */
    if(answer == ""){
        ui->statusbar->showMessage("Не введено предложение",5000);
        return;
    }

    qDebug() << answer << endl;
    /* Сreate a dictionary where we'll be tempted by the letters and their probabilities */
    QHash<QChar,float> Emplyees;
    QHash<QChar,float> Extra;
    float count_symbols = 0.0;
    int symbols = 0;

    /* Filling in the dictionary */
    for(int i = 0; i < answer.size(); i++){
        if(answer[i] == ' ') {
            continue;
        }else if (Emplyees.contains(answer[i])){
            count_symbols++;

            int buff = Emplyees.value(answer[i]);
            buff++;
            Emplyees.insert(answer[i],buff);
        }else{
            Emplyees.insert(answer[i],1);
            count_symbols++;
            symbols++;
        }
    }
    ui->statusbar->showMessage("Предложение введено успешно",5000);

    /* Recording the probability of each symbole.*/
    for(int j = 0; j < answer.size(); j++){
        if(answer[j] == ' ' || Extra.contains(answer[j])){
            continue;
        }else{
            Extra.insert(answer[j],Emplyees.value(answer[j]));
            float buff = Emplyees.value(answer[j]) / count_symbols;
            Emplyees.insert(answer[j],buff);
        }
    }
    Extra.clear();

    /* Console dictionary output */
    float *array = new float[symbols];
    QChar *ch_array = new QChar[symbols];
    int while_count_i = 0;

    QHashIterator<QChar,float>i(Emplyees);
    while(i.hasNext()){
        i.next();
        qDebug() << i.key() << ": " << i.value();
        array[while_count_i] = i.value();
        ch_array[while_count_i] = i.key();
        while_count_i++;
    }
    qDebug() << "count_symbos: " << count_symbols << endl;
    qDebug() << "symbos: " << symbols << endl;

    /* Sort step (temporarily sorting) */
    float temp;
    QChar temp_ch;
    for (int j = 0; j < symbols - 1; j++) {
            for (int g = 0; g <  symbols - j - 1; g++) {
                if (array[g] > array[g + 1]) {
                    // меняем элементы местами
                    temp = array[g];
                    temp_ch = ch_array[g];
                    array[g] = array[g + 1];
                    ch_array[g] = ch_array[g + 1];
                    array[g + 1] = temp;
                    ch_array[g + 1] = temp_ch;
                }
            }
    }
    Emplyees.clear();

    /* Creating scene for drawing lines*/
    int width = ui->graphicsView->width();
    int height = ui->graphicsView->height();
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setSceneRect(ui->graphicsView->x()+5,ui->graphicsView->y()+20,width,height);
    ui->graphicsView->setScene(scene);


    int posHeight = height -80;
    int minus = (width) / (symbols+1);
    int plus_pos = minus;
    for (int j = 0; j < symbols; ++j){
         QGraphicsTextItem *text = scene->addText(ch_array[j]);
         text->setPos(plus_pos ,posHeight);

         QGraphicsTextItem *text2 = scene->addText(QString::number(array[j],'g',2));
         text2->setPos(plus_pos ,posHeight+20);
         plus_pos += minus;
     }

}
