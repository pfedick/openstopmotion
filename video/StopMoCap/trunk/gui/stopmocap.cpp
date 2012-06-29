#define WITH_QT
#include <ppl7.h>
#include "stopmocap.h"
#include "device.h"
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QFileDialog>
#include <QEvent>
#include <QMessageBox>
#include "selectscene.h"

StopMoCap::StopMoCap(QWidget *parent)
    : QWidget(parent)
{
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

	connect(Timer, SIGNAL(timeout()), this, SLOT(on_timer_fired()));
	connect(PlaybackTimer, SIGNAL(timeout()), this, SLOT(on_playbackTimer_fired()));

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

	ui.jpegQualitySlider->setValue(conf.jpegQuality);
	ui.imageFormat->setCurrentIndex(conf.pictureFormat);



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
	bluebox.loadBackground("/raid/video/Brickfilms/Decker/Akt 3/Backgrounds/see.png");

	ui.tolNearSlider->setValue(bluebox.nearTolerance());
	ui.tolFarSlider->setValue(bluebox.farTolerance());
	Tmp.setf("r=%i, g=%i, b=%i",bluebox.colorKey().red(),bluebox.colorKey().green(),bluebox.colorKey().blue());
	ui.keyColor->setText(Tmp);
}

StopMoCap::~StopMoCap()
{
	savethread.stop();
#ifdef USERENDERTHREAD
	rthread.stop();
#endif
	PlaybackTimer->stop();
	Timer->stop();
	conf.mergeFrames=ui.mergeFrames->value();
	conf.skipFrames=ui.skipFrames->value();
	conf.onionValue=ui.onionSkinning->value();
	conf.DeviceName=ui.deviceComboBox->currentText();
	conf.ImageFormat=ui.formatComboBox->currentText();
	conf.ImageSize=ui.resolutionComboBox->currentText();
	conf.scalingMode=ui.viewer->scalingMode();
	conf.jpegQuality=ui.jpegQualitySlider->value();
	conf.pictureFormat=ui.imageFormat->currentIndex();
	conf.save();
	cam.close();
	delete Timer;

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
	Timer->start(10);
#ifdef USERENDERTHREAD
	rthread.setImageSize(ui.viewer->width(),ui.viewer->height());
	rthread.start();
#endif
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

void StopMoCap::grabFrame()
{
	float blendFactor=(float)ui.onionSkinning->value()/99.0f;
	try {
		if (inPreviewMode) {
			capture(grabImg);
		} else {
			cam.readFrame(grabImg);
		}
		if (ui.blueBoxCheckBox->isChecked()) bluebox.process(grabImg);
		grabImg.bltBlend(lastFrame,blendFactor);
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
		ui.captureFPS->setText(Tmp);
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
	img.create(width,height,ppl7::grafix::RGBFormat::X8R8G8B8);
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
	ppl7::String CaptureDir=conf.CaptureDir+"/"+conf.Scene;
	if (ppl7::Dir::exists(CaptureDir)==false) return;
	SaveJob *job=new SaveJob;
	if (!job) throw ppl7::OutOfMemoryException();
	try {
		capture(job->img);

		if (lastFrameNum==0) lastFrameNum=highestSceneFrame();
		lastFrameNum++;

		job->Filename=CaptureDir;
		job->Filename.appendf("/frame_%06i",lastFrameNum);
		lastFrame=job->img;
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

		Tmp.setf("%i",lastFrameNum);
		ui.totalFrames->setText(Tmp);
		ui.frameSlider->setMaximum(lastFrameNum);
	} catch (const ppl7::Exception &e) {
		delete job;
		QApplication::restoreOverrideCursor();
		DisplayException(e,this);
	}
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
#ifdef USE_SCENEMANAGER
	scm.deleteFrame(lastFrameNum);
#endif
	lastFrameNum=highestSceneFrame();
	Tmp.setf("%i",lastFrameNum);
	ui.totalFrames->setText(Tmp);
	ui.frameSlider->setMaximum(lastFrameNum);
	Filename=conf.CaptureDir+"/"+conf.Scene;
	Filename.appendf("/frame_%06i.png",lastFrameNum);
	try {
		lastFrame.load(Filename);
	} catch (...) {
		lastFrame.clear();
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
	ui.frameSlider->setMaximum(lastFrameNum);
#ifdef USE_SCENEMANAGER
	scm.clear();
	scm.loadScene(conf.CaptureDir+"/"+conf.Scene);
#endif
}

void StopMoCap::on_createScene_clicked()
{
	ppl7::String Tmp=conf.CaptureDir+"/"+conf.Scene;
	if (!ppl7::Dir::exists(Tmp)) {
		ppl7::Dir::mkDir(Tmp,true);
		ui.createScene->setEnabled(false);
	}
	lastFrameNum=0;
}

void StopMoCap::on_frameSlider_sliderPressed()
{
	if (lastFrameNum<1) return;
	playbackFrame=0;
	inPlayback=true;
	on_frameSlider_valueChanged((int)ui.frameSlider->value());
}

void StopMoCap::on_frameSlider_sliderReleased()
{
	inPlayback=false;
}


void StopMoCap::on_frameSlider_valueChanged ( int value )
{
	if (lastFrameNum<1) return;
	ppl7::String Tmp;
	Tmp.setf("%i",value);
	ui.frameNum->setText(Tmp);
	if (!inPlayback) return;
#ifdef USE_SCENEMANAGER
	try {
		scm.getImage(value,grabImg);
	} catch (...) {
		grabImg.clear();
	}
	if (grabImg.isEmpty()) {
#endif
		ppl7::String Filename=conf.CaptureDir+"/"+conf.Scene;
		Filename.appendf("/frame_%06i.png",value);
		try {
			grabImg.load(Filename);
#ifdef USE_SCENEMANAGER
			scm.setImage(value,grabImg);
#endif
		} catch (...) {
			return;
		}
#ifdef USE_SCENEMANAGER
	}
#endif
	ui.viewer->update();

}

void StopMoCap::on_playButton_clicked()
{
	inPlayback=true;
	playbackFrame=0;
	PlaybackTimer->start(1000/ui.frameRate->value());
}

void StopMoCap::on_stopButton_clicked()
{
	inPlayback=false;
	PlaybackTimer->stop();
}

void StopMoCap::on_playbackTimer_fired()
{
	ui.frameSlider->setValue(playbackFrame);
	playbackFrame++;
	if (playbackFrame>lastFrameNum) playbackFrame=0;
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
		ui.jpegQuality->setEnabled(true);
	} else {
		ui.jpegQuality->setEnabled(false);
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

void StopMoCap::on_viewer_mouseClicked(int x, int y, ppl7::grafix::Color c)
{
	ppl7::String Tmp;
	bluebox.setColorKey(c);
	Tmp.setf("r=%i, g=%i, b=%i",c.red(),c.green(),c.blue());
	ui.keyColor->setText(Tmp);

}

void StopMoCap::on_tolNearSlider_valueChanged ( int value )
{
	bluebox.setNearTolerance(value);
}

void StopMoCap::on_tolFarSlider_valueChanged ( int value )
{
	bluebox.setFarTolerance(value);
}

