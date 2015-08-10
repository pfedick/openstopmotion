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

#define WITH_QT
#include <ppl7.h>
#include <ppl7-inet.h>
#include "osm.h"
#include "version.h"
#include "stopmocap.h"
#include "device.h"
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QFileDialog>
#include <QEvent>
#include <QMessageBox>
#include <QColorDialog>
#include "selectscene.h"
#include "release.h"

StopMoCap::StopMoCap(QWidget *parent)
    : QWidget(parent)
{
	statusBar=NULL;
	fpaint=NULL;
	ledcontrol=NULL;
	interpolateSequence=0;
	ppl7::String Tmp;
	ui.setupUi(this);
	ui.controlTab->setCurrentIndex(0);


	Tmp.setf("%s - Version %s",STOPMOCAP_APPNAME,STOPMOCAP_VERSION);
	this->setWindowTitle(Tmp);
	controlLayout=NULL;
	fpsTimer=0;
	fpsCounter=0;
	Timer=new QTimer(this);
	PlaybackTimer=new QTimer(this);
	conf.load();
	lastFrameNum=0;
	inPlayback=false;
	inPreviewMode=false;
	playbackFrame=0;
	motorThread=new MotorThread(this);
	QObject::connect(motorThread, SIGNAL(motorStarted()), this, SLOT(motorStarted()), Qt::QueuedConnection);
	QObject::connect(motorThread, SIGNAL(motorStopped()), this, SLOT(motorStopped()), Qt::QueuedConnection);

	connect(Timer, SIGNAL(timeout()), this, SLOT(on_timer_fired()));
	connect(PlaybackTimer, SIGNAL(timeout()), this, SLOT(on_playbackTimer_fired()));





	//DeviceCheckTimer->start(5000);
	ui.captureDir->setText(conf.CaptureDir);
	ui.sceneName->setText(conf.Scene);
	on_sceneName_editingFinished();
	ui.mergeFrames->setValue(conf.mergeFrames);
	ui.skipFrames->setValue(conf.skipFrames);
	ui.onionSkinning->setValue(conf.onionValue);
	ui.viewer->setScaling((PPL7ImageViewer::ScalingMode)conf.scalingMode);
	switch (conf.scalingMode) {
		case PPL7ImageViewer::None:
			ui.zoom11->setChecked(true);
			break;
		case PPL7ImageViewer::Fast:
			ui.zoomFast->setChecked(true);
			break;
		case PPL7ImageViewer::Smooth:
			ui.zoomSmooth->setChecked(true);
			break;
	}

	ui.frameRate->setValue(conf.frameRate);
	ui.overblendFactor->setValue(conf.overblendFactor);
	ui.interpolateFrames->setChecked(conf.interpolate);

	ui.jpegQualitySlider->setValue(conf.jpegQuality);
	ui.imageFormat->setCurrentIndex(conf.pictureFormat);
	on_imageFormat_currentIndexChanged(conf.pictureFormat);

	ui.jpegQualitySliderComp->setValue(conf.jpegQualityComp);
	ui.imageFormatComp->setCurrentIndex(conf.pictureFormatComp);
	on_imageFormatComp_currentIndexChanged(conf.pictureFormatComp);

	ui.saveCamShot->setChecked(conf.saveCamShot);
	ui.saveCompositedImage->setChecked(conf.saveComposited);

	std::list<VideoDevice> devices;
	std::list<VideoDevice>::const_iterator it;

	cam.enumerateDevices(devices);
	ui.deviceComboBox->clear();
	for (it=devices.begin();it!=devices.end();it++) {
		Devices.push_back(*it);
		ui.deviceComboBox->addItem(it->Name,(int)Devices.size()-1);
		if (it->Name==conf.DeviceName) {
			int index=ui.deviceComboBox->findText(conf.DeviceName);
			if (index>=0) ui.deviceComboBox->setCurrentIndex(index);

		}
	}
	ui.viewer->setDrawable(&grabImg);

	// ChromaKeying
	try {
		bluebox.loadBackground(conf.chromaBGImage);
		const ppl7::grafix::Image &bg=bluebox.getBGImage();
		QImage qi((uchar*)bg.adr(),bg.width(),bg.height(), bg.pitch(), QImage::Format_RGB32);
		ui.chromaBackground->setPixmap(QPixmap::fromImage(qi).scaled
				(200,140,Qt::KeepAspectRatio,Qt::SmoothTransformation)
		);
	} catch (...) {

	}
	ui.chromaKeyingEnabled->setChecked(conf.chromaKeyEnabled);
	bluebox.setBGChromaEnabled(conf.chromaKeyEnabled);
	bluebox.setColorKey(conf.chromaKey);
	bluebox.setNearTolerance(conf.chromaToleranceNear);
	bluebox.setFarTolerance(conf.chromaToleranceFar);
	bluebox.setSpillRemoval(conf.chromaSpillRemove);


	bluebox.setColorKeyFG(conf.chromaKeyFG);
	bluebox.setNearToleranceFG(conf.chromaToleranceNearFG);
	bluebox.setFarToleranceFG(conf.chromaToleranceFarFG);
	bluebox.setSpillRemovalFG(conf.chromaSpillRemoveFG);
	try {
		bluebox.loadForeground(conf.chromaFGImage);
		const ppl7::grafix::Image &bg=bluebox.getFGImage();
		QImage qi((uchar*)bg.adr(),bg.width(),bg.height(), bg.pitch(), QImage::Format_RGB32);
		ui.chromaForeground->setPixmap(QPixmap::fromImage(qi).scaled
				(200,140,Qt::KeepAspectRatio,Qt::SmoothTransformation)
		);
	} catch (...) {

	}
	ui.foregroundEnabled->setChecked(conf.foregroundEnabled);
	bluebox.setFGChromaEnabled(conf.foregroundEnabled);
	UpdateColorKeyFG(bluebox.colorKeyFG());


	bluebox.setReplaceColor(conf.replaceColor);

	bluebox.setReplaceMode(0);
	if (conf.chromaReplaceMode==1) {
		bluebox.setReplaceMode(1);
		ui.replaceChromaWithColor->setChecked(true);
	} else 	if (conf.chromaReplaceMode==2) {
		bluebox.setReplaceMode(2);
		ui.replaceChromaWithTransparent->setChecked(true);
	} else {
		ui.replaceChromaWithImage->setChecked(true);
		bluebox.setReplaceMode(0);
	}

	ui.tolNearSlider->setValue(bluebox.nearTolerance());
	ui.tolFarSlider->setValue(bluebox.farTolerance());
	//ui.spillSlider->setValue(bluebox.spillRemoval());
	UpdateColorKeyBG(bluebox.colorKey());

	ui.frameSlider->setMinimum(1);
	ui.lightAndDarkButton->setChecked(conf.darkLayout);
	on_lightAndDarkButton_toggled(conf.darkLayout);

	Tmp.setf("%d",conf.WebControlMoveSteps);
	ui.motorMoveSteps->setText(Tmp);
	Tmp.setf("%d",conf.WebControlTurnSteps);
	ui.motorTurnSteps->setText(Tmp);


	createStatusBar();

	this->restoreGeometry(conf.ScreenGeometry);
	fBuffer.loadAsync(conf.CaptureDir+"/"+conf.Scene);

}

