#ifndef SELECTSCENE_H
#define SELECTSCENE_H

#include <QtGui/QDialog>
#include "ui_selectscene.h"

class SelectScene : public QDialog
{
    Q_OBJECT

public:
    SelectScene(QWidget *parent = 0);
    ~SelectScene();

private:
    Ui::SelectSceneClass ui;
};

#endif // SELECTSCENE_H
