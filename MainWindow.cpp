﻿#include "PCH.h"
#include <stdexcept>
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QCursor>
#include <QTimer>
#include <QLayout>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QShortcut>
#include <QPalette>
#include <QFileInfo>
#include "util.h"

static bool isPlaying = false;
static int thinProgressBar = 5;

//构造函数
MainWindow::MainWindow(QWidget *parent) : StandardDialog(parent)
  , mCursorTimer(0)
  , mpv_container(NULL)
  , m_pProgressBar(NULL)
  , m_pMainMenu(NULL)
  , mpvHandler(NULL)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    //设置头部区域属性
    GetHeader()->SetTitleText(QString::fromLatin1(AppName));
    GetHeader()->SetTitleCenter(true);
    GetHeader()->GetBgColor().SetAllState(MAIN_HB_COLOR);
    GetHeader()->setFixedHeight(30);
    GetBottom()->GetBgColor().SetAllState(MAIN_HB_COLOR);
    GetBottom()->setFixedHeight(0);

    //设置内容区域属性
    BaseWidget *pCenterContainer = new BaseWidget(this);
    mpv_container = new QWidget(pCenterContainer);
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    mpv_container->setAutoFillBackground(true);
    mpv_container->setPalette(pal);
    mpv_container->setMouseTracking(true);
    mpv_container->setAttribute(Qt::WA_DontCreateNativeAncestors);
    mpv_container->setAttribute(Qt::WA_NativeWindow);
    // If you have a HWND, use: int64_t wid = (intptr_t)hwnd;
    int64_t wid = mpv_container->winId();
    mpvHandler = new MpvHandler(wid, mpv_container);
    mpvHandler->Initialize();
    m_pProgressBar = new SProgressBar(pCenterContainer);
    QVBoxLayout *pVBoxLayout = new QVBoxLayout(pCenterContainer);
    pVBoxLayout->setContentsMargins(0,0,0,0);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->addWidget(mpv_container);
    pVBoxLayout->addWidget(m_pProgressBar);
    SetCenterWidget(pCenterContainer);

    CreateMainMenu();

    setAcceptDrops(true);

    createShortcuts();

    setMinimumSize(500, 400);

    QDesktopWidget desktop;
    thinProgressBar = static_cast<float>(desktop.screenGeometry().height()) * 0.005;
    m_pProgressBar->setFixedHeight(thinProgressBar);
    resize(1280, GetHeader()->height() + 720 + m_pProgressBar->height());
    move((desktop.availableGeometry().width() - width()) / 2
         , (desktop.availableGeometry().height() - height()) / 2);

    connect(mpvHandler, SIGNAL(durationChanged(int)), this, SLOT(setProgressBarRange(int)));
    connect(mpvHandler, SIGNAL(timeChanged(int)), this, SLOT(setProgressBarPos(int)));
    connect(m_pProgressBar, SIGNAL(sliderMoved(qint64)), this, SLOT(seek(qint64)));
    connect(mpvHandler, SIGNAL(videoSizeChanged(int,int)), this, SLOT(changeWindowSizeToVideoSize(int,int)));

    QTimer::singleShot(50, this, SLOT(forceUpdateWindow()));
}

MainWindow::~MainWindow()
{
    if (mpvHandler)
    {
        delete mpvHandler;
        mpvHandler = NULL;
    }
}

void MainWindow::play()
{
    mpvHandler->Play();
}

void MainWindow::play(const QString &filePath)
{
    mpvHandler->PlayFile(mpvHandler->LoadPlaylist(filePath));
    if (QFileInfo(filePath).exists() && QFileInfo(filePath).isFile())
    {
        autoLoadExternalSubtitleFile(filePath);
    }
    GetHeader()->SetTitleText(QFileInfo(filePath).fileName());
    setWindowTitle(QFileInfo(filePath).fileName());
    killTimer(mCursorTimer);
    mCursorTimer = startTimer(3000);
    isPlaying = true;
}

