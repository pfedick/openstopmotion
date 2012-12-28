#ifndef FRAMEPAINT_H
#define FRAMEPAINT_H

#include <QtGui/QDialog>
#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "ui_framepaint.h"


class FramePaint : public QWidget
{
    Q_OBJECT

public:


    FramePaint(QWidget *parent = 0);
    ~FramePaint();



private:
    Ui::FramePaintClass ui;

public slots:

};

#endif // SELECTSCENE_H
