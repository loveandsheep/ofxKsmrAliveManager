//
//  ofxKsmrAliveClient.cpp
//  necVideoTrigger
//
//  Created by Ovis aries on 2015/02/06.
//
//

#include "ofxKsmrAliveClient.h"

void ofxKsmrAliveClient::setup(string name,int port){

	state = KSMR_CSTATE_HOLLOW;
	request_interval = 500;
	clientPort = port;
	receiver.setup(clientPort);
	stateLabel = "Healthy";
	master_addr = "";
	master_port = 0;
	clientName = name;
	
	ofAddListener(ofEvents().exit, this, &ofxKsmrAliveClient::onExit);
	
}

void ofxKsmrAliveClient::update(){
	
	unsigned long long cMillis = ofGetElapsedTimeMillis();
	
	while (receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		if (m.getAddress() == "/ksmr/alive/master/announce"){

			if (state == KSMR_CSTATE_HOLLOW){
				last_called = cMillis;
				state = KSMR_CSTATE_FIND_MASTER;
				master_addr = m.getRemoteIp();
				master_port = m.getArgAsInt32(0);
				sender.setup(master_addr, master_port);
			}

		}

		if (m.getAddress() == "/ksmr/alive/master/catch"){
			
			if (state == KSMR_CSTATE_FIND_MASTER){
				last_called = cMillis;
				state = KSMR_CSTATE_CATCHED;
			}

		}

		if (m.getAddress() == "/ksmr/alive/master/call"){

			last_called = cMillis;
			ofxOscMessage res;
			res.setAddress("/ksmr/alive/response");
			res.addStringArg(stateLabel);

			sender.sendMessage(res);

		}
		
		if (m.getAddress() == "/ksmr/alive/master/disconnect"){
			
			state = KSMR_CSTATE_HOLLOW;
			
		}

	}

	if (cMillis - last_called > 15000){
		state = KSMR_CSTATE_HOLLOW;
	}
	
	//Send request
	if ((state == KSMR_CSTATE_FIND_MASTER) &&
		(ofGetElapsedTimeMillis() - last_req > request_interval)){
		last_req = ofGetElapsedTimeMillis();
		ofxOscMessage req;
		req.setAddress("/ksmr/alive/request");
		req.addIntArg(clientPort);
		req.addStringArg(clientName);

		sender.sendMessage(req);
	}
}

void ofxKsmrAliveClient::draw(){

}

void ofxKsmrAliveClient::onExit(ofEventArgs &arg){
	if (master_addr != ""){
		ofxOscMessage req;
		req.setAddress("/ksmr/alive/disconnect");
		sender.sendMessage(req);
	}
}