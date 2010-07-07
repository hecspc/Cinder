/*
 *  TuioObject.cpp
 *  TUIOListener
 *
 *  Created by hec on 7/7/10.
 *  Copyright 2010 aer studio. All rights reserved.
 *
 */

#include "TuioObject.h"

namespace cinder { namespace tuio {
	
	
	Object::Object(long sessionId, int fiducialId, float xpos, float ypos, float angle) : Container(sessionId, xpos, ypos){
		mFiducialId = fiducialId;
		mAngle = angle;
		mRotationSpeed = 0.0f;
		mRotationAccel = 0.0f;
		mXPos = xpos;
		mYPos = ypos;
	}
	
	Object::Object(Object *object) : Container(object) {
		mFiducialId = object->getFiducialId();
		mAngle = object->getAngle();
		mRotationSpeed = object->getRotationSpeed();
		mRotationAccel = object->getRotationAccel();
		mXPos = object->getX();
		mYPos = object->getY();
	}
	
	Object::~Object(){};
	
	void Object::update(float xpos, float ypos, float angle, float xspeed, float yspeed, float rotationSpeed, float motionAccel, float rotationAccel){
		Container::update(xpos, ypos, xspeed, yspeed, motionAccel);
		
		mAngle = angle;
		mRotationSpeed = rotationSpeed;
		mRotationAccel = rotationAccel;
	}
	
	void Object::udpate(Object *object){
		Container::update(object);
		mAngle = object->getAngle();
		mRotationSpeed = object->getRotationSpeed();
		mRotationAccel = object->getRotationAccel();
	}
	
	void Object::draw(Vec2f windowSize){
		
	}
	
	
} // namespace tuio
} // namespace cinder

