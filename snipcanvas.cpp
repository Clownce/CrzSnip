#include "snipcanvas.h"
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include "setconfig.h"

SnipCanvas::SnipCanvas(QWidget *parent) : QWidget(parent)
{
    okFlag=1;
    drawEditFlag=0;
    const QRect screenGeometry=QApplication::desktop()->screenGeometry(this);
    screenWidth=screenGeometry.width();
    screenHeight=screenGeometry.height();
    this->raise();
    this->showFullScreen();
    setMouseTracking(true);
    initCanvas();
    initToolBar();
}
void SnipCanvas::initCanvas()
{
    pointStart.rx()=0;
    pointStart.ry()=0;
    pointEnd.rx()=0;
    pointEnd.ry()=0;
    pointDrag.rx()=0;
    pointDrag.ry()=0;
    rectFlag=DrawStatus::WAITING;
    lineList.clear();
    rectList.clear();
    ellipseList.clear();
    drawPen.setBrush(Qt::red);
    drawPen.setWidthF(1);
    drawPen.setStyle(Qt::SolidLine);
    zoomPoint=QCursor::pos();
    zoomRect.setRect(0,0,100,100);
}
void SnipCanvas::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(rectFlag==DrawStatus::WAITING)
        {
            pointStart.rx()=event->x();
            pointStart.ry()=event->y();
            pointEnd.rx()=event->x();
            pointEnd.ry()=event->y();
            snipArea=getRectF(pointStart,pointEnd);
            rectFlag=DrawStatus::DRAWING;
        }
        else if(rectFlag==DrawStatus::DRAWED)    //捕捉拖拽
        {
            if(drawEditFlag==0)
            {
                pointDrag.setX(event->x());
                pointDrag.setY(event->y());
            }
            else if(drawEditFlag==1)        //画线
            {
                pointStart.rx()=event->x();
                pointStart.ry()=event->y();
                pointEnd.rx()=event->x();
                pointEnd.ry()=event->y();

                LinePaint tempLine(pointStart.toPoint(),pointEnd.toPoint());
                tempLine.setPen(drawPen);
                lineList.append(tempLine);
            }
            else if(drawEditFlag==2)        //画矩形
            {
                pointStart.rx()=event->x();
                pointStart.ry()=event->y();
                pointEnd.rx()=event->x();
                pointEnd.ry()=event->y();

                RectPaint tempRect=getRectF(pointStart,pointEnd);
                tempRect.setPen(drawPen);
                rectList.append(tempRect);
            }
            else if(drawEditFlag==3)        //画椭圆
            {
                pointStart.rx()=event->x();
                pointStart.ry()=event->y();
                pointEnd.rx()=event->x();
                pointEnd.ry()=event->y();

                RectPaint tempRect=getRectF(pointStart,pointEnd);
                tempRect.setPen(drawPen);
                ellipseList.append(tempRect);
            }
        }
    }
    else if(event->button()==Qt::RightButton)       //重新绘制区域
    {
        setCursor(Qt::ArrowCursor);

        if(rectFlag==DrawStatus::WAITING)
        {
            cancelSlot();
        }
        else
        {
            initCanvas();
            hideToolBar();
            update();
        }
    }
}