StopMoCap::~StopMoCap()
{
	delete motorThread;
	savethread.stop();
	if (ledcontrol) {
		ledcontrol->close();
		delete ledcontrol;
		ledcontrol=NULL;
	}
	if (fpaint) {
		fpaint->close();
		delete fpaint;
		fpaint=NULL;
	}
#ifdef USERENDERTHREAD
	rthread.stop();
#endif
	PlaybackTimer->stop();
	Timer->stop();
	conf.interpolate=ui.interpolateFrames->isChecked();
	conf.frameRate=ui.frameRate->value();
	conf.overblendFactor=ui.overblendFactor->value();
	conf.mergeFrames=ui.mergeFrames->value();
	conf.skipFrames=ui.skipFrames->value();
	conf.onionValue=ui.onionSkinning->value();
	conf.DeviceName=ui.deviceComboBox->currentText();
	conf.ImageFormat=ui.formatComboBox->currentText();
	conf.ImageSize=ui.resolutionComboBox->currentText();
	conf.scalingMode=ui.viewer->scalingMode();
	conf.jpegQuality=ui.jpegQualitySlider->value();
	conf.pictureFormat=ui.imageFormat->currentIndex();
	conf.jpegQualityComp=ui.jpegQualitySliderComp->value();
	conf.pictureFormatComp=ui.imageFormatComp->currentIndex();
	conf.saveCamShot=ui.saveCamShot->isChecked();
	conf.saveComposited=ui.saveCompositedImage->isChecked();
	conf.darkLayout=ui.lightAndDarkButton->isChecked();

	conf.WebControlMoveSteps=ui.motorMoveSteps->text().toInt();
	conf.WebControlTurnSteps=ui.motorTurnSteps->text().toInt();

	conf.chromaKeyEnabled=ui.chromaKeyingEnabled->isChecked();
	if (ui.replaceChromaWithColor->isChecked()) conf.chromaReplaceMode=1;
	else if (ui.replaceChromaWithTransparent->isChecked()) conf.chromaReplaceMode=2;
	else conf.chromaReplaceMode=0;
	conf.save();
	cam.close();
	delete Timer;
}

void StopMoCap::createStatusBar()
{
	statusBar=new QStatusBar;
	this->layout()->addWidget(statusBar);
	QLabel *label= new QLabel(tr("Capture FPS:"));
	//locationLabel->setAlignment(Qt::AlignHCenter);
	//locationLabel->setMinimumSize(locationLabel->sizeHint());

	statusBar->addWidget(label);
	statusbar_fps=new QLabel(tr("0"));
	statusbar_fps->setMinimumWidth(40);
	statusbar_fps->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	statusBar->addWidget(statusbar_fps);

	label= new QLabel(tr("time grab: "));
	statusBar->addWidget(label);
	statusbar_time_grab=new QLabel(tr("0"));
	statusbar_time_grab->setMinimumWidth(40);
	statusbar_time_grab->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	statusBar->addWidget(statusbar_time_grab);


	label= new QLabel(tr("time decode: "));
	statusBar->addWidget(label);
	statusbar_time_decode=new QLabel(tr("0"));
	statusbar_time_decode->setMinimumWidth(40);
	statusbar_time_decode->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	statusBar->addWidget(statusbar_time_decode);

	label= new QLabel(tr("time total: "));
	statusBar->addWidget(label);
	statusbar_time_total=new QLabel(tr("0"));
	statusbar_time_total->setMinimumWidth(40);
	statusbar_time_total->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	statusBar->addWidget(statusbar_time_total);

}

void StopMoCap::updateStatusBar()
{
	ppl7::String Tmp;

	Tmp.setf("%0.6f s",cam.time_readFrame);
	statusbar_time_grab->setText(Tmp);

	Tmp.setf("%0.6f s",cam.time_decompress);
	statusbar_time_decode->setText(Tmp);

	Tmp.setf("%0.6f s",cam.time_total);
	statusbar_time_total->setText(Tmp);


}

void StopMoCap::on_deviceComboBox_currentIndexChanged(int index)
{
	Formats.clear();
	std::list<VideoFormat> fmt;
	std::list<VideoFormat>::const_iterator it;
	ui.formatComboBox->clear();
	cam.enumerateImageFormats(fmt,Devices[index]);
	for (it=fmt.begin();it!=fmt.end();it++) {
		Formats.push_back(*it);
		ui.formatComboBox->addItem(it->Description,(int)(Formats.size()-1));
		if (it->Description==conf.ImageFormat) {
			int index=ui.formatComboBox->findText(conf.ImageFormat);
			if (index>=0) ui.formatComboBox->setCurrentIndex(index);

		}
	}

}

void StopMoCap::on_formatComboBox_currentIndexChanged(int index)
{
	ppl7::String Tmp;
	FrameSizes.clear();
	std::list<ppl7::grafix::Size> sizes;
	std::list<ppl7::grafix::Size>::const_iterator it;
	ui.resolutionComboBox->clear();
	cam.enumerateFrameSizes(sizes,
			Devices[ui.deviceComboBox->currentIndex()],
			Formats[index]);
	for (it=sizes.begin();it!=sizes.end();it++) {
		FrameSizes.push_back(*it);
		Tmp.setf("%i x %i",it->width, it->height);
		ui.resolutionComboBox->addItem(Tmp,(int)(FrameSizes.size()-1));
		if (Tmp==conf.ImageSize) {
			int index=ui.resolutionComboBox->findText(Tmp);
			if (index>=0) ui.resolutionComboBox->setCurrentIndex(index);

		}
	}
}

