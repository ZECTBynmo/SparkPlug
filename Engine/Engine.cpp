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
#include "Generator.h"
#include "qaudioformat.h"
#include "qendian.h"

using namespace std;

namespace {
	const uint SAMPLE_RATE= 44100,				// Only one sample rate for now
			   BUFFER_SIZE= 8192,				// Only one buffer size for now
			   MAX_CHANNELS= 2,					// Just change this to 1 for mono
			   CHUNK_SIZE= BUFFER_SIZE * 10000;	// The number of samples we read from an audio file at a time
			 
}

//////////////////////////////////////////////////////////////////////////////
/*! Initialize */
Engine::Engine() :
QThread(),
m_bStopProcessing(false),
m_bInjectAudioFromFile(true),
m_uProcessCount(0),
m_uNumChannels(MAX_CHANNELS),
m_uBufferSize(BUFFER_SIZE),
m_uNumSamples(CHUNK_SIZE) {
	
	// We have to do this to make sure our thread has the correct affinity.
	moveToThread(this);
	
	// Setup our audio format struct
	m_format.setFrequency( SAMPLE_RATE );
	m_format.setChannels( m_uNumChannels );
 	m_format.setSampleSize( 4 );
 	m_format.setCodec( "pcm" );
// 	m_format.setByteOrder( QAudioFormat::LittleEndian );
 	m_format.setSampleType( QAudioFormat::Float );
	
	// Setup our audio device information
	QAudioDeviceInfo info( QAudioDeviceInfo::defaultOutputDevice() );
	if ( !info.isFormatSupported(m_format) ) {
		//qWarning() << "Default format not supported - trying to use nearest";
		m_format = info.nearestFormat( m_format );
	}
	
	// Create our buffer of audio
	m_pAudioBuffer= new QBuffer();
	
	// Allocate for a temp buffer
	m_fTempBuffer.resize( m_uBufferSize );
	m_fTempChunk.resize( CHUNK_SIZE );
	m_fChunkFromFile.resize( MAX_CHANNELS );
	for( uint iChannel=0; iChannel<MAX_CHANNELS; ++iChannel ) {
		m_fChunkFromFile[iChannel].resize( CHUNK_SIZE );
	}
	
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
	
	//m_pGenerator= new Generator( m_format, m_uNumSamples, m_uSampleRate, this );
	
	m_uBufferSize= m_pAudioOutput->periodSize();
} // end Engine::createAudioOutput()


