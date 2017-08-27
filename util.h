//声明：此文件中的所有代码均摘录自“https://github.com/u8sand/Baka-MPlayer/blob/master/src/util.h”

#pragma once

#include <QWidget>
#include <QString>
#include <QTextStream>
#include "recent.h"

//class Settings;

namespace Util {

// platform specific
bool DimLightsSupported();
void SetAlwaysOnTop(WId wid, bool);
QString SettingsLocation();

bool IsValidFile(QString path);
bool IsValidLocation(QString loc); // combined file and url

void ShowInFolder(QString path, QString file);

QString MonospaceFont();

// common
bool IsValidUrl(QString url);

QString FormatTime(int time, int totalTime);
QString FormatRelativeTime(int time);
QString FormatNumber(int val, int length);
QString FormatNumberWithAmpersand(int val, int length);
QString HumanSize(qint64);
QString ShortenPathToParent(const Recent &recent);
QStringList ToNativeSeparators(QStringList list);
QStringList FromNativeSeparators(QStringList list);
int GCD(int v, int u);
QString Ratio(int w, int h);

}

inline QTextStream& qStdout()
{
    static QTextStream r{stdout};
    return r;
}
