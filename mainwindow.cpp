#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPen>
#include <iostream>
#include <math.h>

using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), explorer(this,"Seleccione archivo","C:\\","*.vec")
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //parser.DefineFun(L"seno",sin);
    connect(ui->cant_of_points,SIGNAL(returnPressed()),this,SLOT(on_plotter_clicked()));
    connect(ui->ScanX_max,SIGNAL(returnPressed()),this,SLOT(on_plotter_clicked()));
    connect(ui->ScanX_min,SIGNAL(returnPressed()),this,SLOT(on_plotter_clicked()));
    connect(ui->plotter_line_edit,SIGNAL(returnPressed()),this,SLOT(on_plotter_clicked()));
    //integral
    connect(ui->integral_button,SIGNAL(clicked()),this,SLOT(integral()));
    ui->comboBox->addItem("Maximo y minimo");
    ui->comboBox->addItem("Raiz");
    ui->comboBox->addItem("Area");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_plotter_clicked()
{
    qDebug() << "ploteando";
    if(ui->cant_of_points->text().isEmpty() == true){
        ui->statusbar->showMessage("cantidad de puntos vacia",2500);
        return;
    }
    if(ui->plotter_line_edit->text().isEmpty() == true){
        ui->statusbar->showMessage("funcion vacia",2500);
        return;
    }
    if(ui->ScanX_min->text().isEmpty() == true){
        ui->statusbar->showMessage("Valor maximo de X esta vacio",2500);
        return;
    }
    if(ui->ScanX_max->text().isEmpty() == true){
        ui->statusbar->showMessage("Valor minimo de X esta vacio",2500);
        return;
    }
    len = ui->cant_of_points->text().toInt();
    x.resize(len);  //define tam vector dinamico X
    y.resize(len);  //define tam vector dinamico Y
    QString x_max;
    QString x_min;
    x_max = ui->ScanX_max->text();
    x_min = ui->ScanX_min->text();
    xmin = x_min.toDouble();
    xmax = x_max.toDouble();
    delta_x = ((xmax - xmin)/ (double)len);
    //cargo el vector X con valores
    for (int i=0; i < len ; i++){
        x[i] = xmin + double(i) * delta_x;
       // y[i] = pow(x[i],2.0);
    }
    //define que el texto (x) es la variable
    parser.DefineVar(L"x",x.data());
    //rescato la expresion ingresada en el plotter
    QString func;
    func = ui->plotter_line_edit->text();
    //convertir la expresion en funcion matematica (a estandar width string)
    parser.SetExpr(func.toStdWString());
    try {
        //evaluo el dominio en la func y los guardo en Y
        parser.Eval(y.data(),len);
    } catch (...) {
        ui->statusbar->showMessage("funcion invalida",5000);
        return;
    }
    //agrega grafico al plot
    ui->graphics_output->addGraph();
    //cargar datos al grafico
    ui->graphics_output->graph()->setData(x,y);
    //seteo rango en los ejes
    //ui->graphics_output->xAxis->setRange(xmin,xmax);
    //ui->graphics_output->yAxis->setRange(ymin,ymax);
    ui->graphics_output->rescaleAxes();
    QStringList random_color;
    //cambiar color graficas
    ui->graphics_output->graph()->setPen(QPen(Qt::darkGreen));
    //dar nombre a los ejes
    ui->graphics_output->xAxis->setLabel("X");
    ui->graphics_output->yAxis->setLabel("f(x)");
    //agrego interacciones al grafico
    ui->graphics_output->setInteraction(QCP::iRangeDrag);
    ui->graphics_output->setInteraction(QCP::iRangeZoom);
    //pido que plotee la grafica
    ui->graphics_output->replot();
}

void MainWindow::on_clear_button_clicked()
{
    ui->graphics_output->clearGraphs();
    ui->graphics_output->replot();
}

