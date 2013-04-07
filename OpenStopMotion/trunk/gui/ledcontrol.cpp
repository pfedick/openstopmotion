#include "ledcontrol.h"
#include "stopmocap.h"
#include <QPainter>
#include <QImage>
#include <QFileDialog>


LedControl::LedControl(QWidget *parent)
    : QWidget(parent)
{
	PlaybackTimer=new QTimer(this);
	myColorScheme=0;
	ppl7::String Tmp;
	ui.setupUi(this);
	lc[0].set(255,0,0);
	lc[1].set(0,255,0);
	lc[2].set(0,0,255);
	lc[3].set(255,255,0);
	lc[4].set(255,0,255);
	lc[5].set(0,255,255);
	lc[6].set(127,0,0);
	lc[7].set(0,127,0);
	lc[8].set(0,0,127);
	lc[9].set(127,127,0);
	lc[10].set(127,0,127);
	lc[11].set(0,127,127);

	for (int i=0;i<12;i++) {
		slider[i]=new LedSlider;
		slider[i]->setId(i);
		Tmp.setf("LED %d",i+1);
		slider[i]->setName(Tmp);
		slider[i]->setColor(lc[i]);


		connect(slider[i], SIGNAL(valueChanged(int,int)), this, SLOT(on_slider_valueChanged_fired(int,int)));
		connect(slider[i], SIGNAL(keyFrameSet(int,int)), this, SLOT(on_keyFrameSet_fired(int,int)));
		connect(slider[i], SIGNAL(keyFrameDelete(int)), this, SLOT(on_keyFrameDelete_fired(int)));


		ui.sliderLayout->addWidget(slider[i]);
	}
	ui.frameview->installEventFilter(this);
	connect(PlaybackTimer, SIGNAL(timeout()), this, SLOT(on_playbackTimer_fired()));


}


LedControl::~LedControl()
{
	PlaybackTimer->stop();
	delete PlaybackTimer;
}

void LedControl::resizeEvent ( QResizeEvent * event )
{
	//updateFrameView();
	QWidget::resizeEvent(event);
}

void LedControl::showEvent (  QShowEvent * event )
{
	QWidget::showEvent(event);
}

