//
//  ofxKsmrAliveManager.h
//  necVideoTrigger
//
//  Created by Ovis aries on 2015/02/06.
//
//

#ifndef __necVideoTrigger__ofxKsmrAliveManager__
#define __necVideoTrigger__ofxKsmrAliveManager__

#include "ofxOsc.h"
#include "ofMain.h"

class ksmrRemoteClient;
enum ksmrAliveState{
	KSMR_STATE_ALIVE,
	KSMR_STATE_CALLED,
	KSMR_STATE_NOT_RES,
	KSMR_STATE_DEAD
};

class ofxKsmrAliveManager{
public:

	void setup(string addr,
			   int deadTimeMillis = 1000,
			   int notResTimeMillis = 500,
			   int port = 12444);
	void update();
	void draw(int x,int y);
	void exit(ofEventArgs &arg);
	void mousePressed(ofMouseEventArgs &A);

	ofxOscReceiver	receiver;
	ofxOscSender	sender;

	vector<ksmrRemoteClient*> clients;

	bool				clientIsExist(string address);
	void				removeClient(string address);
	ksmrRemoteClient*	getClient(string address);
	
	void call(ksmrRemoteClient* client);
	
	unsigned long long lastCallMillis;

	string announceAddr;

	int master_port;
	int call_interval_millis;
	int not_res_time_millis;
	int dead_time_millis;
};

class ksmrRemoteClient{
public:

	ksmrRemoteClient(string _address,string _name,int _port = 12555){
		regMillis = 0;
		port = _port;
		address = _address;
		name = _name;
		state = KSMR_STATE_ALIVE;
		calledTime = 0;
		waitTime = 0;
		deadCounter = 0;
		regMillis = ofGetElapsedTimeMillis();
	}

	~ksmrRemoteClient(){

	}

	ksmrAliveState state;
	int		deadCounter;
	int		calledTime;
	int		waitTime;
	int		port;
	string	address;
	string	stateLabel;
	string	name;

	unsigned long long	regMillis;

};

#endif /* defined(__necVideoTrigger__ofxKsmrAliveManager__) */
