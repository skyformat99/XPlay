#include "PCH.h"
#include <QApplication>
#include <QFileInfo>
#include <clocale>
#include "MainWindow.h"
#include "Style/SStyle.h"
#include "mpvtypes.h"
#include "WindowManager.h"

//重载QApplication，使用其消息分发处理;
class SApplication : public QApplication
{
public:
    SApplication(int &argc, char **argv):QApplication(argc,argv)
    {
        setApplicationName(QString::fromLatin1(AppName));
        setApplicationVersion(QString::fromLatin1(AppVersion));
        setOrganizationName(QString::fromLatin1(AppPublisher));
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

    //显示主界面;
    MainWindow window;
    WindowManager::Instance()->SetMainWindow(&window);
    window.show();

    if (app.arguments().count() > 1)
    {
        QFileInfo fi(app.arguments().at(1));
        if (fi.exists() && fi.isFile())
        {
            QString mSuffix = QString("*.") + fi.suffix();
            if (Mpv::media_filetypes.contains(mSuffix))
            {
                window.play(app.arguments().at(1));
            }
        }
    }

    return app.exec();
}
