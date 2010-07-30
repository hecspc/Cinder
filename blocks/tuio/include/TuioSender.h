/*
 *  TuioSender.h
 *  TUIOSender
 *
 *  Created by hec on 7/7/10.
 *  Copyright 2010 aer studio. All rights reserved.
 *
 */

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"

#include "OscSender.h"

#include <list>
#include <string>

#include "TuioObject.h"
#include "TuioCursor.h"

namespace cinder { namespace tuio {
	
	class Sender {
	
	public:
		Sender();
		
		~Sender();
		
		void connect(std::string host, int port);
		void disconnect();
		
		Object* addTuioObject(int symbolId, float xpos, float ypos, float angle);
		void updateTuioObject(Object *object, float xpos, float ypos, float angle);
		void removeTuioObject(Object *object);
		
		void addExternalTuioObject(Object *object);
		void updateExternalTuioObject(Object *object);
		void removeExternalTuioObject(Object *object);
		
		Cursor* addTuioCursor(float xpos, float ypos);
		void udpateTuioCursor(Cursor *cursor, float xpos, float ypos);
		void removeTuioCursor(Cursor *cursor);
		
		void addExternalTuioCursor(Cursor *cursor);
		void updateExternalTuioCursor(Cursor *cursor);
		void removeExternalTuioCursor(Cursor *cursor);
		
		void initFrame();
		void commitFrame();
		
		long getSessionID();
		long getFrameID();
		double getFrameTime();
		
		void sendFullMessages();
		
		
		
		void enableFullUpdate(){ mFullUpdate = true; }
		void disableFullUpdate(){ mFullUpdate = false; }
		
		std::list<Object*> getUntouchedObjects();
		std::list<Cursor*> getUntouchedCursors();
		
		void stopUntouchedMovingObjects();
		void stopUntouchedMovingCursors();
		
		void removeUntouchedStoppedObjects();
		void removeUntouchedStoppedCursors();
		
		std::list<Object*> getTuioObjects();
		std::list<Cursor*> getTuioCursors();
		
		Object* getTuioObject(long sessionId);
		Cursor* getTuioCursor(long sessionId);
		
		Object* getClosestTuioObject(float xp, float yp);
		Cursor* getClosestTuioCursor(float xp, float yp);
		
		bool isConnected(){ return mConnected; }
		
	private:
		
		std::list<Object*> mObjects;
		std::list<Cursor*> mCursors;
		
		int mMaxFingerID;
		std::list<Cursor*> mFreeCursors;
		std::list<Cursor*> mFreeCursorsBuffer;
		
		void sendEmptyCursorBundle();
		void startCursorBundle();
		void addCursorMessage(Cursor *tcur);
		void sendCursorBundle(long fseq);
		
		void sendEmptyObjectBundle();
		void startObjectBundle();
		void addObjectMessage(Object *tobj);
		void sendObjectBundle(long fseq);
		
		bool mFullUpdate;
		
		double mStartFrameTime;
		double mCurrentFrameTime;
		long mCurrentFrame;
		bool mUpdateObject, mUpdateCursor;
		long mLastCursorUpdate, mLastObjectUpdate;
		
		long mSessionID;
		bool mConnected;
		
		osc::Sender mSender;
		osc::Bundle mCursorBundle, mObjectBundle;
		app::AppBasic *app;
	};
	
} // namespace tuio
} // namespace cinder