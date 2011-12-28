///////////////////////////////////////////////////////////
//
// SparkPlug.cpp: DSP Sandbox UI
// Copyright (c) 2011 - Mike Vegeto.  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!class SparkPlug
#include "Sparkplug.h"
#include "Engine.h"
#include <QPushButton>

SparkPlug::SparkPlug(QWidget *parent, Qt::WFlags flags) : 
QMainWindow(parent, flags) {
	
	// Setup the UI using the Qt UI thing
	//ui.setupUi(this);
	
	// Create our engine
	m_pEngine= new Engine();
	
	// Quick layout setup
	setFixedHeight( 50 );
	setFixedWidth( 200 );
	
	// Stars the engine
	m_pStartButton= new QPushButton( "Start the Engine", this );
	m_pStartButton->setVisible( true );
	m_pStartButton->setGeometry( 0, 0, width(), height() );
	connect( m_pStartButton, SIGNAL(pressed()), this, SLOT(slotStartProcessing()) );
		
}

//////////////////////////////////////////////////////////////////////////////
/*! Starts our engine processing cycle */

void SparkPlug::slotStartProcessing() {
	m_pEngine->start();
	
	int test= 0;
} // end slotStartProcessing()


//////////////////////////////////////////////////////////////////////////////
/*! Destructor */
SparkPlug::~SparkPlug() {

}
