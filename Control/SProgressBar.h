#pragma once

#include "BaseWidget.h"

class SProgressBar : public BaseWidget
{
    Q_OBJECT

public:
    SProgressBar(QWidget *parent = nullptr);
    virtual ~SProgressBar();

signals:
    void sliderMoved(qint64 value);

public slots:
    void setMinimum(qint64 value);
    qint64 minimum();
    void setMaximum(qint64 value);
    qint64 maximum();
    void setValue(qint64 value);
    qint64 value();

protected:
    virtual void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    qint64 m_currentProgress, min, max;
};
