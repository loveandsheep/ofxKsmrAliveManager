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

	ofxKsmrAliveClient(){
		
	}
	
	~ofxKsmrAliveClient(){
		
	}
	
	void setup(string name,int port = 12555);
	void update();
	void draw();
	void setState(string state){stateLabel = state;};

	void onExit(ofEventArgs &arg);
	
	ofxOscReceiver	receiver;
	ofxOscSender	sender;

	int clientPort;
	int request_interval;
	unsigned long long last_called;
	unsigned long long last_req;
	string clientName;
	string stateLabel;
	string master_addr;
	int master_port;
	ksmrAliveClientState state;
};

#endif /* defined(__necVideoTrigger__ofxKsmrAliveClient__) */
