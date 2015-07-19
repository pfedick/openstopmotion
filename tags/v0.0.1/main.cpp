#include "stopmocap.h"

#include <QtGui>
#include <QApplication>
#ifdef WIN32
#include <windows.h>
#endif

#undef CATCH

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


#ifdef CATCH
		try {
#endif
			StopMoCap w;
			w.show();
			int ret=a.exec();
#ifdef WIN32
			CoUninitialize();
#endif
			return ret;
#ifdef CATCH
		} catch ( const ppl7::Exception &e) {
			printf("ERROR: %s [%s]\n",e.what(),e.text());
		} catch (...) {
			printf ("Unknown Exception\n");
		}
#endif
	return 1;
	/*
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    int ret=a.exec();
    return ret;
	*/
}