void SnipCanvas::mouseMoveEvent(QMouseEvent *event)
{
    zoomPoint=event->pos();
    if(event->buttons()&Qt::LeftButton)
    {
        if(rectFlag==DrawStatus::DRAWING)
        {
            pointEnd.setX(event->x());
            pointEnd.setY(event->y());
            snipArea=getRectF(pointStart,pointEnd);
        }
        else if(rectFlag==DrawStatus::DRAWED)            //拖拽
        {
            QPointF tempTL,tempBR;

            tempTL=snipArea.topLeft();
            tempBR=snipArea.bottomRight();

            switch(cursorCaptureFlag)
            {
            case 0:                                 //无效区域
                setCursor(Qt::ArrowCursor);
                break;
            case 1:                                 //左上
                setCursor(Qt::SizeFDiagCursor);
                tempTL.setX(event->x());
                tempTL.setY(event->y());
                break;
            case 2:                                 //左下
                setCursor(Qt::SizeBDiagCursor);
                tempTL.setX(event->x());
                tempBR.setY(event->y());
                break;
            case 3:                                 //左边
                setCursor(Qt::SizeHorCursor);
                tempTL.setX(event->x());
                break;

            case 4:                                 //右上
                setCursor(Qt::SizeBDiagCursor);
                tempBR.setX(event->x());
                tempTL.setY(event->y());
                break;
            case 5:                                 //右下
                setCursor(Qt::SizeFDiagCursor);
                tempBR.setX(event->x());
                tempBR.setY(event->y());
                break;
            case 6:                                 //右边
                setCursor(Qt::SizeHorCursor);
                tempBR.setX(event->x());
                snipArea=getRectF(pointStart,pointEnd);
                break;
            case 7:                                 //上边
                setCursor(Qt::SizeVerCursor);
                tempTL.setY(event->y());
                break;
            case 8:                                 //下边
                setCursor(Qt::SizeVerCursor);
                tempBR.setY(event->y());
                break;
            case 9:                                 //中央
                if(drawEditFlag==0)
                {
                    setCursor(Qt::SizeAllCursor);
                    qreal dx=event->x()-pointDrag.x();          //获取坐标差
                    qreal dy=event->y()-pointDrag.y();

                    pointDrag.setX(event->x());                 //刷新拖拽点坐标
                    pointDrag.setY(event->y());

                    if( (tempTL.x()+dx)>0 && (tempBR.x()+dx)<screenWidth )
                    {
                        tempTL.rx()+=dx;
                        tempBR.rx()+=dx;
                    }

                    if( (tempTL.y()+dy)>0 && (tempBR.y()+dy)<screenHeight )
                    {
                        tempTL.ry()+=dy;
                        tempBR.ry()+=dy;
                    }
                }
                else if(drawEditFlag==1)                //绘线
                {
                    pointEnd.setX(event->x());
                    pointEnd.setY(event->y());

                    lineList.last().setPoints(pointStart.toPoint(),pointEnd.toPoint());
                }
                else if(drawEditFlag==2)                //绘矩形
                {
                    pointEnd.setX(event->x());
                    pointEnd.setY(event->y());

                    rectList.last().setTopLeft(pointStart);
                    rectList.last().setBottomRight(pointEnd);
                }
                else if(drawEditFlag==3)                //绘椭圆
                {
                    pointEnd.setX(event->x());
                    pointEnd.setY(event->y());

                    ellipseList.last().setTopLeft(pointStart);
                    ellipseList.last().setBottomRight(pointEnd);
                }

                break;
            }

            snipArea.setTopLeft(tempTL);
            snipArea.setBottomRight(tempBR);

            showToolBar();        //实时更新工具条位置
        }
        update();
    }
    else if(event->buttons()==Qt::NoButton)      //没有按键按下
    {
        if(rectFlag==DrawStatus::DRAWED)    //捕捉拖拽
        {
            cursorCaptureFlag=caputerRect(snipArea,event->x(),event->y());

            switch(cursorCaptureFlag)
            {
            case 0:                                 //无效区域
                setCursor(Qt::ArrowCursor);
                break;
            case 1:                                 //左上
                setCursor(Qt::SizeFDiagCursor);
                break;
            case 2:                                 //左下
                setCursor(Qt::SizeBDiagCursor);
                break;
            case 3:                                 //左边
                setCursor(Qt::SizeHorCursor);
                break;

            case 4:                                 //右上
                setCursor(Qt::SizeBDiagCursor);
                break;
            case 5:                                 //右下
                setCursor(Qt::SizeFDiagCursor);
                break;
            case 6:                                 //右边
                setCursor(Qt::SizeHorCursor);
                break;
            case 7:                                 //上边
                setCursor(Qt::SizeVerCursor);
                break;
            case 8:                                 //下边
                setCursor(Qt::SizeVerCursor);
                break;
            case 9:                                 //中央
                if(drawEditFlag==0)
                {
                    setCursor(Qt::SizeAllCursor);
                }
                else                                //追加图形
                {
                    setCursor(Qt::ArrowCursor);
                }
                break;
            }

        }
    }
}

void SnipCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(rectFlag==DrawStatus::DRAWING)
        {
            pointEnd.setX(event->x());
            pointEnd.setY(event->y());
            snipArea=getRectF(pointStart,pointEnd);
            rectFlag=DrawStatus::DRAWED;         //矩形绘制完成
            showToolBar();
        }
        else if(rectFlag==DrawStatus::DRAWED)
        {
            if(drawEditFlag==0)
            {
                showToolBar();
            }
            else if(drawEditFlag==1)
            {
                if( (pointStart.x()==pointEnd.x()) && (pointStart.y()==pointEnd.y()) )//直线长度为0
                {
                    lineList.removeLast();
                }
            }
            else if(drawEditFlag==2)
            {
                if( (pointStart.x()==pointEnd.x()) && (pointStart.y()==pointEnd.y()) )//矩形区域为空
                {
                    rectList.removeLast();
                }
            }
            else if(drawEditFlag==3)
            {
                if( (pointStart.x()==pointEnd.x()) && (pointStart.y()==pointEnd.y()) )//矩形区域为空
                {
                    ellipseList.removeLast();
                }
            }
        }

        update();
    }
}

void SnipCanvas::paintEvent(QPaintEvent *e)
{

    QPainter painter(this);

    QPixmap tempmask(screenWidth, screenWidth);
    tempmask.fill((QColor(0, 0, 0, 120)));

    painter.drawPixmap(0,0,fullPixmap);     //先绘制全屏原图背景
    painter.drawPixmap(0,0,tempmask);       //然后绘制半透明背景，用来降低亮度


    switch(rectFlag)        //截图状态机
    {
    case DrawStatus::WAITING:
    {
        break;
    }
    case DrawStatus::DRAWING:
    {
        painter.setPen(QPen(Qt::green,2,Qt::DashLine));//设置画笔形式
        painter.drawRect(snipArea);            //然后绘制矩形框
        painter.drawPixmap(snipArea,fullPixmap,snipArea);     //然后将矩形框中的半透明图像替换成原图
        break;
    }
    case DrawStatus::DRAWED:
    {
        painter.setPen(QPen(Qt::green,2,Qt::DashLine));//设置画笔形式
        painter.drawRect(snipArea);            //然后绘制矩形框
        painter.drawPixmap(snipArea,fullPixmap,snipArea);     //然后将矩形框中的半透明图像替换成原图
        break;
    }
    default:
    {
        break;
    }
    }
    //放大框 100*130
    if(rectFlag==DrawStatus::WAITING||rectFlag==DrawStatus::DRAWING)
    {
        QPainter zoomPainter(this);
        QPen pen;
        QBrush brush(QColor(0,0,0));
        int x=zoomPoint.x();
        int y=zoomPoint.y();
        int w=qAbs(pointStart.x()-pointEnd.y());
        int h=qAbs(pointStart.y()-pointEnd.y());
        int r=qRed(fullPixmap.toImage().pixel(x,y));
        int g=qGreen(fullPixmap.toImage().pixel(x,y));
        int b=qBlue(fullPixmap.toImage().pixel(x,y));
        zoomRect=QRect(x-10,y-10,20,20);//放大框显示的矩形位置
        QPixmap tempix=fullPixmap.copy(zoomRect);//该位置的图像
        if((x+100+10)<=screenWidth&&(y+130+10)<=screenHeight)//显示在右下角,误差10
        {
            //图像
            zoomPainter.drawPixmap(x+10,y+10,100,100,tempix);
            pen.setColor(QColor(0,250,0,125));
            pen.setWidth(3);
            zoomPainter.setPen(pen);
            zoomPainter.drawLine(x+10+50,y+15,x+10+50,y+10+95);
            zoomPainter.drawLine(x+15,y+10+50,x+10+95,y+10+50);
            pen.setColor(QColor(0,0,0));
            pen.setWidth(1);
            zoomPainter.setPen(pen);
            zoomPainter.drawRect(x+10,y+10,100,100);
            //信息
            brush.setColor(QColor(0,0,0,200));
            zoomPainter.setBrush(brush);
            zoomPainter.drawRect(x+10,y+10+100,100,30);
            QRect infoRectSize=QRect(x+10,y+10+100,100,15);
            QRect infoRectColor=QRect(x+10,y+10+100+15,100,15);
            pen.setColor(QColor(255,255,255));
            zoomPainter.setPen(pen);
            zoomPainter.drawText(infoRectSize,Qt::AlignLeft|Qt::AlignVCenter,QString("%1 x %2").arg(w).arg(h));
            zoomPainter.drawText(infoRectColor,Qt::AlignLeft|Qt::AlignVCenter,QString("RGB:(%1,%2,%3)").arg(r).arg(g).arg(b));
        }
        else if((x+100+10)<=screenWidth&&(y+130+10)>screenHeight)//显示在上方
        {
            zoomPainter.drawPixmap(x+10,y-130,100,100,tempix);
            pen.setColor(QColor(0,250,0,125));
            pen.setWidth(3);
            zoomPainter.setPen(pen);
            zoomPainter.drawLine(x+10+50,y-125,x+10+50,y-35);
            zoomPainter.drawLine(x+15,y-80,x+10+95,y-80);
            pen.setColor(QColor(0,0,0));
            pen.setWidth(1);
            zoomPainter.setPen(pen);
            zoomPainter.drawRect(x+10,y-130,100,100);
            brush.setColor(QColor(0,0,0,200));
            zoomPainter.setBrush(brush);
            zoomPainter.drawRect(x+10,y-30,100,30);
            QRect infoRectSize=QRect(x+10,y-30,100,15);
            QRect infoRectColor=QRect(x+10,y-30+15,100,15);
            pen.setColor(QColor(255,255,255));
            zoomPainter.setPen(pen);
            zoomPainter.drawText(infoRectSize,Qt::AlignLeft|Qt::AlignVCenter,QString("%1 x %2").arg(w).arg(h));
            zoomPainter.drawText(infoRectColor,Qt::AlignLeft|Qt::AlignVCenter,QString("RGB:(%1,%2,%3)").arg(r).arg(g).arg(b));
        }
        else if((x+100+10)>screenWidth&&(y+130+10)<=screenHeight)//显示在左边
        {
            zoomPainter.drawPixmap(x-110,y+10,100,100,tempix);
            pen.setColor(QColor(0,250,0,125));
            pen.setWidth(3);
            zoomPainter.setPen(pen);
            zoomPainter.drawLine(x-60,y+15,x-60,y+15+90);
            zoomPainter.drawLine(x-105,y+10+50,x-15,y+10+50);
            pen.setColor(QColor(0,0,0));
            pen.setWidth(1);
            zoomPainter.setPen(pen);
            zoomPainter.drawRect(x-110,y+10,100,100);
            brush.setColor(QColor(0,0,0,200));
            zoomPainter.setBrush(brush);
            zoomPainter.drawRect(x-110,y+110,100,30);
            QRect infoRectSize=QRect(x-110,y+110,100,15);
            QRect infoRectColor=QRect(x-110,y+110+15,100,15);
            pen.setColor(QColor(255,255,255));
            zoomPainter.setPen(pen);
            zoomPainter.drawText(infoRectSize,Qt::AlignLeft|Qt::AlignVCenter,QString("%1 x %2").arg(w).arg(h));
            zoomPainter.drawText(infoRectColor,Qt::AlignLeft|Qt::AlignVCenter,QString("RGB:(%1,%2,%3)").arg(r).arg(g).arg(b));
        }
        else//显示在左上
        {
            zoomPainter.drawPixmap(x-110,y-130,100,100,tempix);
            pen.setColor(QColor(0,250,0,125));
            pen.setWidth(3);
            zoomPainter.setPen(pen);
            zoomPainter.drawLine(x-60,y-125,x-60,y-35);
            zoomPainter.drawLine(x-105,y-80,x-15,y-80);
            pen.setColor(QColor(0,0,0));
            pen.setWidth(1);
            zoomPainter.setPen(pen);
            zoomPainter.drawRect(x-110,y-130,100,100);
            brush.setColor(QColor(0,0,0,200));
            zoomPainter.setBrush(brush);
            zoomPainter.drawRect(x-110,y-30,100,30);
            QRect infoRectSize=QRect(x-110,y-30,100,15);
            QRect infoRectColor=QRect(x-110,y-30+15,100,15);
            pen.setColor(QColor(255,255,255));
            zoomPainter.setPen(pen);
            zoomPainter.drawText(infoRectSize,Qt::AlignLeft|Qt::AlignVCenter,QString("%1 x %2").arg(w).arg(h));
            zoomPainter.drawText(infoRectColor,Qt::AlignLeft|Qt::AlignVCenter,QString("RGB:(%1,%2,%3)").arg(r).arg(g).arg(b));
        }
        update();//这句很重要，不加上的话在WAITING状态不显示放大框；
    }
    quint16 len=lineList.length();
    if(len)
    {
        for(quint16 i=0;i<len;i++)
        {
            painter.setPen(lineList[i].getPen());//设置画笔形式
            painter.drawLine(lineList[i]);            //然后绘制矩形框
        }
    }

    len=rectList.length();
    if(len)
    {
        for(quint16 i=0;i<len;i++)
        {
            painter.setPen(rectList[i].getPen());//设置画笔形式
            painter.drawRect(rectList[i]);            //然后绘制矩形框
        }
    }

    len=ellipseList.length();
    if(len)
    {
        for(quint16 i=0;i<len;i++)
        {
            painter.setPen(ellipseList[i].getPen());//设置画笔形式
            painter.drawEllipse(ellipseList[i]);            //然后绘制椭圆
        }
    }
    QWidget::paintEvent(e);
}

