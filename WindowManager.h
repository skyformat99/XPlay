﻿#pragma once

#include "MainWindow.h"

//窗口管理类，可用于后期扩展使用
class WindowManager
{

private:
    WindowManager();
    ~WindowManager();

public:
    //单例模式
    static WindowManager* Instance();
    void SetMainWindow(MainWindow *pMainWindow);
    MainWindow* GetMainWindow();

private:
    MainWindow *m_pMainWindow;
};
