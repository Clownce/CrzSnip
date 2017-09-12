#include "toollineedt.h"
#include <QFileDialog>
#include <QFileInfo>

ToolLineEdt::ToolLineEdt(QWidget *parent):QLineEdit(parent)
{
    m_Mods=Qt::ControlModifier;
    m_Key=Qt::Key_F1;
    saveDir="";
    m_KeyStr=NULL;
    this->setReadOnly(true);
    this->setFocusPolicy(Qt::FocusPolicy(Qt::TabFocus|Qt::ClickFocus));
    this->setStyleSheet("QLineEdit{border:2px solid rgb(240,240,240);}");
}
void ToolLineEdt::setHotKey(Qt::Key t_key, Qt::KeyboardModifiers t_mod)
{
    m_Key=t_key;
    m_Mods=t_mod;
    setShowText(t_key,t_mod);
}
void ToolLineEdt::setKeyString(KeyString *t_keystring)
{
    m_KeyStr=t_keystring;
}
void ToolLineEdt::setShowText(Qt::Key t_key, Qt::KeyboardModifiers t_mod)
{
    QString showText=m_KeyStr->Key2String(t_key,t_mod);
    setText(showText);
}
void ToolLineEdt::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    this->setStyleSheet("QLineEdit{border:2px solid rgb(104,224,208)}");
}
void ToolLineEdt::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    this->setStyleSheet("QLineEdit{border:2px solid rgb(240,240,240)}");
}