void SnipCanvas::setBackgroundPixmap(QPixmap &pix)
{
    fullPixmap=pix;
}

void SnipCanvas::snipScreen(QRectF &rect)      //截图函数
{
    QScreen *screen = QGuiApplication::primaryScreen();

    originalPixmap = screen->grabWindow(0,rect.x(),rect.y(),rect.width(),rect.height());

}

void SnipCanvas::initToolBar()                  //工具条初始化
{
    toolBar=new QWidget(this);

    QWidget *MainToolBar=new QWidget();                    //主工具栏
    QHBoxLayout *mainToolLayout=new QHBoxLayout();

    cancelBtn=new QPushButton(QIcon(":/AppIcon/cancel.png"),tr("Quit"));
    saveFileBtn=new QPushButton(QIcon(":/AppIcon/save.png"),tr("Save"));
    saveFileAsBtn=new QPushButton(QIcon(":/AppIcon/saveas.png"),tr("Save As"));

    drawLineBtn=new QPushButton(QIcon(":/AppIcon/line.png"),tr("Line"));
    drawRectBtn=new QPushButton(QIcon(":/AppIcon/rect.png"),tr("Rect"));
    drawEllipseBtn=new QPushButton(QIcon(":/AppIcon/ellipse.png"),tr("Ellipse"));

    cancelBtn->setToolTip("Quit");
    saveFileAsBtn->setToolTip("Save As");
    saveFileBtn->setToolTip("Save");
    drawLineBtn->setToolTip("Draw Line");
    drawRectBtn->setToolTip("Draw Rect");
    drawEllipseBtn->setToolTip("Draw Ellipse");

    cancelBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
    saveFileAsBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
    saveFileBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");

    mainToolLayout->addWidget(drawLineBtn);
    mainToolLayout->addWidget(drawRectBtn);
    mainToolLayout->addWidget(drawEllipseBtn);
    mainToolLayout->addWidget(saveFileBtn);
    mainToolLayout->addWidget(saveFileAsBtn);
    mainToolLayout->addWidget(cancelBtn);
    mainToolLayout->setContentsMargins(0,0,0,0);            //去除边框间隙
    mainToolLayout->setSpacing(2);
    MainToolBar->setLayout(mainToolLayout);

    shapeToolBar=new QWidget();                             //拓展工具条
    QHBoxLayout *shapeToolLayout=new QHBoxLayout();

    lineSizeCbx=new QComboBox();
    colorSelectBtn=new QPushButton();
    lineStyleCbx=new QComboBox();
    lineSizeCbx->setStyleSheet("background-color: rgb(240, 255, 255);");
    lineStyleCbx->setStyleSheet("background-color: rgb(240, 255, 255);");

    QStringList sizeitems;
    sizeitems<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"13"<<"14"<<"15"<<"16"<<"17"<<"18"<<"19"<<"20";
    lineSizeCbx->addItems(sizeitems);
    colorSelectBtn->setStyleSheet("background-color: rgb(255, 0, 0);");

    lineStyleCbx->addItem(QIcon(":/LineStyle/1.ico"),"");
    lineStyleCbx->addItem(QIcon(":/LineStyle/2.ico"),"");
    lineStyleCbx->addItem(QIcon(":/LineStyle/3.ico"),"");
    lineStyleCbx->addItem(QIcon(":/LineStyle/4.ico"),"");
    lineStyleCbx->addItem(QIcon(":/LineStyle/5.ico"),"");
    lineStyleCbx->setIconSize(QSize(80,20));
    shapeToolLayout->addWidget(lineSizeCbx);
    shapeToolLayout->addWidget(colorSelectBtn);
    shapeToolLayout->addWidget(lineStyleCbx);
    shapeToolLayout->addStretch();
    shapeToolLayout->setContentsMargins(1,2,1,1);            //去除边框间隙
    shapeToolLayout->setSpacing(1);
    shapeToolBar->setLayout(shapeToolLayout);
    shapeToolBar->setVisible(false);

    //初始化拓展工具的初始状态
    lineSizeCbx->setCurrentText(QString::number(drawPen.width()));
    QColor color=drawPen.brush().color();
    QString colorStyle=QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue());
    colorSelectBtn->setStyleSheet(colorStyle);
    switch(drawPen.style())
    {
    case Qt::SolidLine:
        lineStyleCbx->setCurrentIndex(0);
        break;
    case Qt::DashLine:
        lineStyleCbx->setCurrentIndex(1);
        break;
    case Qt::DotLine:
        lineStyleCbx->setCurrentIndex(2);
        break;
    case Qt::DashDotLine:
        lineStyleCbx->setCurrentIndex(3);
        break;
    case Qt::DashDotDotLine:
        lineStyleCbx->setCurrentIndex(4);
        break;
    default:
        lineStyleCbx->setCurrentIndex(0);
        break;
    }

    QVBoxLayout *toolLayout=new QVBoxLayout();
    toolLayout->addWidget(MainToolBar);
    toolLayout->addWidget(shapeToolBar);
    toolLayout->setContentsMargins(0,0,0,0);            //去除边框间隙
    toolLayout->setSpacing(0);
    toolBar->setLayout(toolLayout);
    toolBar->setVisible(false);

    drawLineBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
    drawRectBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
    drawEllipseBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");

    hideToolBar();

    connect(drawLineBtn,SIGNAL(clicked(bool)),this,SLOT(drawLineSlot()));
    connect(drawRectBtn,SIGNAL(clicked(bool)),this,SLOT(drawRectSlot()));
    connect(drawEllipseBtn,SIGNAL(clicked(bool)),this,SLOT(drawEllipseSlot()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(cancelSlot()));
    connect(saveFileAsBtn,SIGNAL(clicked(bool)),this,SLOT(saveFileAsSlot()));
    connect(saveFileBtn,SIGNAL(clicked(bool)),this,SLOT(saveFileSlot()));

    //拓展工具信号槽
    connect(lineSizeCbx,SIGNAL(currentTextChanged(QString)),this,SLOT(changePenWidthSlot(QString)));
    connect(colorSelectBtn,SIGNAL(clicked(bool)),this,SLOT(changePenColorSlot()));
    connect(lineStyleCbx,SIGNAL(currentIndexChanged(int)),SLOT(changePenStyleSlot(int)));
}

