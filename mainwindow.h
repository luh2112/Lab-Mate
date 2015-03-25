#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QSpinBox>


#include <QItemDelegate>
#include <QStandardItemModel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
QVector<double> x, y;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
QClipboard *p_Clipboard ;
QTimer *timer;
QTimer *timer_trigger;
 QTimer *per_updt;
public slots:
void serial_init(QString port);
void refresh();
private slots:
     void ReadSerial();
    // void    on_pushButton_clicked();


     void on_pushButton_3_clicked();
     void on_pushButton_clicked();
     void on_X_LO_editingFinished();
     void on_X_HI_editingFinished();
     void on_Y_LO_editingFinished();
     void on_Y_HI_editingFinished();
     void on_Aquire_Button_clicked();
     void on_ref_clicked();
     void on_X_label_editingFinished();
     void on_Y_label_editingFinished();
void update_things();

     void on_Auto_scale_toggled(bool checked);
     void on_Labels_toggled(bool checked);
     void on_pushButton_2_clicked();
     void on_spinBox_valueChanged(int arg1);
     void on_Chart_to_editingFinished();
     void on_Chart_from_editingFinished();
     void on_checkBox_2_toggled(bool checked);

     void on_Title_enb_toggled(bool checked);
     void on_horizontalSlider_sliderMoved(int position);
     void on_Scroll_forward_clicked();
     void on_Scroll_back_clicked();
     void on_pushButton_6_clicked();
     void on_pushButton_toggled(bool checked);


     void on_horizontalSlider_sliderReleased();
     void on_MarkerSize_sliderReleased();
     void on_LCR_sliderReleased();
     void on_LCG_sliderReleased();
     void on_LCB_sliderReleased();
     void on_LCTR_sliderReleased();
     void on_Line_style_currentIndexChanged(int index);
     void on_actionLoad_triggered();
     void on_actionSave_triggered();
     void on_actionInput_triggered();
     void on_actionExit_triggered();
};

#endif // MAINWINDOW_H
