#define WITH_QT
#include <ppl7.h>
#include "stopmocap.h"
#include "device.h"
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QFileDialog>
#include <QEvent>

StopMoCap::StopMoCap(QWidget *parent)
    : QWidget(parent)
{
	controlLayout=NULL;
	ui.setupUi(this);
	Timer=new QTimer(this);
	PlaybackTimer=new QTimer(this);
	conf.load();
	lastFrameNum=0;
	inPlayback=false;
	playbackFrame=0;


	std::list<VideoDevice> devices;
	std::list<VideoDevice>::const_iterator it;

	cam.enumerateDevices(devices);
	ui.deviceComboBox->clear();
	for (it=devices.begin();it!=devices.end();it++) {
		Devices.push_back(*it);
		ui.deviceComboBox->addItem(it->Name,Devices.size()-1);
		if (it->Name==conf.DeviceName) {
			int index=ui.deviceComboBox->findText(conf.DeviceName);
			if (index>=0) ui.deviceComboBox->setCurrentIndex(index);

		}
	}
	ui.deviceComboBox->setCurrentIndex(0);
	connect(Timer, SIGNAL(timeout()), this, SLOT(on_timer_fired()));
	connect(PlaybackTimer, SIGNAL(timeout()), this, SLOT(on_playbackTimer_fired()));

	ui.captureDir->setText(conf.CaptureDir);
	ui.sceneName->setText(conf.Scene);
	on_sceneName_editingFinished();
	ui.mergeFrames->setValue(conf.mergeFrames);
	ui.skipFrames->setValue(conf.skipFrames);
	ui.onionSkinning->setValue(conf.onionValue);
}

StopMoCap::~StopMoCap()
{
	PlaybackTimer->stop();
	Timer->stop();
	conf.mergeFrames=ui.mergeFrames->value();
	conf.skipFrames=ui.skipFrames->value();
	conf.onionValue=ui.onionSkinning->value();
	conf.DeviceName=ui.deviceComboBox->currentText();
	conf.ImageFormat=ui.formatComboBox->currentText();
	conf.ImageSize=ui.resolutionComboBox->currentText();

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
		ui.formatComboBox->addItem(it->Description,Formats.size()-1);
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
		ui.resolutionComboBox->addItem(Tmp,FrameSizes.size()-1);
		if (Tmp==conf.ImageSize) {
			int index=ui.resolutionComboBox->findText(Tmp);
			if (index>=0) ui.resolutionComboBox->setCurrentIndex(index);

		}
	}
}

