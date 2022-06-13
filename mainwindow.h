#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "muParser.h"
#include <QFileDialog>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //mis modificaciones
private slots:
    void on_plotter_clicked();
    void on_clear_button_clicked();
    void integral();
    void on_calcular_button_clicked();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();

private:
    QVector<double> x; //vector de puntos X
    QVector<double> y; //vector de puntos Y
    int len;           //cantidad de puntos
    double xmin;       //intervalo de grafica X
    double xmax;       //intervalo de grafica Y
    double delta_x;    //variable de desplazamiento
    double ymin;       //intervalo grafica Y
    double ymax;       //intervalo grafica y
    mu::Parser parser; //clase tipo parser
public:
    double NewtonRapson(double xi);
    double area(double x1, double x2);
    double biseccion(double xi, double xf);
    QVector<double> derivada(QVector<double> &x , QVector<double> &y , int len);
    void extremos();
    QFileDialog explorer;
    QMessageBox msg;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
