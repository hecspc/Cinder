/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for MIDI Integration for Cinder framework developed by The Barbarian Group, 2010
 
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


#include "MidiIn.h"

namespace cinder { namespace midi {

	void MidiInCallback(double deltatime, std::vector<unsigned char> *message, void *userData){
		((Input*)userData)->processMessage(deltatime, message);
	}
	
	
	Input::Input(){
		mNumPorts = mMidiIn.getPortCount();
	}
	
	Input::~Input(){
		closePort();
	}
	
	void Input::listPorts(){
		std::cout << "MidiIn: " << mNumPorts << " available." << std::endl;
		for (size_t i = 0; i < mNumPorts; ++i){
			std::cout << i << ": " << mMidiIn.getPortName(i).c_str() << std::endl;
			mPortNames.push_back(mMidiIn.getPortName(i));
		}
	}
	
	void Input::openPort(unsigned int port){
		if (mNumPorts == 0){
			throw MidiExcNoPortsAvailable();
		}
		
		if (port + 1 > mNumPorts){
			throw MidiExcPortNotAvailable();
		}
		
		mPort = port;
		
		mMidiIn.openPort(mPort);
		
		mMidiIn.setCallback(&MidiInCallback, this);
		
		mMidiIn.ignoreTypes(false, false, false);
	}
	
	void Input::closePort(){
		mMidiIn.closePort();
	}
	
	void Input::processMessage(double deltatime, std::vector<unsigned char> *message){
		unsigned int numBytes = message->size();
		 
		if (numBytes > 0){
			Message* msg = new Message();
			msg->port = mPort;
			msg->channel = ((int)(message->at(0)) % 16) + 1;
			msg->status = ((int)message->at(0)) - (msg->channel - 1);
			msg->timeStamp = deltatime;
			
			if (numBytes == 2){
				msg->byteOne = (int)message->at(1);
			}else if (numBytes == 3){
				msg->byteOne = (int)message->at(1);
				msg->byteTwo = (int)message->at(2);
			}
			
			//mSignal(msg);
			
			mMessages.push_back(msg);
		}
		
	}
	
	bool Input::hasWaitingMessages(){
		int queue_length = (int)mMessages.size();
		return queue_length > 0;
	}
	
	bool Input::getNextMessage(Message* message){
		if (mMessages.size() == 0){
			return false;
		}
		
		Message* src_message = mMessages.front();
		message->copy(*src_message);
		delete src_message;
		mMessages.pop_front();
		
		return true;
	}
	
	unsigned int Input::getPort()const{
		return mPort;
	
	}
	

} // namespace midi
} // namespace cinder