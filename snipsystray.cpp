#include "snipsystray.h"
#include "ui_snipsystray.h"
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include "setconfig.h"

SnipSysTray::SnipSysTray(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SnipSysTray)
{
    ui->setupUi(this);
    closeFlag=false;
    isAutoRun=false;
    snipCanvas=NULL;
    keyString=new KeyString;
    ui->hotKeyEdt->setKeyString(keyString);
    ui->hotKeyEdt->installEventFilter(this);
    ui->saveDirEdt->installEventFilter(this);
    QWidget *emptyParent=new QWidget;//隐藏任务栏图标
    setParent(emptyParent);
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint);
    this->setWindowTitle(tr("Setting"));
    this->setWindowIcon(QIcon(":/CrzSnip.png"));
    initSysTray();
    //读取热键
    QString hotKeyStr=SetConfig::readSetting("HotKey","CrzSnip","CTRL+F1").toString();
    keyString->String2Key(hotKeyStr,m_Key,m_Mods);
    ui->hotKeyEdt->setHotKey(m_Key,m_Mods);
    setHotKey();
    //开机启动
    isAutoRun=bool(SetConfig::readSetting("Setting","AutoRun",0).toInt());
    ui->checkBox->setChecked(isAutoRun);
    setAutoRun(isAutoRun);
    //读取快速存储路径
    QString savePath=SetConfig::readSetting("Setting","QuickSaveDir","D:/").toString();
    ui->saveDirEdt->setText(savePath);

    connect(ui->hotKeyEdt,SIGNAL(HotKeyChangedSig(Qt::Key,Qt::KeyboardModifiers )),this,SLOT(changeHotKeySlot(Qt::Key,Qt::KeyboardModifiers)));
    connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(autoRunSlot(int)));
}

SnipSysTray::~SnipSysTray()
{
    unregisterHotKey(m_Key,m_Mods);
    delete ui;
}
void SnipSysTray::initSysTray()
{
    m_SysTray=new QSystemTrayIcon(this);
    m_SysTray->setIcon(QIcon(":/CrzSnip.png"));
    m_SysTray->setToolTip("CrzSnip");

    setAct=new QAction(QIcon(":/AppIcon/setting.png"),tr("Setting(&S)"),this);
    aboutAct=new QAction(QIcon(":/AppIcon/about.png"),tr("About(&A)"),this);
    quitAct=new QAction(QIcon(":/AppIcon/quit.png"),tr("Quit(&Q)"),this);

    QMenu *sysTrayMenu=new QMenu(this);
    sysTrayMenu->addAction(setAct);
    sysTrayMenu->addAction(aboutAct);
    sysTrayMenu->addSeparator();
    sysTrayMenu->addAction(quitAct);

    m_SysTray->setContextMenu(sysTrayMenu);
    m_SysTray->show();
    connect(m_SysTray,&QSystemTrayIcon::activated,this,&SnipSysTray::clickTraySlot);
    connect(setAct,&QAction::triggered,this,&SnipSysTray::setActSlot);
    connect(aboutAct,&QAction::triggered,this,&SnipSysTray::aboutActSlot);
    connect(quitAct,&QAction::triggered,this,&SnipSysTray::quitActSlot);
}
void SnipSysTray::snipStart()
{
    if(snipCanvas!=NULL)
    {
        delete snipCanvas;
        snipCanvas=NULL;
    }
    QScreen *screen=QGuiApplication::primaryScreen();
    fullPixmap=screen->grabWindow(0);
    snipCanvas=new SnipCanvas(0);
    snipCanvas->setBackgroundPixmap(fullPixmap);
}

