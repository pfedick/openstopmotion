#include "stopmocap.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
	ppl7::grafix::Grafix gfx;
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    StopMoCap w;
    w.show();
    return a.exec();

    /*
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    int ret=a.exec();
    return ret;
	*/
}
