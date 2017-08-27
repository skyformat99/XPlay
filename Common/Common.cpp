#include "Common.h"
#include <QMimeDatabase>
#include <QSettings>
#include <QtDebug>
#include <QTranslator>
#include <QCoreApplication>
#include <QtGlobal>

Common::Common()
{

}

QStringList Common::supportedMimeTypes()
{
    return (QStringList() << QString::fromLatin1("video/3gpp")
                          << QString::fromLatin1("video/x-flv")
                          << QString::fromLatin1("video/x-matroska")
                          << QString::fromLatin1("application/vnd.rn-realmedia")
                          << QString::fromLatin1("video/dl")
                          << QString::fromLatin1("video/dv")
                          << QString::fromLatin1("video/fli")
                          << QString::fromLatin1("video/m4v")
                          << QString::fromLatin1("video/vnd.mpegurl")
                          << QString::fromLatin1("video/webm")
                          << QString::fromLatin1("video/x-ms-wmv")
                          << QString::fromLatin1("video/x-ms-asf")
                          << QString::fromLatin1("video/x-msvideo")
                          << QString::fromLatin1("video/isivideo")
                          << QString::fromLatin1("video/x-la-asf")
                          << QString::fromLatin1("video/x-m4v")
                          << QString::fromLatin1("video/x-mng")
                          << QString::fromLatin1("video/quicktime")
                          << QString::fromLatin1("video/x-sgi-movie")
                          << QString::fromLatin1("video/mpeg")
                          << QString::fromLatin1("video/mp4")
                          << QString::fromLatin1("video/x-pv-pvx")
                          << QString::fromLatin1("video/vnd.rn-realvideo")
                          << QString::fromLatin1("video/vdo")
                          << QString::fromLatin1("video/vivo")
                          << QString::fromLatin1("video/x-ms-wm")
                          << QString::fromLatin1("video/x-ms-wmx")
                          << QString::fromLatin1("video/wavelet")
            << QString::fromLatin1("video/x-ms-wvx"));
}

QStringList Common::supportedSuffixes()
{
    QStringList mSupportedSuffixes;
    QMimeDatabase mMimeDatabase;
    const QStringList mSupportedMimeTypes = Common::supportedMimeTypes();
    for (const QString &mFileType : mSupportedMimeTypes)
    {
        const QStringList mSuffixes = mMimeDatabase.mimeTypeForName(mFileType).suffixes();
        for (QString mSuffix : mSuffixes)
        {
            mSuffix.prepend(QString::fromLatin1("*."));
            mSupportedSuffixes << mSuffix;
        }
    }
    return mSupportedSuffixes;
}

