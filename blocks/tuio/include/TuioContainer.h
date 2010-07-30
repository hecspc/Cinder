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

#include <list>

#include "TuioPoint.h"

#define TUIO_ADDED 0
#define TUIO_ACCELERATING 1
#define TUIO_DECELERATING 2
#define TUIO_STOPPED 3
#define TUIO_REMOVED 4

namespace cinder { namespace tuio {
	
	class Container : public Point {
	public:
		Container(long sessionId, float xpos, float ypos) : Point(xpos, ypos){
			mSessionId = sessionId;
			mXSpeed = 0.0f;
			mYSpeed = 0.0f;
			mMotionSpeed = 0.0f;
			mMotionAccel = 0.0f;
			Point point(mCurrentTime, mXPos, mYPos);
			mPath.push_back(point);
			
			
			mState = TUIO_ADDED;
		}
		
		
		Container(Container *container) : Point(container){
			mSessionId = container->getSessionId();
			mXSpeed = container->getXSpeed();
			mYSpeed = container->getYSpeed();
			mMotionSpeed = container->getMotionSpeed();
			mMotionAccel = container->getMotionAccel();
			Point point(mCurrentTime, mXPos, mYPos);
			mPath.push_back(point);
			
			mState = TUIO_ADDED;
		}
		
		Container(double time, long sessionId, float xpos, float ypos) : Point(time, xpos, ypos){
			mSessionId = sessionId;
			mXSpeed = 0.0f;
			mYSpeed = 0.0f;
			mMotionSpeed = 0.0f;
			mMotionAccel = 0.0f;
			Point point(mCurrentTime, mXPos, mYPos);
			mPath.push_back(point);
			
			mState = TUIO_ADDED;
		}
		
		virtual ~Container(){};
		
		virtual void update(float xpos, float ypos, float xspeed, float yspeed, float motionAccel){
			Point::update(xpos, ypos);
			mXSpeed = xspeed;
			mYSpeed = yspeed;
			mMotionSpeed = (float)sqrt(xspeed * xspeed + yspeed * yspeed);
			mMotionAccel = motionAccel;
			
			mPath.pop_back();
			Point point(mCurrentTime, mXPos, mYPos);
			mPath.push_back(point);
			
			if (mMotionAccel > 0) mState = TUIO_ACCELERATING;
			else if (mMotionAccel < 0) mState = TUIO_DECELERATING;
			else mState = TUIO_STOPPED;
		}
		
		virtual void update(double time, float xpos, float ypos){
			Point lastPoint = mPath.back();
			Point::update(time, xpos, ypos);
			
			double dt = mCurrentTime - lastPoint.getTime();
			float dx = mXPos - lastPoint.getX();
			float dy = mYPos - lastPoint.getY();
			
			float dist = sqrt(dx * dx + dy * dy);
			float lastMotionSpeed = mMotionSpeed;
			
			mXSpeed = dx / dt;
			mYSpeed = dy / dt;
			mMotionSpeed = dist / dt;
			mMotionAccel = (mMotionSpeed - lastMotionSpeed ) / dt;
			
			Point point(mCurrentTime, mXPos, mYPos);
			mPath.push_back(point);
			
			if (mMotionAccel > 0) mState = TUIO_ACCELERATING;
			else if (mMotionAccel < 0) mState = TUIO_DECELERATING;
			else mState = TUIO_STOPPED;
			
		}
		
		virtual void stop(double time){
			update(time, mXPos, mYPos);
		}
		
		virtual void update(double time, double xpos, double ypos, double xspeed, double yspeed, double motionAccel){
			Point::update(time, xpos, ypos);
			
			mXSpeed = xspeed;
			mYSpeed = yspeed;
			mMotionSpeed = (float)sqrt(xspeed * xspeed + yspeed * yspeed);
			mMotionAccel = motionAccel;
			
			Point point(time, xpos, ypos);
			mPath.push_back(point);
			
			if (mMotionAccel > 0) mState = TUIO_ACCELERATING;
			else if (mMotionAccel < 0) mState = TUIO_DECELERATING;
			else mState = TUIO_STOPPED;
		}
		
		virtual void update(Container *tuioContainer){
			Point::update(tuioContainer);
			mXSpeed = tuioContainer->getXSpeed();
			mYSpeed = tuioContainer->getYSpeed();
			mMotionSpeed = tuioContainer->getMotionSpeed();
			mMotionAccel = tuioContainer->getMotionAccel();
			
			Point point(tuioContainer->getTime(), mXPos, mYPos);
			mPath.push_back(point);
			
			if (mMotionAccel > 0) mState = TUIO_ACCELERATING;
			else if (mMotionAccel < 0) mState = TUIO_DECELERATING;
			else mState = TUIO_STOPPED;
		}
		
		virtual void remove(double time) {
			mCurrentTime = time;
			mState = TUIO_REMOVED;
		}
		
		virtual long getSessionId()const{
			return mSessionId;
		}
		
		virtual Point getPosition(){
			Point point(mXPos, mYPos);
			return point;
		}
		
		virtual float getXSpeed(){
			return mXSpeed;
		}
		
		virtual float getYSpeed(){
			return mYSpeed;
		}
		
		virtual float getMotionSpeed(){
			return mMotionSpeed;
		}
		
		virtual float getMotionAccel(){
			return mMotionAccel;
		}
		
		virtual std::list<Point> getPath(){
			return mPath;
		}
		
		virtual int getState(){
			return mState;
		}
		
		virtual bool isMoving(){
			if ((mState == TUIO_ACCELERATING) || (mState == TUIO_DECELERATING)) return true;
			else return false;
		}
		
	protected:
		long mSessionId;
		float mXSpeed, mYSpeed;
		float mMotionSpeed, mMotionAccel;
		std::list<Point> mPath;
		
		int mState;
	};


} // namespace tuio
} // namespace cinder