void SnipCanvas::showToolBar()            //显示工具条
{
    qreal x,y;

    int bar_width=toolBar->width();
    int bar_height=toolBar->height();
    int offset=5;

    if(snipArea.bottomLeft().x()+bar_width+offset<screenWidth)      //x轴方向边距足够
    {
        x=snipArea.bottomLeft().x()+offset;
    }
    else                                                //x轴方向边距不足
    {
        x=screenWidth-(bar_width+offset);
    }

    if(screenHeight-snipArea.bottomLeft().y()>(bar_height+offset))      //下边距充足
    {
        y=snipArea.bottomLeft().y()+offset;
    }
    else if(snipArea.topLeft().y()>(bar_height+offset))                  //上边距充足
    {
        y=snipArea.topLeft().y()-(bar_height+offset);
    }
    else if(snipArea.topLeft().y()<0)                   //上下边距都不够,且上边距在桌面外
    {
        y=offset;
    }
    else                                                //上下边距都不够,且上边距在桌面内
    {
        y=snipArea.topLeft().y()+offset;
    }
    toolBar->move(x,y);
    toolBar->setVisible(true);
}

void SnipCanvas::hideToolBar()                    //掩藏工具条
{
    toolBar->setVisible(false);
}

void SnipCanvas::drawLineSlot()
{
    if(drawEditFlag!=1)
    {
        drawEditFlag=1;
        drawLineBtn->setStyleSheet("background-color: rgb(193, 205, 205);border:2px;border-radius:6px;padding:4px;");
        drawRectBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawEllipseBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        shapeToolBar->setVisible(true);
        toolBar->adjustSize();
        showToolBar();
    }
    else
    {
        drawEditFlag=0;
        drawLineBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawRectBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawEllipseBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        shapeToolBar->setVisible(false);
        toolBar->adjustSize();
        showToolBar();
    }
}

