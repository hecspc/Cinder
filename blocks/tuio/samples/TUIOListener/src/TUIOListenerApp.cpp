/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for TUIO Integration for Cinder framework developed by The Barbarian Group, 2010
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;


#include "TuioListener.h"
#include "TuioCursor.h"
#include "OscMessage.h"

class TUIOListenerApp : public AppBasic {
 public:
	void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();
	
	void cursorAdded(tuio::Cursor *cursor);
	void cursorUpdated(tuio::Cursor *cursor);
	void cursorRemoved(tuio::Cursor *cursor);
	void oscMessage(osc::Message *message);
	tuio::Listener tuio;
};

void TUIOListenerApp::prepareSettings(Settings *settings){
	settings->setWindowSize(640, 480);
	settings->setFrameRate(60.0f);
}

void TUIOListenerApp::setup(){
	tuio.connect(3333);
	
	tuio.addCursorAddedCallback(boost::bind(&TUIOListenerApp::cursorAdded, this, _1));
	tuio.addCursorUpdatedCallback(boost::bind(&TUIOListenerApp::cursorUpdated, this, _1));
	tuio.addCursorRemovedCallback(boost::bind(&TUIOListenerApp::cursorRemoved, this, _1));
	tuio.addOSCMessageCallback(boost::bind(&TUIOListenerApp::oscMessage, this, _1));
}

void TUIOListenerApp::cursorAdded(tuio::Cursor *cursor){
	std::cout << "Cursor added " << cursor->getSessionId() << std::endl;
}

void TUIOListenerApp::cursorUpdated(tuio::Cursor *cursor){
	std::cout << "Cursor updated " << cursor->getSessionId() << std::endl;
}

void TUIOListenerApp::cursorRemoved(tuio::Cursor *cursor){
	std::cout << "Cursor removed " << cursor->getSessionId() << std::endl;
}

void TUIOListenerApp::oscMessage(osc::Message *message){
	std::cout << "OSC Message " << message->getAddress() << std::endl;
}

void TUIOListenerApp::update(){
	
	if (tuio.isConnected())
		tuio.getMessage();
}


void TUIOListenerApp::draw()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();	
	gl::enableAlphaBlending();
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	
	gl::setMatricesWindow( getWindowSize() );
	if (tuio.isConnected()){
		tuio.drawCursors( Vec2f( getWindowWidth(), getWindowHeight() ) );
		tuio.drawObjects( Vec2f( getWindowWidth(), getWindowHeight() ) );
	}
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( TUIOListenerApp, RendererGl )