void StopMoCap::on_useDevice_clicked()
{
#ifdef USERENDERTHREAD
	rthread.stop();
#endif
	cam.open(Devices[ui.deviceComboBox->currentIndex()]);

	std::list<CameraControl> controls;
	std::list<CameraControl>::const_iterator it;
	cam.enumerateControls(controls);

	if (ui.controlWidget->layout()) {
		QLayoutItem* item;
		while ( ( item = ui.controlWidget->layout()->takeAt( 0 ) ) != NULL ) {
			item->widget()->deleteLater();
	        delete item;
	    }
	    delete ui.controlWidget->layout();
	}

	controlLayout=new QVBoxLayout;
	controlLayout->setSpacing(0);
	for (it=controls.begin();it!=controls.end();it++) {
		if (it->type==CameraControl::Integer) {
			QLabel *label=new QLabel(it->Name);
			controlLayout->addWidget(label);
			MySlider *slider=new MySlider(Qt::Horizontal);
			slider->setMinimum(it->min);
			slider->setMaximum(it->max);
			slider->setSingleStep(it->step);
			slider->setPageStep(it->step);
			slider->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
			slider->setMaximumWidth(200);
			slider->cont=*it;
			slider->setProperty("ctl_type",CameraControl::Integer);
			slider->lastValue=cam.getControlValue(*it);
			slider->setSliderPosition(slider->lastValue);
			slider->installEventFilter(this);
			controlLayout->addWidget(slider);
		} else if (it->type==CameraControl::Boolean) {
			MyQCheckBox *checkbox=new MyQCheckBox;
			checkbox->setText(it->Name);
			checkbox->cont=*it;
			checkbox->setProperty("ctl_type",CameraControl::Boolean);
			checkbox->lastValue=cam.getControlValue(*it);
			if (checkbox->lastValue) checkbox->setChecked(true);
			else checkbox->setChecked(false);
			checkbox->installEventFilter(this);
			controlLayout->addWidget(checkbox);
		} else if (it->type==CameraControl::Menu) {
			QLabel *label=new QLabel(it->Name);
			controlLayout->addWidget(label);
			MyQComboBox *combobox=new MyQComboBox;
			combobox->cont=*it;
			combobox->setProperty("ctl_type",CameraControl::Menu);
			combobox->lastValue=cam.getControlValue(*it);
			combobox->installEventFilter(this);
			std::list<CameraControl::MenuItem>::const_iterator mi;
			for (mi=it->MenuItems.begin();mi!=it->MenuItems.end();mi++) {
				combobox->addItem(mi->Name,mi->id);
				if (mi->id==combobox->lastValue) {
					int index=combobox->findData(mi->id);
					if (index>=0) combobox->setCurrentIndex(index);
				}
			}
			controlLayout->addWidget(combobox);

		}
	}
	ui.controlWidget->setLayout(controlLayout);
	ui.controlWidget->updateGeometry();

	cam.startCapture(Formats[(int)ui.formatComboBox->currentIndex()],
			FrameSizes[(int)ui.resolutionComboBox->currentIndex()]);

	ui.captureButton->setFocus();
	//grabFrame();
	startCapture();
#ifdef USERENDERTHREAD
	rthread.setImageSize(ui.viewer->width(),ui.viewer->height());
	rthread.start();
#endif
}

void StopMoCap::stopCapture()
{
	if (Timer->isActive()) Timer->stop();
}

void StopMoCap::startCapture()
{
	Timer->start(10);
}

bool StopMoCap::isCaptureActive()
{
	return Timer->isActive();
}


bool StopMoCap::eventFilter(QObject *target, QEvent *event)
{
	if (consumeEvent(target,event)) return true;
	return QWidget::eventFilter(target,event);
}

bool StopMoCap::consumeEvent(QObject *target, QEvent *event)
{
	int ctltype=target->property("ctl_type").toInt();
	int type=event->type();
	//printf ("Event: %i \n",type);
	if (type==QEvent::KeyRelease
			|| type==QEvent::Paint
	) {

		if (ctltype==CameraControl::Integer) {
			MySlider *slider=(MySlider*)target;
			//printf ("slider, type: %i\n",type);
			if (slider->value()!=slider->lastValue) {
				cam.setControlValue(slider->cont,slider->value());
				slider->lastValue=slider->value();
			}

		} else if (ctltype==CameraControl::Boolean) {
			MyQCheckBox *checkbox=(MyQCheckBox*)target;
			if (checkbox->isChecked()!=checkbox->lastValue) {
				//printf ("checkbox, type: %i\n",type);
				cam.setControlValue(checkbox->cont,(int)checkbox->isChecked());
				checkbox->lastValue=checkbox->isChecked();
			}
		} else if (ctltype==CameraControl::Menu) {
			MyQComboBox *combo=(MyQComboBox*)target;
			int index=combo->currentIndex();
			int data=combo->itemData(index).toInt();
			if (data!=combo->lastValue) {
				cam.setControlValue(combo->cont,data);
				combo->lastValue=data;
			}

		}

	}
	return false;
}

void StopMoCap::resizeEvent ( QResizeEvent * event )
{
#ifdef USERENDERTHREAD
	rthread.setImageSize(ui.viewer->width(),ui.viewer->height());
#endif
	QWidget::resizeEvent(event);
}

void StopMoCap::showEvent (  QShowEvent * event )
{
	//const ppl7::grafix::Image &bg=bluebox.getBGImage();

	QWidget::showEvent(event);
}

void StopMoCap::closeEvent(QCloseEvent *event)
{
	conf.ScreenGeometry=saveGeometry();
    QWidget::closeEvent(event);
    if (fpaint) {
    	fpaint->close();
    	delete fpaint;
    	fpaint=NULL;
    }
    if (ledcontrol) {
    	ledcontrol->close();
    	delete ledcontrol;
    	ledcontrol=NULL;
    }
}


static void rotate180(ppl7::grafix::Drawable &img)
{
	int width=img.width();
	int height=img.height();
	ppl7::grafix::Color p1,p2;
	for (int y=0;y<height/2;y++) {
		for (int x=0;x<width;x++) {
			p1=img.getPixel(x,y);
			p2=img.getPixel(width-x-1,height-y-1);
			img.putPixel(x,y,p2);
			img.putPixel(width-x-1,height-y-1,p1);
		}
	}
}

void StopMoCap::grabFrame()
{
	float blendFactor=(float)ui.onionSkinning->value()/99.0f;
	try {
		if (inPreviewMode) {
			capture(grabImg);
		} else {
			cam.readFrame(grabImg);
		}
		if (ui.rotate180->isChecked()) {
			rotate180(grabImg);
		}
		if (ui.pickChromaKeyFG->isChecked()) {
			grabImg.blt(bluebox.getFGImage());
		} else if (ui.pickChromaKey->isChecked()) {

		} else {
			bluebox.process(grabImg);
			if (blendFactor>0) {
				grabImg.bltBlend(lastFrame,blendFactor);
			}
		}
		updateStatusBar();
	} catch (Device::QueryBufFailed) {
		cam.close();
		stopCapture();
	} catch (const ppl7::Exception &e) {
		e.print();
	} catch (...) {

	}
	ui.viewer->update();
}