void SnipSysTray::clickTraySlot(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        snipStart();
        break;
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    default:
        break;
    }
}
void SnipSysTray::setActSlot()
{
    this->show();
}
void SnipSysTray::aboutActSlot()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("About");
    msgBox.setText("CrzSnip is a useful screenshot tool for you.");
    msgBox.setWindowIcon(QIcon(":/CrzSnip.png"));
    msgBox.setIconPixmap(QPixmap(":/CrzSnip.png").scaled(QSize(50,50)));
    msgBox.exec();
}
void SnipSysTray::quitActSlot()
{
    closeFlag=true;
    this->close();
    QApplication::quit();
}
void SnipSysTray::closeEvent(QCloseEvent *event)
{
    if(closeFlag==true)
    {
        event->accept();
    }
    else
    {
        this->hide();
        event->ignore();
    }
}
void SnipSysTray::setHotKey()
{
    m_Key=Qt::Key_F1;
    m_Mods=Qt::ControlModifier;
    registerHotKey(m_Key,m_Mods);
}
bool SnipSysTray::nativeEventFilter(const QByteArray &eventType, void *message, long *result)//windows事件捕捉器
{
    Q_UNUSED(result)
    if(eventType=="windows_generic_MSG"||eventType=="windows_dispatcher_MSG")
    {
        MSG *msg=reinterpret_cast<MSG*>(message);
        if(msg->message==WM_HOTKEY)
        {
            if(HIWORD(msg->lParam)==nativeKeyCode(m_Key)&&LOWORD(msg->lParam)==nativeModifiers(m_Mods))
            {
                snipStart();
            }
        }
    }
    return false;
}
quint32 SnipSysTray::nativeKeyCode(Qt::Key key)
{
    switch (key)
    {
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Pause:
        return VK_PAUSE;
    case Qt::Key_Print:
        return VK_PRINT;
    case Qt::Key_Clear:
        return VK_CLEAR;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    case Qt::Key_F13:
        return VK_F13;
    case Qt::Key_F14:
        return VK_F14;
    case Qt::Key_F15:
        return VK_F15;
    case Qt::Key_F16:
        return VK_F16;
    case Qt::Key_F17:
        return VK_F17;
    case Qt::Key_F18:
        return VK_F18;
    case Qt::Key_F19:
        return VK_F19;
    case Qt::Key_F20:
        return VK_F20;
    case Qt::Key_F21:
        return VK_F21;
    case Qt::Key_F22:
        return VK_F22;
    case Qt::Key_F23:
        return VK_F23;
    case Qt::Key_F24:
        return VK_F24;
    case Qt::Key_Space:
        return VK_SPACE;
    case Qt::Key_Asterisk:
        return VK_MULTIPLY;
    case Qt::Key_Plus:
        return VK_ADD;
    case Qt::Key_Comma:
        return VK_SEPARATOR;
    case Qt::Key_Minus:
        return VK_SUBTRACT;
    case Qt::Key_Slash:
        return VK_DIVIDE;
        // numbers
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        return key;
        // letters
    case Qt::Key_A:
    case Qt::Key_B:
    case Qt::Key_C:
    case Qt::Key_D:
    case Qt::Key_E:
    case Qt::Key_F:
    case Qt::Key_G:
    case Qt::Key_H:
    case Qt::Key_I:
    case Qt::Key_J:
    case Qt::Key_K:
    case Qt::Key_L:
    case Qt::Key_M:
    case Qt::Key_N:
    case Qt::Key_O:
    case Qt::Key_P:
    case Qt::Key_Q:
    case Qt::Key_R:
    case Qt::Key_S:
    case Qt::Key_T:
    case Qt::Key_U:
    case Qt::Key_V:
    case Qt::Key_W:
    case Qt::Key_X:
    case Qt::Key_Y:
    case Qt::Key_Z:
        return key;
    default:
        return 0;
    }
}
quint32 SnipSysTray::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
    // MOD_ALT, MOD_CONTROL, (MOD_KEYUP), MOD_SHIFT, MOD_WIN
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier)
        native |= MOD_SHIFT;
    if (modifiers & Qt::ControlModifier)
        native |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier)
        native |= MOD_ALT;
    if (modifiers & Qt::MetaModifier)
        native |= MOD_WIN;
    return native;
}
bool SnipSysTray::registerHotKey(Qt::Key key, Qt::KeyboardModifiers modifiers)//注册快捷键
{
    const quint32 nativeKey=nativeKeyCode(key);
    const quint32 nativeMods=nativeModifiers(modifiers);
    return RegisterHotKey(0,nativeMods^nativeKey,nativeMods,nativeKey);
}
bool SnipSysTray::unregisterHotKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    return UnregisterHotKey(0,(quint32)nativeModifiers(modifiers)^(quint32)nativeKeyCode(key));
}
void SnipSysTray::changeHotKeySlot(Qt::Key t_key, Qt::KeyboardModifiers t_modifiers)
{
    unregisterHotKey(m_Key,m_Mods);
    m_Key=t_key;
    m_Mods=t_modifiers;
    registerHotKey(m_Key,m_Mods);
    QString strval=keyString->Key2String(m_Key,m_Mods);
    SetConfig::writeSetting("HotKey","CrzSnip",strval);
}
void SnipSysTray::autoRunSlot(int state)
{
    if(state==Qt::Checked)
    {
        isAutoRun=true;
        SetConfig::writeSetting("Setting","AutoRun",1);
    }
    else
    {
        isAutoRun=false;
        SetConfig::writeSetting("Setting","AutoRun",0);
    }
    setAutoRun(isAutoRun);
}
void SnipSysTray::setAutoRun(bool flag)
{
    QSettings *reg=new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    if(flag==true)
    {
        reg->setValue("CrzSnip",QApplication::applicationFilePath().replace("/","\\"));
    }
    else
    {
        reg->remove("CrzSnip");
    }
}
bool SnipSysTray::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->saveDirEdt)
    {
        if(event->type()==QEvent::MouseButtonDblClick)
        {
            QString path=QFileDialog::getExistingDirectory(0,"Setting",ui->saveDirEdt->text());
            if(!path.isEmpty())
            {
                ui->saveDirEdt->saveDir=path;
                ui->saveDirEdt->setText(path);
                SetConfig::writeSetting("Setting","QuickSaveDir",path);
            }
        }
    }
    else if(watched==ui->hotKeyEdt)
    {
        if(event->type()==QEvent::KeyPress)
        {
            QKeyEvent *e=static_cast<QKeyEvent *>(event);
            ui->hotKeyEdt->m_Key=Qt::Key(e->key());
            ui->hotKeyEdt->m_Mods=e->modifiers();
            emit ui->hotKeyEdt->HotKeyChangedSig(ui->hotKeyEdt->m_Key,ui->hotKeyEdt->m_Mods);
            ui->hotKeyEdt->setShowText(ui->hotKeyEdt->m_Key,ui->hotKeyEdt->m_Mods);
        }
    }
    return QWidget::eventFilter(watched,event);
}
