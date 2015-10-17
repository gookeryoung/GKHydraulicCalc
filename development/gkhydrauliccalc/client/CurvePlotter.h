#ifndef CURVEPLOTTER_H
#define CURVEPLOTTER_H

#include <QMap>//包含的Qt的头文件
#include <QPixmap>
#include <QVector>
#include <QWidget>
#include <QPushButton>
#include <cmath>
#include <QWidget>

class QToolButton; //两个前向声明
class PlotSettings;

class CurvePlotter : public QWidget
{
    Q_OBJECT
public:
    CurvePlotter(QWidget *parent = 0);
    void setPlotSettings(const PlotSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);
    void clearCurve(int id);
    QSize minimumSizeHint() const; //重写QWidget::minimumSizeHint()
    QSize sizeHint() const;        //重写QWidget::sizeHint()
public slots:
    void zoomIn();  //放大曲线
    void zoomOut();  //缩小显示曲线
protected:  //重写的事件处理函数
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    void updateRubberBandRegion();
    void refreshPixmap();
    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);
    void drawPoints(QPainter *painter);
    enum { Margin = 50 };
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QMap<int, QVector<QPointF> > curveMap;  //曲线数据
    QVector<PlotSettings> zoomStack;  //PlotSettings堆栈区域
    int curZoom;	//缩放级别
    bool rubberBandIsShown;
    QRect rubberBandRect;
    QPixmap pixmap; //显示在屏幕的控件的一个拷贝，任何绘制总是先在pixmap进行，然后拷贝到控件上

    QString titleX;
    QString titleY;
};

//PlotSettings确定x，y轴的范围，和刻度的个数
class PlotSettings
{
public:
    PlotSettings();

    void scroll(int dx, int dy);
    void adjust();
    double spanX() const { return maxX - minX; }
    double spanY() const { return maxY - minY; }

    double minX;
    double maxX;
    int numXTicks;
    double minY;
    double maxY;
    int numYTicks;
private:
    static void adjustAxis(double &min, double &max, int &numTicks);
};

#endif // CURVEPLOTTER_H