void StopMoCap::on_timer_fired()
{
	//printf ("Timer fired\n");
	if (inPlayback) return;
	fpsCounter++;
	ppluint64 now=ppl7::GetTime();
	if (now!=fpsTimer) {
		ppl7::String Tmp;
		Tmp.setf("%i",fpsCounter);
		fpsTimer=now;
		statusbar_fps->setText(Tmp);
		fpsCounter=0;
	}
#ifdef USERENDERTHREAD
	rthread.grabNext(cam);
	printf ("StopMoCap::on_timer_fired: rthread.ready?\n");
	if (rthread.ready()) {
		printf ("yes\n");
		ui.viewer->setPixmap(rthread.getFrame());
	} else {
		printf ("no\n");
	}
#else
	grabFrame();
#endif
}


void StopMoCap::on_selectDir_clicked()
{
	Timer->stop();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 conf.CaptureDir,
                                                 QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		conf.CaptureDir=dir;
		ui.captureDir->setText(dir);
		conf.save();
	}
	Timer->start(10);
}

void StopMoCap::capture(ppl7::grafix::Image &img)
{
	if (cam.isOpen()==false) return;
	Timer->stop();
	//ui.viewer->setPixmap(QPixmap());
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QCoreApplication::processEvents();
	//ppl7::PrintDebugTime("Start capture\n");
	ppl7::grafix::Image tmp[100];
	int mergeFrames=ui.mergeFrames->value();
	int skipFrames=ui.skipFrames->value();
	for (int i=0;i<mergeFrames;i++) {
		//printf ("Capture %i\n",i);
		cam.readFrame(tmp[i]);
		if (ui.rotate180->isChecked()) {
			rotate180(tmp[i]);
		}
		for (int skip=0;skip<skipFrames;skip++) {
			cam.readFrame(img);
		}
	}
	//ppl7::PrintDebugTime("End capture\n");
	int width=tmp[0].width();
	int height=tmp[0].height();
	int r,g,b;
	ppl7::grafix::Color c;
	//ppl7::PrintDebugTime("Start merge\n");
	img.create(width,height,ppl7::grafix::RGBFormat::A8R8G8B8);
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			r=g=b=0;
			for (int layer=0;layer<mergeFrames;layer++) {
				c=tmp[layer].getPixel(x,y);
				r+=c.red();
				g+=c.green();
				b+=c.blue();
			}
			c.setColor(r/mergeFrames,g/mergeFrames,b/mergeFrames);
			img.putPixel(x,y,c);
		}
	}
	//ppl7::PrintDebugTime("End merge\n");
	QApplication::restoreOverrideCursor();

	Timer->start(10);

}

void StopMoCap::on_captureButton_clicked()
{
	if (cam.isOpen()==false) return;
	ppl7::String Tmp;
	SaveJob *job=NULL;
	ppl7::String CaptureDir=conf.CaptureDir+"/"+conf.Scene;
	if (ppl7::Dir::exists(CaptureDir)==false) return;
	ppl7::grafix::Image img;
	try {
		if (lastFrameNum==0) lastFrameNum=highestSceneFrame();
		lastFrameNum++;
		if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum-1);
		capture(img);

		if (ui.saveCamShot->isChecked()) {
			job=new SaveJob;
			if (!job) throw ppl7::OutOfMemoryException();
			job->img=img;
			job->Filename=CaptureDir;
			job->Filename.appendf("/frame_%06i",lastFrameNum);
			lastFrame=img;
			if (ui.imageFormat->currentIndex()==0) {
				job->format=PictureFormat::png;
				job->Filename+=".png";
			} else if (ui.imageFormat->currentIndex()==1) {
				job->format=PictureFormat::bmp;
				job->Filename+=".bmp";
			} else if (ui.imageFormat->currentIndex()==2) {
				job->format=PictureFormat::jpeg;
				job->quality=ui.jpegQualitySlider->value();
				job->Filename+=".jpg";
			}
			fBuffer.setImage(lastFrameNum,img);
			savethread.addJob(job);
		}
		if (ui.saveCompositedImage->isChecked()) {
			job=new SaveJob;
			if (!job) throw ppl7::OutOfMemoryException();
			job->Filename=CaptureDir;
			job->Filename.appendf("/comp_%06i",lastFrameNum);
			bluebox.process(img);
			job->img=img;
			lastFrame=img;
			if (ui.imageFormatComp->currentIndex()==0) {
				job->format=PictureFormat::png;
				job->Filename+=".png";
			} else if (ui.imageFormatComp->currentIndex()==1) {
				job->format=PictureFormat::bmp;
				job->Filename+=".bmp";
			} else if (ui.imageFormatComp->currentIndex()==2) {
				job->format=PictureFormat::jpeg;
				job->quality=ui.jpegQualitySliderComp->value();
				job->Filename+=".jpg";
			}
			savethread.addJob(job);
		}
		if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum);

		Tmp.setf("%i",lastFrameNum);
		ui.totalFrames->setText(Tmp);
		if (lastFrameNum>0)	ui.frameSlider->setMaximum(lastFrameNum);
		else ui.frameSlider->setMaximum(1);
	} catch (const ppl7::Exception &e) {
		delete job;
		QApplication::restoreOverrideCursor();
		DisplayException(e,this);
	}
}

void StopMoCap::on_captureBackgroundButton_clicked()
{
	if (cam.isOpen()==false) return;
	ppl7::String Tmp;
	SaveJob *job=NULL;
	ppl7::String CaptureDir=conf.CaptureDir+"/"+conf.Scene;
	if (ppl7::Dir::exists(CaptureDir)==false) return;
	if (lastFrameNum==0) lastFrameNum=highestSceneFrame();
	if (lastFrameNum==0) return;
	if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum-1);
	ppl7::grafix::Image img;
	try {
		capture(img);
		if (ui.saveCamShot->isChecked()) {
			job=new SaveJob;
			if (!job) throw ppl7::OutOfMemoryException();
			job->img=img;
			job->Filename=CaptureDir;
			job->Filename.appendf("/frame_bg_%06i",lastFrameNum);
			//lastFrame=img;
			if (ui.imageFormat->currentIndex()==0) {
				job->format=PictureFormat::png;
				job->Filename+=".png";
			} else if (ui.imageFormat->currentIndex()==1) {
				job->format=PictureFormat::bmp;
				job->Filename+=".bmp";
			} else if (ui.imageFormat->currentIndex()==2) {
				job->format=PictureFormat::jpeg;
				job->quality=ui.jpegQualitySlider->value();
				job->Filename+=".jpg";
			}
#ifdef USE_SCENEMANAGER
			scm.setFilename(lastFrameNum,job->Filename);
			scm.setImage(lastFrameNum,job->img);
#endif
			savethread.addJob(job);
		}
		if (ui.saveCompositedImage->isChecked()) {
			job=new SaveJob;
			if (!job) throw ppl7::OutOfMemoryException();
			job->Filename=CaptureDir;
			job->Filename.appendf("/comp_bg_%06i",lastFrameNum);
			bluebox.process(img);
			job->img=img;
			//lastFrame=img;
			if (ui.imageFormatComp->currentIndex()==0) {
				job->format=PictureFormat::png;
				job->Filename+=".png";
			} else if (ui.imageFormatComp->currentIndex()==1) {
				job->format=PictureFormat::bmp;
				job->Filename+=".bmp";
			} else if (ui.imageFormatComp->currentIndex()==2) {
				job->format=PictureFormat::jpeg;
				job->quality=ui.jpegQualitySliderComp->value();
				job->Filename+=".jpg";
			}
			savethread.addJob(job);
		}
	} catch (const ppl7::Exception &e) {
		delete job;
		QApplication::restoreOverrideCursor();
		DisplayException(e,this);
	}
	if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum);
}

