#ifndef SNIPSYSTRAY_H
#define SNIPSYSTRAY_H

#include <QWidget>
#include <QScreen>
#include <QPixmap>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include "snipcanvas.h"
#include <keystring.h>
#include <toollineedt.h>


namespace Ui {
class SnipSysTray;
}

class SnipSysTray : public QWidget,public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit SnipSysTray(QWidget *parent = 0);
    ~SnipSysTray();
    void initSysTray();
    void snipStart();
    void setHotKey();
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    quint32 nativeKeyCode(Qt::Key key);
    quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
    bool registerHotKey(Qt::Key key,Qt::KeyboardModifiers modifiers);
    bool unregisterHotKey(Qt::Key key,Qt::KeyboardModifiers modifiers);
    void setAutoRun(bool flag);

    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::SnipSysTray *ui;
    SnipCanvas *snipCanvas;
    QPixmap fullPixmap;
    QSystemTrayIcon *m_SysTray;
    QAction *setAct,*aboutAct,*quitAct;
    void closeEvent(QCloseEvent *event);
    bool closeFlag;
    Qt::Key m_Key;
    Qt::KeyboardModifiers m_Mods;
    KeyString *keyString;
    bool isAutoRun;
private slots:
    void clickTraySlot(QSystemTrayIcon::ActivationReason reason);
    void setActSlot();
    void aboutActSlot();
    void quitActSlot();
    void changeHotKeySlot(Qt::Key t_key,Qt::KeyboardModifiers t_modifiers);
    void autoRunSlot(int state);
};

#endif // SNIPSYSTRAY_H
