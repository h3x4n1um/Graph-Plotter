#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"

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

    void plot(QCustomPlot *customPlot, QCPRange x_axis, QCPRange y_axis);

private slots:
    void on_BtnPlot_clicked();
    void on_BtnAddFunc_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
