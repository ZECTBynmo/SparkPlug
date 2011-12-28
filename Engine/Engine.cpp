///////////////////////////////////////////////////////////
//
// Engine.cpp: Audio Engine
// Copyright (c) 2011 - Mike Vegeto.  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!class Engine
#include "Engine.h"
#include "sndfile.h"
#include <vector>
using namespace std;

namespace {
	const uint SAMPLE_RATE= 44100,
			   BUFFER_SIZE= 8192,
			   MAX_CHANNELS= 2;
}

//////////////////////////////////////////////////////////////////////////////
/*! Initialize */
Engine::Engine() :
QThread() {
	
	// We have to do this to make sure our thread has the correct affinity.
	moveToThread(this);
	
	// Setup our audio format struct
	m_format.setFrequency( SAMPLE_RATE );
	m_format.setChannels( 2 );
	m_format.setSampleSize( 16 );
	m_format.setCodec( "audio/pcm" );
	m_format.setByteOrder( QAudioFormat::LittleEndian );
	m_format.setSampleType( QAudioFormat::Float );
	
	// Setup our audio device information
	QAudioDeviceInfo info( QAudioDeviceInfo::defaultOutputDevice() );
	if ( !info.isFormatSupported(m_format) ) {
		//qWarning() << "Default format not supported - trying to use nearest";
		m_format = info.nearestFormat( m_format );
	}
	
	// Setup our audio device
	m_pAudioOutput = 0;
	m_pAudioOutput = new QAudioOutput(m_pDevice, m_format, this);
	connect(m_pAudioOutput, SIGNAL(notify()), this, SLOT(slotAudioDeviceNotification()));
	connect(m_pAudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(slotAudioDeviceStateChanged(QAudio::State)));
	
	// Open our audio
	openAudioFile();
} // end Engine::Engine()


//////////////////////////////////////////////////////////////////////////////
/*! We've received a notification from our audio device */
void Engine::slotAudioDeviceNotification() {
	// Don't do anything, but spit out the notification as a warning
// 	qWarning() << "bytesFree = " << m_pAudioOutput->bytesFree()
// 			   << ", " << "elapsedUSecs = " << m_pAudioOutput->elapsedUSecs()
// 			   << ", " << "processedUSecs = " << m_pAudioOutput->processedUSecs();
} // end Engine::slotAudioDeviceNotification()


//////////////////////////////////////////////////////////////////////////////
/*! Our audio device state has changed */
void Engine::slotAudioDeviceStateChanged() {
	// Spit out the new state as a warning
	//qWarning() << "state = " << state;
} // end Engine::slotAudioDeviceStateChanged()


//////////////////////////////////////////////////////////////////////////////
/*! Opens an audio file */
void Engine::openAudioFile() {

	const int format=SF_FORMAT_WAV | SF_FORMAT_FLOAT;
	const char* inFileName="E:/wankfest.wav";

	m_pAudioFile= new SndfileHandle( inFileName );
	vector<float> buffer(10001);
	m_pAudioFile->read( &buffer[0], 10000 );
	int test=0;
} // end Engine::openAudioFile()


//////////////////////////////////////////////////////////////////////////////
/*! Destructor */
Engine::~Engine() {

}
