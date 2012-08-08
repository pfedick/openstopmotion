#include "selectscene.h"
#include <QTreeWidgetItem>

SelectScene::SelectScene(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

SelectScene::~SelectScene()
{

}

void SelectScene::getFrames(const ppl7::String &path, FrameIdent &first, FrameIdent &last)
{
	first.num=-1;
	last.num=-1;
	ppl7::Dir dir;
	dir.open(path);
	ppl7::DirEntry e;
	ppl7::Dir::Iterator it;
	ppl7::Array matches;
	dir.reset(it);
	while (dir.getNextRegExp(e,it,"/^frame_[0-9]+\\..*$/")) {
		if (e.Filename.pregMatch("/^frame_0*([1-9]+[0-9]*)\\..*$/",matches)) {
			int id=matches[1].toInt();
			if (first.num<0) {
				first.num=id;
				first.Filename=e.File;
			}
			if (last.num<0) {
				last.num=id;
				last.Filename=e.File;
			}
			if (id<first.num) {
				first.num=id;
				first.Filename=e.File;
			}
			if (id>last.num) {
				last.num=id;
				last.Filename=e.File;
			}
		}
	}
}

QIcon SelectScene::getIcon(const ppl7::String &Filename, int width, int height)
{

	ppl7::grafix::Image img;
	try {
		img.load(Filename);
		QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
		return QPixmap::fromImage(qi.scaled(width,height,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	} catch (...) {
		return QIcon();
	}
}

void SelectScene::scanDir(const ppl7::String &path)
{
	ppl7::String Tmp;
	ui.sceneTable->clear();
	ppl7::Dir dir;
	dir.open(path,ppl7::Dir::SORT_FILENAME);
	ppl7::DirEntry e;
	ppl7::Dir::Iterator it;
	dir.reset(it);
	FrameIdent first,last;
	QTreeWidgetItem *item;
	while (dir.getNext(e,it)) {
		if (e.Filename=="." || e.Filename=="..") continue;
		if (e.isDir()==true || e.isLink()==true) {
			getFrames(e.File,first,last);
			//printf ("e.Filename=%s\n",(const char*)e.Filename);
			//printf ("   e.File=%s\n",(const char*)e.File);
			//printf ("   first=%i, last=%i\n",first.num,last.num);
			item=new QTreeWidgetItem;
			item->setText(0,e.Filename);
			if (first.num>=0) Tmp.setf("%i",(last.num-first.num+1));
			else Tmp.set("0");
			item->setText(1,Tmp);
			if (first.Filename.notEmpty()) {
				item->setIcon(2,getIcon(first.Filename,60,60*9/16));
			}
			if (last.Filename.notEmpty()) {
				item->setIcon(3,getIcon(last.Filename,60,60*9/16));
			}
			ui.sceneTable->addTopLevelItem(item);
		}
	}
	//ui.sceneTable->ad
}


void SelectScene::on_cancelButton_clicked()
{
	done(0);
}

void SelectScene::on_sceneTable_itemDoubleClicked ( QTreeWidgetItem * item, int  )
{
	SelectedDir=item->text(0);
	done(1);
}

const ppl7::String &SelectScene::selectedScene() const
{
	return SelectedDir;
}
