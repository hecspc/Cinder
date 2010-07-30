/*
 *  TuioSender.cpp
 *  TUIOSender
 *
 *  Created by hec on 7/7/10.
 *  Copyright 2010 aer studio. All rights reserved.
 *
 */


#include "TuioSender.h"

#include "OscBundle.h"
#include "OscMessage.h"

namespace cinder { namespace tuio {
	
	Sender::Sender(){
		app = app::AppBasic::get();
	}
	
	Sender::~Sender(){
		mConnected = false;
		
		sendEmptyCursorBundle();
		sendEmptyObjectBundle();
		
		mSender.shutdown();
	}
	
	void Sender::connect(std::string host, int port){
		mSender.setup(host, port);
		
		mCurrentFrameTime = mStartFrameTime = app->getElapsedSeconds();
		mCurrentFrame = mSessionID = mMaxFingerID = -1;
		mUpdateObject = mUpdateCursor = false;
		mLastObjectUpdate = mLastCursorUpdate = mCurrentFrameTime;
		
		sendEmptyCursorBundle();
		sendEmptyObjectBundle();
		
		mFullUpdate = false;
		mConnected = true;
	}
	
	void Sender::disconnect(){
		mSender.shutdown();
		
		
		for (std::list<Object*>::iterator iter=mObjects.begin(); iter != mObjects.end(); iter++)
			delete (*iter);
		mObjects.clear();
		
		for (std::list<Cursor*>::iterator iter=mCursors.begin(); iter != mCursors.end(); iter++)
			delete (*iter);
		mCursors.clear();
		
		for (std::list<Cursor*>::iterator iter=mFreeCursors.begin(); iter != mFreeCursors.end(); iter++)
			delete(*iter);
		mFreeCursors.clear();
		
		mConnected = false;
	}
	
	Object* Sender::addTuioObject(int fiducialId, float xpos, float ypos, float angle){
		mSessionID++;
		Object *object = new Object(mCurrentFrameTime, mSessionID, fiducialId, xpos, ypos, angle);
		mObjects.push_back(object);
		mUpdateObject = true;
		
		return object;
	}
	
	void Sender::updateTuioObject(Object *object, float xpos, float ypos, float angle){
		if (object == NULL) return;
		if (object->getTime() == mCurrentFrameTime) return;
		
		object->update(mCurrentFrameTime, xpos, ypos, angle);
		mUpdateObject = true;
	}
	
	
	void Sender::removeTuioObject(Object *object){
		if (object == NULL) return;
		mObjects.remove(object);
		delete object;
		mUpdateObject = true;
	}
	
	void Sender::addExternalTuioObject(Object *object){
		if (object == NULL) return;
		mObjects.push_back(object);
		mUpdateObject = true;
	}
	
	
	void Sender::updateExternalTuioObject(Object *object){
		if (object == NULL) return;
		mUpdateObject = true;
	}
	
	void Sender::removeExternalTuioObject(Object *object){
		if (object == NULL) return;
		mObjects.remove(object);
		mUpdateObject = true;
		
	}
	
	Cursor* Sender::addTuioCursor(float xpos, float ypos){
		mSessionID++;
		
		int fingerId = (int)mCursors.size();
		if (((int)(mCursors.size()) <= mMaxFingerID) && ((int)(mFreeCursors.size() > 0))) {
			std::list<Cursor*>::iterator closestCursor = mFreeCursors.begin();
			for (std::list<Cursor*>::iterator it = mFreeCursors.begin(); it != mFreeCursors.end(); it++){
				if ((*it)->getDistance(xpos, ypos) < (*closestCursor)->getDistance(xpos, ypos)) closestCursor = it;
			}
			
			Cursor *freeCursor = (*closestCursor);
			fingerId = (*closestCursor)->getFingerId();
			mFreeCursors.erase(closestCursor);
			delete freeCursor;
		}else {
			mMaxFingerID = fingerId;
		}
		
		Cursor *cursor = new Cursor(mCurrentFrameTime, mSessionID, fingerId, xpos, ypos);
		mCursors.push_back(cursor);
		mUpdateCursor = true;
			
		return cursor;
			
	}
	
