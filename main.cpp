#include "PCH.h"
#include <QApplication>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <clocale>
#include "MainWindow.h"
#include "Style/SStyle.h"
#include "mpvtypes.h"
#include "WindowManager.h"
#include "Common/Common.h"

//重载QApplication，使用其消息分发处理;
class SApplication : public QApplication
{
public:
    SApplication(int &argc, char **argv):QApplication(argc,argv)
    {
        setApplicationName(QString::fromLatin1(AppName));
        setApplicationDisplayName(QString::fromLatin1(AppDisplayName));
        setApplicationVersion(QString::fromLatin1(AppVersion));
        setOrganizationName(QString::fromLatin1(AppPublisher));
        setOrganizationDomain(QString::fromLatin1(AppPublisherURL));
        setAttribute(Qt::AA_EnableHighDpiScaling);
        setFont(QFont("Microsoft YaHei"));
        setWindowIcon(QIcon(":/Icon/XPlay.ico"));
    }

protected:
    //消息投递;
    virtual bool notify(QObject *object, QEvent *event)
    {
        return QApplication::notify(object,event);
    }

};

//程序入口;
int main(int argc, char *argv[])
{
    SApplication app(argc, argv);

    //皮肤样式类;
    SStyle style;
    app.setStyleSheet(style.GetStyle());

    // Qt sets the locale in the QApplication constructor, but libmpv requires
    // the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");

    QDir::setCurrent(qApp->applicationDirPath());

    Common::load_qm();

    if (app.arguments().contains(QString::fromLatin1("--assoc"), Qt::CaseInsensitive))
    {
        if (!Common::associateFileTypes())
        {
            QMessageBox::critical(nullptr, QObject::tr("Error")
                                  , QObject::tr("Cannot associate file types."));
            return -1;
        }
        return 0;
    }
    else if (app.arguments().contains(QString::fromLatin1("--unassoc")
                                      , Qt::CaseInsensitive))
    {
        Common::unassociateFileTypes();
        return 0;
    }

    HANDLE m_hAppMutex;
    int m_iMutexCreateState;
    Common::createMutex(&m_hAppMutex, &m_iMutexCreateState);
    if (m_iMutexCreateState == -1)
    {
        //程序已经运行
        //return 0;
    }
    else if (m_iMutexCreateState == -2)
    {
        //创建Mutex失败
        return -1;
    }
    else
    {
        //Mutex成功创建
    }

    //显示主界面;
    MainWindow window;
    WindowManager::Instance()->SetMainWindow(&window);
    window.show();

    if (app.arguments().count() > 1)
    {
        QFileInfo fi(app.arguments().at(1));
        if (fi.exists() && fi.isFile())
        {
            QMimeDatabase mdb;
            if (Common::supportedMimeTypes().contains(mdb.mimeTypeForFile(fi).name()))
            {
                window.play(app.arguments().at(1));
            }
        }
    }

    int ret = app.exec();

    if (m_iMutexCreateState != -2)
    {
        CloseHandle(m_hAppMutex);
    }

    return ret;
}