//////////////////////////////////////////////////////////////////////////////
/*! Set a new audio device */
void Engine::SetAudioDevice( const QAudioDeviceInfo& deviceInfo ) {
	m_pAudioOutput->stop();
	m_pAudioOutput->disconnect(this);
	m_pDevice = deviceInfo;
	
	// Set the use of float data
	m_format.setSampleType( QAudioFormat::Float );
	m_format.setSampleSize( 4 );
	
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
//////////////////////////////////////////////////////////////////////////
// libSndFile quick format reference
//////////////////////////////////////////////////////////////////////////
// 	    /* Major formats. */
// 		SF_FORMAT_WAV          = 0x010000,     /* Microsoft WAV format (little endian). */
// 		SF_FORMAT_AIFF         = 0x020000,     /* Apple/SGI AIFF format (big endian). */
// 		SF_FORMAT_AU           = 0x030000,     /* Sun/NeXT AU format (big endian). */
// 		SF_FORMAT_RAW          = 0x040000,     /* RAW PCM data. */
// 		SF_FORMAT_PAF          = 0x050000,     /* Ensoniq PARIS file format. */
// 		SF_FORMAT_SVX          = 0x060000,     /* Amiga IFF / SVX8 / SV16 format. */
// 		SF_FORMAT_NIST         = 0x070000,     /* Sphere NIST format. */
// 		SF_FORMAT_VOC          = 0x080000,     /* VOC files. */
// 		SF_FORMAT_IRCAM        = 0x0A0000,     /* Berkeley/IRCAM/CARL */
// 		SF_FORMAT_W64          = 0x0B0000,     /* Sonic Foundry's 64 bit RIFF/WAV */
// 		SF_FORMAT_MAT4         = 0x0C0000,     /* Matlab (tm) V4.2 / GNU Octave 2.0 */
// 		SF_FORMAT_MAT5         = 0x0D0000,     /* Matlab (tm) V5.0 / GNU Octave 2.1 */
// 
// 		/* Subtypes from here on. */
// 
// 		SF_FORMAT_PCM_S8       = 0x0001,       /* Signed 8 bit data */
// 		SF_FORMAT_PCM_16       = 0x0002,       /* Signed 16 bit data */
// 		SF_FORMAT_PCM_24       = 0x0003,       /* Signed 24 bit data */
// 		SF_FORMAT_PCM_32       = 0x0004,       /* Signed 32 bit data */
// 
// 		SF_FORMAT_PCM_U8       = 0x0005,       /* Unsigned 8 bit data (WAV and RAW only) */
// 
// 		SF_FORMAT_FLOAT        = 0x0006,       /* 32 bit float data */
// 		SF_FORMAT_DOUBLE       = 0x0007,       /* 64 bit float data */
// 
// 		SF_FORMAT_ULAW         = 0x0010,       /* U-Law encoded. */
// 		SF_FORMAT_ALAW         = 0x0011,       /* A-Law encoded. */
// 		SF_FORMAT_IMA_ADPCM    = 0x0012,       /* IMA ADPCM. */
// 		SF_FORMAT_MS_ADPCM     = 0x0013,       /* Microsoft ADPCM. */
// 
// 		SF_FORMAT_GSM610       = 0x0020,       /* GSM 6.10 encoding. */
// 		SF_FORMAT_VOX_ADPCM    = 0x0021,       /* Oki Dialogic ADPCM encoding. */
// 
// 		SF_FORMAT_G721_32      = 0x0030,       /* 32kbs G721 ADPCM encoding. */
// 		SF_FORMAT_G723_24      = 0x0031,       /* 24kbs G723 ADPCM encoding. */
// 		SF_FORMAT_G723_40      = 0x0032,       /* 40kbs G723 ADPCM encoding. */
// 
// 		SF_FORMAT_DWVW_12      = 0x0040,       /* 12 bit Delta Width Variable Word encoding. */
// 		SF_FORMAT_DWVW_16      = 0x0041,       /* 16 bit Delta Width Variable Word encoding. */
// 		SF_FORMAT_DWVW_24      = 0x0042,       /* 24 bit Delta Width Variable Word encoding. */
// 		SF_FORMAT_DWVW_N       = 0x0043,       /* N bit Delta Width Variable Word encoding. */
// 
// 		/* Endian-ness options. */
// 
// 		SF_ENDIAN_FILE         = 0x00000000,   /* Default file endian-ness. */
// 		SF_ENDIAN_LITTLE       = 0x10000000,   /* Force little endian-ness. */
// 		SF_ENDIAN_BIG          = 0x20000000,   /* Force big endian-ness. */
// 		SF_ENDIAN_CPU          = 0x30000000,   /* Force CPU endian-ness. */
// 
// 		SF_FORMAT_SUBMASK      = 0x0000FFFF,
// 		SF_FORMAT_TYPEMASK     = 0x0FFF0000,
// 		SF_FORMAT_ENDMASK      = 0x30000000
//////////////////////////////////////////////////////////////////////////

	// Setup our wav format
	const int format= SF_FORMAT_WAV | SF_FORMAT_FLOAT;
	const char* inFileName="C:/song.wav";

	// Open the audio file
	m_pAudioFile= new SndfileHandle( inFileName );
	
	uint uSampleRate= m_pAudioFile->samplerate(),
		 uNumChannels= m_pAudioFile->channels();
		 
	int iWavFormat= m_pAudioFile->format();
		 
	m_format.setSampleRate( uSampleRate );
	m_format.setChannelCount( uNumChannels );
	m_format.setChannels( m_uNumChannels );
	
	m_uSampleRate= uSampleRate;
	
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
		m_uNumSamples= m_pAudioFile->read( &m_fTempChunk[0], CHUNK_SIZE );
		
		// Separate the audio we've read into two channels
		separateChannels();
	} else {
		// Read mono data directly into our chunk
		m_uNumSamples= m_pAudioFile->readf( &m_fChunkFromFile[0][0], CHUNK_SIZE );
	}
} // end Engine::readChunkOfAudioFromFile()


