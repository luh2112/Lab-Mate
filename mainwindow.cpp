#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <QTimer>
#include <QProgressBar>
#include <QTableView>
#include <QSpinBox>
#include <QDateTime>
#include <QScrollBar>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <qcustomplot.h>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <termios.h>
#include <signal.h>
#include <QClipboard>
#include <QFileDialog>
//#include <QSound>

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>

double pomiarA=0,b,base=1,Scroll_chart=0;
int nPom=0,err=0,t_units=1,a;
struct termios tio;
struct termios stdio;
struct termios old_stdio;
int  tty_fd,data_val, prev_data_val, g, rec_bit = 1,rec_data[4][256], timeout,Byte,point;
double liczba, pomiar;
QString Jednostka,Rz_wiel,prund,plik,clipboard;

QPen blueDotPen;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->textEdit->setStyleSheet ("font: 30pt \"Serif\"; background-color: rgba(255, 255, 255, 0);");
    ui->textEdit_2->setStyleSheet ("font: 20pt \"Serif\"; background-color: rgba(255, 255, 255, 0);");
    ui->customPlot->addGraph();
 ui->customPlot->plotLayout()->insertRow(0);
 ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, "Plot"));
 timer_trigger = new QTimer(this);


 timer = new QTimer(this);
 connect(timer, SIGNAL(timeout()), this, SLOT(ReadSerial()));

 connect(timer_trigger, SIGNAL(timeout()), this, SLOT(on_Aquire_Button_clicked()));

    per_updt = new QTimer(this);


    connect(per_updt, SIGNAL(timeout()), this, SLOT(update_things()));

