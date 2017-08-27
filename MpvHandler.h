//声明：此文件中的所有代码均摘录自“https://github.com/u8sand/Baka-MPlayer/blob/master/src/mpvhandler.h”，并做了部分改动

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include <mpv/client.h>

#include "mpvtypes.h"

#define MPV_REPLY_COMMAND 1
#define MPV_REPLY_PROPERTY 2

class MpvHandler : public QObject
{
    Q_OBJECT

public:
    explicit MpvHandler(int64_t wid, QObject *parent = nullptr);
    ~MpvHandler();

    void Initialize();
    const Mpv::FileInfo &getFileInfo()      { return fileInfo; }
    Mpv::PlayState getPlayState()           { return playState; }
    QString getFile()                       { return file; }
    QString getPath()                       { return path; }
    QString getScreenshotFormat()           { return screenshotFormat; }
    QString getScreenshotTemplate()         { return screenshotTemplate; }
    QString getScreenshotDir()              { return screenshotDir; }
    QString getVo()                         { return vo; }
    QString getMsgLevel()                   { return msgLevel; }
    double getSpeed()                       { return speed; }
    int getTime()                           { return time; }
    int getDuration()                       { return duration; }
    int getVolume()                         { return volume; }
    int getVideoWidth()                     { return videoWidth; }
    int getVideoHeight()                    { return videoHeight; }
    void getVideoSize(int *w, int *h)       { *w = videoWidth; *h = videoHeight; }
    int getVid()                            { return vid; }
    int getAid()                            { return aid; }
    int getSid()                            { return sid; }
    bool getSubtitleVisibility()            { return subtitleVisibility; }
    bool getMute()                          { return mute; }

    int getOsdWidth()                       { return osdWidth; }
    int getOsdHeight()                      { return osdHeight; }

    QString getMediaInfo();

protected:
    bool FileExists(QString);

public slots:
    void LoadFile(QString);
    QString LoadPlaylist(QString);
    bool PlayFile(QString);

    void AddOverlay(int id, int x, int y, QString file, int offset, int w, int h);
    void RemoveOverlay(int id);

    void Play();
    void Pause();
    void Stop();
    void PlayPause(QString fileIfStopped);
    void Restart();
    void Rewind();
    void Mute(bool);

    void Seek(int pos, bool relative = false, bool osd = false);
    int Relative(int pos);
    void FrameStep();
    void FrameBackStep();

    void Chapter(int);
    void NextChapter();
    void PreviousChapter();

    void Volume(int, bool osd = false);
    void Speed(double);
    void Aspect(QString);
    void Vid(int);
    void Aid(int);
    void Sid(int);

    void Screenshot(bool withSubs = false);

    void ScreenshotFormat(QString);
    void ScreenshotTemplate(QString);
    void ScreenshotDirectory(QString);

    void AddSubtitleTrack(QString);
    void AddAudioTrack(QString);
    void ShowSubtitles(bool);
    void SubtitleScale(double scale, bool relative = false);

    void Deinterlace(bool);
    void Interpolate(bool);
    void Vo(QString);

    void MsgLevel(QString level);

    void ShowText(QString text);

    void LoadTracks();
    void LoadChapters();
    void LoadVideoParams();
    void LoadAudioParams();
    void LoadMetadata();
    void LoadOsdSize();

    void Command(const QStringList &strlist);
    void SetOption(QString key, QString val);

protected slots:
    void OpenFile(QString);
    QString PopulatePlaylist();
    void LoadFileInfo();
    void SetProperties();

    void AsyncCommand(const char *args[]);
    void Command(const char *args[]);
    void HandleErrorCode(int);

private slots:
    void setPlaylist(const QStringList& l)  { emit playlistChanged(l); }
    void setFileInfo()                      { emit fileInfoChanged(fileInfo); }
    void setPlayState(Mpv::PlayState s)     { emit playStateChanged(playState = s); }
    void setFile(QString s)                 { emit fileChanged(file = s); }
    void setPath(QString s)                 { emit pathChanged(path = s); }
    void setScreenshotFormat(QString s)     { emit screenshotFormatChanged(screenshotFormat = s); }
    void setScreenshotTemplate(QString s)   { emit screenshotTemplateChanged(screenshotTemplate = s); }
    void setScreenshotDir(QString s)        { emit screenshotDirChanged(screenshotDir = s); }
    void setVo(QString s)                   { emit voChanged(vo = s); }
    void setMsgLevel(QString s)             { emit msgLevelChanged(msgLevel = s); }
    void setSpeed(double d)                 { emit speedChanged(speed = d); }
    void setTime(int i)                     { emit timeChanged(time = i); }
    void setDuration(int i)                 { emit durationChanged(duration = i); }
    void setVolume(int i)                   { emit volumeChanged(volume = i); }
    void setVideoSize(int w, int h)         { emit videoSizeChanged(videoWidth = w, videoHeight = h); }
    void setIndex(int i)                    { emit indexChanged(index = i); }
    void setVid(int i)                      { emit vidChanged(vid = i); }
    void setAid(int i)                      { emit aidChanged(aid = i); }
    void setSid(int i)                      { emit sidChanged(sid = i); }
    void setSubtitleVisibility(bool b)      { emit subtitleVisibilityChanged(subtitleVisibility = b); }
    void setMute(bool b)                    { if(mute != b) emit muteChanged(mute = b); }

    void on_mpv_events();

private:
    void handle_mpv_event(mpv_event *event);

signals:
    void playlistChanged(const QStringList&);
    void fileInfoChanged(const Mpv::FileInfo&);
    void trackListChanged(const QList<Mpv::Track>&);
    void chaptersChanged(const QList<Mpv::Chapter>&);
    void videoParamsChanged(const Mpv::VideoParams&);
    void audioParamsChanged(const Mpv::AudioParams&);
    void playStateChanged(Mpv::PlayState);
    void fileChanging(int, int);
    void fileChanged(QString);
    void pathChanged(QString);
    void screenshotFormatChanged(QString);
    void screenshotTemplateChanged(QString);
    void screenshotDirChanged(QString);
    void voChanged(QString);
    void msgLevelChanged(QString);
    void speedChanged(double);
    void timeChanged(int);
    void durationChanged(int);
    void volumeChanged(int);
    void videoSizeChanged(int, int);
    void indexChanged(int);
    void vidChanged(int);
    void aidChanged(int);
    void sidChanged(int);
    void debugChanged(bool);
    void subtitleVisibilityChanged(bool);
    void muteChanged(bool);

    void messageSignal(QString m);

    void mpv_events();

private:
    mpv_handle *mpv = nullptr;

    // variables
    Mpv::PlayState playState = Mpv::Idle;
    Mpv::FileInfo fileInfo;
    QString     file,
                path,
                screenshotFormat,
                screenshotTemplate,
                screenshotDir,
                suffix,
                vo,
                msgLevel;
    double      speed = 1;
    int         time = 0,
                lastTime = 0,
                duration = 0,
                volume = 100,
                videoWidth = 0,
                videoHeight = 0,
                index = 0,
                vid,
                aid,
                sid;
    bool        init = false,
                playlistVisible = false,
                subtitleVisibility = true,
                mute = false;
    int         osdWidth,
                osdHeight;
};
