#include "SettingsManager.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

SettingsManager *SettingsManager::Instance()
{
    static SettingsManager settingsManager;
    return &settingsManager;
}

void SettingsManager::save()
{
    checkValues();
    settings.setValue(QString::fromLatin1("volume"), volume);
    settings.setValue(QString::fromLatin1("mute"), mute);
    settings.setValue(QString::fromLatin1("file_dir"), fileDir);
    settings.setValue(QString::fromLatin1("capture_dir"), captureDir);
}

void SettingsManager::reload()
{
    volume = settings.value(QString::fromLatin1("volume"), 100).toInt();
    mute = settings.value(QString::fromLatin1("mute"), false).toBool();
    fileDir = settings.value(QString::fromLatin1("file_dir"), QString()).toString();
    captureDir = settings.value(QString::fromLatin1("capture_dir"), QString()).toString();
    checkValues();
}

void SettingsManager::reset(bool saveNow)
{
    volume = 100;
    mute = false;
    fileDir = QCoreApplication::applicationDirPath();
    captureDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (saveNow)
    {
        save();
    }
}

void SettingsManager::setFileDir(const QString &value)
{
    fileDir = value;
}

void SettingsManager::setCaptureDir(const QString &value)
{
    captureDir = value;
}

void SettingsManager::setVolume(const int &value)
{
    volume = value;
}

void SettingsManager::setMute(const bool &value)
{
    mute = value;
}

QString SettingsManager::getFileDir() const
{
    return fileDir;
}

QString SettingsManager::getCaptureDir() const
{
    return captureDir;
}

int SettingsManager::getVolume() const
{
    return volume;
}

bool SettingsManager::getMute() const
{
    return mute;
}

SettingsManager::SettingsManager()
{
    reload();
}

SettingsManager::~SettingsManager()
{
    save();
}

void SettingsManager::checkValues()
{
    if (volume < 0)
    {
        volume = 0;
    }
    else if (volume > 100)
    {
        volume = 100;
    }
    if (fileDir.isEmpty())
    {
        fileDir = QCoreApplication::applicationDirPath();
    }
    else if (!QDir(fileDir).exists())
    {
        fileDir = QCoreApplication::applicationDirPath();
    }
    if (captureDir.isEmpty())
    {
        captureDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
    else if (!QDir(captureDir).exists())
    {
        captureDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
}
