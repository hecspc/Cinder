#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;
using namespace std;


#include "TuioSender.h"

class TUIOSenderApp : public AppBasic {
 public:
	void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();
	
	tuio::Sender tuio;
	
};

void TUIOSenderApp::prepareSettings(Settings *settings){
	settings->setWindowSize(640, 480);
	settings->setFrameRate(60.0f);
}

void TUIOSenderApp::setup(){

	tuio.connect("localhost", 3333);
}

void TUIOSenderApp::update(){
	tuio.initFrame();
	
	tuio.commitFrame();
	
}

void TUIOSenderApp::draw(){
	gl::enableDepthRead();
	gl::enableDepthWrite();	
	gl::enableAlphaBlending();
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	
	gl::setMatricesWindow( getWindowSize() );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( TUIOSenderApp, RendererGl )
