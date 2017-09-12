#ifndef SNIPCANVAS_H
#define SNIPCANVAS_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <QPixmap>
#include <QApplication>
#include <QGuiApplication>
#include <QPen>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QComboBox>
#include <QColorDialog>
#include "linepaint.h"
#include "rectpaint.h"
enum DrawStatus
{
    WAITING=0,DRAWING,DRAWED
};

class SnipCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit SnipCanvas(QWidget *parent = nullptr);
    void setBackgroundPixmap(QPixmap &pix);
    void snipScreen(QRectF &rect);
    void initCanvas();
    void initToolBar();
    void showToolBar();
    void hideToolBar();
    void updateToolBar();

    quint8 caputerRect(QRectF t_rect,qreal t_x,qreal t_y);
    RectPaint getRectF(QPointF p1,QPointF p2);    //通过两个坐标点生成矩形

public slots:
    void drawLineSlot();
    void drawRectSlot();
    void drawEllipseSlot();
    void saveFileSlot();
    void saveFileAsSlot();            //保存到文件
    void cancelSlot();

    void changePenWidthSlot(QString s);
    void changePenColorSlot();
    void changePenStyleSlot(int index);

private:
    int screenWidth,screenHeight;
    quint8 okFlag;

    QPointF pointStart;     //鼠标绘制起点
    QPointF pointEnd;     //鼠标绘制终点
    RectPaint snipArea;    //截图区域

    QList<LinePaint> lineList;         //直线列表
    QList<RectPaint> rectList;         //矩形列表
    QList<RectPaint> ellipseList;      //椭圆列表

    QPointF pointDrag;              //拖拽点

    DrawStatus rectFlag=DrawStatus::WAITING;
    quint8 drawEditFlag;          //绘图修改

    QPixmap fullPixmap;         //原始全屏图片
    QPixmap originalPixmap;

    QWidget *toolBar;           //工具条
    QPushButton *saveFileBtn;
    QPushButton *saveFileAsBtn;
    QPushButton *cancelBtn;
    QPushButton *drawLineBtn;      //画直线
    QPushButton *drawRectBtn;      //画矩形
    QPushButton *drawEllipseBtn;   //画椭圆

    QPen drawPen;                   //线宽颜色风格
    QWidget *shapeToolBar;
    QWidget *textToolBar;

    QComboBox *lineSizeCbx;
    QPushButton *colorSelectBtn;
    QComboBox *lineStyleCbx;
    QPoint zoomPoint;//放大框左上角
    QRect zoomRect;//放大框

    quint8 cursorCaptureFlag=0;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *e);
};

#endif // SNIPCANVAS_H
