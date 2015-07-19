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

#ifndef FRAMEPAINT_H
#define FRAMEPAINT_H

#include <QDialog>
#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "FrameBuffer.h"
#include "ppl7framepainter.h"
#include "config.h"
#include <QTimer>

#include "ui_framepaint.h"


class FramePaint : public QWidget
{
    Q_OBJECT

public:


    FramePaint(QWidget *parent = 0);
    ~FramePaint();
    void setFrameBuffer(FrameBuffer &buffer);
    void setConfig (Config &conf);
    void setMaxFrameNumber(int num);
    void setFrameRate(int fps);
    void init();
    void clearCache();
    void setDarkColorScheme(bool flag);


private:
    Ui::FramePaintClass ui;
    Config *conf;
    FrameBuffer *fBuffer;
    int lastFrameNum;
    bool inPlayback;
    int interpolateSequence;
    QTimer *PlaybackTimer;

    ppl7::grafix::Image currentFrame;
    ppl7::grafix::Image displayFrame;

    void updateCurrentFrame(int frame);

public slots:
	void on_frameSlider_sliderPressed();
	void on_frameSlider_sliderReleased();
	void on_frameSlider_valueChanged ( int value );

	void on_overlaySlider_valueChanged ( int value );

	void on_lightAndDarkButton_toggled(bool checked);

	// Playback-Steuerung
	void on_frameFirstButton_clicked();
	void on_frameLastButton_clicked();
	void on_frameBackButton_clicked();
	void on_frameForwardButton_clicked();
    void on_playButton_clicked();
    void on_stopButton_clicked();
    void on_frameRate_valueChanged(int value);


    void on_playbackTimer_fired();



};

#endif // FRAMEPAINT_H
