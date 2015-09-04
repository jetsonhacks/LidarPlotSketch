#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lidarlite.h"
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    LidarLite *lidarLite ;
    void setupVelocityPlot(QCustomPlot *customPlot);
    void setupDistancePlot(QCustomPlot *customPlot) ;

private slots:
  void velocityDataSlot();
  void distanceDataSlot() ;

private:
    Ui::MainWindow *ui;
    QString demoName;
    QTimer velocityDataTimer;
    QTimer distanceDataTimer ;

};

#endif // MAINWINDOW_H
