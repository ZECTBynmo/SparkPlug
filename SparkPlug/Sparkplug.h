#ifndef SPARKPLUG_H
#define SPARKPLUG_H

#include <QtGui/QMainWindow>
#include "ui_Sparkplug.h"
#include "../src/gui/widgets/qpushbutton.h"
#include "Engine.h"

class SparkPlug : public QMainWindow
{
	Q_OBJECT

public:
	SparkPlug(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SparkPlug();
	
private slots:
	void slotStartProcessing();		//!< Starts our engine processing cycle

private:
	Ui::SparkPlugClass ui;
	
	Engine* m_pEngine;				//! Our engine instance
	QPushButton* m_pStartButton;	//!< Our start processing button
};

#endif // SPARKPLUG_H
