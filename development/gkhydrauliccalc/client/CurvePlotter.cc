#include <QPainter>
#include <QtGui>
#include <QWidget>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include "CurvePlotter.h"
#include <cmath>

CurvePlotter::CurvePlotter(QWidget *parent) :
    QWidget(parent)
{
    QPalette palette;
    palette.setBrush(QPalette::Background, Qt::yellow);
    palette.setBrush(QPalette::Dark, Qt::black);
    palette.setBrush(QPalette::Light, Qt::blue);
    palette.setBrush(QPalette::Mid, Qt::green);
    setPalette(palette);

    setBackgroundRole(QPalette::Light);                             //设置曲线图使用”暗“分量显示
    setAutoFillBackground(true);                                    //设置是否自动填充背景色
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  //设置窗口不见可缩放，在X,Y方向均如此
    setFocusPolicy(Qt::StrongFocus);                                //设置焦点
    rubberBandIsShown = false;                                      //鼠标圈住的橡皮筋区域是否显示

    zoomInButton = new QPushButton(tr("Zoom In"),this);             //放大缩小功能按钮
    //zoomInButton->setIcon(QIcon(":/images/zoomin.png"));
    zoomInButton->adjustSize();
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    zoomOutButton = new QPushButton(tr("Zoom Out"),this);
    //zoomOutButton->setIcon(QIcon(":/resources/zoomout.png"));
    zoomOutButton->adjustSize();
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));

    titleX = QObject::tr("Pressure(MPa)");
    titleY = QObject::tr("Delay time(min)");

    PlotSettings settings;
    setPlotSettings(settings);
}
void CurvePlotter::setPlotSettings(const PlotSettings &settings)	//初始化
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;
    zoomInButton->hide();
    zoomOutButton->hide();
    refreshPixmap();
}

void CurvePlotter::zoomOut()	//缩小功能实现
{
    if (curZoom > 0)
    {
        --curZoom;
        zoomOutButton->setEnabled(curZoom > 0);
        zoomInButton->setEnabled(true);
        zoomInButton->show();
        refreshPixmap();
    }
}

void CurvePlotter::zoomIn()	//放大功能实现
{
    if (curZoom < zoomStack.count() - 1)
    {
        ++curZoom;
        zoomInButton->setEnabled(curZoom < zoomStack.count() - 1);
        zoomOutButton->setEnabled(true);
        zoomOutButton->show();
        refreshPixmap();
    }
}
void CurvePlotter::setCurveData(int id, const QVector<QPointF> &data)        //设置给定曲线的ID号，若存在，则替换原数据，若不存在，则插入新曲线
{
    curveMap[id] = data;
    refreshPixmap();
}

void CurvePlotter::clearCurve(int id)                                        //移除一条给定曲线
{
    curveMap.remove(id);
    refreshPixmap();
}

QSize CurvePlotter::minimumSizeHint() const                                  //指定窗口部件理想的最小大小
{
    return QSize(6 * Margin, 4 * Margin);
}

QSize CurvePlotter::sizeHint() const	//指定窗口部件理想大小
{
    return QSize(12 * Margin, 8 * Margin);
}

void CurvePlotter::paintEvent(QPaintEvent * /* event */)                     //将refreshPixmap中绘制好的图像，复制到窗口部件的（0，0）位置
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    if (rubberBandIsShown)	//橡皮筋选择框显示中
    {
        painter.setPen(QPen(palette().mid().color(), 1));                      //橡皮筋选择区域显示亮背景，以区别于整个图片区域
        painter.drawRect(rubberBandRect.normalized()
                         .adjusted(0, 0, -1, -1));
    }
    if (hasFocus()) //判断绘图区域是否拥有焦点
    {
        QStyleOptionFocusRect option;	//焦点区域的风格设置
        option.initFrom(this);	//按照控件风格初始化焦点框
        option.backgroundColor = palette().dark().color();//设置焦点框颜色
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);//控制焦点框，背景，曲线图等控件
    }
}