void MainWindow::integral()
{
    //metodo del rectangulo sin recursividad
    if(ui->plotter->text().isEmpty() == true)
        return;
    double x1 = xmin;//-0.25;
    double x2 = xmax;//0.75;
    int index_x1;
    int index_x2;
    //rescato los indices del vector
    index_x1 = (x1 - xmin) / delta_x;
    index_x2 = (x2 - xmin) / delta_x;
    double area = 0.0;
    double area_trapecio = 0.0;
    for(int i = index_x1 ; i < index_x2 ; i++)
        area = area + y[i]*delta_x;
    //metodo del trapecio
    for(int i = index_x1 ; i < index_x2-1 ; i++)
       area_trapecio = area_trapecio + ((y[i] + y[i+1])/2.0) * delta_x;
    QString area1;
    area1 ="el area es: " + QString::number(area_trapecio);
    msg.setText(area1);
    msg.exec();
    return;
}
//metodo de newton- rawson
double MainWindow::NewtonRapson(double xi)//raiz
{
    int iterations = 0;
    //creo un nuevo parser, ya que el anterior esta ocupado para el grafico
    mu::Parser parser;
    //seteo la expresion
    QString func;
    func = ui->plotter_line_edit->text();
    parser.SetExpr(func.toStdWString());
    //solo es una variable la que debo evaluar
    double x;
    parser.DefineVar(L"x",&x);
    //derivada //ecuacion de la recta mx + b
    //definicion de derivada  f(x+dx)-f(x) / dx
    double m;
    double dy,dx = 1e-12;
    double fx; //f(x)
    double fxdx; //f(x+dx)
    while(iterations < 10){
        x = xi + dx;
        parser.Eval(&fxdx,1);

        x = xi;
        parser.Eval(&fx,1);

        dy = fxdx - fx;
        m = dy / dx ;
        iterations++;
        if(fabs(fx) < 1e-10)
            return xi;
        xi = -fx/m + xi;
    }
    return xi;
}

double MainWindow::area(double x1, double x2)
{
    //metodo del rectangulo sin recursividad
    int index_x1;
    int index_x2;
    //rescato los indices del vector
    index_x1 = (x1 - xmin) / delta_x;
    index_x2 = (x2 - xmin) / delta_x;
    //double area = 0.0;
    double area_trapecio = 0.0;
   // for(int i = index_x1 ; i < index_x2 ; i++)
   //     area = area + y[i]*delta_x;
    //metodo del trapecio
    for(int i = index_x1 ; i < index_x2-1 ; i++)
       area_trapecio = area_trapecio + ((y[i] + y[i+1])/2.0) * delta_x;

    return area_trapecio;
    qDebug() << "el area trapecio es : " << area_trapecio;
}

double MainWindow::biseccion(double xi, double xf)
{

    mu::Parser parser;
    QString func;
    func = ui->plotter_line_edit->text();
    //seteo la funcion
    parser.SetExpr(func.toStdWString());
    //definir variable
    double x;
    parser.DefineVar(L"x",&x); //reemplaza la variable x de la funcion por mi variable x(por el valor que contenga)
    //determinar si hay cambio de signo
    double fxi; //f(xi)
    x = xi;
    parser.Eval(&fxi,1);
    double fxf; //f(xf)
    x = xf;
    parser.Eval(&fxf,1);
    while(true){
        //verifico si hubo un cruce por 0
        if( fxi * fxf < 0){
            //hay raiz
            double fxm; //f(xm)
            double xm = (xf + xi)/2.0;
            x = xm;
            parser.Eval(&fxm,1);
            if( fxi * fxm < 0){
                //raiz a la izq
                xf = xm;
            }
            else{//raiz a la derecha
                xi = xm;
            }
            if(fabs(fxm) < 1e-9)
                return xm;
        }
        else{
            //no hay raiz
            throw ("no hay raiz");
        }
    }
}

QVector<double> MainWindow::derivada(QVector<double> &x, QVector<double> &y, int len)
{
    QVector<double> derivate;
    derivate.resize(len);
    for(int i = 0 ; i < len-1 ;i++){
        derivate[i] = ((y[i+1]-y[i]) / (x[i+1]-x[i]));
    }
    return derivate;
}