void MainWindow::open()
{
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open a media file"));
    fileDialog.setNameFilters(QStringList()
                              << QString::fromLatin1("*")
                              << Mpv::video_filetypes);
    //fileDialog.setDirectory(QFileInfo(Config::instance().lastFile()).absoluteDir());
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QString file = fileDialog.selectedUrls().constFirst().toLocalFile();
        if (file.isEmpty())
        {
            return;
        }
        if (QFileInfo(file).exists() && QFileInfo(file).isFile())
        {
            play(file);
        }
    }
}

void MainWindow::openURL()
{
    QString url = QInputDialog::getText(this, tr("Open an URL")
                                        , QString::fromLatin1("URL"));
    if (url.isEmpty())
    {
        return;
    }
    play(url);
}

void MainWindow::stop()
{
    if (windowState() == Qt::WindowFullScreen)
    {
        toggleFullscreen();
    }
    killTimer(mCursorTimer);
    mCursorTimer = 0;
    unsetCursor();
    showThinProgressBar();
    showWindowHeader();
    mpvHandler->Stop();
    isPlaying = false;
}

void MainWindow::pause()
{
    mpvHandler->Pause();
}

void MainWindow::togglePlayPause()
{
    if (isPlaying)
    {
        mpvHandler->ShowText(tr("Pause"));
        mpvHandler->Pause();
    }
    else
    {
        mpvHandler->Play();
        mpvHandler->ShowText(tr("Play"));
    }
    isPlaying = !isPlaying;
}

