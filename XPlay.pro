#-------------------------------------------------
#
# XPlay
#
#-------------------------------------------------

QT += \
    core \
    gui \
    widgets \
    winextras

contains(QT_ARCH, x86_64) {
    TARGET = XPlay64
} else {
    TARGET = XPlay
}

TEMPLATE = app

INCLUDEPATH += \
    $$PWD \
    $$PWD/include

DEPENDPATH += \
    $$PWD \
    $$PWD/include

PRECOMPILED_HEADER = $$PWD/PCH.h

LIBS += \
    -lUser32 \
    -lShell32 \
    -L$$PWD/lib\
    -lmpv

CONFIG += \
    C++11 \
    C++14

DESTDIR = bin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    $$PWD/MainWindow.h \
    $$PWD/StandardDialog/StandardDialog.h \
    $$PWD/StandardDialog/StandardHeader.h \
    $$PWD/Control/BaseWidget.h \
    $$PWD/Control/ObjectFourTuple.h \
    $$PWD/Control/SButton.h \
    $$PWD/WindowManager.h \
    $$PWD/PCH.h \
    $$PWD/Style/SStyle.h \
    $$PWD/StandardDialog/StandardBottom.h \
    $$PWD/Control/SProgressBar.h \
    $$PWD/MpvHandler.h \
    $$PWD/mpvtypes.h \
    $$PWD/recent.h \
    $$PWD/util.h \
    $$PWD/Common/Common.h \
    $$PWD/SettingsManager.h

SOURCES += \
    $$PWD/main.cpp\
    $$PWD/MainWindow.cpp \
    $$PWD/StandardDialog/StandardDialog.cpp \
    $$PWD/StandardDialog/StandardHeader.cpp \
    $$PWD/Control/BaseWidget.cpp \
    $$PWD/Control/SButton.cpp \
    $$PWD/WindowManager.cpp \
    $$PWD/Style/SStyle.cpp \
    $$PWD/StandardDialog/StandardBottom.cpp \
    $$PWD/Control/SProgressBar.cpp \
    $$PWD/MpvHandler.cpp \
    $$PWD/util.cpp \
    $$PWD/Common/Common.cpp \
    $$PWD/SettingsManager.cpp

RESOURCES += \
    $$PWD/Resources/Icon/Icon.qrc \
    $$PWD/Resources/Image/Image.qrc \
    $$PWD/Resources/QSS/QSS.qrc \
    $$PWD/Resources/i18n/i18n.qrc

TRANSLATIONS += \
    $$PWD/Resources/i18n/XPlay_zh_CN.ts

RC_ICONS = $$PWD/Resources/Icon/XPlay.ico

RC_LANG = 0x0004

VERSION = 1.0.1.0

QMAKE_TARGET_COMPANY = "wangwenx190"

QMAKE_TARGET_DESCRIPTION = "XPlay - Enjoy geek life"

QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2017 by wangwenx190. All rights reserved."

QMAKE_TARGET_PRODUCT = "XPlay"
