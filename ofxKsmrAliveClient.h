//
//  ofxKsmrAliveClient.h
//  necVideoTrigger
//
//  Created by Ovis aries on 2015/02/06.
//
//

#ifndef __necVideoTrigger__ofxKsmrAliveClient__
#define __necVideoTrigger__ofxKsmrAliveClient__

#include "ofMain.h"
#include "ofxOsc.h"

enum ksmrAliveClientState{
	KSMR_CSTATE_HOLLOW,
	KSMR_CSTATE_FIND_MASTER,
	KSMR_CSTATE_CATCHED
};

class ofxKsmrAliveClient{
public:

	void setup(int port = 12555);
	void update();
	void draw();
	void setState(string state){stateLabel = state;};

	void onExit();
	
	ofxOscReceiver	receiver;
	ofxOscSender	sender;

	int clientPort;
	int request_interval;
	unsigned long long last_req;
	string stateLabel;
	string master_addr;
	string master_port;
	ksmrAliveClientState state;
};

#endif /* defined(__necVideoTrigger__ofxKsmrAliveClient__) */
