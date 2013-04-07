/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "selectscene.h"
#include <QTreeWidgetItem>

SelectScene::SelectScene(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	qRegisterMetaType<Scene>("Scene");

//	connect(this, SIGNAL(update_signal(Scene)),
//	            this, SLOT(updateTree(Scene)), Qt::QueuedConnection);

	connect(this, SIGNAL(update_signal(Scene)),
	            this, SLOT(updateTree(Scene)), Qt::AutoConnection);

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

QImage SelectScene::getIcon(const ppl7::String &Filename, int width, int height)
{

	ppl7::grafix::Image img;
	try {
		img.load(Filename);
		QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
		return qi.scaled(width,height,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	} catch (...) {
		return QImage();
	}
}

void SelectScene::scanDir(const ppl7::String &path)
{
	ppl7::String Tmp;
	ui.sceneTable->clear();
	ppl7::Dir dir;
	//ppl7::PrintDebugTime("SelectScene::scanDir: %s\n",(const char*)path);
	dir.open(path,ppl7::Dir::SORT_FILENAME);
	ppl7::DirEntry e;
	ppl7::Dir::Iterator it;
	dir.reset(it);
	QTreeWidgetItem *item;
	ui.sceneTable->setSortingEnabled(false);

	while (dir.getNext(e,it)) {
		if (e.Filename=="." || e.Filename=="..") continue;
		if (e.isDir()==true || e.isLink()==true) {
			//getFrames(e.File,first,last);
			item=new QTreeWidgetItem;
			item->setText(0,e.Filename);
			Scene s;
			s.item=item;
			s.File=e.File;
			SceneList.push_back(s);
			ui.sceneTable->addTopLevelItem(item);
		}
	}
	//ui.sceneTable->ad
	ui.sceneTable->sortByColumn ( 0, Qt::AscendingOrder );
	ui.sceneTable->setSortingEnabled(true);

	threadStart();
}

void SelectScene::run()
{
	std::list<Scene>::iterator it;
	for (it=SceneList.begin();it!=SceneList.end();it++) {
		if (threadShouldStop()) break;
		//ppl7::PrintDebugTime("SelectScene::scanDir: next: %s\n",(const char*)it->File);
		getFrames(it->File,it->first,it->last);
		//ppl7::PrintDebugTime ("   first=%i, last=%i\n",it->first.num,it->last.num);
		if (threadShouldStop()) break;
		if (it->first.Filename.notEmpty()) {
			it->firstIcon=getIcon(it->first.Filename,60,60*9/16);
		}
		if (threadShouldStop()) break;
		if (it->last.Filename.notEmpty()) {
			it->lastIcon=getIcon(it->last.Filename,60,60*9/16);
		}
		if (threadShouldStop()) break;

		emit update_signal(*it);

	}
}

void SelectScene::updateTree(Scene s)
{
	ppl7::String Tmp;
	//ppl7::PrintDebugTime ("SelectScene::updateTree\n");
	if (s.first.num>=0) Tmp.setf("%i",(s.last.num-s.first.num+1));
	else Tmp.set("0");
	s.item->setText(1,Tmp);
	if (s.first.Filename.notEmpty()) {
		s.item->setIcon(2,QPixmap::fromImage(s.firstIcon));
	}
	if (s.last.Filename.notEmpty()) {
		s.item->setIcon(3,QPixmap::fromImage(s.lastIcon));
	}
	qApp->processEvents();

}



void SelectScene::on_cancelButton_clicked()
{
	threadStop();
	done(0);
}

void SelectScene::on_sceneTable_itemDoubleClicked ( QTreeWidgetItem * item, int  )
{
	threadStop();
	SelectedDir=item->text(0);
	done(1);
}

const ppl7::String &SelectScene::selectedScene() const
{
	return SelectedDir;
}
