#include "SProgressBar.h"
#include <QPainter>
#include <QMouseEvent>

SProgressBar::SProgressBar(QWidget *parent) : BaseWidget(parent)
  , m_currentProgress(0)
  , min(0)
  , max(0)
{

}

SProgressBar::~SProgressBar()
{

}

void SProgressBar::setMinimum(qint64 value)
{
    min = value;
}

qint64 SProgressBar::minimum()
{
    return min;
}

void SProgressBar::setMaximum(qint64 value)
{
    max = value;
}

qint64 SProgressBar::maximum()
{
    return max;
}

qint64 SProgressBar::value()
{
    return m_currentProgress;
}

void SProgressBar::setValue(qint64 value)
{
    if (value <= min)
    {
        m_currentProgress = min;
    }
    else if (value >= max)
    {
        m_currentProgress = max;
    }
    else
    {
        m_currentProgress = value;
    }
    update();
}

void SProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter objPainter(this);
    objPainter.setRenderHint(QPainter::Antialiasing);
    //绘背景
    objPainter.fillRect(rect(),QColor(31,31,31));
    //绘内容区背景
    objPainter.fillRect(contentsRect(),QColor(78,78,78));
    qint64 range = max - min;
    if (range <= 0)
    {
        range = 1;
    }
    qint64 nWidth = static_cast<float>(contentsRect().width())
            * static_cast<float>(m_currentProgress) / static_cast<float>(range);
    //绘进度条背景;
    objPainter.fillRect(contentsRect().x(),contentsRect().y(),nWidth,contentsRect().height(),QColor(26,158,255));

    BaseWidget::paintEvent(event);
}

void SProgressBar::mousePressEvent(QMouseEvent *event)
{
    qint64 range = max - min;
    if (range <= 0)
    {
        range = 1;
    }
    qint64 value = (static_cast<float>(event->pos().x()) / static_cast<float>(width()))
                                                                * static_cast<float>(range);
    setValue(value);
    emit sliderMoved(value);
    BaseWidget::mousePressEvent(event);
}