void StopMoCap::on_captureOverblendButton_clicked()
{
	if (cam.isOpen()==false) return;
	ppl7::String Tmp;
	SaveJob *job=NULL;
	ppl7::String CaptureDir=conf.CaptureDir+"/"+conf.Scene;
	if (ppl7::Dir::exists(CaptureDir)==false) return;
	if (lastFrameNum==0) lastFrameNum=highestSceneFrame();
	if (lastFrameNum==0) return;
	ppl7::grafix::Image img;
	float blendFactor=1.0f-(float)(ui.overblendFactor->value())/100.0f;

	if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum-1);

	try {
		capture(img);
		if (ui.saveCamShot->isChecked()) {
			job=new SaveJob;
			if (!job) throw ppl7::OutOfMemoryException();
			job->Filename=CaptureDir;
			job->Filename.appendf("/frame_%06i",lastFrameNum);
			lastFrame.bltBlend(img,blendFactor);
			job->img=lastFrame;
			if (ui.imageFormat->currentIndex()==0) {
				job->format=PictureFormat::png;
				job->Filename+=".png";
			} else if (ui.imageFormat->currentIndex()==1) {
				job->format=PictureFormat::bmp;
				job->Filename+=".bmp";
			} else if (ui.imageFormat->currentIndex()==2) {
				job->format=PictureFormat::jpeg;
				job->quality=ui.jpegQualitySlider->value();
				job->Filename+=".jpg";
			}
			fBuffer.setImage(lastFrameNum,lastFrame);
			savethread.addJob(job);
		}
		if (ui.saveCompositedImage->isChecked()) {
			job=new SaveJob;
			if (!job) throw ppl7::OutOfMemoryException();
			job->Filename=CaptureDir;
			job->Filename.appendf("/comp_%06i",lastFrameNum);
			bluebox.process(img);
			lastFrame.bltBlend(img,blendFactor);
			job->img=lastFrame;
			//lastFrame=img;
			if (ui.imageFormatComp->currentIndex()==0) {
				job->format=PictureFormat::png;
				job->Filename+=".png";
			} else if (ui.imageFormatComp->currentIndex()==1) {
				job->format=PictureFormat::bmp;
				job->Filename+=".bmp";
			} else if (ui.imageFormatComp->currentIndex()==2) {
				job->format=PictureFormat::jpeg;
				job->quality=ui.jpegQualitySliderComp->value();
				job->Filename+=".jpg";
			}
			savethread.addJob(job);
		}
	} catch (const ppl7::Exception &e) {
		delete job;
		QApplication::restoreOverrideCursor();
		DisplayException(e,this);
	}
	if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum);
}



int StopMoCap::highestSceneFrame()
{
	try {
		ppl7::String CaptureDir=conf.CaptureDir+"/"+conf.Scene;
		int highest=0;
		ppl7::Dir dir(CaptureDir);
		ppl7::DirEntry e;
		ppl7::Dir::Iterator it;
		ppl7::Array matches;
		dir.reset(it);
		while (dir.getNextRegExp(e,it,"/^frame_[0-9]+\\..*$/")) {
			//printf ("e.File=%s\n",(const char*)e.Filename);
			if (e.Filename.pregMatch("/^frame_0*([1-9]+[0-9]*)\\..*$/",matches)) {
				int id=matches[1].toInt();
				//printf ("Match: %i (von: %s)\n",id, (const char*) matches[1]);
				if (id>highest) highest=id;
			}
		}
		return highest;
	} catch (...) {

	}
	return 0;
}

void StopMoCap::on_undoButton_clicked()
{
	ui.captureButton->setFocus();
	ppl7::String Tmp;
	if (lastFrameNum==0) lastFrameNum=highestSceneFrame();
	ppl7::String Filename=conf.CaptureDir+"/"+conf.Scene;
	Filename.appendf("/frame_%06i.png",lastFrameNum);
	try {
		ppl7::File::unlink(Filename);
	} catch (const ppl7::Exception &e) {
		DisplayException(e,this);
		return;
	}
	fBuffer.popBack();
	lastFrameNum=highestSceneFrame();
	Tmp.setf("%i",lastFrameNum);
	ui.totalFrames->setText(Tmp);
	if (lastFrameNum>0)	ui.frameSlider->setMaximum(lastFrameNum);
	else ui.frameSlider->setMaximum(1);
	Filename=conf.CaptureDir+"/"+conf.Scene;
	Filename.appendf("/frame_%06i.png",lastFrameNum);
	try {
		lastFrame.load(Filename);
	} catch (...) {
		lastFrame.clear();
	}
	if (ledcontrol) {
		if (lastFrameNum>0)	ledcontrol->setCurrentFrame(lastFrameNum-1);
		else ledcontrol->setCurrentFrame(0);
	}

}

void StopMoCap::on_sceneName_textChanged (const QString & text)
{
	ppl7::String Tmp=text;
	Tmp.trim();
	if (Tmp.notEmpty()==true) {
		if (!ppl7::Dir::exists(conf.CaptureDir+"/"+Tmp)) {
			ui.createScene->setEnabled(true);
			ui.deleteScene->setEnabled(false);
		} else {
			ui.createScene->setEnabled(false);
			ui.deleteScene->setEnabled(true);
		}
		if (conf.Scene!=Tmp) conf.Scene=Tmp;
	}
	lastFrameNum=0;
}

void StopMoCap::on_sceneName_editingFinished()
{
	//on_createScene_clicked();
	on_sceneName_textChanged(ui.sceneName->text());

	lastFrameNum=highestSceneFrame();
	ppl7::String Filename=conf.CaptureDir+"/"+conf.Scene;
	Filename.appendf("/frame_%06i.png",lastFrameNum);
	try {
		lastFrame.load(Filename);
	} catch (...) {

	}
	ppl7::String Tmp;
	Tmp.setf("%i",lastFrameNum);
	ui.totalFrames->setText(Tmp);
	if (lastFrameNum>0)	ui.frameSlider->setMaximum(lastFrameNum);
	else ui.frameSlider->setMaximum(1);
	fBuffer.loadAsync(conf.CaptureDir+"/"+conf.Scene);
}

