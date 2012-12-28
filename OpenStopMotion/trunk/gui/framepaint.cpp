#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>

#include "framepaint.h"
#include <QTreeWidgetItem>

FramePaint::FramePaint(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

}

FramePaint::~FramePaint()
{

}
