#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMap>
#include <QVector>
#include <QVectorIterator>
#include <QDebug>
#include <QtAlgorithms>
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

/* Structer that saved each symbol,chance,bin_code; x,y - rendering*/

struct Frequence{
public:
       float chance;
       int x;
       int y;
       QChar symbol;
       QString bin_code ;

       bool CheckSymbolInBinCode(QChar chr){
           for(int i = 1; i < bin_code.size()-1; i++){
               if(bin_code[i-1] == "'" && bin_code[i+1] == "'"){
                 if(bin_code[i] == chr){
                     return false;
                 }
               }
           }
           return true;
       }
       void OnGoToSymbol(int count){
           for(int i = bin_code.size();  i > count; i--){
                bin_code[i] = bin_code[i-1];
                qDebug() << bin_code;
           }
       }
       void AddBinarySymbol(QChar bin){


           for(int i = 1; i < bin_code.size() - 1 ; i++){
               if(bin_code[i-1] == "'" && bin_code[i+1] == "'"){
                  OnGoToSymbol(i+2);
                  bin_code[i+2] = bin;
               }
           }

       }
       void AddCharacher(QString chr){
           bin_code += chr;
       }

       Frequence& operator=(const Frequence &s){
           chance = s.chance;
           symbol = s.symbol;
           x = s.x;
           y = s.y;
           return(*this);
       }
   };

void VectorSort(QVector<Frequence>& vector){
    Frequence temp;
    for(int j= 0; j < vector.size() - 1;j++){
        for(int g = 0; g < (vector.size() - j - 1); g++){
            if(vector[g].chance > vector[g+1].chance){
                temp = vector[g+1];
                vector[g+1] = vector[g];
                vector[g] = temp;
            }
           }
    }

}

bool CheckVector(QVector<Frequence>& vector){
    for( int j = 0; j < vector.size(); j++){
        if(vector[j].chance > 1){
            qDebug() << "Ошибка в алгоритме : vector[j].chance > 1  - " << vector[j].chance << endl;
            return true;
        }
        if(vector[j].chance != 1){
            return false;
        }
    }
    return true;
}