void StopMoCap::on_createScene_clicked()
{
	ppl7::String Tmp=conf.CaptureDir+"/"+conf.Scene;
	if (!ppl7::Dir::exists(Tmp)) {
		ppl7::Dir::mkDir(Tmp,true);
		ui.createScene->setEnabled(false);
	}
	fBuffer.clear();
	lastFrameNum=0;
}

void StopMoCap::on_frameSlider_sliderPressed()
{
	if (lastFrameNum<1) return;
	playbackFrame=1;
	inPlayback=true;
	on_frameSlider_valueChanged((int)ui.frameSlider->value());
}

void StopMoCap::on_frameSlider_sliderReleased()
{
	inPlayback=false;
	if (ledcontrol) ledcontrol->setCurrentFrame(lastFrameNum);
}


void StopMoCap::on_frameSlider_valueChanged ( int value )
{
	if (lastFrameNum<1) return;
	ppl7::String Tmp;
	Tmp.setf("%i",value);
	arduino.setCounter(value);
	if (ledcontrol) ledcontrol->setCurrentFrame(value);
	ui.frameNum->setText(Tmp);
	if (!inPlayback) return;
	try {
		if (ui.interpolateFrames->isChecked()==true && (interpolateSequence&1)==1) {
			fBuffer.copyImage(value,grabImg);
			int next=value+1;
			if (next>lastFrameNum) next=1;
			grabImg.bltBlend(fBuffer.getImage(next),0.5);
		} else {
			fBuffer.copyImage(value,grabImg);
		}
	} catch (...) {
		return ;
	}
	/*
	ppl7::String Filename=conf.CaptureDir+"/"+conf.Scene;
	Filename.appendf("/frame_%06i.png",value);
	try {
		grabImg.load(Filename);
	} catch (...) {
		return;
	}
	*/
	if (ui.chromaKeyingEnabled->isChecked()==true || ui.foregroundEnabled->isChecked()==true) bluebox.process(grabImg);
	ui.viewer->update();
}

void StopMoCap::on_playButton_clicked()
{
	interpolateSequence=0;
	inPlayback=true;
	playbackFrame=1;
	PlaybackTimer->start(1000/ui.frameRate->value());
}

void StopMoCap::on_stopButton_clicked()
{
	inPlayback=false;
	PlaybackTimer->stop();
}

void StopMoCap::on_playbackTimer_fired()
{
	if (playbackFrame<1) playbackFrame=1;
	ui.frameSlider->setValue(playbackFrame);
	if (ui.interpolateFrames->isChecked()==true) {
		interpolateSequence++;
		if ((interpolateSequence&1)==1) playbackFrame++;
	} else {
		playbackFrame++;
	}
	if (playbackFrame>lastFrameNum) playbackFrame=1;
}

void StopMoCap::on_frameRate_valueChanged(int value)
{
	if (inPlayback) {
		PlaybackTimer->stop();
		PlaybackTimer->start(1000/value);
	}
	conf.frameRate=value;
}

void StopMoCap::on_previewButton_toggled ( bool checked )
{
	if (checked) {
		inPreviewMode=true;
	} else {
		inPreviewMode=false;
	}
}

void StopMoCap::on_zoom11_clicked ( bool checked )
{
	if (checked) ui.viewer->setScaling(PPL7ImageViewer::None);
}

void StopMoCap::on_zoomFast_clicked ( bool checked )
{
	if (checked) ui.viewer->setScaling(PPL7ImageViewer::Fast);
}

void StopMoCap::on_zoomSmooth_clicked ( bool checked )
{
	if (checked) ui.viewer->setScaling(PPL7ImageViewer::Smooth);
}

void StopMoCap::on_deleteScene_clicked()
{
	 int ret = QMessageBox::question(this, STOPMOCAP_APPNAME,
	                                tr("Are you sure you want to delete this scene?"),
	                                QMessageBox::Yes | QMessageBox::No
	                                ,
	                                QMessageBox::No);
	 if (ret!=QMessageBox::Yes) return;
	 ppl7::String Filename=conf.CaptureDir+"/"+conf.Scene;
	 try {
		 ppl7::File::unlink(Filename);
	 } catch ( const ppl7::Exception &e) {
		 DisplayException(e,this);
	 }
	 on_sceneName_textChanged(ui.sceneName->text());
}

void StopMoCap::on_selectScene_clicked()
{
	SelectScene sel(this);
	try {
		sel.scanDir(conf.CaptureDir);
		if (sel.exec()!=1) return;
		ui.sceneName->setText(sel.selectedScene());
		on_sceneName_editingFinished();
	} catch (const ppl7::Exception &e) {
		DisplayException(e,this);
	}

}

void StopMoCap::on_imageFormat_currentIndexChanged(int index)
{
	if (index==2) {
		ui.jpegQualitySlider->setEnabled(true);
		ui.jpegQualitySpinBox->setEnabled(true);
	} else {
		ui.jpegQualitySlider->setEnabled(false);
		ui.jpegQualitySpinBox->setEnabled(false);
	}
}

void StopMoCap::on_jpegQualitySlider_valueChanged (int value)
{
	ui.jpegQualitySpinBox->setValue(value);
}

void StopMoCap::on_jpegQualitySpinBox_valueChanged ( int value )
{
	ui.jpegQualitySlider->setValue(value);
}

void StopMoCap::on_imageFormatComp_currentIndexChanged(int index)
{
	if (index==2) {
		ui.jpegQualitySliderComp->setEnabled(true);
		ui.jpegQualitySpinBoxComp->setEnabled(true);
	} else {
		ui.jpegQualitySliderComp->setEnabled(false);
		ui.jpegQualitySpinBoxComp->setEnabled(false);
	}
}

void StopMoCap::on_jpegQualitySliderComp_valueChanged (int value)
{
	ui.jpegQualitySpinBoxComp->setValue(value);
}

void StopMoCap::on_jpegQualitySpinBoxComp_valueChanged ( int value )
{
	ui.jpegQualitySliderComp->setValue(value);
}

void StopMoCap::on_viewer_mouseClicked(int , int , ppl7::grafix::Color c)
{
	if (ui.pickChromaKey->isChecked()) {
		UpdateColorKeyBG(c);
		ui.pickChromaKey->setChecked(false);
	} else if (ui.pickChromaKeyFG->isChecked()) {
		UpdateColorKeyFG(c);
		ui.pickChromaKeyFG->setChecked(false);
	}
}

