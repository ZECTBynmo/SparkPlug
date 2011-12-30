///////////////////////////////////////////////////////////
//
// DropArea.cpp: Area to drop files
// Copyright (c) 2011 - Mike Vegeto.  All Rights Reserved
// 
//////////////////////////////////////////////////////////////////////////////
// DevStudio!class DropArea
#include "DropArea.h"
#include <QtGui>
#include <QList>
#include <QMimeData>
#include <QEvent>
#include "qpixmap.h"
#include "qlabel.h"
#include "qlist.h"
#include "qurl.h"
#include "qnamespace.h"
#include "qpainter.h"

//////////////////////////////////////////////////////////////////////////////
/*! Initialize */
DropArea::DropArea( QWidget* pParent ) :
QLabel( pParent ) {	
	
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setAlignment(Qt::AlignCenter);
	setAcceptDrops(true);
	setPixmap( QPixmap("C://Users//ZECTBynmo//Documents//Visual Studio 2008//Projects//SongSorter//SongSorter//dropSongsHere.png") );
	
	
} // end DropArea::DropArea()


//////////////////////////////////////////////////////////////////////////////
/*! Handle drop events */
void DropArea::dropEvent(QDropEvent *event) {
	const QMimeData* pMimeData = event->mimeData();
	
	if ( pMimeData->hasUrls() ) {
		QList<QUrl> urlList = pMimeData->urls();
		QString text;
		for (int iUrl = 0; iUrl < urlList.size() && iUrl < 32; ++iUrl) {
			QString url = urlList.at(iUrl).toLocalFile();
			text = url;
			emit sigSongDropped( text );
		}
		setText( text );
	}
	
	// Put our picture back
	setPixmap( QPixmap("C://Users//ZECTBynmo//Documents//Visual Studio 2008//Projects//SongSorter//SongSorter//dropSongsHere.png") );
} // end DropArea::dropEvent()


//////////////////////////////////////////////////////////////////////////////
/*! Handle drag enter events */
void DropArea::dragEnterEvent(QDragEnterEvent *event) {
	// Get our file name from our incoming metadata
	const QMimeData* pMimeData = event->mimeData();
	QList<QUrl> urlList = pMimeData->urls();
	
	bool bAcceptDrop= true;
	
	// Go through each of the files the user is proposing to drop and make sure they're all .wav's
	for (int iUrl = 0; iUrl < urlList.size() && iUrl < 32; ++iUrl) {
		QString url = urlList.at(iUrl).toLocalFile();
		
		// Make sure our filetype is right
		if( !url.contains( ".wav" ) ) {
			bAcceptDrop= false; 
			break;
		}
	} // end for each file dropped
	
	if( bAcceptDrop ) {

		setText(tr("<Drop the song!>"));
		setBackgroundRole(QPalette::Highlight);

		event->acceptProposedAction();
	} else {
		return;
	}
	
} // end DropArea::dragEnterEvent()


//////////////////////////////////////////////////////////////////////////////
/*! Handle drag leave events */
void DropArea::dragLeaveEvent(QDragLeaveEvent *event) {
	setText( "Drop Files Here!" );
} // end DropArea::dragLeaveEvent()


//////////////////////////////////////////////////////////////////////////////
/*! Overridden paint event */
void DropArea::paintEvent(QPaintEvent *) {
	QPainter* pPainter = new QPainter(this);
	
	// Setup background and "drop files here" pixmaps
	QPixmap pDropHerePixmap= QPixmap("./Images/dropSongsHere.png");
	QPixmap pBackgroundPixmap= QPixmap("./Images/DropAreaBackground.png");
	
	// Calculate the xy point that centers our drop here pixmap
	uint xMidDropHere= pDropHerePixmap.rect().width()/2,
		 xCorrectedLoc= width()/2 - xMidDropHere,
		 yMidDropHere= pDropHerePixmap.rect().height()/2,
		 yCorrectedLoc= height()/2 - yMidDropHere;
	
	// Draw our pixmaps
	pPainter->drawPixmap( -1, -1, pBackgroundPixmap.width(), pBackgroundPixmap.height(), pBackgroundPixmap );
	pPainter->drawPixmap( xCorrectedLoc, yCorrectedLoc, pDropHerePixmap.rect().width(), pDropHerePixmap.rect().height(), pDropHerePixmap );

	delete pPainter;
	
	// Put our picture back
	setPixmap( QPixmap("C://Users//ZECTBynmo//Documents//Visual Studio 2008//Projects//SongSorter//SongSorter//dropSongsHere.png") );
} // end DropArea::paintEvent()