void MainWindow::on_ButtonAnswer_clicked(){

    QString answer = ui->EditAnswer->text();
    QHash<QChar,float> Emplyees;            // Dictionary symbols
    QHash<QChar,float> Extra;               // Intermediate dictionary
    QVector<Frequence> vector;              // Vector of structers in which we will store data


    int count_symbols = 0;
    int symbols = 0;


    /* Empty offer check */

    if(answer == "" || answer.size() == 1){
        ui->statusbar->showMessage("Не введено предложение",5000);
        return;
    }

    /* Filling in the dictionary */

    for(int i = 0; i < answer.size(); i++){
        if (Emplyees.contains(answer[i])){
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
        if(Extra.contains(answer[j])){
            continue;
        }else{
            Extra.insert(answer[j],Emplyees.value(answer[j]));
            float buff = Emplyees.value(answer[j]) / count_symbols;
            Emplyees.insert(answer[j],buff);
        }
    }
    Extra.clear();


    /* Console dictionary output and input in list */

    QHashIterator<QChar,float>i(Emplyees);
    while(i.hasNext()){
        i.next();
        qDebug() << i.key() << ": " << i.value();
        Frequence fre;
        fre.chance = i.value();
        fre.symbol = i.key();
        vector.push_back(fre);

    }


    /* First step - sorting elements/chances */

    VectorSort(vector);


    qDebug() << "Check VectorSort " << endl;
    for(int j = 0; j < vector.size(); j++){
        qDebug() << "chance = " << vector[j].chance << " : symbol = " << vector[j].symbol;
    }

    qDebug() << "====Отсортированный массив====" << endl;


    /* Creating scene for drawing lines*/

    int width = ui->graphicsView->width();
    int height = ui->graphicsView->height();
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setSceneRect(ui->graphicsView->x()+5,ui->graphicsView->y()+20,width,height);
    ui->graphicsView->setScene(scene);


    /* Initialization  of variables that will be used in rendering */

    int step_width = (width)/(vector.size() + 1);
    int plus_width = step_width;
    int step_hight = height - 80;

    /* Drawing first line with elements in vector*/

    for(int j = 0; j < vector.size(); j++){
        if(vector[j].symbol == ' '){
            QGraphicsTextItem *text = scene->addText("Space"
                                                     );
            text->setPos(plus_width ,step_hight);
            QGraphicsTextItem *text2 = scene->addText(QString::number(vector[j].chance,'g',3));
            text2->setPos(plus_width ,step_hight+20);

            vector[j].x = plus_width;
            vector[j].y = step_hight;

            plus_width += step_width;
        }else{

            QGraphicsTextItem *text = scene->addText(vector[j].symbol);
            text->setPos(plus_width ,step_hight);
            QGraphicsTextItem *text2 = scene->addText(QString::number(vector[j].chance,'g',3));
            text2->setPos(plus_width ,step_hight+20);

            vector[j].x = plus_width;
            vector[j].y = step_hight;

            plus_width += step_width;
        }


    }


    /* First fixed  width and hight between branches*/
    int add_width = 80;
    int add_height = 80;

    while(!CheckVector(vector)){

        /* Gnenerating new width between each branch*/

        add_width = rand() % 101 +70;


        /* Loop for the searching 2 min elements */

        float min = 1;
        int min_j = 0;
        float min_2 = 1;
        int min_2_j = 0;
        for(int j = 0; j < vector.size(); j++){
            if(vector[j].chance < min){
                min_2 = min;
                min_2_j = min_j;

                min = vector[j].chance;
                min_j = j;

            }else if (vector[j].chance < min_2){
                min_2 = vector[j].chance;
                min_2_j = j;
            }

        }


        /* Consle output */

        qDebug() << " min = " << min << "(min_j = " << min_j << ") : min_2 = " << min_2 << "(min_2_j = " << min_2_j << ")" << endl;


        /* Cases of rendering breches */

        if(vector[min_j].y == vector[min_2_j].y && vector[min_j].x < vector[min_2_j].x){        // ---Case 1---


             /* Console output */

             qDebug() << " ---- if = 1.1 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;


             /* Rendering line on scnece between the first min and the second min */

             scene->addLine(vector[min_j].x,vector[min_j].y,vector[min_j].x + add_width,vector[min_j].y - add_height);
             vector[min_j].x += add_width;
             vector[min_j].y -= add_height;
             scene->addLine(vector[min_2_j].x,vector[min_2_j].y,vector[min_j].x,vector[min_j].y);

             /* Create tree branches and draw them */

             if(vector[min_j].chance >= vector[min_2_j].chance){
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 /* Store binaru code each symbol*/

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 1.1" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('1');
                     qDebug() << vector[min_j].bin_code << " if -- 1.2" << endl;
                 }


                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 /* Store binaru code each symbol*/

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 1.3" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('2');
                     qDebug() << vector[min_2_j].bin_code << " if -- 1.4" << endl;
                 }

             }else{
                 QGraphicsTextItem *text1 = scene->addText("(0)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 1.5" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('2');
                     qDebug() << vector[min_j].bin_code << " if -- 1.6" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(1)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 1.7" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('1');
                     qDebug() << vector[min_2_j].bin_code << " if -- 1.8" << endl;
                 }
             }

             /* Console output */

             qDebug() << " ---- if = 1.2 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;

         }else if(vector[min_j].y == vector[min_2_j].y && vector[min_j].x > vector[min_2_j].x){ // ---Case 2---

             /* Console output */

            qDebug() << " ---- if = 2.1 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;


             /* Rendering line on scnece between the first min and the second min */

             scene->addLine(vector[min_j].x,vector[min_j].y,vector[min_j].x - add_width,vector[min_j].y - add_height);
             vector[min_j].x -= add_width;
             vector[min_j].y -= add_height;
             scene->addLine(vector[min_2_j].x,vector[min_2_j].y,vector[min_j].x,vector[min_j].y);


             if(vector[min_j].chance >= vector[min_2_j].chance){
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 2.1" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('1');
                     qDebug() << vector[min_j].bin_code << " if -- 2.2" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 int avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 int avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 2.3" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('2');
                     qDebug() << vector[min_2_j].bin_code << " if -- 2.4" << endl;
                 }

             }else{
                 QGraphicsTextItem *text1 = scene->addText("(0)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 2.5" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('2');
                     qDebug() << vector[min_j].bin_code << " if -- 2.6" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(1)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 2.7" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('1');
                     qDebug() << vector[min_2_j].bin_code << " if -- 2.8" << endl;
                 }
             }
             /* Console output */

             qDebug() << " ---- if = 2.2 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;

         }else if(vector[min_j].y < vector[min_2_j].y && vector[min_j].x < vector[min_2_j].x){  // ---Case 3---

             /* Console output */

             qDebug() << " ---- if = 3.1 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl;
             qDebug() << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;


             /* Rendering line on scnece between the first min and the second min */

             scene->addLine(vector[min_j].x,vector[min_j].y,vector[min_j].x + add_width,vector[min_j].y - add_height);
             vector[min_j].x += add_width;
             vector[min_j].y -= add_height;
             scene->addLine(vector[min_2_j].x,vector[min_2_j].y,vector[min_j].x,vector[min_j].y);

             if(vector[min_j].chance >= vector[min_2_j].chance){
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 3.1" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('1');
                     qDebug() << vector[min_j].bin_code << " if -- 3.2" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 3.3" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('2');
                     qDebug() << vector[min_2_j].bin_code << " if -- 3.4" << endl;
                 }

             }else{
                 QGraphicsTextItem *text1 = scene->addText("(0)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 3.5" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('2');
                     qDebug() << vector[min_j].bin_code << " if -- 3.6" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(1)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 3.7" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('1');
                     qDebug() << vector[min_2_j].bin_code << " if -- 3.8" << endl;
                 }

             }

             /* Console output */

             qDebug() << " ---- if = 3.2 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;

         }else if(vector[min_j].y < vector[min_2_j].y && vector[min_j].x > vector[min_2_j].x){  // ---Case 4---

             /* Console output */

             qDebug() << " ---- if = 4.1 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;


             /* Rendering line on scnece between the first min and the second min */

             scene->addLine(vector[min_j].x,vector[min_j].y,vector[min_j].x - add_width,vector[min_j].y - add_height);
             vector[min_j].x -= add_width;
             vector[min_j].y -= add_height;
             scene->addLine(vector[min_2_j].x,vector[min_2_j].y,vector[min_j].x,vector[min_j].y);

             if(vector[min_j].chance >= vector[min_2_j].chance){
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 int avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 4.1" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('1');
                     qDebug() << vector[min_j].bin_code << " if -- 4.2" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 4.3" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('2');
                     qDebug() << vector[min_2_j].bin_code << " if -- 4.4" << endl;
                 }

             }else{
                 QGraphicsTextItem *text1 = scene->addText("(0)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 4.5" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('2');
                     qDebug() << vector[min_j].bin_code << " if -- 4.6" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(1)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 4.7" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('1');
                     qDebug() << vector[min_2_j].bin_code << " if -- 4.8" << endl;
                 }

             }

             /* Console output */

             qDebug() << " ---- if = 4.2 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;

         }else if(vector[min_j].y > vector[min_2_j].y && vector[min_j].x > vector[min_2_j].x){  // ---Case 5---

             /* Console output */

             qDebug() << " ---- if = 5.1 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;


             /* Rendering line on scnece between the first min and the second min */

             scene->addLine(vector[min_2_j].x,vector[min_2_j].y,vector[min_2_j].x + add_width,vector[min_2_j].y - add_height);
             vector[min_2_j].x += add_width;
             vector[min_2_j].y -= add_height;
             scene->addLine(vector[min_j].x,vector[min_j].y,vector[min_2_j].x,vector[min_2_j].y);
             vector[min_j].x = vector[min_2_j].x;
             vector[min_j].y = vector[min_2_j].y;

             if(vector[min_j].chance >= vector[min_2_j].chance){
                 qDebug() << "fffaaa_1";
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 + 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 + 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 5.1" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('1');
                     qDebug() << vector[min_j].bin_code << " if -- 5.2" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 + 15;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 + 10;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 5.3" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('2');
                     qDebug() << vector[min_2_j].bin_code << " if -- 5.4" << endl;
                 }

             }else{
                 qDebug() << "fffaaa_2";
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                  qDebug() << "avarage_x : " << avarage_x;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                  qDebug() << "avarage_y : " << avarage_y;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 5.5" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('2');
                     qDebug() << vector[min_j].bin_code << " if -- 5.6" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 + 10;
                 qDebug() << "avarage_x_2 : " << avarage_x_2;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 + 5;
                 qDebug() << "avarage_y_2 : " << avarage_y_2;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 5.7" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('1');
                     qDebug() << vector[min_2_j].bin_code << " if -- 5.8" << endl;
                 }

             }
             /* Console output */

            qDebug() << " ---- if = 5.2 ----";
            qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                     << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;

         }else if(vector[min_j].y > vector[min_2_j].y && vector[min_j].x < vector[min_2_j].x){  // ---Case 6---

            /* Console output */

             qDebug() << " ---- if = 6.1 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;


             /* Rendering line on scnece between the first min and the second min */

             scene->addLine(vector[min_2_j].x,vector[min_2_j].y,vector[min_j].x - add_width,vector[min_j].y - add_height);
             vector[min_2_j].x -= add_width;
             vector[min_2_j].y -= add_height;
             scene->addLine(vector[min_j].x,vector[min_j].y,vector[min_2_j].x,vector[min_2_j].y);
             vector[min_j].x = vector[min_2_j].x;
             vector[min_j].y = vector[min_2_j].y;

             if(vector[min_j].chance >= vector[min_2_j].chance){
                 QGraphicsTextItem *text1 = scene->addText("(1)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 6.1" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('1');
                     qDebug() << vector[min_j].bin_code << " if -- 6.2" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(0)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 6.3" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('2');
                     qDebug() << vector[min_2_j].bin_code << " if -- 6.4" << endl;
                 }

             }else{
                 QGraphicsTextItem *text1 = scene->addText("(0)");
                 qreal avarage_x = (vector[min_j].x - add_width + vector[min_j].x) / 2 - 10;
                 qreal avarage_y = (vector[min_j].y + add_height + vector[min_j].y) / 2 - 5;
                 text1->setPos(avarage_x,avarage_y);

                 if(vector[min_j].CheckSymbolInBinCode(vector[min_j].symbol)){
                     QString str = "'";
                     str += vector[min_j].symbol;
                     str = str + "'";
                     str += '2';
                     vector[min_j].AddCharacher(str);
                     qDebug() << vector[min_j].bin_code << " if -- 6.5" << endl;
                     str = "";
                 }else{
                     vector[min_j].AddBinarySymbol('2');
                     qDebug() << vector[min_j].bin_code << " if -- 6.6" << endl;
                 }

                 QGraphicsTextItem *text2 = scene->addText("(1)");
                 qreal avarage_x_2 = (vector[min_2_j].x + vector[min_j].x) / 2 - 10;
                 qreal avarage_y_2 = (vector[min_2_j].y + vector[min_j].y) / 2 - 5;
                 text2->setPos(avarage_x_2,avarage_y_2);

                 if(vector[min_2_j].CheckSymbolInBinCode(vector[min_2_j].symbol)){
                     QString str = "'";
                     str += vector[min_2_j].symbol;
                     str = str + "'";
                     str += '1';
                     vector[min_2_j].AddCharacher(str);
                     qDebug() << vector[min_2_j].bin_code << " if -- 6.7" << endl;
                     str = "";
                 }else{
                     vector[min_2_j].AddBinarySymbol('1');
                     qDebug() << vector[min_2_j].bin_code << " if -- 6.8" << endl;
                 }

             }

             /* Console output */

             qDebug() << " ---- if = 6.2 ----";
             qDebug() << " vector[min_j].x = " << vector[min_j].x << " : vector[min_j].y = " << vector[min_j].y << endl
                      << " vector[min_2_j].x = " << vector[min_2_j].x << " : vector[min_2_j].y = " << vector[min_2_j].x << endl;
         }


        /* Making new branch on tree */

        vector[min_j].chance += vector[min_2_j].chance;
        vector[min_j].bin_code += vector[min_2_j].bin_code;
        vector[min_2_j].chance = 1;


        QGraphicsTextItem *text2 = scene->addText(QString::number(vector[min_j].chance,'g',3));
        text2->setPos(vector[min_j].x - 10 ,vector[min_j].y-20);


        /* Console output */

        for(int j = 0; j < vector.size(); j++){
             qDebug() << "chance = " << vector[j].chance << " : symbol = " << vector[j].symbol;
         }
         qDebug() << "vector size  = " << vector.size() << endl;

         // End while() loop
     }


    int line; // saved number line with binary code in vector

    /* Loop for find line with all binary code */
    for(int j = 0; j < vector.size(); j++){             // going to line symbols
        QString buff = vector[j].bin_code;
        int count = 0;
        for(int g = 1; g < buff.size() - 1; g++){
            // going on symbols in bin_code in vector
            if(buff[g-1] == "'" && buff[g+1] == "'"){

                for(int h = 0; h < vector.size(); h++){     // goint on symbols in vector order to find needed symbol in buff
                    if(buff[g] == vector[h].symbol){
                     count++;
                    }
                }
            }

        }
        if(count == vector.size()){                     // if we fined needed line in vector - break loop
            line = j;
            break;
        }

    }

    /* Rendering codes*/
    int heigh_this = 100;
    int widht_this = 50;
    QString store = vector[line].bin_code;
    if(store.size() <= 12 ){
        int count = 1;
        while(count < store.size()){
                QString str = " - ";
                if(store[count+2] == 2){
                    str += "0";
                }else{
                    str += "1";
                }
                QChar gag = store[count];


                if(gag == ' '){
                    QGraphicsTextItem *text1 = scene->addText("Space");
                    text1->setPos(widht_this,heigh_this);
                    QGraphicsTextItem *text2 = scene->addText(str);
                    text2->setPos(widht_this+40,heigh_this);
                    heigh_this += 20;
                }else{
                    QGraphicsTextItem *text1 = scene->addText(gag);
                    text1->setPos(widht_this,heigh_this);
                    QGraphicsTextItem *text2 = scene->addText(str);
                    text2->setPos(widht_this+10,heigh_this);
                    heigh_this += 20;
                }
                count += 4;
                str = "";
            }
    }else{
    for(int j = 1; j < store.size() - 1; j++){
        if(store[j-1] == "'" && store[j+1] == "'"){
            QString str = " - ";
            QChar gag = store[j];  
            store[j+1] = ' ';
            int count = j+2;
            while(store[count] != "'" && !store[count].isNull()){
                 if(store[count] == '2'){
                     str += '0';
                     count++;
                 }else{
                     str += store[count];
                     count++;
                 }
            }
            if(gag == ' '){
                QGraphicsTextItem *text1 = scene->addText("Space");
                text1->setPos(widht_this,heigh_this);
                QGraphicsTextItem *text2 = scene->addText(str);
                text2->setPos(widht_this+40,heigh_this);
                heigh_this += 20;
            }else{
                QGraphicsTextItem *text1 = scene->addText(gag);
                text1->setPos(widht_this,heigh_this);
                QGraphicsTextItem *text2 = scene->addText(str);
                text2->setPos(widht_this+10,heigh_this);
                heigh_this += 20;
            }
            str = "";
        }

    }
  }
}