void CurvePlotter::resizeEvent(QResizeEvent * /* event */)		//当窗口部件大小改变的时候，通过该函数实现绘图区域的大小重定义
{
    int x = width() - (zoomInButton->width()
                       + zoomOutButton->width() + 10);
    zoomInButton->move(x, 5);
    zoomOutButton->move(x + zoomInButton->width() + 5, 5);	//将缩放按钮放在窗口的右上角
    refreshPixmap();
}
void CurvePlotter::mousePressEvent(QMouseEvent *event)		//鼠标左键按下事件
{
    QRect rect(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);
    if (event->button() == Qt::LeftButton)	//左键按下
    {
        if (rect.contains(event->pos())) //显示橡皮筋小框框
        {
            rubberBandIsShown = true;
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);	//设置鼠标形状
        }
    }
}
void CurvePlotter::mouseMoveEvent(QMouseEvent *event)	//鼠标移动事件
{
    if (rubberBandIsShown)	//鼠标左键已经按下
    {
        updateRubberBandRegion();	//更新橡皮筋的大小
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}
void CurvePlotter::mouseReleaseEvent(QMouseEvent *event)	//鼠标释放事件
{
    if ((event->button() == Qt::LeftButton) && rubberBandIsShown)	//鼠标圈出橡皮筋并且释放
    {
        rubberBandIsShown = false;	//橡皮筋不再显示
        updateRubberBandRegion();	//更新橡皮筋
        unsetCursor();	//改变鼠标形状
        QRect rect = rubberBandRect.normalized();	//取得橡皮筋区域
        if (rect.width() < 4 || rect.height() < 4)
            return;									//橡皮筋区域小于4X4不作任何操作
        rect.translate(-Margin, -Margin);	//移动画布
        PlotSettings prevSettings = zoomStack[curZoom];	//更改缩放级别
        PlotSettings settings;
        double dx = prevSettings.spanX() / (width() - 2 * Margin);
        double dy = prevSettings.spanY() / (height() - 2 * Margin);
        settings.minX = prevSettings.minX + dx * rect.left();
        settings.maxX = prevSettings.minX + dx * rect.right();
        settings.minY = prevSettings.maxY - dy * rect.bottom();
        settings.maxY = prevSettings.maxY - dy * rect.top();
        settings.adjust();											//更改坐标轴长度
        zoomStack.resize(curZoom + 1);
        zoomStack.append(settings);
        zoomIn();							 //将当前缩放大小压栈并完成放大
    }
}

void CurvePlotter::keyPressEvent(QKeyEvent *event)		//处理键盘按键事件
{
    switch (event->key())
    {
    case Qt::Key_Plus: //+号键
        zoomIn();
        break;
    case Qt::Key_Minus: //-号键
        zoomOut();
        break;
    case Qt::Key_Left: //方向键左
        zoomStack[curZoom].scroll(-1,0);
        refreshPixmap();
        break;
    case Qt::Key_Right: //方向键右
        zoomStack[curZoom].scroll(+1,0);
        refreshPixmap();
        break;
    case Qt::Key_Down: //方向键下
        zoomStack[curZoom].scroll(0,-1);
        refreshPixmap();
        break;
    case Qt::Key_Up: //方向键上
        zoomStack[curZoom].scroll(0,+1);
        refreshPixmap();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void CurvePlotter::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;
    if (event->orientation() == Qt::Horizontal)
    {
        zoomStack[curZoom].scroll(numTicks, 0);
    }
    else
    {
        zoomStack[curZoom].scroll(0, numTicks);
    }
    refreshPixmap();
}
void CurvePlotter::updateRubberBandRegion()	////完成对橡皮筋的绘制，重绘，擦除
{
    QRect rect = rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 2);
    update(rect.left(), rect.top(), 2, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 2);
    update(rect.right(), rect.top(), 2, rect.height());
}
void CurvePlotter::refreshPixmap()	//将绘图区重新绘制到内存的脱屏像素映射上，并更新
{
    pixmap = QPixmap(size());	 //定义一个和绘图区一样大小的画布
    //    pixmap.fill(this, 0, 0);
    pixmap.fill();

    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    drawPoints(&painter);
    update();					//从绘图区获取图形，并绘制到脱屏像素上，更新
}

void CurvePlotter::drawGrid(QPainter *painter)		//用来绘制坐标系后面的网格
{
    QRect rect(Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);		//取得绘图区域，大小要减去旁白
    if (!rect.isValid())								//获取失败
        return;
    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = palette().dark().color().light();
    //    QPen quiteDark(Qt::black);
    QPen light = palette().light().color();					//设置缩放级别，背景色，画笔颜色，画笔风格
    //    QPen light(Qt::blue);
    painter->drawText(rect.center().x() - 50, rect.bottom() + 30, 100, 20, Qt::AlignHCenter | Qt::AlignTop, titleX);
    painter->save();
    painter->rotate(-90.0);
    painter->drawText(-rect.center().y() - Margin, rect.left() - 85, 100, 100, Qt::AlignRight | Qt::AlignVCenter, titleY);
    painter->restore();
    for (int i = 0; i <= settings.numXTicks; ++i)			//绘制X轴上的网格和坐标
    {
        int x = rect.left() + (i * (rect.width() - 1) / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX() / settings.numXTicks);
        painter->setPen(quiteDark);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(light);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          QString::number(label));
    }
    for (int j = 0; j <= settings.numYTicks; ++j)			//绘制Y轴上的网格和坐标
    {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                 / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                        / settings.numYTicks);
        painter->setPen(quiteDark);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(light);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);
        painter->drawText(rect.left() - Margin, y - 10, Margin - 5, 20,
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString::number(label));
    }
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
}

