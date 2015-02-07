//
//  ofxKsmrAliveManager.cpp
//  necVideoTrigger
//
//  Created by Ovis aries on 2015/02/06.
//
//

#include "ofxKsmrAliveManager.h"

// /ksmr/alive/request i(portNumber), s(Label)
// /ksmr/alive/response s(stateLabel),

void ofxKsmrAliveManager::setup(string addr,int deadTimeMillis,int notResTimeMillis,int port){

	announceAddr = addr;
	dead_time_millis = deadTimeMillis;
	not_res_time_millis = notResTimeMillis;
	call_interval_millis = 500;
	master_port = port;
	receiver.setup(port);
	
	
}

void ofxKsmrAliveManager::update(){

	while (receiver.hasWaitingMessages()){

		ofxOscMessage msg;
		receiver.getNextMessage(&msg);

		if (msg.getAddress() == "/ksmr/alive/request"){

			if (!clientIsExist(msg.getRemoteIp())){

				ksmrRemoteClient* cl = new ksmrRemoteClient(msg.getRemoteIp(),
															msg.getArgAsString(1),
															msg.getArgAsInt32(0));
				clients.push_back(cl);

				sender.setup(cl->address, cl->port);
				ofxOscMessage ret;
				ret.setAddress("/ksmr/alive/master/catch");
				sender.sendMessage(ret);

			}else{
				
				ksmrRemoteClient* cl = getClient(msg.getRemoteIp());
				
				sender.setup(cl->address, cl->port);
				ofxOscMessage ret;
				ret.setAddress("/ksmr/alive/master/catch");
				sender.sendMessage(ret);
				
			}

		}

		if (msg.getAddress() == "/ksmr/alive/response"){

			ksmrRemoteClient* cl = getClient(msg.getRemoteIp());

			if (cl != NULL){

				cl->stateLabel = msg.getArgAsString(0);
				cl->state = KSMR_STATE_ALIVE;
				cl->waitTime = 0;

			}

		}

		if (msg.getAddress() == "/ksmr/alive/disconnect"){

			removeClient(msg.getRemoteIp());

		}

	}

	//Alive Check
	if (ofGetElapsedTimeMillis() - lastCallMillis > call_interval_millis){
		lastCallMillis = ofGetElapsedTimeMillis();

		//Announcement
		sender.setup(announceAddr, 12555);
		ofxOscMessage ann;
		ann.setAddress("/ksmr/alive/master/announce");
		ann.addIntArg(master_port);
		sender.sendMessage(ann);

		//Client check
		for (int i = 0;i < clients.size();i++){

			if (clients[i]->state == KSMR_STATE_ALIVE){
				
				call(clients[i]);
				clients[i]->state = KSMR_STATE_CALLED;

			}else if (clients[i]->state == KSMR_STATE_CALLED){

				clients[i]->waitTime = ofGetElapsedTimeMillis() - clients[i]->calledTime;

				if (clients[i]->waitTime > not_res_time_millis){
					clients[i]->state = KSMR_STATE_NOT_RES;
				}

			}else if (clients[i]->state == KSMR_STATE_NOT_RES){

				clients[i]->waitTime = ofGetElapsedTimeMillis() - clients[i]->calledTime;

				if (clients[i]->waitTime > dead_time_millis){
					clients[i]->state = KSMR_STATE_DEAD;
					clients[i]->deadCounter++;
				}

			}else if (clients[i]->state == KSMR_STATE_DEAD){

				call(clients[i]);
				
			}

		}

	}
}

void ofxKsmrAliveManager::draw(int x,int y){

	ofPushMatrix();
	ofTranslate(x, y);

	for (int i = 0;i < clients.size();i++){
		
		switch (clients[i]->state) {
			case KSMR_STATE_ALIVE:
				ofSetHexColor(0x7FD5AB);
				break;
			case KSMR_STATE_CALLED:
				ofSetHexColor(0x7FE4Ab);
				break;
			case KSMR_STATE_NOT_RES:
				ofSetHexColor(0xFF8C56);
				break;
			case KSMR_STATE_DEAD:
				ofSetHexColor(0xE60D44);
				break;
		}
		ofRect(0, i*120, 200, 120);
		
		ofSetColor(255);
		ofNoFill();
		ofRect(0, i*120, 200,120);
		ofFill();
		string info = "";
		info += clients[i]->address + "\n";
		
		if (clients[i]->state == KSMR_STATE_ALIVE)
			info += "State :Alive\n";
		if (clients[i]->state == KSMR_STATE_CALLED)
			info += "State :Called\n";
		if (clients[i]->state == KSMR_STATE_NOT_RES)
			info += "State :Not Respond\n";
		if (clients[i]->state == KSMR_STATE_DEAD)
			info += "State :Dead\n";
		
		info += "Name :" + clients[i]->name + "\n";
		info += "Dead count :" + ofToString(clients[i]->deadCounter) + "\n";
		info += "StateLabel :" + clients[i]->stateLabel + "\n";
		
		ofDrawBitmapString(info, 20,20);
	}

	ofPopMatrix();

}

bool ofxKsmrAliveManager::clientIsExist(string address){

	for (int i = 0;i < clients.size();i++){
		if (address == clients[i]->address) return true;
	}

	return false;
}

ksmrRemoteClient* ofxKsmrAliveManager::getClient(string address){

	for (int i = 0;i < clients.size();i++){
		if (address == clients[i]->address) return clients[i];
	}

	return NULL;
}

void ofxKsmrAliveManager::removeClient(string address){

	ksmrRemoteClient* cl = NULL;
	int idx = -1;

	for (int i = 0;i < clients.size();i++){
		if (address == clients[i]->address){
			cl = clients[i];
			idx = i;
		}
	}

	if (cl != NULL){
		clients.erase(clients.begin()+idx);
		delete cl;
	}

}

void ofxKsmrAliveManager::call(ksmrRemoteClient *client){
	sender.setup(client->address, client->port);
	ofxOscMessage msg;
	msg.setAddress("/ksmr/alive/master/call");
	sender.sendMessage(msg);
	client->calledTime = ofGetElapsedTimeMillis();
}