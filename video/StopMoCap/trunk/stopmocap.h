#ifndef STOPMOCAP_H
#define STOPMOCAP_H

#include <QtGui/QWidget>
#include "ui_stopmocap.h"

class StopMoCap : public QWidget
{
    Q_OBJECT

public:
    StopMoCap(QWidget *parent = 0);
    ~StopMoCap();

private:
    Ui::StopMoCapClass ui;
};

#endif // STOPMOCAP_H
