/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
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

#ifndef SELECTSCENE_H
#define SELECTSCENE_H

#include <QtGui/QDialog>
#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "ui_selectscene.h"


class SelectScene : public QDialog, public ppl7::Thread
{
    Q_OBJECT

public:
    class FrameIdent {
    	public:
    		int num;
    		ppl7::String Filename;
    };

    class Scene
    {
    	public:
    		QTreeWidgetItem *item;
    		ppl7::String File;
    		FrameIdent first,last;
    		QImage firstIcon, lastIcon;
    };

    SelectScene(QWidget *parent = 0);
    ~SelectScene();

    void scanDir(const ppl7::String &path);
    const ppl7::String &selectedScene() const;


private:
    Ui::SelectSceneClass ui;
    ppl7::String SelectedDir;
    std::list<Scene> SceneList;

    void getFrames(const ppl7::String &path, FrameIdent &first, FrameIdent &last);
    QImage getIcon(const ppl7::String &Filename, int width, int height);

    void run();

public slots:
	void on_cancelButton_clicked();
	void on_sceneTable_itemDoubleClicked ( QTreeWidgetItem * item, int column );

	void updateTree(Scene s);


    signals:
        void update_signal(Scene s);


};

#endif // SELECTSCENE_H