void SnipCanvas::drawRectSlot()
{
    if(drawEditFlag!=2)
    {
        drawEditFlag=2;
        drawLineBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawRectBtn->setStyleSheet("background-color: rgb(193, 205, 205);border:2px;border-radius:6px;padding:4px;");
        drawEllipseBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        shapeToolBar->setVisible(true);
        toolBar->adjustSize();
        showToolBar();
    }
    else
    {
        drawEditFlag=0;
        drawLineBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawRectBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawEllipseBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        shapeToolBar->setVisible(false);
        toolBar->adjustSize();
        showToolBar();
    }
}

void SnipCanvas::drawEllipseSlot()
{
    if(drawEditFlag!=3)
    {
        drawEditFlag=3;
        drawLineBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawRectBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawEllipseBtn->setStyleSheet("background-color: rgb(193, 205, 205);border:2px;border-radius:6px;padding:4px;");
        shapeToolBar->setVisible(true);
        toolBar->adjustSize();
        showToolBar();
    }
    else
    {
        drawEditFlag=0;
        drawLineBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawRectBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        drawEllipseBtn->setStyleSheet("background-color: rgb(240, 255, 255);border:2px;border-radius:6px;padding:4px;");
        shapeToolBar->setVisible(false);
        toolBar->adjustSize();
        showToolBar();
    }
}

