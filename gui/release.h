/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 513 $
 * $Date: 2015-07-02 18:00:30 +0200 (Do, 02. Jul 2015) $
 * $Id: ledslider.h 513 2015-07-02 16:00:30Z pafe1234@gmail.com $
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

#ifndef RELEASE_H
#define RELEASE_H
#define WITH_QT
#include <QWidget>
#include "FrameBuffer.h"
#include "ui_release.h"
#include "config.h"

class ReleaseVideo : public QDialog
{
    Q_OBJECT

public:
    ReleaseVideo(QWidget *parent = 0);
    ~ReleaseVideo();

    void setReleaseDir(const ppl7::String &dir);
    void setReleaseCommand(const ppl7::String &cmd);
    void setScene(const ppl7::String &name);
    void setSourceFrameRate(int fps);

    void exportVideo(FrameBuffer &buffer);

private:
    Ui::ReleaseDialogClass ui;
    ppl7::String ReleaseDir, ReleaseCommand;
    ppl7::String SceneName;
    int source_fps;


public slots:
	void on_abortButton_clicked();


};

#endif // RELEASE_H
