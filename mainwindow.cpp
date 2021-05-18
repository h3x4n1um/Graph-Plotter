#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "exprtk.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->Plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->Plotter->xAxis->setLabel("x");
    ui->Plotter->yAxis->setLabel("y");

    connect(ui->Plotter->xAxis,
            static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),    //https://stackoverflow.com/questions/16794695/
            this,
            [this, customPlot = ui->Plotter](QCPRange x_axis){plot(customPlot, x_axis, customPlot->yAxis->range());});
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QVector <QColor> colorArr = {
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::cyan,
    Qt::magenta
};

void MainWindow::plot(QCustomPlot *customPlot, QCPRange x_axis, QCPRange y_axis){
    // math parser
    exprtk::symbol_table<double> symbol_table;
    QVector <exprtk::expression<double>> expressionArr;
    exprtk::parser<double> parser;

    double x_exp;
    symbol_table.add_variable("x", x_exp);

    // find QLineEdit
    QList<QLineEdit *> funcArr = ui->FuncBox->findChildren<QLineEdit *>();
    for (int i = 0; i < funcArr.size(); ++i){
        // parse function
        expressionArr.push_back(exprtk::expression<double>(symbol_table));
        parser.compile(funcArr.at(i)->text().toStdString(), expressionArr[i]);

        // generate point data
        const int seg = 100; // plot 100 points of function
        QVector<double> x(seg), y(seg); // initialize with entries 0..seg
        for (int j = 0; j < seg; ++j){
          x_exp = j*(x_axis.upper - x_axis.lower)/seg + x_axis.lower; // x goes from lower to upper
          x[j] = x_exp;
          y[j] = expressionArr[i].value();
        }

        // create graph and assign data to it
        if (customPlot->graphCount() <= i) customPlot->addGraph(); // create graph if not exist
        customPlot->graph(i)->setPen(QPen(colorArr[i%colorArr.size()]));
        customPlot->graph(i)->setData(x, y, true);
        customPlot->graph(i)->setName(funcArr.at(i)->text());
    }

    // remove obsolete graph
    for (int i = funcArr.size(); i < customPlot->graphCount(); ++i) customPlot->removeGraph(i);

    // show legend
    customPlot->legend->setVisible(funcArr.size());

    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(x_axis);
    customPlot->yAxis->setRange(y_axis);

    // set QLineEdit
    ui->XMin->setText(QString::number(x_axis.lower));
    ui->XMax->setText(QString::number(x_axis.upper));
    ui->YMin->setText(QString::number(y_axis.lower));
    ui->YMax->setText(QString::number(y_axis.upper));

    // plot
    customPlot->replot();
}

void MainWindow::on_BtnPlot_clicked(){
    plot(ui->Plotter,
         QCPRange(ui->XMin->text().toDouble() ,ui->XMax->text().toDouble()),
         QCPRange(ui->YMin->text().toDouble() ,ui->YMax->text().toDouble()));
}

void MainWindow::on_BtnAddFunc_clicked(){
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QLabel *LblFunc = new QLabel(ui->FuncBox);
    QLineEdit *Func = new QLineEdit(ui->FuncBox);
    QPushButton *DelFunc = new QPushButton(ui->FuncBox);

    LblFunc->setText("f(x)=");
    LblFunc->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    Func->setMaximumWidth(ui->XMin->maximumWidth());
    DelFunc->setText("Remove");

    LblFunc->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    Func->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    DelFunc->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    horizontalLayout->addWidget(LblFunc);
    horizontalLayout->addWidget(Func);
    horizontalLayout->addWidget(DelFunc);

    connect(DelFunc,
            &QPushButton::clicked,
            horizontalLayout,
            [layout = horizontalLayout](){
                // remove item in layout
                QLayoutItem *item;
                while(item = layout->takeAt(0)) item->widget()->deleteLater();
                // remove layout
                layout->deleteLater();
    });

    ui->FuncLayout->insertLayout(ui->FuncLayout->count(), horizontalLayout);
}
