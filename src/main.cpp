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
			DisplayException(e)
			//printf("ERROR: %s [%s]\n",e.what(),e.text());
		} catch ( const std::exception &e) {
			DisplayException(e)
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


void DisplayException(const ppl7::Exception &e, QWidget *object, const ppl7::String &msg)
{
	ppl7::String a=QObject::tr("Error");
	ppl7::String title=STOPMOCAP_APPNAME;
	title+=": "+a;
	ppl7::String name=e.what();
	ppl7::String descr=ppl7::EscapeHTMLTags(e.text());

	ppl7::String m="<html><body><b>";
	if (msg.isEmpty()) {
		m+=QObject::tr("An error occured:");
	} else {
		m+=msg;
	}
	m+="</b><br>\n";
	m+=QObject::tr("Exception name")+": <b style=\"color: red;\">";
	m+=name+"</b><br>\n";
	m+=QObject::tr("Description")+": <b style=\"color: red;\">";
	m+=descr+"</b><br>\n";

	m+="</body></html>";
	QMessageBox::critical(object, title, m, QMessageBox::Ok);
}

void DisplayException(const std::exception &e, QWidget *object, const ppl7::String &msg)
{
	ppl7::String a=QObject::tr("Error");
	ppl7::String title=STOPMOCAP_APPNAME;
	title+=": "+a;
	ppl7::String name=e.what();
	ppl7::String m="<html><body><b>";
	if (msg.isEmpty()) {
		m+=QObject::tr("An error occured:");
	} else {
		m+=msg;
	}
	m+="</b><br>\n";
	m+=QObject::tr("Exception name")+": <b style=\"color: red;\">";
	m+=name+"</b><br>\n";
	m+="</body></html>";
	QMessageBox::critical(object, title, m, QMessageBox::Ok);
}