ui->lcdNumber_4->display("--------");





    model = new QStandardItemModel(0,4,this);
    for(int i=0;i<1000;i++){
        model->setItem(i, 0,new QStandardItem(QString("")));
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("A"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("B"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("C"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Time"));
        MainWindow::refresh();

    }



    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0,80);
    ui->tableView->setColumnWidth(1,80);
    ui->tableView->setColumnWidth(2,80);
    ui->tableView->setColumnWidth(3,100);





    ui->tableView->show();


}
void MainWindow::ReadSerial(){

    // Pobieranie bajtu z portu tty_fd
    if (read(tty_fd, &data_val, 1) > 0) {


        // ekstrakcja numeru odebranego bajtu
        Byte = data_val >> 4;
        // usuwanie starszego nibbla (rozcinanie)
        data_val &= ~(1 << 4);
        data_val &= ~(1 << 5);
        data_val &= ~(1 << 6);
        data_val &= ~(1 << 7);
        // zapis nibbla zawierajacego dane do tablicy
        rec_data[0][Byte] = data_val;

        // odtwarzanie bajtów z zebranych danych
        if (Byte==14) {

            rec_data[1][0] = rec_data[0][3] | rec_data[0][4] << 4;
            rec_data[1][1] = rec_data[0][5] | rec_data[0][6] << 4;
            rec_data[1][2] = rec_data[0][7] | rec_data[0][8] << 4;
            rec_data[1][3] = rec_data[0][9] | rec_data[0][10] << 4;
            rec_data[1][4] = rec_data[0][11] | rec_data[0][12] << 4;
            rec_data[1][5] = rec_data[0][13] | rec_data[0][14] << 4;
            rec_data[1][6] = rec_data[0][1] | rec_data[0][2] << 4;
            Jednostka=Rz_wiel=prund="";
            // dekodowanie znaków
            for (int i = 0; i < 14; i++) {

                Byte = rec_data[1][i];

                rec_data[3][i] = Byte & (1 << 0);

                Byte &= ~(1 << 0);

                switch (Byte) {
                case 190:
                    liczba = 0;
                    Jednostka = "";
                    break;
                case 160:
                    liczba = 1;
                    Jednostka = "";
                    break;
                case 218:
                    liczba = 2;
                    Jednostka = "";
                    break;
                case 248:
                    liczba = 3;
                    Jednostka = "";
                    break;
                case 228:
                    liczba = 4;
                    Jednostka = "";
                    break;
                case 124:
                    liczba = 5;
                    Jednostka = "";
                    break;
                case 126:
                    liczba = 6;
                    Jednostka = "";
                    break;
                case 168:
                    liczba = 7;
                    Jednostka = "";
                    break;
                case 254:
                    liczba = 8;
                    Jednostka = "";
                    break;
                case 252:
                    liczba = 9;
                    Jednostka = "";
                    break;
                case 30:
                    liczba = 0;
                    Jednostka = "°C";
                    break;
                case 78:
                    liczba = 0;
                    Jednostka = "°F";
                    break;

                }
                // dekodowanie jednostek i rzedow wielkosci
                if(rec_data[1][5]& (1 << 6)) Jednostka="V";
                if(rec_data[1][5]& (1 << 5)) Jednostka="A";
                if(rec_data[1][5]& (1 << 2)) Jednostka="F";
                if(rec_data[1][4]& (1 << 5)) Jednostka="Hz";
                if(rec_data[1][4]& (1 << 6)) Jednostka="Ω";
base=1;
                if(rec_data[1][4]& (1 << 4)) {Rz_wiel="n";base=pow(10.00,-9);}
                if(rec_data[1][5]& (1 << 0)){ Rz_wiel="m";base=pow(10.00,-3);}
                if(rec_data[1][5]& (1 << 1)) {Rz_wiel="μ";base=pow(10.00,-6);}
                if(rec_data[1][4]& (1 << 1)){ Rz_wiel="M";base=pow(10.00,6);}
                if(rec_data[1][4]& (1 << 0)){ Rz_wiel="k";base=pow(10.00,3);}

                if(rec_data[1][6]& (1 << 1)) prund="AC";
                if(rec_data[1][6]& (1 << 2)) prund="DC";



                Jednostka=Rz_wiel+Jednostka;




                rec_data[2][i] = liczba;

                // Składanie cyfr w liczbe
                pomiar = ((double) (1000 * rec_data[2][0])
                        + (100 * rec_data[2][1]) + (10 * rec_data[2][2])
                        + (1 * rec_data[2][3]));
                point=0;
                // Korekcja rzedu wielkosci
                if (rec_data[3][0]){

                    pomiar = -pomiar;}

                if (rec_data[3][1]){
                    point=3;
                    pomiar = pomiar / 1000;}

                if (rec_data[3][2]){
                    point=2;
                    pomiar = pomiar / 100;}

                if (rec_data[3][3]){
                    point=1;
                    pomiar = pomiar / 10;}


            }


            if (rec_data[1][3] == 30 || rec_data[1][3] == 78)
                pomiar = pomiar / 10;
  pomiarA= pomiar;
       ui->lcdNumber->display(QString("%1").arg(pomiarA, 3, 10, point,QChar('O')));
       ui->textEdit->setText(Jednostka);
       ui->textEdit_2->setText(prund);

        }








        //   model->setItem(nPom, 0,new QStandardItem(QString::number(pomiarA)));
        //    MainWindow::refresh();
    }



    QString s = QString::number(pomiarA);









}
MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_3_clicked()
{

    int gennr=0;
    for (double gen=ui->lineEdit->text().toDouble();gen<=ui->lineEdit_3->text().toDouble();gen+=ui->lineEdit_2->text().toDouble()){



        model->setItem(gennr+(ui->TCG_Row->value()), ui->TCG_Col->value(),new QStandardItem(QString::number(gen)));
        gennr++;

    }

    MainWindow::refresh();
}
void MainWindow::serial_init(QString port) {

    tcgetattr(STDOUT_FILENO, &old_stdio);

    memset(&stdio, 0, sizeof(stdio));
    stdio.c_iflag = 0;
    stdio.c_oflag = 0;
    stdio.c_cflag = 0;
    stdio.c_lflag = 0;
    stdio.c_cc[VMIN] = 1;
    stdio.c_cc[VTIME] = 0;
    tcsetattr(STDOUT_FILENO, TCSANOW, &stdio);
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    memset(&tio, 0, sizeof(tio));
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = CS8 | CREAD;
    tio.c_lflag = 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;

    QByteArray byteArray = port.toUtf8();
    char* port2 = byteArray.data();

    tty_fd = open(port2, O_RDWR | O_NONBLOCK);

    cfsetospeed(&tio, B9600);
    cfsetispeed(&tio, B9600);

    tcsetattr(tty_fd, TCSANOW, &tio);

}

void MainWindow::on_pushButton_clicked()
{



}
void MainWindow::refresh(){

    QVector<double> x(ui->Chart_to->text().toInt()+1), y(ui->Chart_to->text().toInt()+1);


    for(int i=0;i<=ui->Chart_to->text().toInt();i++){

        y[i]  =  (model ->data(model->index(i, 0),Qt::DisplayRole).toDouble());
        x[i]  =  (model ->data(model->index(i, 1),Qt::DisplayRole).toDouble());

    }

    blueDotPen.setColor(QColor(ui->LCR->value(), ui->LCG->value(), ui->LCB->value(), ui->LCTR->value()));


if (ui->Line_style->currentText()=="Solid"){
    blueDotPen.setStyle(Qt::SolidLine);}
 else{blueDotPen.setStyle(Qt::DotLine);}



    blueDotPen.setWidthF(ui->horizontalSlider->value());
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, ui->MarkerSize->value()));

    ui->customPlot->graph(0)->setPen(blueDotPen);


    ui-> customPlot->xAxis2->setVisible(true);
    ui-> customPlot->yAxis2->setVisible(true);

    ui->customPlot->xAxis2->setTickLabels(false);

    ui->customPlot->yAxis2->setTickLabels(false);
    ui->customPlot->graph(0)->setData(x, y);

    if(ui->Auto_scale->checkState()){ui->customPlot->rescaleAxes();
    }
    else
    {ui-> customPlot->xAxis->setRange(ui->X_LO->text().toDouble()+Scroll_chart, ui->X_HI->text().toDouble()+Scroll_chart);
        ui-> customPlot->yAxis->setRange(ui->Y_LO->text().toDouble(), ui->Y_HI->text().toDouble());}
    if(ui->Labels->checkState()){
        ui-> customPlot->xAxis->setLabel(ui->X_label->text());
        ui-> customPlot->yAxis->setLabel(ui->Y_label->text());
    }else {

        ui-> customPlot->xAxis->setLabel("");
        ui-> customPlot->yAxis->setLabel("");
    }



    ui->customPlot->replot();







}