//创建菜单项
void MainWindow::CreateMainMenu()
{
    m_pMainMenu = new QMenu(this);
    QAction *openFileAct = m_pMainMenu->addAction(tr("Open file"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(open()));
    QAction *openURLAct = m_pMainMenu->addAction(tr("Open URL"));
    connect(openURLAct, SIGNAL(triggered()), this, SLOT(openURL()));
    m_pMainMenu->addSeparator();
    QAction *helpAct = m_pMainMenu->addAction(tr("Help"));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));
    QAction *aboutAct = m_pMainMenu->addAction(tr("About"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    m_pMainMenu->addSeparator();
    QAction *exitAct = m_pMainMenu->addAction(tr("Exit"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(Slot_CloseClicked()));
}

void MainWindow::createShortcuts()
{
    QShortcut *aboutShortcut = new QShortcut(QKeySequence::HelpContents, this);
    connect(aboutShortcut, SIGNAL(activated()), this, SLOT(help()));

    QShortcut *quitShortcut = new QShortcut(QKeySequence::Cancel, this);
    connect(quitShortcut, SIGNAL(activated()), this, SLOT(escKeyPressed()));

    QShortcut *openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, SIGNAL(activated()), this, SLOT(open()));

    QShortcut *togglePlayPauseShortcut = new QShortcut(Qt::Key_Space, this);
    connect(togglePlayPauseShortcut, SIGNAL(activated()), this, SLOT(togglePlayPause()));

    QShortcut *toggleFullscreenShortcut = new QShortcut(QKeySequence::FullScreen, this);
    connect(toggleFullscreenShortcut, SIGNAL(activated()), this
                                                              , SLOT(toggleFullscreen()));

    QShortcut *toggleFullscreenShortcut2 = new QShortcut(Qt::Key_Return, this);
    connect(toggleFullscreenShortcut2, SIGNAL(activated()), this
                                                              , SLOT(toggleFullscreen()));

    QShortcut *toggleFullscreenShortcut3 = new QShortcut(Qt::Key_Enter, this);
    connect(toggleFullscreenShortcut3, SIGNAL(activated()), this
                                                              , SLOT(toggleFullscreen()));

    QShortcut *toggleMuteShortcut = new QShortcut(Qt::Key_M, this);
    connect(toggleMuteShortcut, SIGNAL(activated()), this, SLOT(toggleMuteState()));

    QShortcut *forwardShortcut = new QShortcut(Qt::Key_Right, this);
    connect(forwardShortcut, SIGNAL(activated()), this, SLOT(seekForward()));

    QShortcut *backwardShortcut = new QShortcut(Qt::Key_Left, this);
    connect(backwardShortcut, SIGNAL(activated()), this, SLOT(seekBackward()));

    QShortcut *increaseShortcut = new QShortcut(Qt::Key_Up, this);
    connect(increaseShortcut, SIGNAL(activated()), this, SLOT(increaseVolume()));

    QShortcut *decreaseShortcut = new QShortcut(Qt::Key_Down, this);
    connect(decreaseShortcut, SIGNAL(activated()), this, SLOT(decreaseVolume()));

    QShortcut *nextChapterShortcut = new QShortcut(QKeySequence::MoveToNextPage, this);
    connect(nextChapterShortcut, SIGNAL(activated()), this, SLOT(seekNextChapter()));

    QShortcut *previousChapterShortcut = new QShortcut(QKeySequence::MoveToPreviousPage
                                                                                  , this);
    connect(previousChapterShortcut, SIGNAL(activated()), this
                                                           , SLOT(seekPreviousChapter()));
}

void MainWindow::autoLoadExternalSubtitleFile(const QString &filePath)
{
    if (filePath.isEmpty())
    {
        qDebug() << QString::fromLatin1("[Load external subtitle] : Empty file path : \"")
                 << filePath.toUtf8().constData()
                 << QString::fromLatin1("\"");
        return;
    }
    //下面这一段是查找和给定文件除后缀名外文件名完全相同的文件，找到就自动加载后返回
    QString fileDir = QFileInfo(filePath).absolutePath() + QString::fromLatin1("/");
    QString subFileBasePath = fileDir + QFileInfo(filePath).completeBaseName();
    QString subFileFullPath = subFileBasePath + QString::fromLatin1(".ass");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    subFileFullPath = subFileBasePath + QString::fromLatin1(".srt");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    subFileFullPath = subFileBasePath + QString::fromLatin1(".sub");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    subFileFullPath = subFileBasePath + QString::fromLatin1(".idx");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    //下面这一段是查找和给定文件名字相似的文件，找到就自动加载第一个（如果有不止一个的话）后返回
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters << QString::fromLatin1("*.ass")
            << QString::fromLatin1("*.srt")
            << QString::fromLatin1("*.idx")
            << QString::fromLatin1("*.sub");
    dir.setNameFilters(filters);
    QFileInfoList fiList = dir.entryInfoList();
    if (fiList.count() <= 0)
    {
        return;
    }
    for (int i = 0; i <= (fiList.count() - 1); ++i)
    {
        if (fiList.at(i).baseName().toLower() == QFileInfo(filePath).baseName().toLower())
        {
            loadExternalSubtitleFile(fiList.at(i).absoluteFilePath());
            return;
        }
    }
}

//单击Logo图标，弹出菜单
void MainWindow::Slot_LogoClicked()
{
    QPoint pt = GetHeader()->rect().bottomLeft();
    m_pMainMenu->popup(GetHeader()->mapToGlobal(QPoint(pt.x(),pt.y()+1)));
}

void MainWindow::Slot_MaxClicked()
{
    QRect mRestoreGeometry = geometry();
    QSize mRestoreSize = size();
    bool changed = false;
    if (!isDialogMaximized() && m_pProgressBar->height() <= thinProgressBar)
    {
        changed = true;
        showNormalProgressBar();
    }
    StandardDialog::Slot_MaxClicked();
    if (changed)
    {
        setRestoreGeometry(mRestoreGeometry);
        setRestoreSize(mRestoreSize);
    }
    changed = false;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == mCursorTimer)
    {
        setCursor(Qt::BlankCursor);
        if (!GetHeader()->rect().contains(GetHeader()->mapFromGlobal(QCursor::pos())))
        {
            hideWindowHeader();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    unsetCursor();
    if (m_pProgressBar->rect().contains(m_pProgressBar->mapFromGlobal(QCursor::pos())))
    {
        showNormalProgressBar();
    }
    else
    {
        showThinProgressBar();
    }
    if (GetHeader()->rect().contains(GetHeader()->mapFromGlobal(QCursor::pos())))
    {
        showWindowHeader();
    }
    if (windowState() != Qt::WindowFullScreen)
    {
        StandardDialog::mouseMoveEvent(event);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!event->angleDelta().isNull())
    {
        if (event->angleDelta().y() > 0)
        {
            increaseVolume();
        }
        else if (event->angleDelta().y() < 0)
        {
            decreaseVolume();
        }
    }
    StandardDialog::wheelEvent(event);
}

static bool canHandleDrop(const QDragEnterEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.size() < 1)
    {
        return false;
    }
    QString filePath = urls.constFirst().toLocalFile();
    QString mSuffix = QString("*.") + QFileInfo(filePath).suffix();
    if (Mpv::media_filetypes.contains(mSuffix))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->setAccepted(canHandleDrop(event));
    StandardDialog::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    event->accept();
    play(event->mimeData()->urls().constFirst().toLocalFile());
    StandardDialog::dropEvent(event);
}

void MainWindow::help()
{

}

void MainWindow::about()
{

}

void MainWindow::toggleFullscreen()
{
    if (windowState() == Qt::WindowFullScreen)
    {
        m_pProgressBar->parentWidget()->layout()->addWidget(m_pProgressBar);
    }
    else
    {
        m_pProgressBar->parentWidget()->layout()->removeWidget(m_pProgressBar);
        QDesktopWidget desktop;
        m_pProgressBar->setGeometry(0
                             , desktop.screenGeometry().height() - m_pProgressBar->height()
                             , desktop.screenGeometry().width(), m_pProgressBar->height());
    }
    setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void MainWindow::toggleMuteState()
{
    mpvHandler->Mute(!mpvHandler->getMute());
}

void MainWindow::seekBackward()
{
    if (mpvHandler->getPlayState() != Mpv::PlayState::Stopped
                && mpvHandler->getPlayState() != Mpv::PlayState::Idle)
    {
        mpvHandler->ShowText(QString("%1/%2")\
             .arg(Util::FormatTime(mpvHandler->getTime() - 5, mpvHandler->getDuration()))\
             .arg(Util::FormatTime(mpvHandler->getDuration(), mpvHandler->getDuration())));
        mpvHandler->Seek(-5, true);
    }
}

void MainWindow::seek(qint64 pos)
{
    if (mpvHandler->getPlayState() != Mpv::PlayState::Stopped
                && mpvHandler->getPlayState() != Mpv::PlayState::Idle)
    {
        mpvHandler->ShowText(QString("%1/%2")\
             .arg(Util::FormatTime(static_cast<int>(pos), mpvHandler->getDuration()))\
             .arg(Util::FormatTime(mpvHandler->getDuration(), mpvHandler->getDuration())));
        mpvHandler->Seek(static_cast<int>(pos));
    }
}

void MainWindow::seekForward()
{
    if (mpvHandler->getPlayState() != Mpv::PlayState::Stopped
                && mpvHandler->getPlayState() != Mpv::PlayState::Idle)
    {
        mpvHandler->ShowText(QString("%1/%2")\
             .arg(Util::FormatTime(mpvHandler->getTime() + 5, mpvHandler->getDuration()))\
             .arg(Util::FormatTime(mpvHandler->getDuration(), mpvHandler->getDuration())));
        mpvHandler->Seek(5, true);
    }
}

void MainWindow::seekNextChapter()
{
    mpvHandler->NextChapter();
}

void MainWindow::seekPreviousChapter()
{
    mpvHandler->PreviousChapter();
}

void MainWindow::escKeyPressed()
{
    if (windowState() == Qt::WindowFullScreen)
    {
        toggleFullscreen();
        return;
    }
    if (mpvHandler->getPlayState() == Mpv::PlayState::Playing)
    {
        stop();
        return;
    }
    Slot_CloseClicked();
}

void MainWindow::increaseVolume()
{
    int vol = mpvHandler->getVolume() + 1;
    if (vol > 100)
    {
        vol = 100;
    }
    mpvHandler->Volume(vol, true);
}

void MainWindow::decreaseVolume()
{
    int vol = mpvHandler->getVolume() - 1;
    if (vol < 0)
    {
        vol = 0;
    }
    mpvHandler->Volume(vol, true);
}

void MainWindow::setProgressBarPos(int pos)
{
    m_pProgressBar->setValue(pos);
}

void MainWindow::setProgressBarRange(int duration)
{
    m_pProgressBar->setMinimum(0);
    m_pProgressBar->setMaximum(duration);
}

void MainWindow::showNormalProgressBar()
{
    if (!m_pProgressBar->isVisible())
    {
        m_pProgressBar->show();
    }
    if (m_pProgressBar->height() > thinProgressBar)
    {
        return;
    }
    m_pProgressBar->setFixedHeight(30);
    if (windowState() == Qt::WindowFullScreen)
    {
        QDesktopWidget desktop;
        m_pProgressBar->setGeometry(0
                             , desktop.screenGeometry().height() - m_pProgressBar->height()
                             , desktop.screenGeometry().width(), m_pProgressBar->height());
    }
    else
    {
        resize(width(), height() - thinProgressBar + 30);
    }
}

void MainWindow::showThinProgressBar()
{
    if (!m_pProgressBar->isVisible())
    {
        m_pProgressBar->show();
    }
    if (m_pProgressBar->height() <= thinProgressBar)
    {
        return;
    }
    m_pProgressBar->setFixedHeight(thinProgressBar);
    if (windowState() == Qt::WindowFullScreen)
    {
        QDesktopWidget desktop;
        m_pProgressBar->setGeometry(0
                             , desktop.screenGeometry().height() - m_pProgressBar->height()
                             , desktop.screenGeometry().width(), m_pProgressBar->height());
    }
    else
    {
        resize(width(), height() + thinProgressBar - 30);
    }
}

void MainWindow::showWindowHeader()
{
    if (windowState() != Qt::WindowFullScreen)
    {
        if (!GetHeader()->isVisible())
        {
            GetHeader()->show();
            move(geometry().x(), geometry().y() - GetHeader()->height());
            resize(width(), height() + GetHeader()->height());
        }
    }
}

void MainWindow::hideWindowHeader()
{
    if (windowState() != Qt::WindowFullScreen)
    {
        if (GetHeader()->isVisible())
        {
            GetHeader()->hide();
            move(geometry().x(), geometry().y() + GetHeader()->height());
            resize(width(), height() - GetHeader()->height());
        }
    }
}

void MainWindow::changeWindowSizeToVideoSize(int w, int h)
{
    if (windowState() == Qt::WindowFullScreen || isDialogMaximized())
    {
        return;
    }
    else
    {
        int h2 = GetHeader()->height() + h + 30;
        QDesktopWidget desktop;
        if (h2 >= desktop.availableGeometry().height())
        {
            int w2 = (static_cast<float>(desktop.availableGeometry().height()) * static_cast<float>(w)) / static_cast<float>(h2);
            if (GetHeader()->isVisible())
            {
                resize(w2, desktop.availableGeometry().height() - 30 + thinProgressBar);
                move((desktop.availableGeometry().width() - w2) / 2, 0);
            }
            else
            {
                resize(w2, desktop.availableGeometry().height() - GetHeader()->height() - 30 + thinProgressBar);
                move((desktop.availableGeometry().width() - w2) / 2, GetHeader()->height());
            }
        }
        else
        {
            if (GetHeader()->isVisible())
            {
                h2 = h + GetHeader()->height();
            }
            else
            {
                h2 = h;
            }
            h2 += m_pProgressBar->height();
            resize(w, h2);
        }
    }
}

void MainWindow::forceUpdateWindow()
{
    Slot_MaxClicked();
    QTimer::singleShot(50, this, SLOT(Slot_MaxClicked()));
}

void MainWindow::loadExternalSubtitleFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }
    if (QFileInfo(fileName).exists() && QFileInfo(fileName).isFile())
    {
        qDebug() << QString::fromLatin1("[Load external subtitle] : \"")
                 << fileName.toUtf8().constData()
                 << QString::fromLatin1("\" loaded.");
        mpvHandler->AddSubtitleTrack(fileName);
    }
}