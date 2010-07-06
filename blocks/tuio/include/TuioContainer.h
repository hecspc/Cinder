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

#include "TuioPoint.h"

namespace cinder { namespace tuio {
	
	class Container : public Point {
	public:
		Container(long sessionId, float xpos, float ypos) : Point(xpos, ypos){
			mSessionId = sessionId;
			mXSpeed = 0.0f;
			mYSpeed = 0.0f;
			mMotionSpeed = 0.0f;
			mMotionAccel = 0.0f;
			mXPos = xpos;
			mYPos = ypos;
		}
		
		
		Container(Container *container) : Point(container){
			mSessionId = container->getSessionId();
			mXSpeed = container->getXSpeed();
			mYSpeed = container->getYSpeed();
			mMotionSpeed = container->getMotionSpeed();
			mMotionAccel = container->getMotionAccel();
			mXPos = container->getX();
			mYPos = container->getY();
		}
		
		virtual ~Container(){};
		
		virtual void update(float xpos, float ypos, float xspeed, float yspeed, float motionAccel){
			Point::update(xpos, ypos);
			mXSpeed = xspeed;
			mYSpeed = yspeed;
			mMotionSpeed = (float)sqrt(xspeed * xspeed + yspeed * yspeed);
			mMotionAccel = motionAccel;
		}
		
		virtual void update(Container *tuioContainer){
			Point::update(tuioContainer);
			mXSpeed = tuioContainer->getXSpeed();
			mYSpeed = tuioContainer->getYSpeed();
			mMotionSpeed = tuioContainer->getMotionSpeed();
			mMotionAccel = tuioContainer->getMotionAccel();
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
		
	protected:
		long mSessionId;
		float mXSpeed, mYSpeed;
		float mMotionSpeed, mMotionAccel;
	};


} // namespace tuio
} // namespace cinder