void SnipCanvas::saveFileSlot()
{
    snipScreen(snipArea);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyyMMdd_hhmmss");
    QString savefile="CrzSnip_"+current_date+".jpg";

    QString savepath=SetConfig::readSetting("Setting","QuickSaveDir",".").toString();
    if(!savepath.endsWith("/"))
        savepath+="/";
    QString fileName=savepath+savefile;
    originalPixmap.save(fileName,"jpg");
    cancelSlot();
}

void SnipCanvas::saveFileAsSlot()
{
    snipScreen(snipArea);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyyMMdd_hhmmss");
    QString savefile="CrzSnip_"+current_date+".jpg";
    QString savepath=SetConfig::readSetting("Setting","QuickSaveDir",".").toString();

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),savepath+"/"+savefile,tr("JPEG File (*.jpg)"));
    originalPixmap.save(fileName,"jpg");
    cancelSlot();
}

void SnipCanvas::cancelSlot()
{
    rectFlag=DrawStatus::WAITING;
    this->close();
}

void SnipCanvas::changePenWidthSlot(QString s)
{
    drawPen.setWidth(s.toInt());
}

void SnipCanvas::changePenColorSlot()
{
    QColor color = QColorDialog::getColor(Qt::blue);
    if(color.isValid())
    {
        QString colorStyle=QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue());
        colorSelectBtn->setStyleSheet(colorStyle);
    }
    drawPen.setColor(color);
}