void StopMoCap::UpdateColorKeyBG(ppl7::grafix::Color c)
{
	bluebox.setColorKey(c);
	conf.chromaKey=c;
	conf.save();
	ppl7::String Tmp;
	Tmp.setf("%i, %i, %i",c.red(),c.green(),c.blue());
	ui.keyColor->setText(Tmp);
	Tmp.setf("background-color: rgb(%i, %i, %i);\n",c.red(),c.green(),c.blue());
	ppl7::grafix::Color n=c.negativ();
	Tmp.appendf("color: rgb(%i, %i, %i);",n.red(),n.green(),n.blue());
	ui.keyColor->setStyleSheet(Tmp);
}

void StopMoCap::UpdateColorKeyFG(ppl7::grafix::Color c)
{
	bluebox.setColorKeyFG(c);
	conf.chromaKeyFG=c;
	conf.save();
	ppl7::String Tmp;
	Tmp.setf("%i, %i, %i",c.red(),c.green(),c.blue());
	ui.keyColorFG->setText(Tmp);
	Tmp.setf("background-color: rgb(%i, %i, %i);\n",c.red(),c.green(),c.blue());
	ppl7::grafix::Color n=c.negativ();
	Tmp.appendf("color: rgb(%i, %i, %i);",n.red(),n.green(),n.blue());
	ui.keyColorFG->setStyleSheet(Tmp);
}

void StopMoCap::on_tolNearSlider_valueChanged ( int value )
{
	bluebox.setNearTolerance(value);
	conf.chromaToleranceNear=value;
}

void StopMoCap::on_tolFarSlider_valueChanged ( int value )
{
	bluebox.setFarTolerance(value);
	conf.chromaToleranceFar=value;
}

void StopMoCap::on_spillSlider_valueChanged ( int value )
{
	bluebox.setSpillRemoval(value);
	conf.chromaSpillRemove=value;
}

void StopMoCap::on_chromaBackgroundSelect_clicked()
{
	Timer->stop();
	ppl7::String Dir=ppl7::File::getPath(conf.chromaBGImage);
	if (Dir.isEmpty()) {
		Dir=ppl7::Dir::homePath();
	}

	QString newfile = QFileDialog::getOpenFileName(this, tr("Select background image"),
			Dir,
			tr("Images (*.png *.bmp *.jpg)"));
	if (!newfile.isNull()) {
		try {
			bluebox.loadBackground(newfile);
			conf.chromaBGImage=newfile;
			const ppl7::grafix::Image &bg=bluebox.getBGImage();
			QImage qi((uchar*)bg.adr(),bg.width(),bg.height(), bg.pitch(), QImage::Format_RGB32);
			ui.chromaBackground->setPixmap(QPixmap::fromImage(qi).scaled
					(200,140,Qt::KeepAspectRatio,Qt::SmoothTransformation)
			);
		} catch (...) {

		}
	}
	Timer->start(10);
}


void StopMoCap::on_replaceChromaWithColor_toggled(bool)
{
	bluebox.setReplaceMode(1);
	conf.chromaReplaceMode=1;
}

void StopMoCap::on_replaceChromaWithImage_toggled(bool)
{
	bluebox.setReplaceMode(0);
	conf.chromaReplaceMode=0;
}

void StopMoCap::on_replaceChromaWithTransparent_toggled(bool)
{
	bluebox.setReplaceMode(2);
	conf.chromaReplaceMode=2;
}

void StopMoCap::on_chromaKeyingEnabled_toggled(bool checked)
{
	bluebox.setBGChromaEnabled(checked);
	conf.chromaKeyEnabled=checked;

}

void StopMoCap::on_foregroundEnabled_toggled(bool checked)
{
	bluebox.setFGChromaEnabled(checked);
	conf.foregroundEnabled=checked;

}

void StopMoCap::on_tolNearSliderFG_valueChanged ( int value )
{
	bluebox.setNearToleranceFG(value);
	conf.chromaToleranceNearFG=value;
}

void StopMoCap::on_tolFarSliderFG_valueChanged ( int value )
{
	bluebox.setFarToleranceFG(value);
	conf.chromaToleranceFarFG=value;
}

void StopMoCap::on_spillSliderFG_valueChanged ( int value )
{
	bluebox.setSpillRemovalFG(value);
	conf.chromaSpillRemoveFG=value;
}

void StopMoCap::on_chromaForegroundSelect_clicked()
{
	printf ("Debug 1\n");
	Timer->stop();
	ppl7::String Dir=ppl7::File::getPath(conf.chromaFGImage);
	if (Dir.isEmpty()) {
		Dir=ppl7::Dir::homePath();
	}

	QString newfile = QFileDialog::getOpenFileName(this, tr("Select foreground image"),
			Dir,
			tr("Images (*.png *.bmp *.jpg)"));
	if (!newfile.isNull()) {
		try {
			bluebox.loadForeground(newfile);
			conf.chromaFGImage=newfile;
			const ppl7::grafix::Image &bg=bluebox.getFGImage();
			QImage qi((uchar*)bg.adr(),bg.width(),bg.height(), bg.pitch(), QImage::Format_RGB32);
			ui.chromaForeground->setPixmap(QPixmap::fromImage(qi).scaled
					(200,140,Qt::KeepAspectRatio,Qt::SmoothTransformation)
			);
		} catch (...) {

		}
	}
	Timer->start(10);
}

void StopMoCap::on_bgColorSelect_clicked()
{
	Timer->stop();
	QColorDialog dialog(this);
	ppl7::grafix::Color c=bluebox.replaceColor();
	dialog.setOption(QColorDialog::ShowAlphaChannel,false);
	dialog.setCurrentColor(QColor(c.red(),c.green(),c.blue(),c.alpha()));
	if (dialog.exec()==1) {
		QColor qc=dialog.selectedColor();
		conf.replaceColor.set(qc.red(),qc.green(),qc.blue(),qc.alpha());
		bluebox.setReplaceColor(conf.replaceColor);
		//printf ("color alpha=%i\n",qc.alpha());
	}

	Timer->start(10);
}

void StopMoCap::on_chromaKeyColorSelect_clicked()
{
	Timer->stop();
	QColorDialog dialog(this);
	ppl7::grafix::Color c=bluebox.colorKey();
	dialog.setOption(QColorDialog::ShowAlphaChannel,false);
	dialog.setCurrentColor(QColor(c.red(),c.green(),c.blue(),c.alpha()));
	if (dialog.exec()==1) {
		QColor qc=dialog.selectedColor();
		c.set(qc.red(),qc.green(),qc.blue(),qc.alpha());
		UpdateColorKeyBG(c);
	}

	Timer->start(10);
}

