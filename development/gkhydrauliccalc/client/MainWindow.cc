#include <QDebug>
#include <QKeyEvent>
#include "Python.h"
#include "CurvePlotter.h"
#include "ScriptParser.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _ui->_tbOutput->setFont(QFont("consolas", 12));
    _ui->_pbCalc->setFocus();

    connect(_ui->_pbClear, SIGNAL(clicked()), this, SLOT(onClear()));

    _parser = ScriptParser::instance();

    _ptPlotter = new CurvePlotter;
    _ptPlotter->hide();
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        onCalc();
    }
}

void MainWindow::onCalc()
{
    readParams();
    calcResults();
    outputResults();
}

void MainWindow::onPlotPt()
{
    if (_ptPlotter->isHidden()) {
        QVector<QPointF> points0;
        QVector<QPointF> points1;
        QVector<QPointF> points2;
        double y1,y2;
        for (double x = -10.0000; x < 10.0001; x=0.0001+x)
        {
            y1 = x*x;
            points0.append(QPointF(x, y1));
        }
        for (double x = -10.0000; x < 10.0001; x=0.0001+x)
        {
            y2 = x+30;
            points1.append(QPointF(x, y2));
        }

        _ptPlotter->setCurveData(0, points0);
        _ptPlotter->setCurveData(1, points1);
        _ptPlotter->show();
        _ptPlotter->raise();
    } else {
        _ptPlotter->hide();
    }
}

void MainWindow::onPlotTt()
{
}

void MainWindow::onClear()
{
    _ui->_tbOutput->clear();
}

void MainWindow::readParams()
{
    _param._d1 = _ui->_dsCylinderD->text().toDouble();
    _param._l = _ui->_dsCylinderL->text().toDouble();
    _param._valveD2 = _ui->_dsValveD->text().toDouble();
    _param._valveL = _ui->_dsValveL->text().toDouble();
    _param._valveDeta = _ui->_dsValveGap->text().toDouble();
    _param._p = _ui->_dsExternP->text().toDouble();
    _param._t = _ui->_dsTemperature->text().toDouble();
}

void MainWindow::calcResults()
{
    calcVolume();
    calcU1();
    calcQ();
    calcDelayT();
}

void MainWindow::outputResults()
{
    _ui->_tbOutput->append(tr("Cylinder volume: %1 [mm^3]").arg(_param._volume));
    _ui->_tbOutput->append(tr("Dynamic viscosity: %1 [Pa·s]").arg(_param._u1));
    _ui->_tbOutput->append(tr("Quantity of flow: %1 [m^3/s]").arg(_param._Q));
    _ui->_tbOutput->append(tr("Delay time: %1 [s]\n").arg(_param._delayT));
}

void MainWindow::calcVolume()
{
    qreal d1 = _param._d1;
    qreal l = _param._l;
    PyObject *v = PyObject_CallMethod(_parser->module(), "calc_cylinder_volume", "ff", d1, l);
    _param._volume = PyFloat_AsDouble(v);
}

void MainWindow::calcU1()
{
    qreal d1 = _param._d1;
    qreal p = _param._p;
    qreal t = _param._t;

    PyObject *u1 = PyObject_CallMethod(_parser->module(), "calc_u1", "fff", d1, p, t);
    _param._u1 = PyFloat_AsDouble(u1);
}

void MainWindow::calcQ()
{
    qreal vd2 = _param._valveD2;
    qreal vdeta = _param._valveDeta;
    qreal vl = _param._valveL;
    qreal u1 = _param._u1;
    qreal d1 = _param._d1;
    qreal p = _param._p;

    PyObject *Q = PyObject_CallMethod(_parser->module(), "calc_Q", "ffffff", vd2, vdeta, vl, u1, d1, p);
    _param._Q = PyFloat_AsDouble(Q);
}

void MainWindow::calcDelayT()
{
    qreal v = _param._volume;
    qreal Q = _param._Q;

    PyObject *t = PyObject_CallMethod(_parser->module(), "calc_t", "ff", v, Q);
    _param._delayT = PyFloat_AsDouble(t);
}
