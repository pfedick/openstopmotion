#include "stopmocap.h"

#include <QtGui>
#include <QApplication>
#ifdef WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef WIN32
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr)) {
#endif
		ppl7::grafix::Grafix gfx;
		QApplication a(argc, argv);
		QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
		QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


		StopMoCap w;
		w.show();
		int ret=a.exec();
#ifdef WIN32
		CoUninitialize();
#endif
		return ret;
	}
    /*
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    int ret=a.exec();
    return ret;
	*/
}