bool Common::associateFileTypes()
{
    QString displayName = QGuiApplication::applicationDisplayName();
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();

    const QString key = QString::fromLatin1("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    if (settings.status() != QSettings::NoError)
    {
        qWarning() << "Cannot access registry key" << key;
        return false;
    }
    settings.setValue(QString::fromLatin1("FriendlyAppName"), displayName);

    settings.beginGroup(QString::fromLatin1("SupportedTypes"));
    QMimeDatabase mimeDatabase;
    const QStringList mSupportedMimeTypes = Common::supportedMimeTypes();
    for (const QString &fileType : mSupportedMimeTypes)
    {
        const QStringList suffixes = mimeDatabase.mimeTypeForName(fileType).suffixes();
        for (QString suffix : suffixes)
        {
            suffix.prepend('.');
            settings.setValue(suffix, QString());
        }
    }
    settings.endGroup();

    settings.beginGroup(QString::fromLatin1("shell"));
    settings.beginGroup(QString::fromLatin1("open"));
    settings.setValue(QString::fromLatin1("FriendlyAppName"), displayName);
    settings.beginGroup(QString::fromLatin1("Command"));
    settings.setValue(QString::fromLatin1(".")
                      , QLatin1Char('"') + QDir::toNativeSeparators(filePath)
                      + QString::fromLatin1("\" \"%1\""));

    qDebug() << "File association registry key has been set.";

    return true;
}

void Common::unassociateFileTypes()
{
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();
    const QString key = QString::fromLatin1("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    settings.clear();
    qDebug() << "File association registry key has been removed.";
}

bool Common::isFileTypesAssociated()
{
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();
    const QString key = QString::fromLatin1("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    if (settings.contains(QString::fromLatin1("FriendlyAppName")))
    {
        qDebug() << "Video file association check : associated.";
        return true;
    }
    qDebug() << "Video file association check : not associated.";
    return false;
}

bool Common::executeProgramWithAdministratorPrivilege(const QString &exePath
                                                          , const QString &exeParam)
{
    if (exePath.isEmpty())
    {
        qDebug() << "[UAC] : Empty file path.";
        return false;
    }

    SHELLEXECUTEINFO execInfo{sizeof(SHELLEXECUTEINFO)};
    execInfo.lpVerb = TEXT("runas");
    execInfo.lpFile = reinterpret_cast<const wchar_t *>(exePath.utf16());
    execInfo.nShow = SW_HIDE;
    execInfo.lpParameters = reinterpret_cast<const wchar_t *>(exeParam.utf16());

    if(!ShellExecuteEx(&execInfo))
    {
        DWORD dwStatus = GetLastError();
        if(dwStatus == ERROR_CANCELLED)
        {
            qDebug() << "[UAC] : User denied to give admin privilege.";
        }
        else if(dwStatus == ERROR_FILE_NOT_FOUND)
        {
            qDebug() << "[UAC] : File not found -- " << exePath.toUtf8().constData();
        }
        return false;
    }
    return true;
}

void Common::load_qm(const QString &lang)
{
    QString l;
    if (!lang.isEmpty())
    {
        l = lang;
    }
    if (l.toLower() == QLatin1String("system"))
    {
        l = QLocale::system().name();
    }
    QTranslator *QtTs = new QTranslator(qApp);
    if (QtTs->load(":/i18n/Qt_" + l))
    {
        qApp->installTranslator(QtTs);
    }
    else
    {
        delete QtTs;
    }
    QTranslator *XPlayTs = new QTranslator(qApp);
    if (XPlayTs->load(":/i18n/XPlay_" + l))
    {
        qApp->installTranslator(XPlayTs);
    }
    else
    {
        delete XPlayTs;
    }
}

bool Common::createMutex(HANDLE *m_pMutexHandleOut, int *m_iMutexStateOut)
{
    *m_pMutexHandleOut = CreateMutex(NULL, TRUE
                 , reinterpret_cast<const wchar_t *>(QString::fromLatin1(AppID).utf16()));
    DWORD dwRet = GetLastError();
    if (*m_pMutexHandleOut)
    {
        if (ERROR_ALREADY_EXISTS == dwRet)
        {
            qDebug() << "[Mutex] : Application already running.";
            /*QString splayerCommand;
            if (app.arguments().count() > 1)
            {
                splayerCommand = app.arguments().at(1);
            }
            else
            {
                splayerCommand = QString::fromLatin1("ActiveMainWindow");
            }
            HWND hwnd = NULL;
            do
            {
               LPWSTR path = (LPWSTR)QString::fromLatin1(AppDisplayName).utf16();  //path = L"SendMessage"
               hwnd = ::FindWindowW(NULL, path);
            } while (hwnd == reinterpret_cast<HWND>(window.effectiveWinId())); // 忽略自己
            if (::IsWindow(hwnd))
            {
                QByteArray data = splayerCommand.toUtf8();

                COPYDATASTRUCT copydata;
                copydata.dwData = CUSTOM_SPLAYER_TYPE;  // 用户定义数据
                copydata.lpData = data.data();  //数据大小
                copydata.cbData = data.size();  // 指向数据的指针

                HWND sender = reinterpret_cast<HWND>(window.effectiveWinId());

                ::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(sender), reinterpret_cast<LPARAM>(&copydata));
            }
            CloseHandle(*m_pMutexHandleOut);
            return 0;*/
            *m_iMutexStateOut = -1;
            return false;
        }
    }
    else
    {
        qDebug() << "[Mutex] : Failed to create mutex. Program exit.";
        CloseHandle(*m_pMutexHandleOut);
        //return -1;
        *m_iMutexStateOut = -2;
        return false;
    }
    qDebug() << "[Mutex] : Create mutex succeeded.";
    *m_iMutexStateOut = 0;
    return true;
}

bool Common::isDigitStr(const QString &str)
{
    const char *s = str.toUtf8().constData();

    while (*s && *s>='0' && *s<='9')
    {
        s++;
    }

    if (*s)
    {
        //不是纯数字
        return false;
    }
    else
    {
        //纯数字
        return true;
    }
}
