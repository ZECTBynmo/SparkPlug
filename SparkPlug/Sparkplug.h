#ifndef SPARKPLUG_H
#define SPARKPLUG_H

#include <QtGui/QMainWindow>
#include "ui_Sparkplug.h"
#include <QPushButton>
#include "Engine.h"
#include "DropArea.h"
#include "qcombobox.h"

class SparkPlug : public QMainWindow
{
	Q_OBJECT

public:
	SparkPlug(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SparkPlug();
	
private slots:
	void slotStartProcessing();		//!< Starts our engine processing cycle
	void slotDeviceChanged( int iDevice );		//!< Changes our audio device

private:
	Ui::SparkPlugClass ui;
	
	Engine* m_pEngine;				//!< Our engine instance
	QPushButton* m_pStartButton;	//!< Our start processing button
	DropArea* m_pDropArea;			//!< Our drop area
	QComboBox* m_pDeviceSelector;	//!< Our audio device combo box
	
	void setupLayout();					//!< Programattically setup our layout
};

#endif // SPARKPLUG_H
