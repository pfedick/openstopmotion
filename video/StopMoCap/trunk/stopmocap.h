#ifndef STOPMOCAP_H
#define STOPMOCAP_H

#include <vector>
#include <QtGui/QWidget>
#include "ui_stopmocap.h"
#define WITH_QT
#include "device.h"

class StopMoCap : public QWidget
{
    Q_OBJECT

public:
    StopMoCap(QWidget *parent = 0);
    ~StopMoCap();

private:
    Ui::StopMoCapClass ui;
    Device cam;
    std::vector<VideoDevice> Devices;
    std::vector<VideoFormat> Formats;
    std::vector<ppl7::grafix::Size> FrameSizes;


public slots:
    void on_deviceComboBox_currentIndexChanged(int index);
    void on_formatComboBox_currentIndexChanged(int index);
    void on_useDevice_clicked();
};

#endif // STOPMOCAP_H
