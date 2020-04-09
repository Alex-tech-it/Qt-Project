#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_ButtonAnswer_clicked();


private:
    Ui::MainWindow *ui;
    void MainWindow::quickSort(float* array, QChar* ch_array,int left, int right);

};
#endif // MAINWINDOW_H
