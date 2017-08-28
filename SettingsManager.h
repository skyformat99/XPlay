#pragma once

#include "PCH.h"
#include <QSettings>

class SettingsManager
{
public:
    static SettingsManager* Instance();

    void save();
    void reload();
    void reset(bool saveNow = true);

    void setFileDir(const QString &value);
    void setCaptureDir(const QString &value);
    void setVolume(const int &value);
    void setMute(const bool &value);

    QString getFileDir() const;
    QString getCaptureDir() const;
    int getVolume() const;
    bool getMute() const;

private:
    SettingsManager();
    ~SettingsManager();

    void checkValues();

private:
    QSettings settings;
    int volume = 100;
    bool mute = false;
    QString fileDir = QString();
    QString captureDir = QString();
};
