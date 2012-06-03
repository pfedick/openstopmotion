#define WITH_QT
#include <ppl7.h>
#include "stopmocap.h"
#include "device.h"
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QFileDialog>

StopMoCap::StopMoCap(QWidget *parent)
    : QWidget(parent)
{
	controlLayout=NULL;
	ui.setupUi(this);
	Timer=new QTimer(this);
	conf.load();


	std::list<VideoDevice> devices;
	std::list<VideoDevice>::const_iterator it;

	cam.enumerateDevices(devices);
	ui.deviceComboBox->clear();
	for (it=devices.begin();it!=devices.end();it++) {
		Devices.push_back(*it);
		ui.deviceComboBox->addItem(it->Name,Devices.size()-1);
	}
	ui.deviceComboBox->setCurrentIndex(0);
	connect(Timer, SIGNAL(timeout()), this, SLOT(on_timer_fired()));

	ui.captureDir->setText(conf.CaptureDir);
	ui.sceneName->setText(conf.Scene);
	ui.mergeFrames->setValue(conf.mergeFrames);
	ui.skipFrames->setValue(conf.skipFrames);
	ui.mergeFrames->setValue(conf.onionValue);
}

StopMoCap::~StopMoCap()
{
	Timer->stop();
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
			controlLayout->addWidget(slider);
		} else if (it->type==CameraControl::Boolean) {
			MyQCheckBox *checkbox=new MyQCheckBox;
			checkbox->setText(it->Name);
			if (it->defaultValue>0) checkbox->setChecked(true);
			checkbox->cont=*it;
			controlLayout->addWidget(checkbox);
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

void StopMoCap::grabFrame()
{
	ppl7::grafix::Image img;
	cam.readFrame(img);
	//ppl7::grafix::ImageFilter_PNG png;
	//png.saveFile("test.png",img);
	QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
	QPixmap pm=QPixmap::fromImage(qi);
	ui.viewer->setPixmap(pm.scaled(ui.viewer->width(),ui.viewer->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


void StopMoCap::on_timer_fired()
{
	//printf ("Timer fired\n");
	grabFrame();
}


void StopMoCap::on_selectDir_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 conf.CaptureDir,
                                                 QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		conf.CaptureDir=dir;
		ui.captureDir->setText(dir);
		conf.save();
	}
}

void StopMoCap::on_captureButton_clicked()
{
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


	ppl7::grafix::ImageFilter_PNG png;
	png.saveFile("test.png",final);
	QApplication::restoreOverrideCursor();

	Timer->start(10);
}
