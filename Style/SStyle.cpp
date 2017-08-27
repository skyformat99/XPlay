#include "SStyle.h"
#include <QFile>
#include <QTextStream>

SStyle::SStyle()
{
    //读取皮肤样式文本
    QFile skinFile(":/QSS/Default.qss");
    skinFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts(&skinFile);
    m_styleStr = ts.readAll();
    skinFile.close();
}

SStyle::~SStyle()
{

}

QString SStyle::GetStyle()
{
    return m_styleStr;
}
