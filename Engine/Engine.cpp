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
#include <QTimer>
#include "qdatetime.h"
using namespace std;

namespace {
	const uint SAMPLE_RATE= 44100,				// Only one sample rate for now
			   BUFFER_SIZE= 8192,				// Only one buffer size for now
			   MAX_CHANNELS= 2,					// Just change this to 1 for mono
			   CHUNK_SIZE= BUFFER_SIZE * 10000;	// The number of samples we read from an audio fle at a time
			 
}

//////////////////////////////////////////////////////////////////////////////
/*! Initialize */
Engine::Engine() :
QThread(),
m_bStopProcessing(false),
m_bInjectAudioFromFile(true),
m_uProcessCount(0),
m_uNumChannels(MAX_CHANNELS) {
	
	// We have to do this to make sure our thread has the correct affinity.
	moveToThread(this);
	
	// Setup our audio format struct
	m_format.setFrequency( SAMPLE_RATE );
	m_format.setChannels( m_uNumChannels );
// 	m_format.setSampleSize( 32 );
// 	m_format.setCodec( "audio/pcm" );
// 	m_format.setByteOrder( QAudioFormat::LittleEndian );
// 	m_format.setSampleType( QAudioFormat::Float );
	
	// Setup our audio device information
	QAudioDeviceInfo info( QAudioDeviceInfo::defaultOutputDevice() );
	if ( !info.isFormatSupported(m_format) ) {
		//qWarning() << "Default format not supported - trying to use nearest";
		m_format = info.nearestFormat( m_format );
	}
	
	// Create our buffer of audio
	m_pAudioBuffer= new QBuffer();
	
	// Allocate for a temp buffer
	m_fTempBuffer.resize( BUFFER_SIZE );
	m_fTempChunk.resize( CHUNK_SIZE );
	m_fChunkFromFile.resize( MAX_CHANNELS );
	for( uint iChannel=0; iChannel<MAX_CHANNELS; ++iChannel )
		m_fChunkFromFile[iChannel].resize( CHUNK_SIZE );
	
	// Setup our audio device
	createAudioOutput();
	
	// Start up our IO device
	m_pOutput= m_pAudioOutput->start();
	QAudio::Error err= m_pAudioOutput->error();
	
	// Open our audio
	openAudioFile();
} // end Engine::Engine()


//////////////////////////////////////////////////////////////////////////////
/*! Create our audio output */
void Engine::createAudioOutput() {
	m_pAudioOutput = 0;
	m_pAudioOutput = new QAudioOutput(m_pDevice, m_format, this);
	connect(m_pAudioOutput, SIGNAL(notify()), this, SLOT(slotAudioDeviceNotification()));
	connect(m_pAudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(slotAudioDeviceStateChanged(QAudio::State)));
	
	m_pOutput= m_pAudioOutput->start();
	QAudio::Error err= m_pAudioOutput->error();
} // end Engine::createAudioOutput()


//////////////////////////////////////////////////////////////////////////////
/*! Set a new audio device */
void Engine::SetAudioDevice( const QAudioDeviceInfo& deviceInfo ) {
	m_pAudioOutput->stop();
	m_pAudioOutput->disconnect(this);
	m_pDevice = deviceInfo;
	
	// Setup our audio device information
	if ( !m_pDevice.isFormatSupported(deviceInfo.preferredFormat()) ) {
		m_format = m_pDevice.nearestFormat( deviceInfo.preferredFormat() );
	}
	createAudioOutput();
	
} // end Engine::setAudioDevice()


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

	// Setup our wav format
	const int format= SF_FORMAT_WAV | SF_FORMAT_FLOAT;
	const char* inFileName="C:/TheXX-VCR.wav";

	// Open the audio file
	m_pAudioFile= new SndfileHandle( inFileName );
	
	uint uSampleRate= m_pAudioFile->samplerate(),
		 uNumChannels= m_pAudioFile->channels();
		 
	m_format.setSampleRate( uSampleRate );
	m_format.setChannelCount( uNumChannels );
	m_format.setChannels( m_uNumChannels );
	
	// Recreate our audio output because we have a new format
	createAudioOutput();
			 
	// Set our current number of channels
	m_uNumChannels= uNumChannels;
		 
	// Read some audio !!TEST!!
	readChunkOfAudioFromFile();
} // end Engine::openAudioFile()


//////////////////////////////////////////////////////////////////////////////
/*! Reads a chunk of audio into memory for fast processing */
void Engine::readChunkOfAudioFromFile() {
	// Read in a chunk of audio
	if( m_uNumChannels > 1 ) {
		// Read audio into our temp chunk buffer
		uint uSamplesRead= m_pAudioFile->read( &m_fTempChunk[0], CHUNK_SIZE );
		
		// Separate the audio we've read into two channels
		separateChannels();
	} else {
		// Read mono data directly into our chunk
		uint uSamplesRead= m_pAudioFile->read( &m_fChunkFromFile[0][0], CHUNK_SIZE );
	}
} // end Engine::readChunkOfAudioFromFile()


//////////////////////////////////////////////////////////////////////////////
/*! Split the temporary buffer into two channels */
void Engine::separateChannels() {
	uint iFrame= 0;
	for( uint iSample=0; iSample<m_fTempChunk.size(); iSample+= 2 ) {
		m_fChunkFromFile[0][iFrame]= m_fTempChunk[iSample];
		m_fChunkFromFile[1][iFrame]= m_fTempChunk[iSample];
		iFrame++;		
	}
} // end Engine::separateChannels()