	void Sender::udpateTuioCursor(Cursor *cursor, float xpos, float ypos){
		if (cursor == NULL) return;
		if (cursor->getTime() == mCurrentFrameTime) return;
		cursor->update(mCurrentFrameTime, xpos, ypos);
		mUpdateCursor = true;
	}
	
	void Sender::removeTuioCursor(Cursor *cursor){
		if (cursor == NULL) return;
		mCursors.remove(cursor);
		cursor->remove(mCurrentFrameTime);
		mUpdateCursor = true;
		
		if (cursor->getFingerId() == mMaxFingerID){
			mMaxFingerID = -1;
			delete cursor;
			
			if (mCursors.size() > 0){
				std::list<Cursor*>::iterator clist;
				for (clist = mCursors.begin(); clist != mCursors.end(); clist++){
					int fingerId = (*clist)->getFingerId();
					if (fingerId > mMaxFingerID) mMaxFingerID = fingerId;
				}
				
				mFreeCursorsBuffer.clear();
				
				for (std::list<Cursor*>::iterator flist = mFreeCursors.begin(); flist != mFreeCursors.end(); flist++){
					Cursor *freeCursor = (*flist);
					if (freeCursor->getFingerId() > mMaxFingerID) delete freeCursor;
					else mFreeCursorsBuffer.push_back(freeCursor);
				}
				mFreeCursors = mFreeCursorsBuffer;
			}else {
				for (std::list<Cursor*>::iterator flist = mFreeCursors.begin(); flist != mFreeCursors.end(); flist++){
					Cursor *freeCursor = (*flist);
					delete freeCursor;
				}
				mFreeCursors.clear();
			}

		}else if (cursor->getFingerId() < mMaxFingerID){
			mFreeCursors.push_back(cursor);
		}
	}
	
	void Sender::addExternalTuioCursor(Cursor *cursor){
		if (cursor == NULL) return;
		mCursors.push_back(cursor);
		mUpdateCursor = true;
	}
	
	void Sender::updateExternalTuioCursor(Cursor *cursor){
		if (cursor == NULL) return;
		mUpdateCursor = true;
	}
	
	void Sender::removeExternalTuioCursor(Cursor *cursor){
		if (cursor == NULL) return;
		mCursors.remove(cursor);
		mUpdateCursor = true;
	}
	
	
	void Sender::initFrame(){
		mCurrentFrameTime = app->getElapsedSeconds() - mStartFrameTime;
		mCurrentFrame++;
	}
	
	void Sender::commitFrame(){
		if (mUpdateCursor){
			startCursorBundle();
			for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++){
				Cursor *cursor = (*it);
				if (mFullUpdate || (cursor->getTime() == mCurrentFrameTime)) addCursorMessage(cursor);
			}
			sendCursorBundle(mCurrentFrame);
		}else if (mLastCursorUpdate < mCurrentFrameTime) {
			mLastCursorUpdate = mCurrentFrameTime;
			startCursorBundle();
			sendCursorBundle(mCurrentFrame);
		}
		mUpdateCursor = false;
		
		if (mUpdateObject){
			startObjectBundle();
			for (std::list<Object*>::iterator it = mObjects.begin(); it != mObjects.end(); it++){
				Object *object = (*it);
				if (mFullUpdate || object->getTime() == mCurrentFrameTime) addObjectMessage(object);
			}
			sendObjectBundle(mCurrentFrame);
		}else if (mLastObjectUpdate < mCurrentFrameTime) {
			mLastObjectUpdate = mCurrentFrameTime;
			startObjectBundle();
			sendObjectBundle(mCurrentFrame);
		}
		mUpdateObject = false;
		
	}
	
