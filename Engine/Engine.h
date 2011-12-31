///////////////////////////////////////////////////////////
//
// DropArea.h: Area to drop files
// Copyright (c) 2011 - Mike Vegeto. All Rights Reserved
//
//////////////////////////////////////////////////////////////////////////////
#ifndef ENGINE_H
#define ENGINE_H

#include "engine_global.h"
#include <QThread>
#include <QIODevice>
#include <QAudioOutput>
#include "sndfile.hh"
#include "qbuffer.h"
#include <vector>
using namespace std;

class /*ENGINE_EXPORT*/ Engine : public QThread {
	Q_OBJECT
public:
	Engine();	//!< Initialize
	~Engine();
	
	//! Set a new audio device
	void SetAudioDevice( const QAudioDeviceInfo& deviceInfo );
	
signals:
	
private slots:
	void slotAudioDeviceNotification();		//!< We've received a notification from our audio device
	void slotAudioDeviceStateChanged();		//!< Our audio device state has changed	
	void slotRunProcessingThread();			//!< Perform DSP thread operations

protected:
	void run();		//!< Begin processing, called from the UI thread to start processing

private:
	QAudioDeviceInfo m_pDevice;			//!< Our audio device info
	QAudioOutput* m_pAudioOutput;		//!< Our audio output
	QIODevice* m_pOutput;				//!< A reference to the system IO device
	QAudioFormat m_format;				//!< Our audio format struct
	SndfileHandle* m_pAudioFile;		//!< The piece of audio we're currently looking at
	
	bool m_bStopProcessing,				//!< Set when we're going to stop processing after next round;
		 m_bInjectAudioFromFile;		//!< Set when we're injecting audio from some file into the processing stream
		 
	uint m_uNumChannels;				//<! Our current number of channels
		 
	qint64 m_uLastStartTime,			//!< The time that the last round of processing started
		   m_uProcessCount;				//!< The number of times we've processed
		
	vector<float> m_fTempChunk;			//!< A temporary 1d vector to hold the chunk of audio
	
	vector< vector<float> > m_fTempBuffer,			//!< A temporary buffer to hold audio samples before they're
						    m_fChunkFromFile;		//!< The chunk of audio we've read from file and are holding in memory

	QBuffer* m_pAudioBuffer;			//<! The current buffer of audio we're working with
	
	
	
	//! Perform DSP thread operations
	void runProcessingThread();	
	//! Schedule another round of processing
	void scheduleProcessing();
	//! Processes a buffer of audio
	void processBuffer();	
	//! Clear the buffer's data
	void clearBuffer();		
	//! Inject our audio file's data into the stream
	void injectAudioFromFile();			
	//! Opens an audio file
	void openAudioFile();
	//! Sends audio through the audio output
	void outputAudio();
	//! Reads a chunk of audio into memory for fast processing
	void readChunkOfAudioFromFile();
	//! Split the temporary buffer into two channels
	void separateChannels();	
	//! Create our audio output
	void createAudioOutput();
};

#endif // ENGINE_H