void SnipCanvas::changePenStyleSlot(int index)
{
    switch(index)
    {
    case 0:
        drawPen.setStyle(Qt::SolidLine);
        break;
    case 1:
        drawPen.setStyle(Qt::DashLine);
        break;
    case 2:
        drawPen.setStyle(Qt::DotLine);
        break;
    case 3:
        drawPen.setStyle(Qt::DashDotLine);
        break;
    case 4:
        drawPen.setStyle(Qt::DashDotDotLine);
        break;
    default:
        drawPen.setStyle(Qt::SolidLine);
        break;
    }
}

//通过任意两点构造一个矩形
RectPaint SnipCanvas::getRectF(QPointF p1, QPointF p2)
{
    float x1,y1,x2,y2;
    if(p1.x()<p2.x())
    {
        x1=p1.x();
        x2=p2.x();
    }
    else
    {
        x1=p2.x();
        x2=p1.x();
    }

    if(p1.y()<p2.y())
    {
        y1=p1.y();
        y2=p2.y();
    }
    else
    {
        y1=p2.y();
        y2=p1.y();
    }

    QPointF ps(x1,y1);
    QPointF pe(x2,y2);
    RectPaint rect(ps,pe);
    return rect;
}

quint8 SnipCanvas::caputerRect(QRectF t_rect,qreal t_x,qreal t_y)
{
    quint8 pos=0;
    if(qAbs(t_x-t_rect.topLeft().x())<2)           //左
    {
        if(qAbs(t_y-t_rect.topLeft().y())<2)       //上
        {
            pos=1;
        }
        else if(qAbs(t_y-t_rect.bottomRight().y())<2)   //下
        {
            pos=2;
        }
        else if( (t_y>=t_rect.topLeft().y()+2) && (t_y<=t_rect.bottomRight().y()-2) )  //上下之间
        {
            pos=3;
        }
        else
        {
            pos=0;
        }
    }
    else if(qAbs(t_x-t_rect.bottomRight().x())<2)           //右
    {
        if(qAbs(t_y-t_rect.topLeft().y())<2)                //上
        {
            pos=4;
        }
        else if(qAbs(t_y-t_rect.bottomRight().y())<2)       //下
        {
            pos=5;
        }
        else if( (t_y>=t_rect.topLeft().y()+2) && (t_y<=t_rect.bottomRight().y()-2) )  //上下之间
        {
            pos=6;
        }
        else
        {
            pos=0;
        }
    }
    else if((t_x>=t_rect.topLeft().x()+2) && (t_x<=t_rect.bottomRight().x()-2))          //左右之间
    {
        if(qAbs(t_y-t_rect.topLeft().y())<2)            //上
        {
            pos=7;
        }
        else if(qAbs(t_y-t_rect.bottomRight().y())<2)   //下
        {
            pos=8;
        }
        else if( (t_y>=t_rect.topLeft().y()+2) && (t_y<=t_rect.bottomRight().y()-2) )  //上下之间
        {
            pos=9;
        }
        else
        {
            pos=0;
        }
    }
    else
    {
        pos=0;
    }
    return pos;
}