void StopMoCap::on_chromaKeyFGColorSelect_clicked()
{
	Timer->stop();
	QColorDialog dialog(this);
	ppl7::grafix::Color c=bluebox.colorKeyFG();
	dialog.setOption(QColorDialog::ShowAlphaChannel,false);
	dialog.setCurrentColor(QColor(c.red(),c.green(),c.blue(),c.alpha()));
	if (dialog.exec()==1) {
		QColor qc=dialog.selectedColor();
		c.set(qc.red(),qc.green(),qc.blue(),qc.alpha());
		UpdateColorKeyFG(c);
	}

	Timer->start(10);
}


void StopMoCap::getSceneList(ppl7::Array &scenes)
{
	scenes.clear();
	ppl7::Dir dir;
	dir.open(conf.CaptureDir,ppl7::Dir::SORT_FILENAME);
	ppl7::DirEntry e;
	ppl7::Dir::Iterator it;
	dir.reset(it);

	while (dir.getNext(e,it)) {
		if (e.Filename=="." || e.Filename=="..") continue;
		if (e.isDir()==true || e.isLink()==true) {
			scenes.add(e.Filename);
		}
	}

}



void StopMoCap::on_sceneUpButton_clicked()
{
	ppl7::Array scenes;
	getSceneList(scenes);
	ppl7::String prev;
	ppl7::String current=ui.sceneName->text();
	for (size_t i=0;i<scenes.size();i++) {
		if (scenes[i]==current) {
			if (prev.notEmpty()) {
				ui.sceneName->setText(prev);
				on_sceneName_editingFinished();
				on_playButton_clicked();
			}
			break;
		}
		prev=scenes[i];
	}
}

void StopMoCap::on_sceneDownButton_clicked()
{
	ppl7::Array scenes;
	getSceneList(scenes);
	ppl7::String current=ui.sceneName->text();
	for (size_t i=0;i<scenes.size()-1;i++) {
		if (scenes[i]==current) {
			ui.sceneName->setText(scenes[i+1]);
			on_sceneName_editingFinished();
			on_playButton_clicked();
			break;
		}
	}
}

void StopMoCap::on_lightAndDarkButton_toggled(bool checked)
{
	if (checked) {
		setStyleSheet("background-color: rgb(48, 48, 48);\ncolor: rgb(175, 175, 175);");
		ui.viewer->setBackground(QColor(0,0,0));
		if (ledcontrol) ledcontrol->setColorScheme(1);

	} else {
		setStyleSheet("");
		ui.viewer->setBackground(QColor(233,232,230));
		if (ledcontrol) ledcontrol->setColorScheme(0);
	}


}

/*
void StopMoCap::on_editScene_clicked()
{
	if (!fpaint) {
		fpaint=new FramePaint();
		fpaint->setWindowFlags(Qt::Window);
		fpaint->setFrameBuffer(fBuffer);
		fpaint->setConfig(conf);
	}
	fpaint->init();
	fpaint->setMaxFrameNumber(lastFrameNum);
	fpaint->setFrameRate(ui.frameRate->value());
	fpaint->setDarkColorScheme(ui.lightAndDarkButton->isChecked());
	fpaint->show();
}
*/

void StopMoCap::on_arduinoButton_clicked()
{
	if (!ledcontrol) {
		ledcontrol=new LedControl();
		ledcontrol->setMainCapture(this);
		ledcontrol->setWindowFlags(Qt::Window);
		ledcontrol->setArduino(arduino);
		try {
			ledcontrol->setConfig(conf);
		} catch (...) {

		}

		if (ui.lightAndDarkButton->isChecked()) ledcontrol->setColorScheme(1);
		else ledcontrol->setColorScheme(0);
	}
	ledcontrol->show();
}


void StopMoCap::on_releaseButton_clicked()
{
	//conf.ReleaseCommand;
	//conf.ReleaseDir;
	if (highestSceneFrame()<1) {
		 QMessageBox::critical(this, STOPMOCAP_APPNAME,
		                                tr("No frames captured yet, export not possible"));
		return;
	}
	if (highestSceneFrame()!=fBuffer.size()) {
		 QMessageBox::critical(this, STOPMOCAP_APPNAME,
		                                tr("Scene is not complete in cache, please wait a few seconds"));
		return;
	}
	on_stopButton_clicked();
	ReleaseVideo release;
	ppl7::String Tmp=conf.ReleaseDir;
	Tmp.replace("%captureDir",conf.CaptureDir);
	release.setReleaseDir(Tmp);
	release.setReleaseCommand(conf.ReleaseCommand);
	release.setScene(ui.sceneName->text());
	release.setSourceFrameRate(ui.frameRate->value());
	release.exportVideo(fBuffer);

}


void StopMoCap::on_motorMoveLeft_clicked()
{
	if(conf.WebControlBaseUri.isEmpty()) return;
	ppl7::String Uri=conf.WebControlBaseUri+"camera/move/left/";
	Uri+=ui.motorMoveSteps->text();
	motorThread->getUri(Uri);
}

void StopMoCap::on_motorMoveRight_clicked()
{
	if(conf.WebControlBaseUri.isEmpty()) return;
	ppl7::String Uri=conf.WebControlBaseUri+"camera/move/right/";
	Uri+=ui.motorMoveSteps->text();
	motorThread->getUri(Uri);
}

void StopMoCap::on_motorTurnLeft_clicked()
{
	if(conf.WebControlBaseUri.isEmpty()) return;
	ppl7::String Uri=conf.WebControlBaseUri+"camera/turn/left/";
	Uri+=ui.motorMoveSteps->text();
	try {
		ppl7::Curl::getUri(Uri);
	} catch (const ppl7::Exception &e) {
		e.print();
	}
}

void StopMoCap::on_motorTurnRight_clicked()
{
	if(conf.WebControlBaseUri.isEmpty()) return;
	ppl7::String Uri=conf.WebControlBaseUri+"camera/turn/left/";
	Uri+=ui.motorMoveSteps->text();
	try {
		ppl7::Curl::getUri(Uri);
	} catch (const ppl7::Exception &e) {
		e.print();
	}

}

void StopMoCap::motorStarted()
{
	ui.frameMotors->setEnabled(false);
}

void StopMoCap::motorStopped()
{
	ui.frameMotors->setEnabled(true);
}



MotorThread::MotorThread(StopMoCap *gui)
{
	this->gui=gui;
}

void MotorThread::getUri(const ppl7::String &Uri)
{
	this->Uri=Uri;
	this->start();
}

void MotorThread::run()
{
	emit motorStarted();
	try {
		ppl7::Curl::getUri(Uri);
	} catch (const ppl7::Exception &e) {
		//e.print();
	}
	emit motorStopped();
}

