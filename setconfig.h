#ifndef SETCONFIG_H
#define SETCONFIG_H

#include <QVariant>
#include <QString>
#include <QSettings>

class SetConfig
{
public:
    static void writeSetting(const QString &group,const QString &key,const QVariant &value)//write ini file
    {
        QSettings settings("./setting.ini",QSettings::IniFormat);
        settings.beginGroup(group);
        settings.setValue(key,value);
    }
    static QVariant readSetting(const QString &group,const QString &key,const QVariant &defaultvalue)//read ini file
    {
        QVariant val;
        QString keypos=group+"/"+key;
        QSettings settings("./setting.ini",QSettings::IniFormat);
        val=settings.value(keypos,defaultvalue);
        return val;
    }
};

#endif // SETCONFIG_H