void MainWindow::extremos()
{
    mu::Parser parser;
        QString func;

        func = ui->plotter_line_edit->text();
        parser.SetExpr(func.toStdWString());

        double xi = ui->x1_lineedit->text().toDouble();
        double xf = ui->x2_lineedit->text().toDouble();
        double x = xi;

        parser.DefineVar(L"x",&x);

        double m;
        //calculo la derivada
        double dy;
        double dx=1e-3;
        double fx;
        double fxdx;//f(x+dx);
        double m_ant, m_sig;
        while(xi<xf)
        {
            x = xi + dx;
            parser.Eval(&fxdx,1);

            x=xi;
            parser.Eval(&fx,1);
            dy=fxdx-fx;
            m=dy/dx; //pendiente

            if(fabs(m)< 1e-3)
            {
                x=xi;
                parser.Eval(&fxdx,1);

                x=xi-dx;
                parser.Eval(&fx,1);

                dy=fxdx-fx;
                m_ant=dy/dx;

                x=xi+(2*dx);
                parser.Eval(&fxdx,1);
                x=xi+dx;
                parser.Eval(&fx,1);
                dy=fxdx-fx;
                m_sig=dy/dx;

                if( m_ant < 0 && m_sig > m )
                {
                    QString msj;
                    msj =  "Hay minimo en x= ";
                    ui->statusbar->showMessage(msj + QString::number(xi));
                }

                if( m_ant > m && m_sig < 0 )
                {
                    QString msj;
                    msj =  "Hay maximo en x= ";
                    ui->statusbar->showMessage(msj + QString::number(xi));

                }
            }
             xi=xi+dx;
        }
}

void MainWindow::on_calcular_button_clicked()
{
    if(ui->x1_lineedit->text().isEmpty() == true){
        msg.setText("Por favor complete el campo X1");
        msg.exec();
        return;}
    if(ui->x2_lineedit->text().isEmpty() == true){
        msg.setText("Por favor complete el campo x2");
        msg.exec();
        return;}
    int value;
    double area_func;
    double raiz;
    value = ui->comboBox->currentIndex();
    double x1,x2;
    x1 = ui->x1_lineedit->text().toDouble();
    x2 = ui->x2_lineedit->text().toDouble();
    if(value == 0)//extremos
    {
        extremos();
    }
    if(value == 1){
        if(x1 == 0)
            x1 = x1 + 0.003;
        raiz = biseccion(x1,x2);
        QString aux;
        aux = "la raiz hallada proxima a [ "+QString::number(x1) + "] es: " + QString::number(raiz);
        msg.setText(aux);
        raiz = NewtonRapson(x1);
        QString aux1;
        aux1 = "la raiz hallada por newton-rapson proxima a [ "+QString::number(x1) + "] es: " + QString::number(raiz);
        msg.setText(aux1);
        msg.exec();
    }
    if(value == 2){
        try {
            area_func = area(x1,x2);
            QString aux;
            aux = "el area entre el intervalo [ "+ QString::number(x1)+" ; "+QString::number(x2)+" ] es: "
                    + QString::number(area_func);
            msg.setText(aux);
            msg.exec();
        } catch (...) {
            ui->statusbar->showMessage("no hay raices",7000);
        }

    }
}

void MainWindow::on_actionOpen_triggered()
{
    FILE *f;
    f =fopen("C:\\Users\\Reaper\\Desktop\\proyecto.dat","rb");
    if(f == nullptr){
        msg.setText("no se pudo abrir archivo");
        msg.exec();
        return;
    }
    fread(&len,sizeof (double),1,f);
    x.resize(len);
    y.resize(len);
    fread(x.data(),sizeof(double),len,f);
    fread(y.data(),sizeof(double),len,f);
    ui->graphics_output->addGraph();
    ui->graphics_output->graph()->setData(x,y);
    ui->graphics_output->rescaleAxes();
    ui->graphics_output->graph()->setPen(QPen(Qt::darkRed));
    ui->graphics_output->xAxis->setLabel("X");
    ui->graphics_output->yAxis->setLabel("f(x)");
    ui->graphics_output->setInteraction(QCP::iRangeDrag);
    ui->graphics_output->setInteraction(QCP::iRangeZoom);
    ui->graphics_output->replot();
    fclose(f);
    return;
}

void MainWindow::on_actionSave_triggered()
{
    FILE *f;
    f =fopen("C:\\Users\\Reaper\\Desktop\\proyecto.dat","wb");
    if(f == nullptr){
        msg.setText("no se pudo abrir archivo");
        msg.exec();
        return;
    }
    fwrite(&len,sizeof(double),1,f);
    fwrite(x.data(),sizeof(double),len,f);
    fwrite(y.data(),sizeof(double),len,f);
    fclose(f);
    return;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}
