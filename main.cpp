#include "snipsystray.h"
#include <QApplication>
/*
 * 感谢程序原作者QTCN网友qiuzhidian，
 * 我在原程序基础上做了修改。
 *
 *
 **/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    SnipSysTray w;
    w.setWindowIcon(QIcon(":/CrzSnip.png"));
    w.hide();
    a.installNativeEventFilter(&w);

    return a.exec();
}
