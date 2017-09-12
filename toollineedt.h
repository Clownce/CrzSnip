#ifndef TOOLLINEEDT_H
#define TOOLLINEEDT_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include <QMouseEvent>
#include "keystring.h"

class ToolLineEdt : public QLineEdit
{
    Q_OBJECT
public:
    Qt::KeyboardModifiers m_Mods;
    Qt::Key m_Key;
    QString saveDir;
    KeyString *m_KeyStr;
    ToolLineEdt(QWidget *parent=0);
    void setHotKey(Qt::Key t_key,Qt::KeyboardModifiers t_mod);
    void setShowText(Qt::Key t_key,Qt::KeyboardModifiers t_mod);
    void setKeyString(KeyString *t_keystring);
private:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:
    void HotKeyChangedSig(Qt::Key t_key,Qt::KeyboardModifiers t_mod);
    void QuickSaveDirChangedSig(QString path);
};

#endif // TOOLLINEEDT_H
