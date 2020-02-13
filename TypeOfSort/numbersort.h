#ifndef NUMBERSORT_H
#define NUMBERSORT_H

#include <QMainWindow>
#include <QFile>
#include <QTime>
#include <QString>
QT_BEGIN_NAMESPACE
namespace Ui { class NumberSort; }
QT_END_NAMESPACE

class NumberSort : public QMainWindow
{
    Q_OBJECT

public:
    NumberSort(QWidget *parent = nullptr);
    ~NumberSort();

private slots:
    void on_SortArray_clicked();

    void on_CreatArray_clicked();

private:
    Ui::NumberSort *ui;
    int N;
};
#endif // NUMBERSORT_H
