//
//  ofxKsmrAliveClient.cpp
//  necVideoTrigger
//
//  Created by Ovis aries on 2015/02/06.
//
//

#include "ofxKsmrAliveClient.h"

void ofxKsmrAliveClient::setup(int port){

	state = KSMR_CSTATE_HOLLOW;
	request_interval = 500;
	clientPort = port;
	receiver.setup(clientPort);
	stateLabel = "Healthy";
}

void ofxKsmrAliveClient::update(){
	while (receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		if (m.getAddress() == "/ksmr/alive/master/announce"){

			state = KSMR_CSTATE_FIND_MASTER;
			sender.setup(m.getRemoteIp(), m.getArgAsInt32(0));

		}

		if (m.getAddress() == "/ksmr/alive/master/catch"){

			state = KSMR_CSTATE_CATCHED;

		}

		if (m.getAddress() == "/ksmr/alive/master/call"){

			ofxOscMessage res;
			res.setAddress("/ksmr/alive/response");
			res.addStringArg(stateLabel);

			sender.sendMessage(res);

		}

	}

	//Send request
	if ((state == KSMR_CSTATE_FIND_MASTER) &&
		(ofGetElapsedTimeMillis() - last_req < request_interval)){
		ofxOscMessage req;
		req.setAddress("/ksmr/alive/request");
		req.addIntArg(clientPort);

		sender.sendMessage(req);
	}
}

void ofxKsmrAliveClient::draw(){

}