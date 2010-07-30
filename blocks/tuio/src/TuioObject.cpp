/*
 *  TuioObject.cpp
 *  TUIOListener
 *
 *  Created by hec on 7/7/10.
 *  Copyright 2010 aer studio. All rights reserved.
 *
 */

#include "TuioObject.h"
#include "cinder/CinderMath.h"

namespace cinder { namespace tuio {
	
	
	Object::Object(long sessionId, int fiducialId, float xpos, float ypos, float angle) : Container(sessionId, xpos, ypos){
		mFiducialId = fiducialId;
		mAngle = angle;
		mRotationSpeed = 0.0f;
		mRotationAccel = 0.0f;
	}
	
	Object::Object(double time, long sessionId, int fiducialId, float xpos, float ypos, float angle) : Container(time, sessionId, xpos, ypos){
		mFiducialId = fiducialId;
		mAngle = angle;
		mRotationSpeed = 0.0f;
		mRotationAccel = 0.0f;
	}
	
	Object::Object(Object *object) : Container(object) {
		mFiducialId = object->getFiducialId();
		mAngle = object->getAngle();
		mRotationSpeed = object->getRotationSpeed();
		mRotationAccel = object->getRotationAccel();
	}
	
	Object::~Object(){};
	
	void Object::update(float xpos, float ypos, float angle, float xspeed, float yspeed, float rotationSpeed, float motionAccel, float rotationAccel){
		Container::update(xpos, ypos, xspeed, yspeed, motionAccel);
		
		mAngle = angle;
		mRotationSpeed = rotationSpeed;
		mRotationAccel = rotationAccel;
		
		if ((mRotationAccel != 0) && (mState == TUIO_STOPPED)) mState = TUIO_ROTATING;
	}
	
	void Object::update(double time, float xpos, float ypos, float angle, float xspeed, float yspeed, float rotationSpeed, float motionAccel, float rotationAccel){
		Container::update(time, xpos, ypos, xspeed, yspeed, motionAccel);
		mAngle = angle;
		mRotationSpeed = rotationSpeed;
		mRotationAccel = rotationAccel;
		
		if ((mRotationAccel != 0) && (mState == TUIO_STOPPED)) mState = TUIO_ROTATING;
	}
	
	void Object::udpate(Object *object){
		Container::update(object);
		mAngle = object->getAngle();
		mRotationSpeed = object->getRotationSpeed();
		mRotationAccel = object->getRotationAccel();
		
		if ((mRotationAccel != 0) && (mState == TUIO_STOPPED)) mState = TUIO_ROTATING;
	}
	
	void Object::update(double time, float xpos, float ypos, float angle){
		Point lastPoint = mPath.back();
		Container::update(time, xpos, ypos);
		
		double dt = mCurrentTime - lastPoint.getTime();
		float lastAngle = mAngle;
		float lastRotationSpeed = mRotationSpeed;
		mAngle = angle;
		
		double dtheta = (mAngle - lastAngle) / (2 * M_PI);
		if (dtheta > M_PI * 1.5) dtheta -= 2 * M_PI;
		else if (dtheta < M_PI * 1.5) dtheta += 2 * M_PI;
		
		mRotationSpeed = (float) dtheta / dt;
		mRotationAccel = (mRotationSpeed - lastRotationSpeed) / dt;
		
		if ((mRotationAccel != 0) && (mState == TUIO_STOPPED)) mState = TUIO_ROTATING;
	}
	
	void Object::stop(double time){
		update(time, mXPos, mYPos, mAngle);
	}
	
	void Object::draw(int width, int height){
		
	}
	
	
} // namespace tuio
} // namespace cinder