//////////////////////////////////////////////////////////////////////////////
/*! Begin processing, called from the UI thread to start processing */
void Engine::run() {
	// This schedules slotRunProcessingThread()
	// to run just after our event loop starts up
	QTimer::singleShot( 0, this, SLOT(slotRunProcessingThread()) );

	// This starts the event loop. Note that
	// exec() does not return until the
	// event loop is stopped.
	exec();
		
} // end Engine::run()


//////////////////////////////////////////////////////////////////////////////
/*! Perform DSP thread operations */
void Engine::slotRunProcessingThread() {
	// Run it already!
	runProcessingThread();
	
} // end Engine::slotRunProcessingThread()


//////////////////////////////////////////////////////////////////////////////
/*! Perform DSP thread operations */
void Engine::runProcessingThread() {
	//////////////////////////////////////////////////////////////////////////
	// We need to be outputting audio at as close to real time as possible
	// To do this, we trigger a round of processing and output using a timer. 
	//
	// The timer is set to go off one second from the last time we STARTED 
	// a round of processing. Assuming a round of processing takes about the
	// same amount of time each round, this should work pretty well.
	//
	// Because we get timestamps in milliseconds, we can't be any more
	// accurately regular than +/- 1ms, and the actual regularity may be
	// skewed more than that by the locking process inherent in the 
	// signal/slot system to the outside world
	//////////////////////////////////////////////////////////////////////////
	
	// Get a timestamp for the start of this round of processing
	m_uLastStartTime= QDateTime::currentMSecsSinceEpoch();
	
	m_uProcessCount++;
	
	// Clear out the previous buffer of audio
	clearBuffer();
	
	if( m_bInjectAudioFromFile ) {
		// Copy some audio from our audio file 
		injectAudioFromFile();
	}

	// Process the buffer
	processBuffer();	
	
	// Send our processed audio out the sound card
	outputAudio();
	
	// Schedule another round of processing
	scheduleProcessing();
} // end Engine::runProcessingThread()


//////////////////////////////////////////////////////////////////////////////
/*! Schedule another round of processing */
void Engine::scheduleProcessing() {
	// Unless we're about to stop, kick off another round of processing
	if( m_bStopProcessing ) {	
		// Exit the thread routine
		m_bStopProcessing= false;
		
		// Reset our processing count
		m_uProcessCount= 0;
		
		// Exit the processing round for good (THIS EXITS THE THREAD, BAD!)
		exit();
	} else {
		// Schedule the next round of processing for one second after this round started
		uint uBufferMS= 1000 * ( (float)BUFFER_SIZE / (float)SAMPLE_RATE );
		qint64 iTimerDelay;
		//if( QDateTime::currentMSecsSinceEpoch() > m_uLastStartTime + uBufferMS )
			iTimerDelay= uBufferMS - QDateTime::currentMSecsSinceEpoch() + m_uLastStartTime;
		//else 
		//	iTimerDelay= 0;

		QTimer::singleShot( iTimerDelay, this, SLOT(slotRunProcessingThread()) );
	}
} // end Engine::scheduleProcessing()


//////////////////////////////////////////////////////////////////////////////
/*! Processes a buffer of audio */
void Engine::processBuffer() {
	// We can do our processing here and not worry about locking
	int test = 0;
} // end Engine::processBuffer()


//////////////////////////////////////////////////////////////////////////////
/*! Clear the buffer's data */
void Engine::clearBuffer() {
	
} // end Engine::clearBuffer()


//////////////////////////////////////////////////////////////////////////////
/*! Inject our audio file's data into the stream */
void Engine::injectAudioFromFile() {	
	
	int test=0;
} // end Engine::injectAudioFromFile()


//////////////////////////////////////////////////////////////////////////////
/*! Sends audio through the audio output */
void Engine::outputAudio() {
	QByteArray tempAudio;

	uint uPeriod= m_pAudioOutput->periodSize();

	uint uBufferStart= m_uProcessCount*BUFFER_SIZE,
		 uBufferEnd= m_uProcessCount*BUFFER_SIZE + BUFFER_SIZE;
		 
// 	for( uint iSample= uBufferStart; iSample<uBufferEnd; ++iSample ) {
// 		if( iSample >= m_fChunkFromFile[0].size() ) {
// 			m_bStopProcessing= true;
// 			return;
// 		}
// 		q
// 		tempAudio.append( reinterpret_cast<const char*>(&m_fChunkFromFile[0][iSample]), sizeof(m_fChunkFromFile[0][iSample]) );
// 		m_pAudioBuffer->wo rite( tempAudio );
// 	}
	
	if( m_pOutput ) {
// 		m_pOutput->write( m_pAudioBuffer->data(), m_pAudioOutput->periodSize() );
		if( uBufferStart + 2*BUFFER_SIZE < m_fChunkFromFile[0].size() )
			m_pOutput->write( (const char*)&m_fChunkFromFile[0][uBufferStart], m_format.channelCount()*BUFFER_SIZE*sizeof(float) );
		else {
			m_bStopProcessing= true;
			return;
		}		
	} else {
		m_bStopProcessing= true;
		return;
	}
} // end Engine::outputAudio()


//////////////////////////////////////////////////////////////////////////////
/*! Destructor */
Engine::~Engine() {

}