void MainWindow::on_X_LO_editingFinished()
{
    MainWindow::refresh();
}
void MainWindow::on_X_HI_editingFinished()
{
    MainWindow::refresh();
}
void MainWindow::on_Y_LO_editingFinished()
{
    MainWindow::refresh();
}
void MainWindow::on_Y_HI_editingFinished()
{
    MainWindow::refresh();
}

void MainWindow::on_Aquire_Button_clicked()
{
    QTime time = QTime::currentTime();

    QString timeString = time.toString();
QApplication::beep();

    if(ui->checkBox->isChecked()) {nPom--;


    if(ui->Auto_scroll->isChecked()==true) Scroll_chart-=ui->Scroll_step->text().toDouble();



    }else{


        if(ui->Auto_scroll->isChecked()==true) Scroll_chart+=ui->Scroll_step->text().toDouble();










        nPom++;}

if(ui->Base_units->isChecked()){  model->setItem(nPom-1, ui->Chan_A_dest->value(),new QStandardItem(QString::number((pomiarA*base))));
     }else{
    model->setItem(nPom-1, ui->Chan_A_dest->value(),new QStandardItem(QString::number((pomiarA))));


}


    //model->setItem(nPom-1, ui->Chan_A_dest->value(),new QStandardItem(QString::number((pomiarA)) +" ["+Rz_wiel+"]"  ));
    //model->setItem(nPom, 1,new QStandardItem(QString("-------")));
    //model->setItem(nPom, 2,new QStandardItem(QString("-------")));
    model->setItem(nPom-1, 3,new QStandardItem(QString(timeString)));
    MainWindow::refresh();

    ui->spinBox->setValue(nPom);
    QSortFilterProxyModel proxy;
    proxy.setSourceModel(model);

    ui->tableView->scrollTo( proxy.mapToSource(proxy.index(nPom,0)), QAbstractItemView::EnsureVisible );



}

void MainWindow::on_ref_clicked()
{
    MainWindow::refresh();
}

void MainWindow::on_X_label_editingFinished()
{
    MainWindow::refresh();
}

void MainWindow::on_Y_label_editingFinished()
{
    MainWindow::refresh();
}



void MainWindow::on_Auto_scale_toggled(bool checked)
{

    if(checked==true){ui->customPlot->rescaleAxes();
    }
    else
    {ui-> customPlot->xAxis->setRange(ui->X_LO->text().toDouble(), ui->X_HI->text().toDouble());
        ui-> customPlot->yAxis->setRange(ui->Y_LO->text().toDouble(), ui->Y_HI->text().toDouble());}
    MainWindow::refresh();
}

void MainWindow::on_Labels_toggled(bool checked)
{

    if(checked==true){
        ui-> customPlot->xAxis->setLabel(ui->X_label->text());
        ui-> customPlot->yAxis->setLabel(ui->Y_label->text());
    }else {

        ui-> customPlot->xAxis->setLabel("");
        ui-> customPlot->yAxis->setLabel("");
    }

    MainWindow::refresh();
}