	long Sender::getSessionID(){
		mSessionID++;
		return mSessionID;
	}
	
	long Sender::getFrameID(){
		return mCurrentFrame;
	}
	
	double Sender::getFrameTime(){
		return mCurrentFrameTime;
	}
	
	std::list<Object*> Sender::getUntouchedObjects(){
		std::list<Object*> untouched;
		for (std::list<Object*>::iterator it = mObjects.begin(); it!=mObjects.end(); it++) {
			Object *object = (*it);
			if (object->getTime() != mCurrentFrameTime) untouched.push_back(object);
		}	
		return untouched;
	}
	
	std::list<Cursor*> Sender::getUntouchedCursors(){
		std::list<Cursor*> untouched;
		for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++) {
			Cursor *cursor = (*it);
			if (cursor->getTime() != mCurrentFrameTime) untouched.push_back(cursor);
		}	
		return untouched;
	}
	
	void Sender::stopUntouchedMovingObjects(){
		std::list<Object*> untouched;
		for (std::list<Object*>::iterator it = mObjects.begin(); it != mObjects.end(); it++) {
			
			Object *object = (*it);
			if ((object->getTime() != mCurrentFrameTime) && (object->isMoving())) {
				object->stop(mCurrentFrameTime);
				mUpdateObject = true;
			}
		}
	}
	
	void Sender::stopUntouchedMovingCursors(){
		std::list<Cursor*> untouched;
		for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++) {
			
			Cursor *cursor = (*it);
			if ((cursor->getTime() != mCurrentFrameTime) && (cursor->isMoving())) {
				cursor->stop(mCurrentFrameTime);
				mUpdateCursor = true;
			}
		}
	}
	
	void Sender::removeUntouchedStoppedObjects(){
		std::list<Object*>::iterator it = mObjects.begin();
		while (it != mObjects.end()) {
			Object *object = (*it);
			if ((object->getTime() != mCurrentFrameTime) && (!object->isMoving())) {
				removeTuioObject(object);
				it = mObjects.begin();
			} else it++;
		}
	}
	
	void Sender::removeUntouchedStoppedCursors(){
		std::list<Cursor*>::iterator it = mCursors.begin();
		while (it != mCursors.end()) {
			Cursor *cursor = (*it);
			if ((cursor->getTime() != mCurrentFrameTime) && (!cursor->isMoving())) {
				removeTuioCursor(cursor);
				it = mCursors.begin();
			} else it++;
		}
	}
	
	std::list<Object*> Sender::getTuioObjects(){
		return mObjects;
	}
	
	std::list<Cursor*> Sender::getTuioCursors(){
		return mCursors;
	}
	
	Object* Sender::getTuioObject(long sessionId){
		for (std::list<Object*>::iterator it = mObjects.begin(); it != mObjects.end(); it++)
			if ((*it)->getSessionId() == sessionId) return (*it);
		
		return NULL;
	}
	
	Cursor* Sender::getTuioCursor(long sessionId){
		for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++)
			if ((*it)->getSessionId() == sessionId) return (*it);
		
		return NULL;
	}
	
	Object* Sender::getClosestTuioObject(float xpos, float ypos){
		Object *closestObject = NULL;
		float closestDistance = 1.0f;
		
		for (std::list<Object*>::iterator it = mObjects.begin(); it != mObjects.end(); it++){
			float distance = (*it)->getDistance(xpos, ypos);
			if (distance < closestDistance){
				closestObject = (*it);
				closestDistance = distance;
			}
		}
		return closestObject;
	}
	
	Cursor* Sender::getClosestTuioCursor(float xpos, float ypos){
		Cursor *closestCursor = NULL;
		float closestDistance = 1.0f;
		
		for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++){
			float distance = (*it)->getDistance(xpos, ypos);
			if (distance < closestDistance){
				closestCursor = (*it);
				closestDistance = distance;
			}
		}
		return closestCursor;
	}
	
	void Sender::sendEmptyCursorBundle(){
		osc::Bundle bundle;
		
		osc::Message aliveMessage;
		aliveMessage.setAddress("/tuio/2Dcur");
		aliveMessage.addStringArg("alive");
		
		bundle.addMessage(aliveMessage);
		
		osc::Message fseqMessage;
		fseqMessage.setAddress("/tuio/2Dcur");
		fseqMessage.addStringArg("fseq");
		fseqMessage.addIntArg(-1);
		
		bundle.addMessage(fseqMessage);
		
		mSender.sendBundle(bundle);
	}
	
	void Sender::startCursorBundle(){
		mCursorBundle.clear();
		osc::Message aliveMessage;
		aliveMessage.setAddress("/tuio/2Dcur");
		aliveMessage.addStringArg("alive");
		for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++){
			aliveMessage.addIntArg((int32_t)((*it)->getSessionId()));
		}
		mCursorBundle.addMessage(aliveMessage);
								   
	}
	
	void Sender::addCursorMessage(Cursor *cursor){
		osc::Message message;
		message.setAddress("/tuio/2Dcur");
		message.addStringArg("set");
		message.addIntArg((int32_t)(cursor->getSessionId()));
		message.addFloatArg(cursor->getX());
		message.addFloatArg(cursor->getY());
		message.addFloatArg(cursor->getXSpeed());
		message.addFloatArg(cursor->getYSpeed());
		message.addFloatArg(cursor->getMotionAccel());
		mCursorBundle.addMessage(message);
	}
	
	void Sender::sendCursorBundle(long fseq){
		osc::Message message;
		message.setAddress("/tuio/2Dcur");
		message.addStringArg("set");
		message.addIntArg((int32_t)fseq);
		
		mCursorBundle.addMessage(message);
		
		mSender.sendBundle(mCursorBundle);
	}
	
	void Sender::sendEmptyObjectBundle(){
		osc::Bundle bundle;
		
		osc::Message aliveMessage;
		aliveMessage.setAddress("/tuio/2Dobj");
		aliveMessage.addStringArg("alive");
		
		bundle.addMessage(aliveMessage);
		
		osc::Message fseqMessage;
		fseqMessage.setAddress("/tuio/2Dobj");
		fseqMessage.addStringArg("fseq");
		fseqMessage.addIntArg(-1);
		
		bundle.addMessage(fseqMessage);
		
		mSender.sendBundle(bundle);
	}
	
	void Sender::startObjectBundle(){
		mObjectBundle.clear();
		osc::Message aliveMessage;
		aliveMessage.setAddress("/tuio/2Dobj");
		aliveMessage.addStringArg("alive");
		for (std::list<Object*>::iterator it = mObjects.begin(); it != mObjects.end(); it++){
			aliveMessage.addIntArg((int32_t)((*it)->getSessionId()));
		}
		mObjectBundle.addMessage(aliveMessage);
	}
	
	void Sender::addObjectMessage(Object *object){
		osc::Message message;
		message.setAddress("/tuio/2Dobj");
		message.addStringArg("set");
		message.addIntArg((int32_t)(object->getSessionId()));
		message.addFloatArg(object->getX());
		message.addFloatArg(object->getY());
		message.addFloatArg(object->getAngle());
		message.addFloatArg(object->getXSpeed());
		message.addFloatArg(object->getYSpeed());
		message.addFloatArg(object->getRotationSpeed());
		message.addFloatArg(object->getMotionAccel());
		message.addFloatArg(object->getRotationAccel());
		mCursorBundle.addMessage(message);
	}
	
	void Sender::sendObjectBundle(long fseq){
		osc::Message message;
		message.setAddress("/tuio/2Dobj");
		message.addStringArg("set");
		message.addIntArg((int32_t)fseq);
		
		mObjectBundle.addMessage(message);
		
		mSender.sendBundle(mObjectBundle);
	}
	

	
} // namespace tuio
} // namespace cinder