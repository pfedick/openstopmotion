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

#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#define WITH_QT

#include <QtGui/QWidget>
#include <QSlider>
#include <QTimer>
#include <QSizePolicy>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "arduino.h"
#include "config.h"
#include "ledslider.h"
#include "ui_ledcontrol.h"

class LedControl : public QWidget
{
    Q_OBJECT

public:
    LedControl(QWidget *parent = 0);
    ~LedControl();

    void setConfig (Config &conf);
    void setArduino (Arduino &conf);
    void setColorScheme(int scheme);
    void setCurrentFrame(int frame);

    void load(const ppl7::String &filename);
    void save(const ppl7::String &filename);

protected:
    void resizeEvent ( QResizeEvent * event );
    void showEvent (  QShowEvent * event );
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::LedControlClass ui;
    Config *conf;
    Arduino *arduino;
    LedSlider *slider[12];
    QTimer *PlaybackTimer;
    ppl7::grafix::Image img;
    int myColorScheme;
    ppl7::grafix::Color lc[12];
    ppl7::String Filename;

    ppl7::AVLTree<int, int> keyframes[12];
    ppl7::AVLTree<int, int> interpolatedframes[12];

    void updateFrameView();
    void recalcFrames(int id);
    bool unsaved;

    void remindSave();

public slots:

	void on_connectButton_clicked();
	void on_slider_valueChanged_fired(int id, int value);
	void on_keyFrameSet_fired(int id, int value);
	void on_keyFrameDelete_fired(int id);
	void on_maxFrame_textChanged(const QString & text);
	void on_offsetFrame_textChanged(const QString & text);

	void on_frameSlider_valueChanged(int value);

	void on_loadButton_clicked();
	void on_saveButton_clicked();
	void on_saveAsButton_clicked();

	void on_frameNextButton_clicked();
	void on_frameBackButton_clicked();
	void on_playButton_clicked();
	void on_stopButton_clicked();

	void on_playbackTimer_fired();

};

#endif // LEDCONTROL_H
