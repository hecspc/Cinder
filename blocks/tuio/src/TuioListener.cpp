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
	
	
	void Listener::getMessage(){
		while (listener.hasWaitingMessages()) {
			osc::Message message;
			listener.getNextMessage(&message);
			
			if (message.getAddress() == "/tuio/2Dobj"){
				if (message.getArgAsString(0) == "set"){
					if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
					
					long sid  = (long)(message.getArgAsInt32(1));
					int fiducial = message.getArgAsInt32(2);
					float xpos =  message.getArgAsFloat(3);
					float ypos =  message.getArgAsFloat(4);
					float angle =  message.getArgAsFloat(5);
					float xspeed =  message.getArgAsFloat(6);
					float yspeed =  message.getArgAsFloat(7);
					float rspeed =  message.getArgAsFloat(8);
					float maccel =  message.getArgAsFloat(9);
					float raccel =  message.getArgAsFloat(10);
					
					std::list<Object*>::iterator it;
					for (it = mObjects.begin(); it != mObjects.end(); it++){
						if ((*it)->getSessionId() == sid) break;
					
					}
					
					if (it == mObjects.end()){
						Object *addedObject = new Object(sid, fiducial, xpos, ypos, angle);
						mObjects.push_back(addedObject);
						
						// NOTIFY ADDED OBJECT
						std::cout << "Object added " << addedObject->getSessionId() << std::endl;
						
					}else if ( ((*it)->getX()!=xpos) || ((*it)->getY()!=ypos) || 
						      ((*it)->getAngle()!=angle) || ((*it)->getXSpeed()!=xspeed) || 
						      ((*it)->getYSpeed()!=yspeed) || ((*it)->getRotationSpeed()!=rspeed) || 
						      ((*it)->getMotionAccel()!=maccel) || ((*it)->getRotationAccel()!=raccel)) {
						(*it)->update(xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel);
						//(*it)->update(Vec2f(xpos, ypos), angle, Vec2f(xspeed, yspeed), rspeed, maccel, raccel);
						// NOTIFY UPDATE OBJECT
						
						std::cout << "Object updated " << (*it)->getSessionId() << std::endl;
					}
				
				}else if (message.getArgAsString(0) == "alive"){
					if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
					for (int i = 1; i < message.getNumArgs(); i++){
						long sid = (long)(message.getArgAsInt32(i));
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
								// NOTIFY REMOVE OBJECT
								std::cout << "Object removed " << deleteObject->getSessionId() << std::endl;
								
								mObjects.erase(objectIt);
								
								delete deleteObject;
								break;
							}
						}
					}
					mAliveObjects = mNewObjects;
					mNewObjects.clear();
				
				}else if (message.getArgAsString(0) == "fseq"){
					if (mCurrentFrame > 0) mLastFrame = mCurrentFrame;
					mCurrentFrame = (int)(message.getArgAsInt32(1));
				}
			}else if (message.getAddress() == "/tuio/2Dcur") {
				
				if (message.getArgAsString(0) == "set"){
					if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
					
					long sid  = (long)(message.getArgAsInt32(1));
					float xpos =  message.getArgAsFloat(2);
					float ypos =  message.getArgAsFloat(3);
					float xspeed =  message.getArgAsFloat(4);
					float yspeed =  message.getArgAsFloat(5);
					float maccel =  message.getArgAsFloat(6);
					
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
						
						// NOTIFY CURSOR ADDED
						std::list< boost::function<void(Cursor*)> >::iterator callbackIt;
						for (callbackIt = mCursorAddedCallbacks.begin(); callbackIt != mCursorAddedCallbacks.end(); callbackIt++){
							(*callbackIt)(addCursor);
						}
						//std::cout << "Cursor added " << addCursor->getSessionId() << std::endl;
						
					} else if ( ((*it)->getX() != xpos) || ((*it)->getY() != ypos) || 
							    ((*it)->getXSpeed()!=xspeed) || ((*it)->getYSpeed()!=yspeed) || 
							    ((*it)->getMotionAccel()!=maccel)){
						
						(*it)->update(xpos, ypos, xspeed, yspeed, maccel);
						//(*it)->update(Vec2f(xpos, ypos), Vec2f(xspeed, yspeed), maccel);
						// NOTIFY CURSOR UPDATED
						
						std::list< boost::function<void(Cursor*)> >::iterator callbackIt;
						for (callbackIt = mCursorUpdatedCallbacks.begin(); callbackIt != mCursorUpdatedCallbacks.end(); callbackIt++){
							(*callbackIt)(*it);
						}
						
						//std::cout << "Cursor updated " << (*it)->getSessionId() << std::endl;
						
					}
				}else if (message.getArgAsString(0) == "alive"){
					
					if ((mCurrentFrame < mLastFrame) && (mCurrentFrame > 0)) return;
					
					for (int i = 1; i < message.getNumArgs(); i++){
						long sid = (long)(message.getArgAsInt32(i));
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
								
								// NOTIFY REMOVE CURSOR
								std::list< boost::function<void(Cursor*)> >::iterator callbackIt;
								for (callbackIt = mCursorRemovedCallbacks.begin(); callbackIt != mCursorRemovedCallbacks.end(); callbackIt++){
									(*callbackIt)(deleteCursor);
								}
								
								//std::cout << "Cursor removed " << deleteCursor->getSessionId() << std::endl;
								
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
				
				}else if (message.getArgAsString(0) == "fseq"){
					if (mCurrentFrame > 0) mLastFrame = mCurrentFrame;
					mCurrentFrame = (int)(message.getArgAsInt32(1));
				}

			}else {
				// NOTIFY OSC MESSAGE
				
				std::list< boost::function<void(osc::Message*)> >::iterator callbackIt;
				for (callbackIt = mOSCMessageCallbacks.begin(); callbackIt != mOSCMessageCallbacks.end(); callbackIt++){
					(*callbackIt)(&message);
				}
				
				//std::cout << "OSC Message " << message.getAddress() << std::endl;
			}

		}
	}
	
	void Listener::drawCursors(Vec2f windowSize){
		std::list<Cursor*>::iterator it;
		for (it = mCursors.begin(); it != mCursors.end(); it++) {
			Cursor *blob = (*it);
			blob->draw(windowSize);
			//gl::color(Color(1, 1, 1));
			//gl::drawSolidCircle(Vec2f(blob->getX() * windowSize.x, blob->getY() * windowSize.y), 10);
			//gl::disableAlphaBlending();
			// DRAW CURSORS
			/*glColor3f(1.0,1.0,1.0);
			ofEllipse(blob->getX()*ofGetWidth(), blob->getY()*ofGetHeight(), 10.0, 10.0);
			string str = "SessionId: "+ofToString((int)(blob->getSessionId()));
			ofDrawBitmapString(str, blob->getX()*ofGetWidth()-10.0, blob->getY()*ofGetHeight()+25.0);
			 */
		}
	}
	void Listener::drawObjects(Vec2f windowSize){
		std::list<Object*>::iterator it;
		for (it = mObjects.begin(); it != mObjects.end(); it++) {
			Object *blob = (*it);
			
			// DRAW OBJECTS
			/*
			glColor3f(1.0,0.0,0.0);
			glPushMatrix();
		    glTranslatef(blob->getX()*ofGetWidth(), blob->getY()*ofGetHeight(), 0.0);
			glRotatef(blob->getAngleDegrees(), 0.0, 0.0, 1.0);
			ofRect(-10.0, -10.0, 20.0, 20.0);
			glColor3f(1.0,1.0,1.0);
			ofLine(0, 0, 0, 10);
			glPopMatrix();
			string str = "FiducialId: "+ofToString((int)(blob->getFiducialId()));
			ofDrawBitmapString(str, blob->getX()*ofGetWidth()-10.0, blob->getY()*ofGetHeight()+25.0);
			str = "SessionId: "+ofToString((int)(blob->getSessionId()));
			ofDrawBitmapString(str, blob->getX()*ofGetWidth()-10.0, blob->getY()*ofGetHeight()+40.0);
			 */
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
