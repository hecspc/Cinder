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

#include "TuioCursor.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include <string>
#include <iostream>
#include <sstream>

namespace cinder { namespace tuio {

	Cursor::Cursor(long sessionId, int fingerId, float xpos, float ypos) : Container (sessionId, xpos, ypos){
		mFingerId = fingerId;
		mRadius = 3 * DEFAULT_RADIUS;
		createLabel();
	}
	
	Cursor::Cursor(double time, long sessionId, int fingerId, float xpos, float ypos) : Container(time, sessionId, xpos, ypos){
		mFingerId = fingerId;
		mRadius = 3 * DEFAULT_RADIUS;
		createLabel();
	}
	
	Cursor::Cursor(Cursor *cursor) : Container(cursor){
		mFingerId = cursor->getFingerId();
		mRadius = 3 * DEFAULT_RADIUS;
		createLabel();
	}
	
	void Cursor::createLabel(){
		TextLayout layout;
		
		layout.setFont(Font("ArialMT", 11));
		std::stringstream label;
		label << "Finger ID: " << getFingerId();
		layout.setColor( Color( 1.0f, 0.7f, 0.0f ) );
		layout.addLine(label.str());
		layout.setColor( Color( 0.5f, 0.2f, 0.0f ) );
		label.str("");
		label << "Session ID: " << getSessionId();
		layout.addLine(label.str());
		
		//mTexture = gl::Texture(layout.render(true));
	}
	
	void Cursor::draw(int width, int height){
		
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 0.6f));
		if (mRadius > DEFAULT_RADIUS){
			mRadius += (DEFAULT_RADIUS - mRadius) / 10.0f;
		}else{
			mRadius = DEFAULT_RADIUS;
		}
		gl::drawSolidCircle(Vec2f(getScreenX(width), getScreenY(height)), mRadius);

	}
	
} // namespace tuio
} // namespace cinder