bool LedControl::eventFilter(QObject *obj, QEvent *event)
{
	if (obj==ui.frameview && event->type() == QEvent::Paint) {
		updateFrameView();
		return true;
	} else {
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
}

void LedControl::setConfig (Config &conf)
{
	this->conf=&conf;
}

void LedControl::setArduino (Arduino &arduino)
{
	this->arduino=&arduino;
}

void LedControl::setCurrentFrame(int frame)
{
	ui.frameSlider->setValue(frame);
}

void LedControl::setColorScheme(int scheme)
{
	if (scheme==1) {
		myColorScheme=1;
		setStyleSheet("background-color: rgb(48, 48, 48);\ncolor: rgb(175, 175, 175);");
		//ui.viewer->setBackground(QColor(0,0,0));
	} else {
		myColorScheme=0;
		setStyleSheet("");
		//ui.viewer->setBackground(QColor(233,232,230));
	}
}



void LedControl::on_connectButton_clicked()
{
	if (arduino->open(ui.serialDevice->text(),ui.serialBaud->text().toInt())) {
		arduino->allLightsOn();
		ppl7::MSleep(500);
		arduino->allLightsOff();
		arduino->setCounter(0);
		for (int i=0;i<12;i++) {
			arduino->setLight(i,slider[i]->value());
		}
	}
}

void LedControl::on_slider_valueChanged_fired(int id, int value)
{
	//printf ("Slider: %d, Value: %d\n",id,value);
	arduino->setLight(id,value);
}

void LedControl::on_keyFrameSet_fired(int id, int value)
{
	try {
		keyframes[id].erase(ui.frameSlider->value());
	} catch (...) {

	}
	keyframes[id].add(ui.frameSlider->value(),value);
	recalcFrames(id);
	ui.frameview->update();
}

void LedControl::on_keyFrameDelete_fired(int id)
{
	try {
		keyframes[id].erase(ui.frameSlider->value());
	} catch (...) {

	}
	recalcFrames(id);
	ui.frameview->update();
}

void LedControl::on_frameSlider_valueChanged(int value)
{
	ppl7::String Tmp;
	Tmp.setf("%d",value);
	ui.currentFrame->setText(Tmp);
	ui.frameview->update();
	arduino->setCounter(value);
	for (int i=0;i<12;i++) {
		if (interpolatedframes[i].exists(value))
			slider[i]->setValue(interpolatedframes[i][value]);
			//arduino->setLight(i,interpolatedframes[i][value]);
	}
}

void LedControl::on_maxFrame_textChanged(const QString & text)
{
	ui.frameSlider->setMaximum(text.toInt());
}

void LedControl::on_loadButton_clicked()
{
	ppl7::String Path=Filename;
	if (Path.isEmpty()) Path=conf->LedControlFile;
	if (Path.isEmpty()) Path=ppl7::Dir::currentPath();
	ppl7::String newfile = QFileDialog::getOpenFileName(this, tr("Load LED-Control data"),
	                                                 Path,
	                                                 tr("XML files (*.xml)"));
	if (newfile.notEmpty()) {
		Filename=newfile;
		try {
			load(Filename);
			conf->LedControlFile=Filename;
		} catch (const ppl7::Exception &e) {
			DisplayException(e);
		}
	}
}

void LedControl::on_saveButton_clicked()
{
	if (Filename.isEmpty()) on_saveAsButton_clicked();
	else {
		try {
			save(Filename);
		} catch (const ppl7::Exception &e) {
			DisplayException(e);
		}
	}
}

void LedControl::on_saveAsButton_clicked()
{
	ppl7::String Path=Filename;
	if (Path.isEmpty()) Path=ppl7::Dir::currentPath();
	ppl7::String newfile = QFileDialog::getSaveFileName(this, tr("Save LED-Control data"),
	                            Path,
	                            tr("XML files (*.xml)"));
	if (newfile.notEmpty()) {
		Filename=newfile;
		try {
			save(Filename);
			conf->LedControlFile=Filename;
		} catch (const ppl7::Exception &e) {
			DisplayException(e);
		}
	}
}

void LedControl::on_frameNextButton_clicked()
{
	PlaybackTimer->stop();
	int frame=ui.frameSlider->value();
	frame++;
	if (frame<ui.frameSlider->maximum()) ui.frameSlider->setValue(frame);
}

void LedControl::on_frameBackButton_clicked()
{
	PlaybackTimer->stop();
	int frame=ui.frameSlider->value();
	frame--;
	if (frame>=0) ui.frameSlider->setValue(frame);

}

void LedControl::on_playButton_clicked()
{
	PlaybackTimer->start(1000/conf->frameRate);
}

void LedControl::on_stopButton_clicked()
{
	PlaybackTimer->stop();
}


void LedControl::on_playbackTimer_fired()
{
	int frame=ui.frameSlider->value();
	frame++;
	if (frame<ui.frameSlider->maximum()) ui.frameSlider->setValue(frame);
}


void LedControl::load(const ppl7::String &filename)
{
	ppl7::File ff;
	ppl7::String content;
	ppl7::Array matches;
	ff.load(content,filename);
	for (int i=0;i<12;i++) {
		keyframes[i].clear();
		interpolatedframes[i].clear();
		ui.frameview->update();
	}

	if (content.pregMatch("/\\<header\\>(.*?)\\<\\/header\\>/s",matches)) {
		ppl7::String header=matches[1];
		if (header.pregMatch("/\\<maxframes\\>(.*?)\\<\\/maxframes\\>/s",matches)) {
			ui.maxFrame->setText(matches[1].trimmed());
		}

	}
	ppl7::Array rows;
	rows.explode(content,"<led>");
	for (size_t i=1;i<rows.size();i++) {
		if (rows[i].pregMatch("/\\<id\\>(.*)\\<\\/id\\>/s",matches)) {
			int led=matches[1].toInt();
			ppl7::Array keys;
			keys.explode(rows[i],"<key>");
			//keys.list("keys");
			for (size_t k=1;k<keys.size();k++) {
				//printf ("Loading led %i, keyframe: %i\n",led,(int)k);
				if (keys[k].pregMatch("/\\<frame\\>(.*?)\\<\\/frame\\>\\<value\\>(.*?)\\<\\/value\\>/s",matches)) {
					//printf ("match\n");
					keyframes[led].add(matches[1].toInt(),matches[2].toInt());
				}
			}
			recalcFrames(led);
		}
	}
	ui.frameSlider->setValue(0);
	on_frameSlider_valueChanged(0);

	ui.frameview->update();
}

void LedControl::save(const ppl7::String &filename)
{
	ppl7::File ff;
	ff.open(filename,ppl7::File::WRITE);
	ppl7::String s;
	s="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	s+="<ledcontrol>\n";
	s+="<header>\n";
	s+="   <maxframes>"+ui.maxFrame->text()+"</maxframes>\n";
	s+="</header>\n";
	for (int led=0;led<12;led++) {
		s+="<led>\n";
		s+="   <id>"+ppl7::ToString("%d",led)+"</id>\n";
		ppl7::AVLTree<int, int>::Iterator it;
		keyframes[led].reset(it);
		while (keyframes[led].getNext(it)) {
			s+=ppl7::ToString("      <key><frame>%d</frame><value>%d</value></key>\n",it.key(),it.value());
		}
		s+="</led>\n";
	}
	s+="</ledcontrol>\n";

	ff.puts(s);
	ff.close();
}




void LedControl::recalcFrames(int id)
{
	interpolatedframes[id].clear();
	int frame=0;
	int maxf=ui.maxFrame->text().toInt();
	int value=0;
	int nextframe;
	int nextvalue;
	int steps;

	ppl7::AVLTree<int, int>::Iterator it;
	keyframes[id].reset(it);

	while (keyframes[id].getNext(it)) {
		nextframe=it.key();
		nextvalue=it.value();
		steps=nextframe-frame;
		if (nextframe==0) {	// Frame 0
			interpolatedframes[id].add(frame,nextvalue);
		} else {
			for (int f=1;f<steps;f++) {
				interpolatedframes[id].add(frame+f,(value*(steps-f)+nextvalue*f)/steps);
			}
			interpolatedframes[id].add(nextframe,nextvalue);
		}

		frame=nextframe;
		value=nextvalue;
	}



}

void LedControl::updateFrameView()
{
	int w=ui.frameview->width();
	int h=ui.frameview->height();

	if (w!=img.width() || h!=img.height()) {
		//printf ("Erstelle zeichenflaeche: %i x %i\n",w,h);
		img.create(w,h);
	}
	if (img.isEmpty()) return;
	ppl7::grafix::Color bg;
	ppl7::grafix::Color framepointer;

	if (myColorScheme==0) {
		bg.set(255,255,255);
		framepointer.set(200,200,200);
	} else {
		bg.set(0,0,0);
		framepointer.set(64,64,64);
	}

	img.cls(bg);


	int frame=ui.frameSlider->value();
	int endframe=ui.maxFrame->text().toInt();
	int scale=h-28;
	int bottom=h-1;

	int middle=w/3;
	int offset=frame*5;

	int x=ui.frameSlider->value();
	img.line(middle+5,0,middle+5,h,framepointer);



	for (int led=0;led<12;led++) {
		int lastvalue=0;
		int x=middle;
		int y=bottom-(led*2);
		for (int f=0;f<endframe;f++) {
			int value=lastvalue;
			if (interpolatedframes[led].exists(f)) value=interpolatedframes[led][f];
			img.line(x-offset,y,x+5-offset,bottom-(led*2)-(value*scale/255),lc[led]);
			x+=5;
			y=bottom-(led*2)-(value*scale/255);

			if (interpolatedframes[led].exists(f)) lastvalue=interpolatedframes[led][f];
			if (keyframes[led].exists(f)) {
				img.drawRect(x-3-offset,y-3,x+3-offset,y+3,lc[led]);

			}
		}

	}


	QPainter painter(ui.frameview);
	QPoint p(0,0);
	QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
	painter.drawImage(p,qi);

}