void StopMoCap::on_useDevice_clicked()
{
	cam.open(Devices[ui.deviceComboBox->currentIndex()]);

	std::list<CameraControl> controls;
	std::list<CameraControl>::const_iterator it;
	cam.enumerateControls(controls);
	if (ui.controlWidget->layout()) {
		delete (ui.controlWidget->layout());
		ui.controlWidget->updateGeometry();
	}

	controlLayout=new QVBoxLayout;
	controlLayout->setSpacing(0);
	for (it=controls.begin();it!=controls.end();it++) {
		if (it->type==CameraControl::Integer) {
			QLabel *label=new QLabel(it->Name);
			controlLayout->addWidget(label);
			MyQSlider *slider=new MyQSlider(Qt::Horizontal);
			slider->setMinimum(it->min);
			slider->setMaximum(it->max);
			slider->setSingleStep(it->step);
			slider->setPageStep(it->step);
			slider->setSliderPosition(it->defaultValue);
			slider->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
			slider->setMaximumWidth(200);
			slider->cont=*it;
			slider->setProperty("ctl_type",CameraControl::Integer);
			slider->lastValue=it->defaultValue;
			slider->installEventFilter(this);
			controlLayout->addWidget(slider);
		} else if (it->type==CameraControl::Boolean) {
			MyQCheckBox *checkbox=new MyQCheckBox;
			checkbox->setText(it->Name);
			if (it->defaultValue>0) checkbox->setChecked(true);
			checkbox->cont=*it;
			checkbox->setProperty("ctl_type",CameraControl::Boolean);
			checkbox->lastValue=it->defaultValue;
			checkbox->installEventFilter(this);
			controlLayout->addWidget(checkbox);
		} else if (it->type==CameraControl::Menu) {
			QLabel *label=new QLabel(it->Name);
			controlLayout->addWidget(label);
			MyQComboBox *combobox=new MyQComboBox;
			combobox->cont=*it;
			combobox->setProperty("ctl_type",CameraControl::Menu);
			combobox->installEventFilter(this);
			std::list<CameraControl::MenuItem>::const_iterator mi;
			for (mi=it->MenuItems.begin();mi!=it->MenuItems.end();mi++) {
				combobox->addItem(mi->Name,mi->id);
			}
			controlLayout->addWidget(combobox);

		}
	}
	ui.controlWidget->setLayout(controlLayout);
	ui.controlWidget->updateGeometry();

	cam.startCapture(Formats[ui.formatComboBox->currentIndex()],
			FrameSizes[ui.resolutionComboBox->currentIndex()]);

	ui.captureButton->setFocus();
	//grabFrame();
	Timer->start(10);
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
	if (ctltype==CameraControl::Integer) {
		MyQSlider *slider=(MyQSlider*)target;
		//printf ("slider, type: %i\n",type);
		if (slider->value()!=slider->lastValue) {
			cam.setControlValue(slider->cont,slider->value());
			slider->lastValue=slider->value();
		}

	} else if (ctltype==CameraControl::Boolean) {
		MyQCheckBox *checkbox=(MyQCheckBox*)target;
		if (checkbox->isChecked()!=checkbox->lastValue) {
			//printf ("checkbox, type: %i\n",type);
			cam.setControlValue(checkbox->cont,checkbox->isChecked());
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
	return false;
}

void StopMoCap::grabFrame()
{
	ppl7::grafix::Image img;
	cam.readFrame(img);
	float blendFactor=(float)ui.onionSkinning->value()/99.0f;
	if (blendFactor>0.0f) {
		img.blend(lastFrame,blendFactor);
	}

	//ppl7::grafix::ImageFilter_PNG png;
	//png.saveFile("test.png",img);
	QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
	QPixmap pm=QPixmap::fromImage(qi);
	ui.viewer->setPixmap(pm.scaled(ui.viewer->width(),ui.viewer->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


void StopMoCap::on_timer_fired()
{
	//printf ("Timer fired\n");
	if (inPlayback) return;
	grabFrame();
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

void StopMoCap::on_captureButton_clicked()
{
	ppl7::String Tmp;
	ppl7::String CaptureDir=conf.CaptureDir+"/"+conf.Scene;
	if (ppl7::Dir::exists(CaptureDir)==false) return;
	Timer->stop();
	ui.viewer->setPixmap(QPixmap());
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QCoreApplication::processEvents();

	ppl7::grafix::Image final, img[100];
	int mergeFrames=ui.mergeFrames->value();
	int skipFrames=ui.skipFrames->value();
	for (int i=0;i<mergeFrames;i++) {
		//printf ("Capture %i\n",i);
		cam.readFrame(img[i]);
		for (int skip=0;skip<skipFrames;skip++) {
			cam.readFrame(final);
		}
	}
	int width=img[0].width();
	int height=img[0].height();
	int r,g,b;
	ppl7::grafix::Color c;

	final.create(width,height,ppl7::grafix::RGBFormat::X8R8G8B8);
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			r=g=b=0;
			for (int layer=0;layer<mergeFrames;layer++) {
				c=img[layer].getPixel(x,y);
				r+=c.red();
				g+=c.green();
				b+=c.blue();
			}
			c.setColor(r/mergeFrames,g/mergeFrames,b/mergeFrames);
			final.putPixel(x,y,c);
		}
	}

	if (lastFrameNum==0) lastFrameNum=highestSceneFrame();
	lastFrameNum++;
	ppl7::String Filename=CaptureDir;
	Filename.appendf("/frame_%06i.png",lastFrameNum);
	ppl7::grafix::ImageFilter_PNG png;
	png.saveFile(Filename,final);
	lastFrame=final;
	Tmp.setf("%i",lastFrameNum);
	ui.totalFrames->setText(Tmp);
	ui.frameSlider->setMaximum(lastFrameNum);
	QApplication::restoreOverrideCursor();

	Timer->start(10);
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

void StopMoCap::on_sceneName_textChanged (const QString & text)
{
	ppl7::String Tmp=text;
	Tmp.trim();
	if (Tmp!=conf.Scene && Tmp.notEmpty()==true) {
		if (!ppl7::Dir::exists(conf.CaptureDir+"/"+Tmp)) ui.createScene->setEnabled(true);
		else ui.createScene->setEnabled(false);
		conf.Scene=Tmp;
	}
	lastFrameNum=0;
}

void StopMoCap::on_sceneName_editingFinished()
{
	on_createScene_clicked();
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
	on_frameSlider_valueChanged(ui.frameSlider->value());
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
	ppl7::grafix::Image img;
	ppl7::String Filename=conf.CaptureDir+"/"+conf.Scene;
	Filename.appendf("/frame_%06i.png",value);
	try {
		img.load(Filename);
	} catch (...) {
		return;
	}
	QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
	QPixmap pm=QPixmap::fromImage(qi);
	ui.viewer->setPixmap(pm.scaled(ui.viewer->width(),ui.viewer->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

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