//////////////////////////////////////////////////////////////////////////////
/*! Split the temporary buffer into two channels */
void Engine::separateChannels() {
	uint iFrame= 0;
	for( uint iSample=0; iSample<m_fTempChunk.size(); iSample+= 2 ) {
		m_fChunkFromFile[0][iFrame]= m_fTempChunk[iSample];
		m_fChunkFromFile[1][iFrame]= m_fTempChunk[iSample+1];
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
	// skewed more by any of a number of factors
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
		uint uBufferMS= 1000.0f * ( (float)m_uBufferSize / (float)m_uSampleRate );
		qint64 iTimerDelay;
		if( QDateTime::currentMSecsSinceEpoch() < m_uLastStartTime + uBufferMS )
			iTimerDelay= uBufferMS - QDateTime::currentMSecsSinceEpoch() + m_uLastStartTime;
		else 
			iTimerDelay= 0;

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

	uint uBufferStart= (m_uProcessCount-1)*m_uBufferSize,
		 uBufferEnd= m_uProcessCount*m_uBufferSize + m_uBufferSize;
		 
	// TODO: Get rid of this test stuff
	QAudioFormat::Endian byteOrd= m_format.byteOrder();
	uint uChannels= m_format.channelCount(),
		 uBytes= m_format.sampleSize(),
		 uFloatSize= sizeof(float);
		 
	QAudioFormat::SampleType format= m_format.sampleType();
		 
	uint samplesToWrite= m_pAudioOutput->periodSize();

	
	if( m_pOutput ) {
		if( uBufferStart + 2*m_uBufferSize < m_fChunkFromFile[0].size() )
// 			for( uint iWriteCycle=0; (iWriteCycle+2)*samplesToWrite<m_fChunkFromFile[0].size(); ++iWriteCycle ) {
// 				uint uBufferStart= m_uProcessCount*samplesToWrite + iWriteCycle*samplesToWrite;
// 				m_pOutput->write( qToLittleEndian((const char*)&m_fChunkFromFile[0][uBufferStart]), /*m_uNumChannels**/samplesToWrite/**sizeof(float)*/ );
// 			}
			
			m_pOutput->write( qToLittleEndian((const char*)&m_fChunkFromFile[0][uBufferStart]), 100*m_uNumChannels*m_uBufferSize*sizeof(float) );
		else {
			m_bStopProcessing= true;
			return; 
		}		
	} else {
		m_bStopProcessing= true;
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// GENERATOR TEST
	//////////////////////////////////////////////////////////////////////////
// 	if (m_pAudioOutput && m_pAudioOutput->state() != QAudio::StoppedState) {
// 		int chunks = m_pAudioOutput->bytesFree()/m_pAudioOutput->periodSize();
// 		while (chunks) {
// 			const qint64 len = m_pGenerator->read(m_buffer.data(), m_pAudioOutput->periodSize());
// 			if (len)
// 				m_pOutput->write(m_buffer.data(), len);
// 			if (len != m_pAudioOutput->periodSize())
// 				break;
// 			--chunks;
// 		}
//  	}
} // end Engine::outputAudio()


//////////////////////////////////////////////////////////////////////////////
/*! Destructor */
Engine::~Engine() {

}
