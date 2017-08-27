#pragma once

#include <QMenu>
#include <QWinTaskbarProgress>
#include <QWinTaskbarButton>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QWinJumpList>
#include <QWinJumpListCategory>
#include <QWinJumpListItem>
#include <QtWin>
#include "StandardDialog/StandardDialog.h"
#include "Control/SProgressBar.h"
#include "Control/SButton.h"
#include "MpvHandler.h"

class MainWindow : public StandardDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void play();
    void play(const QString &filePath);
    void open();
    void openURL();
    void stop();
    void pause();

private:
    void CreateMainMenu();
    void createShortcuts();
    void autoLoadExternalSubtitleFile(const QString &filePath);
    void createJumpList();
    void createTaskbar();
    void createThumbnailToolBar();

private slots:
    void help();
    void about();
    void togglePlayPause();
    void toggleFullscreen();
    void toggleMuteState();
    void increaseVolume();
    void decreaseVolume();
    void seek(qint64 pos);
    void seekForward();
    void seekBackward();
    void seekNextChapter();
    void seekPreviousChapter();
    void escKeyPressed();
    void setProgressBarPos(int pos);
    void setProgressBarRange(int duration);
    void showNormalProgressBar();
    void showThinProgressBar();
    void showWindowHeader();
    void hideWindowHeader();
    void changeWindowSizeToVideoSize(int w, int h);
    void forceUpdateWindow();
    void loadExternalSubtitleFile(const QString &fileName);

protected slots:
    void Slot_LogoClicked();
    void Slot_MaxClicked();

protected:
    void timerEvent(QTimerEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    //控件成员
private:
    QWidget *mpv_container;
    SProgressBar *m_pProgressBar;
    QMenu *m_pMainMenu;

    MpvHandler *mpvHandler;

    int mCursorTimer;

    QWinTaskbarButton *mpTaskbarButton;
    QWinTaskbarProgress *mpTaskbarProgress;
    QWinThumbnailToolBar *mpThumbnailToolBar;
    QWinThumbnailToolButton *mpPlayToolButton;
    QWinThumbnailToolButton *mpStopToolButton;
    QWinThumbnailToolButton *mpMuteToolButton;
};
