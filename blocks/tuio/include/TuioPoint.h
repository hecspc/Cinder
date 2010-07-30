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
#include "cinder/app/AppBasic.h"

namespace cinder { namespace tuio{

	class Point {
	public:
		
		Point(float xpos, float ypos){
			app = app::AppBasic::get();
			mXPos = xpos;
			mYPos = ypos;
			mCurrentTime = app->getElapsedSeconds();
			mStartTime = mCurrentTime;
		}
		
		Point(Point *tuiopoint){
			mXPos = tuiopoint->getX();
			mYPos = tuiopoint->getY();
			mCurrentTime = app->getElapsedSeconds();
			mStartTime = mCurrentTime;
		}
		
		Point(double time, float xpos, float ypos){
			mXPos = xpos;
			mYPos = ypos;
			mCurrentTime = time;
			mStartTime = mCurrentTime;
		}
		
		~Point(){};
		
		void update(float xpos, float ypos){
			mXPos = xpos;
			mYPos = ypos;
		}
		
		void update(double time, float xpos, float ypos){
			mXPos = xpos;
			mYPos = ypos;
			mCurrentTime = time;
		}
		
		
		void update(Point *tuiopoint){
			mXPos = tuiopoint->getX();
			mYPos = tuiopoint->getY();
		}
		
		float getX(){
			return mXPos;
		}
		
		float getY(){
			return mYPos;
		}
		
		int getScreenX(int width = 0){
			int pos;
			if (width == 0){
				pos = (int)floor(mXPos * app->getWindowWidth() + .5f);
			}else {
				pos = (int)floor(mXPos * width + .5f);
			}
			return pos;
		}
		
		int getScreenY(int height = 0){
			int pos;
			if (height == 0){
				pos = (int)floor(mYPos * app->getWindowHeight() + .5f);
			}else {
				pos = (int)floor(mYPos * height + .5f);
			}
			return pos;
		}
		
		double getTime(){
			return mCurrentTime;
		}
		
		double getStartTime(){
			return mStartTime;
		}
		
		float getDistance(float x, float y){
			float dx = mXPos - x;
			float dy = mYPos - y;
			return sqrtf(dx * dx + dy * dy);
		}
		
		
		float getDistance(Point *tuiopoint){
			return getDistance(tuiopoint->getX(), tuiopoint->getY());
		}
		
		float getAngle(float xpos, float ypos, bool degrees = false){
			float dx = xpos - mXPos;
			float dy = ypos - mYPos;
			
			float angle = atan2(dx, dy);
			
			if (!degrees){
				return angle;
			}else {
				return toDegrees(angle);
			}
		}
		
		float getAngle(Point *tuiopoint, bool degrees = false){
			return getAngle(tuiopoint->getX(), tuiopoint->getY(), degrees);
		}
		
		
	protected:
		float mXPos, mYPos;
		double mStartTime, mCurrentTime;
		app::AppBasic* app;
	
	};
	
} // namespace tuio
} // namespace cinder