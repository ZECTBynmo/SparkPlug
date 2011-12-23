#ifndef SPARKPLUG_H
#define SPARKPLUG_H

#include <QtGui/QMainWindow>
#include "ui_Sparkplug.h"

class SparkPlug : public QMainWindow
{
	Q_OBJECT

public:
	SparkPlug(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SparkPlug();

private:
	Ui::SparkPlugClass ui;
};

#endif // SPARKPLUG_H
