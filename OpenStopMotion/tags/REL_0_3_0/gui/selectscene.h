#ifndef SELECTSCENE_H
#define SELECTSCENE_H

#include <QtGui/QDialog>
#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "ui_selectscene.h"

class SelectScene : public QDialog
{
    Q_OBJECT

public:
    class FrameIdent {
    	public:
    		int num;
    		ppl7::String Filename;
    };

    SelectScene(QWidget *parent = 0);
    ~SelectScene();

    void scanDir(const ppl7::String &path);
    const ppl7::String &selectedScene() const;

private:
    Ui::SelectSceneClass ui;
    ppl7::String SelectedDir;

    void getFrames(const ppl7::String &path, FrameIdent &first, FrameIdent &last);
    QIcon getIcon(const ppl7::String &Filename, int width, int height);

public slots:
	void on_cancelButton_clicked();
	void on_sceneTable_itemDoubleClicked ( QTreeWidgetItem * item, int column );
};

#endif // SELECTSCENE_H
