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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "OscListener.h"
#include "OscMessage.h"

#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "TuioObject.h"
#include "TuioCursor.h"

#define DEFAULT_TUIO_PORT 3333

namespace cinder  { namespace tuio {
	class Listener  {
	public:
		Listener();
		
		void connect(int port = DEFAULT_TUIO_PORT);
		void disconnect();
		bool isConnected(){ return mConnected;}
		
		void getMessage();
		
		void drawCursors(Vec2f windowSize);
		void drawObjects(Vec2f windowSize);
		
		std::list<Object*> getTuioObjects();
		std::list<Cursor*> getTuioCursors();
		
		Object* getTuioObject(long sessionId);
		Cursor* getTuioCursor(long sessionId);
		
		
		// Callbacks for Cursors
		void addCursorAddedCallback(boost::function<void(Cursor*)> callback);
		void addCursorUpdatedCallback(boost::function<void(Cursor*)> callback);
		void addCursorRemovedCallback(boost::function<void(Cursor*)> callback);
		
		// Callbacks for Objects
		void addObjectAddedCallback(boost::function<void(Object*)> callback);
		void addObjectUpdatedCallback(boost::function<void(Object*)> callback);
		void addObjectRemovedCallback(boost::function<void(Object*)> callback);
		
		// Callback for OSC Messages
		void addOSCMessageCallback(boost::function<void(osc::Message*)> callback);
		
		//TODO: Implementation for removing callbacks
		
	private:
		
		osc::Listener listener;
		
		std::list<Cursor*> mCursors;
		std::list<Cursor*> mFreeCursors;
		std::list<Cursor*> mFreeCursorsBuffer;
		std::list<Object*> mObjects;
		std::list<long> mAliveObjects;
		std::list<long> mNewObjects;
		std::list<long> mAliveCursors;
		std::list<long> mNewCursors;
		
		std::list< boost::function<void(Cursor*)> > mCursorAddedCallbacks;
		std::list< boost::function<void(Cursor*)> > mCursorUpdatedCallbacks;
		std::list< boost::function<void(Cursor*)> > mCursorRemovedCallbacks;
		
		std::list< boost::function<void(Object*)> > mObjectAddedCallbacks;
		std::list< boost::function<void(Object*)> > mObjectUpdatedCallbacks;
		std::list< boost::function<void(Object*)> > mObjectRemovedCallbacks;
		
		std::list< boost::function<void(osc::Message*)> > mOSCMessageCallbacks;
		
		int mCurrentFrame;
		int mLastFrame;
		int mMaxFingerID;
		
		bool mConnected;
	};

	
} // namespace tuio
} // namespace cinder
