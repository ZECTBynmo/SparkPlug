///////////////////////////////////////////////////////////
//
// DropArea.h: Area to drop files
// Copyright (c) 2011 - Mike Vegeto. All Rights Reserved
//
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <QLabel>
#include <QMimeData>

//////////////////////////////////////////////////////////////////////////
//! Area to drop files
class DropArea : public QLabel {
	Q_OBJECT
	
public:
	DropArea( QWidget* pParent ); //!< Initialize
	
signals:
	void sigSongDropped( QString strFileName );
	
private:
	//! Overridden paint event
	virtual void paintEvent(QPaintEvent *);

	void dropEvent(QDropEvent *event);				//!< Handle drop events
	void dragEnterEvent(QDragEnterEvent *event);	//!< Handle drag enter events
	void dragLeaveEvent(QDragLeaveEvent *event);	//!< Handle drag leave events
	
}; // end class DropArea


