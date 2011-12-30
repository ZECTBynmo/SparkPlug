///////////////////////////////////////////////////////////
//
// SparkPlug.cpp: DSP Sandbox UI
// Copyright (c) 2011 - Mike Vegeto.  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!class SparkPlug
#include "Sparkplug.h"
#include "Engine.h"
#include "DropArea.h"
#include <QPushButton>
#include "qcombobox.h"

SparkPlug::SparkPlug(QWidget *parent, Qt::WFlags flags) : 
QMainWindow(parent, flags) {
	
	// Setup the UI using the Qt UI thing
	//ui.setupUi(this);
	
	// Create our engine
	m_pEngine= new Engine();
	
	// Create our Drop Area
	m_pDropArea= new DropArea( this );
	
	// Create our audio output selector
	m_pDeviceSelector = new QComboBox(this);
	foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
		m_pDeviceSelector->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
	connect( m_pDeviceSelector, SIGNAL(activated(int)), this, SLOT(slotDeviceChanged(int)) );
	
	// Stars the engine
	m_pStartButton= new QPushButton( "Start the Engine", this );
	connect( m_pStartButton, SIGNAL(pressed()), this, SLOT(slotStartProcessing()) );
				
	// Setup our layout
	setupLayout();
} // end SparkPlug::SparkPlug()


//////////////////////////////////////////////////////////////////////////////
/*! Changes our audio device */
void SparkPlug::slotDeviceChanged( int iDevice ) {
	m_pEngine->SetAudioDevice( m_pDeviceSelector->itemData(iDevice).value<QAudioDeviceInfo>() );
} // end SparkPlug::slotDeviceChanged()


//////////////////////////////////////////////////////////////////////////////
/*! Programattically setup our layout */
void SparkPlug::setupLayout() {
	// Main window geometry
	setFixedHeight( 252 + m_pDeviceSelector->height() );
	setFixedWidth( 250 );
	
	// Device selector
	m_pDeviceSelector->setGeometry( 0, 0, width(), m_pDeviceSelector->height() );
	
	// Drop Area
	m_pDropArea->setGeometry( 0, m_pDeviceSelector->height()+2, 250, 200 );
	
	// Startbutton
	m_pStartButton->setGeometry( 0, m_pDropArea->height()+2 + m_pDeviceSelector->height(), width(), 50 );
} // end SparkPlug::setupLayout()


//////////////////////////////////////////////////////////////////////////////
/*! Starts our engine processing cycle */

void SparkPlug::slotStartProcessing() {
	// Start processing!
	m_pEngine->start();
} // end slotStartProcessing()


//////////////////////////////////////////////////////////////////////////////
/*! Destructor */
SparkPlug::~SparkPlug() {

}