void MainWindow::on_pushButton_2_clicked()
{

clipboard="";
MainWindow::p_Clipboard->setText(clipboard);
for (int i=0;i<=ui->Chart_to->text().toInt();i++){

        clipboard.append(model ->data(model->index(i, 0),Qt::DisplayRole).toString());
        clipboard.append("\t");
        clipboard.append(model ->data(model->index(i, 1),Qt::DisplayRole).toString());
        clipboard.append("\t");
        clipboard.append(model ->data(model->index(i, 2),Qt::DisplayRole).toString());
        clipboard.append("\t");
        clipboard.append(model ->data(model->index(i, 3),Qt::DisplayRole).toString());
        clipboard.append("\n");

    }

    MainWindow::p_Clipboard->setText(clipboard);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    nPom=arg1;
}

void MainWindow::on_Chart_to_editingFinished()
{
    MainWindow::refresh();
}

void MainWindow::on_Chart_from_editingFinished()
{
    MainWindow::refresh();
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{

    if(ui->time_units->currentText()=="ms") t_units=1;
    if(ui->time_units->currentText()=="s") t_units=1000;
    if(ui->time_units->currentText()=="min") t_units=60000;
    if(ui->time_units->currentText()=="hr") t_units=3600000;


    if(checked==true){  timer_trigger->start((ui->Interval_time->text().toInt())*t_units);on_Aquire_Button_clicked();per_updt->start(94);}
    if(checked==false) {  timer_trigger->stop();per_updt->stop();ui->lcdNumber_4->display("--------");}

}
void MainWindow::update_things(){
a=timer_trigger->remainingTime();
b=a/((double)t_units);

//QString("%1").arg((timer_trigger->remainingTime()/t_units), 3, 10, point,QChar('O'))
    ui->lcdNumber_4->display(QString::number(b,'f', 4));



}


void MainWindow::on_Title_enb_toggled(bool checked)
{
checked+=0;
    /*

    if(checked==true){

        ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, ui->Title->text()));

    }else {

       ui->customPlot->plotLayout()->

    }
*/
    MainWindow::refresh();

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
position++;
}

void MainWindow::on_Scroll_forward_clicked()
{
    Scroll_chart+=ui->Scroll_step->text().toDouble();
    MainWindow::refresh();
}

void MainWindow::on_Scroll_back_clicked()
{
    Scroll_chart-=ui->Scroll_step->text().toDouble();
    MainWindow::refresh();
}

void MainWindow::on_pushButton_6_clicked()
{

}

void MainWindow::on_pushButton_toggled(bool checked)
{
ui->Port_A->setEnabled(false);
    if (checked==1){
    ui->pushButton->setText("Connected");

    MainWindow::serial_init( ui->Port_A->text() );


    timer->start(1);
} else {
ui->Port_A->setEnabled(true);
        timer->stop();
        ui->pushButton->setText("Connect");
        ui->lcdNumber->display(QString("%1").arg(-888888, 3, 10, point,QChar('O')));
        ui->textEdit->setText("");
        ui->textEdit_2->setText("Off");

::close(tty_fd);


    }







}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    MainWindow::refresh();
}

void MainWindow::on_MarkerSize_sliderReleased()
{
    MainWindow::refresh();
}

void MainWindow::on_LCR_sliderReleased()
{
        MainWindow::refresh();
}

void MainWindow::on_LCG_sliderReleased()
{
        MainWindow::refresh();
}

void MainWindow::on_LCB_sliderReleased()
{
        MainWindow::refresh();
}

void MainWindow::on_LCTR_sliderReleased()
{
        MainWindow::refresh();
}

void MainWindow::on_Line_style_currentIndexChanged(int index)
{
    index++;
           MainWindow::refresh();
}

void MainWindow::on_actionLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
           tr("Load data"), "~/", tr("Data (*.txt)"));
}

void MainWindow::on_actionSave_triggered()
{
    QString outputFilename = QFileDialog::getSaveFileName(this,
           tr("Save data"), "~/", tr("Data (*.txt)"));


   QFile outputFile(outputFilename);
   outputFile.open(QIODevice::WriteOnly);

   /* Check it opened OK */
   if(!outputFile.isOpen()){

   }
   clipboard="";

   for (int i=0;i<=ui->Chart_to->text().toInt();i++){

           clipboard.append(model ->data(model->index(i, 0),Qt::DisplayRole).toString());
           clipboard.append("\t");
           clipboard.append(model ->data(model->index(i, 1),Qt::DisplayRole).toString());
           clipboard.append("\t");
           clipboard.append(model ->data(model->index(i, 2),Qt::DisplayRole).toString());
           clipboard.append("\t");
           clipboard.append(model ->data(model->index(i, 3),Qt::DisplayRole).toString());
           clipboard.append("\n");

       }
   /* Point a QTextStream object at the file */
   QTextStream outStream(&outputFile);

   /* Write the line to the file */
   outStream << clipboard;

   /* Close the file */
   outputFile.close();

}

void MainWindow::on_actionInput_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}
