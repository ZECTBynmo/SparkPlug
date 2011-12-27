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

class /*ENGINE_EXPORT*/ Engine : public QThread {
	Q_OBJECT
public:
	Engine();	//!< Initialize
	~Engine();
	
private slots:
	void slotAudioDeviceNotification();		//!< We've received a notification from our audio device
	void slotAudioDeviceStateChanged();		//!< Our audio device state has changed	

private:
	QAudioDeviceInfo m_pDevice;			//!< Our audio device info
	QAudioOutput* m_pAudioOutput;		//!< Our audio output
	QIODevice* m_pOutput;				//!< A reference to the system IO device
	QAudioFormat m_format;				//!< Our audio format struct
	SndfileHandle* m_pAudioFile;			//!< The piece of audio we're currently looking at
	
	void openAudioFile();				//!< Opens an audio file
};

#endif // ENGINE_H
