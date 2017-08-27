#pragma once

#include "PCH.h"
#include <QString>
#include <QStringList>

class Common
{
public:
    Common();

    static QStringList supportedMimeTypes();
    static QStringList supportedSuffixes();
    static bool associateFileTypes();
    static void unassociateFileTypes();
    static bool isFileTypesAssociated();
    static bool executeProgramWithAdministratorPrivilege(const QString &exePath
                                                         , const QString &exeParam);

    static void load_qm(const QString &lang = QLatin1String("system"));

    static bool createMutex(HANDLE *m_pMutexHandleOut, int *m_iMutexStateOut);

    static bool isDigitStr(const QString &str);
};
