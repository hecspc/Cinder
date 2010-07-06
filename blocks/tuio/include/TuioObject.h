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
#include "cinder/CinderMath.h"

#include "TuioContainer.h"

namespace cinder { namespace tuio {
	
	class Object : public Container {
	public:
		
		Object(long sessionId, int fiducialId, float xpos, float ypos, float angle) : Container(sessionId, xpos, ypos){
			mFiducialId = fiducialId;
			mAngle = angle;
			mRotationSpeed = 0.0f;
			mRotationAccel = 0.0f;
			mXPos = xpos;
			mYPos = ypos;
		}
		
		
		Object(Object *object) : Container(object) {
			mFiducialId = object->getFiducialId();
			mAngle = object->getAngle();
			mRotationSpeed = object->getRotationSpeed();
			mRotationAccel = object->getRotationAccel();
			mXPos = object->getX();
			mYPos = object->getY();
		}
		
		~Object(){};
		
		void update(float xpos, float ypos, float angle, float xspeed, float yspeed, float rotationSpeed, float motionAccel, float rotationAccel){
			Container::update(xpos, ypos, xspeed, yspeed, motionAccel);
			
			mAngle = angle;
			mRotationSpeed = rotationSpeed;
			mRotationAccel = rotationAccel;
		}
		
		void udpate(Object *object){
			Container::update(object);
			mAngle = object->getAngle();
			mRotationSpeed = object->getRotationSpeed();
			mRotationAccel = object->getRotationAccel();
		}
		
		int getFiducialId() { 
			return mFiducialId; 
		};
		
		float getAngle(bool degrees = false) { 
			if (!degrees){
				return mAngle;
			}else {
				return toDegrees(mAngle);
			}

		};
		
		float getRotationSpeed() { 
			return mRotationSpeed; 
		};
		
		float getRotationAccel() { 
			return mRotationAccel; 
		};
		
	protected:
		
		int mFiducialId;
		float mAngle;
		float mRotationSpeed, mRotationAccel;
	
	};

} // namespace tuio
} // namespace cinder