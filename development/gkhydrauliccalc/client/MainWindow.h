#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CylinderParam.h"

namespace Ui {
class MainWindow;
}

class ScriptParser;
class CurvePlotter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum {
        PlotWindowWidth = 850, PlotWindowHeight = 530, PltWindowOffset = 20
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);


private slots:
    void onCalc(void);
    void onPlotPt(void);
    void onPlotTt(void);
    void onClear(void);

private:
    void readParams(void);
    void calcResults(void);
    void outputResults(void);

    void calcVolume(void);
    void calcU1(void);
    void calcQ(void);
    void calcDelayT(void);

private:
    Ui::MainWindow *_ui;

    CylinderParam _param;
    ScriptParser *_parser;
    CurvePlotter *_ptPlotter;
};

#endif // MAINWINDOW_H
