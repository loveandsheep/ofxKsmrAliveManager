//
//  ofxKsmrAliveManager.cpp
//  necVideoTrigger
//
//  Created by Ovis aries on 2015/02/06.
//
//

#include "ofxKsmrAliveManager.h"

// /ksmr/alive/request i(portNumber), s(Label)
// /ksmr/alive/response s(stateLabel)

void ofxKsmrAliveManager::setup(int deadTimeMillis,int notResTimeMillis,int port){

	dead_time_millis = deadTimeMillis;
	not_res_time_millis = notResTimeMillis;
	call_interval_millis = 500;
	receiver.setup(port);

}

void ofxKsmrAliveManager::update(){

	while (receiver.hasWaitingMessages()){

		ofxOscMessage msg;
		receiver.getNextMessage(&msg);

		if (msg.getAddress() == "/ksmr/alive/request"){

			if (!clientIsExist(msg.getRemoteIp())){

				ksmrRemoteClient* cl = new ksmrRemoteClient(msg.getRemoteIp(),
															msg.getArgAsInt32(0));
				clients.push_back(cl);

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

		for (int i = 0;i < clients.size();i++){

			if (clients[i]->state == KSMR_STATE_ALIVE){

				sender.setup(clients[i]->address, clients[i]->port);
				ofxOscMessage msg;
				msg.setAddress("/ksmr/alive/master/call");
				sender.sendMessage(msg);
				clients[i]->state = KSMR_STATE_CALLED;
				clients[i]->calledTime = ofGetElapsedTimeMillis();

			}else if (clients[i]->state == KSMR_STATE_CALLED){

				clients[i]->waitTime = ofGetElapsedTimeMillis() - clients[i]->calledTime;

				if (clients[i]->waitTime > not_res_time_millis){
					clients[i]->state == KSMR_STATE_NOT_RES;
				}

			}else if (clients[i]->state == KSMR_STATE_NOT_RES){

				clients[i]->waitTime = ofGetElapsedTimeMillis() - clients[i]->calledTime;

				if (clients[i]->waitTime > dead_time_millis){
					clients[i]->state == KSMR_STATE_DEAD;
				}

			}

		}

	}
}

void ofxKsmrAliveManager::draw(){

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