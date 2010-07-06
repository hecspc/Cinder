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

#include "TuioListener.h"

#include <iostream>

#include "cinder/gl/gl.h"

namespace cinder { namespace tuio {
	
	Listener::Listener(){
		mCurrentFrame = mLastFrame = mMaxFingerID = -1;
	}
	
	void Listener::connect(int port){
		listener.setup(port);
		listener.addMessageCallback(boost::bind(&Listener::OSCMessageReceived, this, _1));
		mConnected = true;
	}
	
	void Listener::disconnect(){
		
		listener.shutdown();
		
		mAliveObjects.clear();
		mAliveCursors.clear();
		
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
	
	
	void Listener::OSCMessageReceived(osc::Message *message){
		
		
		
		if (message->getAddress() == "/tuio/2Dobj"){
			if (message->getArgAsString(0) == "set"){
				if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
					
				long sid  = (long)(message->getArgAsInt32(1));
				int fiducial = message->getArgAsInt32(2);
				float xpos =  message->getArgAsFloat(3);
				float ypos =  message->getArgAsFloat(4);
				float angle =  message->getArgAsFloat(5);
				float xspeed =  message->getArgAsFloat(6);
				float yspeed =  message->getArgAsFloat(7);
				float rspeed =  message->getArgAsFloat(8);
				float maccel =  message->getArgAsFloat(9);
				float raccel =  message->getArgAsFloat(10);
					
				std::list<Object*>::iterator it;
				for (it = mObjects.begin(); it != mObjects.end(); it++){
					if ((*it)->getSessionId() == sid) break;
				
				}
					
				if (it == mObjects.end()){
					Object *addedObject = new Object(sid, fiducial, xpos, ypos, angle);
					mObjects.push_back(addedObject);
					
					std::list< boost::function<void(Object*)> >::iterator callbackObjectIt;
					for (callbackObjectIt = mObjectAddedCallbacks.begin(); callbackObjectIt != mObjectAddedCallbacks.end(); callbackObjectIt++){
						(*callbackObjectIt)(addedObject);
					}
					
						
				}else if ( ((*it)->getX()!=xpos) || ((*it)->getY()!=ypos) || 
					      ((*it)->getAngle()!=angle) || ((*it)->getXSpeed()!=xspeed) || 
					      ((*it)->getYSpeed()!=yspeed) || ((*it)->getRotationSpeed()!=rspeed) || 
					      ((*it)->getMotionAccel()!=maccel) || ((*it)->getRotationAccel()!=raccel)) {
					(*it)->update(xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel);
						
					std::list< boost::function<void(Object*)> >::iterator callbackObjectIt;
					for (callbackObjectIt = mObjectUpdatedCallbacks.begin(); callbackObjectIt != mObjectUpdatedCallbacks.end(); callbackObjectIt++){
						(*callbackObjectIt)(*it);
					}
					
				}
				
			}else if (message->getArgAsString(0) == "alive"){
				if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
				for (int i = 1; i < message->getNumArgs(); i++){
					long sid = (long)(message->getArgAsInt32(i));
					mNewObjects.push_back((long)(sid));
					std::list<long>::iterator it;
					it = find(mAliveObjects.begin(), mAliveObjects.end(), (long)(sid));
					if (it != mAliveObjects.end()) mAliveObjects.erase(it);
				}
					
				std::list<long>::iterator aliveIt;
				for (aliveIt = mAliveObjects.begin(); aliveIt != mAliveObjects.end(); aliveIt++){
					std::list<Object*>::iterator objectIt;
					for (objectIt = mObjects.begin(); objectIt != mObjects.end(); objectIt++){
						Object *deleteObject = (*objectIt);
						if (deleteObject->getSessionId() == *aliveIt){
							
							std::list< boost::function<void(Object*)> >::iterator callbackObjectIt;
							for (callbackObjectIt = mObjectRemovedCallbacks.begin(); callbackObjectIt != mObjectRemovedCallbacks.end(); callbackObjectIt++){
								(*callbackObjectIt)(deleteObject);
							}
							
							mObjects.erase(objectIt);
								
							delete deleteObject;
							break;
						}
					}
				}
				mAliveObjects = mNewObjects;
				mNewObjects.clear();
				
			}else if (message->getArgAsString(0) == "fseq"){
				if (mCurrentFrame > 0) mLastFrame = mCurrentFrame;
				mCurrentFrame = (int)(message->getArgAsInt32(1));
			}
		}else if (message->getAddress() == "/tuio/2Dcur") {
			
			if (message->getArgAsString(0) == "set"){
				if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
				
				long sid  = (long)(message->getArgAsInt32(1));
				float xpos =  message->getArgAsFloat(2);
				float ypos =  message->getArgAsFloat(3);
				float xspeed =  message->getArgAsFloat(4);
				float yspeed =  message->getArgAsFloat(5);
				float maccel =  message->getArgAsFloat(6);
				
				std::list<Cursor*>::iterator it;
				for (it = mCursors.begin(); it != mCursors.end(); it++){
					if ((*it)->getSessionId() == sid) break;
						
				}
					
				if (it == mCursors.end()){
					int fid = (int)(mCursors.size());
					if ((int)(mCursors.size()) <= mMaxFingerID){
						std::list<Cursor*>::iterator closestCursor = mFreeCursors.begin();
						for (std::list<Cursor*>::iterator testCursor = mFreeCursors.begin(); testCursor != mFreeCursors.end(); testCursor++){
							if ((*testCursor)->getDistance(xpos, ypos) < (*closestCursor)->getDistance(xpos, ypos)) closestCursor = testCursor;
						}
						
						fid = (*closestCursor)->getFingerId();
						mFreeCursors.erase(closestCursor);
						delete *closestCursor;
					}else mMaxFingerID = fid;
					
					Cursor *addCursor = new Cursor(sid, fid, xpos, ypos);
					mCursors.push_back(addCursor);
						
						
					std::list< boost::function<void(Cursor*)> >::iterator callbackIt;
					for (callbackIt = mCursorAddedCallbacks.begin(); callbackIt != mCursorAddedCallbacks.end(); callbackIt++){
						(*callbackIt)(addCursor);
					}
						
						
				} else if ( ((*it)->getX() != xpos) || ((*it)->getY() != ypos) || 
						    ((*it)->getXSpeed()!=xspeed) || ((*it)->getYSpeed()!=yspeed) || 
						    ((*it)->getMotionAccel()!=maccel)){
						
					(*it)->update(xpos, ypos, xspeed, yspeed, maccel);
						
						
					std::list< boost::function<void(Cursor*)> >::iterator callbackIt;
					for (callbackIt = mCursorUpdatedCallbacks.begin(); callbackIt != mCursorUpdatedCallbacks.end(); callbackIt++){
						(*callbackIt)(*it);
					}
						
						
						
				}
			}else if (message->getArgAsString(0) == "alive"){
				
				if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
				
				for (int i = 1; i < message->getNumArgs(); i++){
					long sid = (long)(message->getArgAsInt32(i));
					mNewCursors.push_back((long)(sid));
					
					std::list<long>::iterator it;
					it = find(mAliveCursors.begin(), mAliveCursors.end(), (long)(sid));
					if (it != mAliveCursors.end()) mAliveCursors.erase(it);
				}
				
				std::list<long>::iterator aliveIt;
				for (aliveIt = mAliveCursors.begin(); aliveIt != mAliveCursors.end(); aliveIt++){
					std::list<Cursor*>::iterator cursorIt;
					for (cursorIt = mCursors.begin(); cursorIt != mCursors.end(); cursorIt++){
						Cursor *deleteCursor = (*cursorIt);
						if (deleteCursor->getSessionId() == *aliveIt){
							
								
							std::list< boost::function<void(Cursor*)> >::iterator callbackIt;
							for (callbackIt = mCursorRemovedCallbacks.begin(); callbackIt != mCursorRemovedCallbacks.end(); callbackIt++){
								(*callbackIt)(deleteCursor);
							}
																
							mCursors.erase(cursorIt);
								
							if (deleteCursor->getFingerId()== mMaxFingerID) {
								mMaxFingerID = -1;
								delete deleteCursor;
									
								if (mCursors.size() > 0) {
									std::list<Cursor*>::iterator clist;
									for (clist = mCursors.begin(); clist != mCursors.end(); clist++) {
										int fid = (*clist)->getFingerId();
										if (fid > mMaxFingerID) mMaxFingerID = fid;
									}
									
									std::list<Cursor*>::iterator flist;
									for (flist = mFreeCursors.begin(); flist != mFreeCursors.end(); flist++) {
										Cursor *freeCursor = (*flist);
										if (freeCursor->getFingerId() > mMaxFingerID) delete freeCursor;
										else mFreeCursorsBuffer.push_back(freeCursor);
									}
										
									mFreeCursors = mFreeCursorsBuffer;
									mFreeCursorsBuffer.clear();
								}
							} else if (deleteCursor->getFingerId() < mMaxFingerID) mFreeCursors.push_back(deleteCursor);
								
							break;
								
						}
					}
				}
				mAliveCursors = mNewCursors;
				mNewCursors.clear();
			
			}else if (message->getArgAsString(0) == "fseq"){
				if (mCurrentFrame > 0) mLastFrame = mCurrentFrame;
				mCurrentFrame = (int)(message->getArgAsInt32(1));
			}
		}else {
				
			std::list< boost::function<void(osc::Message*)> >::iterator callbackIt;
			for (callbackIt = mOSCMessageCallbacks.begin(); callbackIt != mOSCMessageCallbacks.end(); callbackIt++){
				(*callbackIt)(message);
			}
				

		}

	}

	
	void Listener::drawCursors(Vec2f windowSize){
		std::list<Cursor*>::iterator it;
		for (it = mCursors.begin(); it != mCursors.end(); it++) {
			Cursor *blob = (*it);
			blob->draw(windowSize);
			
		}
	}
	void Listener::drawObjects(Vec2f windowSize){
		std::list<Object*>::iterator it;
		for (it = mObjects.begin(); it != mObjects.end(); it++) {
			Object *blob = (*it);
			
		}
	}
	
	std::list<Object*> Listener::getTuioObjects(){
		return mObjects;
	}
	std::list<Cursor*> Listener::getTuioCursors(){
		return mCursors;
	}
	
	Object* Listener::getTuioObject(long sessionId){
		for (std::list<Object*>::iterator it = mObjects.begin(); it != mObjects.end(); it++) {
			if((*it)->getSessionId() == sessionId) {
				return (*it);
			}
		}	
		return NULL;
	}
	
	
	Cursor* Listener::getTuioCursor(long sessionId){
		for (std::list<Cursor*>::iterator it = mCursors.begin(); it != mCursors.end(); it++) {
			if((*it)->getSessionId() == sessionId) {
				return (*it);
			}
		}	
		return NULL;
	}
	
	void Listener::addCursorAddedCallback(boost::function<void(Cursor*)> callback){
		mCursorAddedCallbacks.push_back(callback);
	}
	
	void Listener::addCursorUpdatedCallback(boost::function<void(Cursor*)> callback){
		mCursorUpdatedCallbacks.push_back(callback);
	}
	
	void Listener::addCursorRemovedCallback(boost::function<void(Cursor*)> callback){
		mCursorRemovedCallbacks.push_back(callback);
	}
	
	void Listener::addObjectAddedCallback(boost::function<void(Object*)> callback){
		mObjectAddedCallbacks.push_back(callback);
	}
	
	void Listener::addObjectUpdatedCallback(boost::function<void(Object*)> callback){
		mObjectUpdatedCallbacks.push_back(callback);
	}
	
	void Listener::addObjectRemovedCallback(boost::function<void(Object*)> callback){
		mObjectRemovedCallbacks.push_back(callback);
	}
	
	void Listener::addOSCMessageCallback(boost::function<void(osc::Message*)> callback){
		mOSCMessageCallbacks.push_back(callback);
	}
	
} // namespace tuio
} // namesapce cinder