void CurvePlotter::drawCurves(QPainter *painter)		//绘制曲线
{
    //定义一个枚举类型，存储曲线颜色
    static const QColor colorForIds[10]={Qt::red,Qt::green,Qt::blue,Qt::cyan,Qt::magenta,Qt::yellow,Qt::darkBlue,Qt::darkGreen,Qt::darkRed,Qt::white};
    //获得图形设置参数
    PlotSettings settings=zoomStack[curZoom];
    //获得绘图区域
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);
    if (!rect.isValid())
        return;
    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));
    QMapIterator<int, QVector<QPointF> > i(curveMap);
    while (i.hasNext())
    {
        i.next();
        int id = i.key();
        const QVector<QPointF> &data = i.value();
        QPolygonF polyline(data.count());
        for (int j = 0; j < data.count(); ++j)
        {
            double dx = data[j].x() - settings.minX;
            double dy = data[j].y() - settings.minY;
            double x = rect.left() + (dx * (rect.width() - 1)
                                      / settings.spanX());
            double y = rect.bottom() - (dy * (rect.height() - 1)
                                        / settings.spanY());
            polyline[j] = QPointF(x, y);
        }
        painter->setPen(colorForIds[uint(id) % 6]);
        painter->drawPolyline(polyline);
    }
}
//画点
void CurvePlotter::drawPoints(QPainter *painter)		//绘制点
{
    PlotSettings settings=zoomStack[curZoom];
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);
    if (!rect.isValid())
        return;
    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));
    double x2,rx,ry,y2,y3;
    for (x2=-10;x2<10;x2=x2+1)
    {
        y2 = x2*x2;
        y3 = x2+30;
        if (y3==y2)
        {
            rx=x2;
            ry=rx*rx;
            double dx = rx - settings.minX;
            double dy = ry - settings.minY;
            double x = rect.left() + (dx * (rect.width() - 1)
                                      / settings.spanX());
            double y = rect.bottom() - (dy * (rect.height() - 1)
                                        / settings.spanY());
            painter->setPen(QPen(Qt::blue));
            painter->setBrush(QBrush(Qt::blue,Qt::SolidPattern));
            painter->drawEllipse(x-2.5,y-2.5,5,5);

            QString str;
            //str = "B";
            str.append("(");
            str.append(QString::number(rx));
            str.append(",");
            str.append(QString::number(ry));
            str.append(")");
            painter->drawText(x-10, y-10 ,50,50,Qt::AlignRight | Qt::AlignVCenter,str);
        }
    }

}
PlotSettings::PlotSettings()		//设置坐标系的坐标轴长度，x和y分别是负荷曲线值的最大最小值
{
    minX =-12.0;
    maxX = 12.0;
    numXTicks = 8;
    minY = 0.0;
    maxY = 120.0;
    numYTicks = 8;
}

void PlotSettings::scroll(int dx, int dy)		//该功能主要用于鼠标滚轮滚动过程
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;
    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings::adjust()		//当橡皮筋释放之后，用来调整坐标轴上的刻度和坐标长度
{
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

void PlotSettings::adjustAxis(double &min, double &max,
                              int &numTicks)
{
    const int MinTicks = 4;		//设置最小的刻度
    double grossStep = (max - min) / MinTicks; //简单确定两个坐标点之间的距离，步长
    double step = pow(10.0, floor(log10(grossStep)));//采用10的n次，2X10的n次的形式表示步长
    if (5 * step < grossStep)	//确定最合适的步长
    {
        step *= 5;
    }
    else if (2 * step < grossStep)
    {
        step *= 2;
    }
    numTicks = int(ceil(max / step) - floor(min / step));		 //确定坐标刻度点数
    if (numTicks < MinTicks)
        numTicks = MinTicks;
    min = floor(min / step) * step;		 //得到坐标轴距离
    max = ceil(max / step) * step;
}
