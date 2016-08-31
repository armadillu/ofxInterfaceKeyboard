//
//  ofxInterfaceKeyboard.cpp
//  PortraitMachine
//
//  Created by Oriol Ferrer Mesià on 28/8/16.
//
//

#include "ofxInterfaceKeyboard.h"

ofxInterfaceKeyboard::ofxInterfaceKeyboard(){
}

ofxInterfaceKeyboard::~ofxInterfaceKeyboard(){
	ofRemoveListener(eventTouchDown, this, &ofxInterfaceKeyboard::onTouchDown);
	ofRemoveListener(eventTouchUp, this, &ofxInterfaceKeyboard::onTouchUp);
	ofRemoveListener(eventTouchMove, this, &ofxInterfaceKeyboard::onTouchMove);
	ofRemoveListener(eventClick, this, &ofxInterfaceKeyboard::onClick);
}

void ofxInterfaceKeyboard::setup(float x, float y, float w, float h){
	ofAddListener(eventTouchDown, this, &ofxInterfaceKeyboard::onTouchDown);
	ofAddListener(eventTouchUp, this, &ofxInterfaceKeyboard::onTouchUp);
	ofAddListener(eventTouchMove, this, &ofxInterfaceKeyboard::onTouchMove);
	ofAddListener(eventClick, this, &ofxInterfaceKeyboard::onClick);
	setSize(w, h);
	setPosition(x, y, 0);
}


void ofxInterfaceKeyboard::clearKeyboard(){

	knownStates.clear();
	currentStates.clear();
	stateStr = "default";
	pressedChars.clear();
	
	for(auto b : charButtons){
		ofRemoveListener(b->eventClick, this, &ofxInterfaceKeyboard::onCharKeyClick);
		ofRemoveListener(b->eventTouchDown, this, &ofxInterfaceKeyboard::onCharKeyDown);
		ofRemoveListener(b->eventTouchUp, this, &ofxInterfaceKeyboard::onCharKeyUp);

		delete removeChild(b);
	}
	charButtons.clear();

	for(auto b : modifierButtons){
		ofRemoveListener(b->eventClick, this, &ofxInterfaceKeyboard::onModifierKeyClick);
		delete removeChild(b);
	}
	modifierButtons.clear();

	for(auto b : specialButtons){
		ofRemoveListener(b->eventClick, this, &ofxInterfaceKeyboard::onSpecialKeyClick);
		delete removeChild(b);
	}
	specialButtons.clear();
}

string ofxInterfaceKeyboard::hash(const vector<string> & states){
	if(states.size() == 0 ) return "default";
	if(states.size() == 1) return states.back();
	
	vector<string> s = states;
	std::sort(s.begin(), s.end());
	string hash;
	for(auto & ss : s){
		hash = hash + "_" + ss;
	}
	return hash;
}


void ofxInterfaceKeyboard::loadConfig(const string & path){

	ofxJSON json;
	bool ok = json.openLocal(path);

	if(ok){

		clearKeyboard();

		float scale = json["config"]["touchRegionScale"].asFloat();

		for( Json::ValueIterator itr = json["images"].begin() ; itr != json["images"].end() ; itr++ ) {

			//states
			vector<string> kbStates;
			int nstates = (*itr)["states"].size();
			for(int i = 0; i < nstates; i++){
				string state = (*itr)["states"][i].asString();
				kbStates.push_back(state);

				if(std::find(knownStates.begin(), knownStates.end(), state) == knownStates.end()){
					knownStates.push_back(state);
				}
			}

			string stateHash = hash(kbStates);
			KeyboardTextureSet texSet;
			string depressedImgPath = (*itr)["images"]["depressed"].asString();
			string pressedImgPath = (*itr)["images"]["pressed"].asString();
			texSet.depressed = new ofTexture();
			texSet.pressed = new ofTexture();
			ofLoadImage(*texSet.depressed, depressedImgPath);
			ofLoadImage(*texSet.pressed, pressedImgPath);
			kbTextures[stateHash] = texSet;
			setSize(texSet.depressed->getWidth(),texSet.depressed->getHeight());
		}


		for( Json::ValueIterator itr = json["buttons"].begin() ; itr != json["buttons"].end() ; itr++ ) {

			float x = (*itr)["rect"][0].asFloat() * scale;
			float y = (*itr)["rect"][1].asFloat() * scale;
			float w = (*itr)["rect"][2].asFloat() * scale;
			float h = (*itr)["rect"][3].asFloat() * scale;
			KeyboardButton * k = new KeyboardButton();
			k->setPosition(x,y);
			k->setSize(w,h);
			k->setPlane(1000);
			addChild(k);

			if((*itr)["chars"].isObject()){ //this is a "normal" key

				k->keyType = KeyboardButton::CHAR_KEY;
				//walk all keycodes depending on modifier keys
				for( auto itr2 = (*itr)["chars"].begin() ; itr2 != (*itr)["chars"].end() ; itr2++ ) {
					string keyChar = itr2.key().asString();
					int nStates = (*itr2).size();
					vector<string> keyStates;
					for(int i = 0; i < nStates; i++){
						keyStates.push_back((*itr2)[i].asString());
					}
					string stateHash = hash(keyStates);
					k->chars[stateHash] = keyChar; //add all per-state keycodes to the keybutton itself
					if (k->chars.find("default") != k->chars.end() ){
						k->setName(k->chars["default"]);
					}
				}

				ofAddListener(k->eventClick, this, &ofxInterfaceKeyboard::onCharKeyClick);
				ofAddListener(k->eventTouchDown, this, &ofxInterfaceKeyboard::onCharKeyDown);
				ofAddListener(k->eventTouchUp, this, &ofxInterfaceKeyboard::onCharKeyUp);
				charButtons.push_back(k);

			}else{ //this is a modifier key!
				k->keyType = KeyboardButton::MODIFIER_KEY;
				string modifier = (*itr)["modifier"].asString();
				k->modifier = modifier;
				k->setName(modifier);

				ofAddListener(k->eventClick, this, &ofxInterfaceKeyboard::onModifierKeyClick);
				modifierButtons.push_back(k);

			}
		}

		//parse special keys
		for( Json::ValueIterator itr = json["specialKeys"].begin() ; itr != json["specialKeys"].end() ; itr++ ) {

			string specialCommand = itr.key().asString();
			float x = (*itr)[0].asFloat() * scale;
			float y = (*itr)[1].asFloat() * scale;
			float w = (*itr)[2].asFloat() * scale;
			float h = (*itr)[3].asFloat() * scale;

			KeyboardButton * k = new KeyboardButton();
			k->setPosition(x,y);
			k->setSize(w,h);
			k->setName(specialCommand);
			k->keyType = KeyboardButton::SPECIAL_KEY;
			k->specialCommand = specialCommand;
			addChild(k);
			ofAddListener(k->eventClick, this, &ofxInterfaceKeyboard::onSpecialKeyClick);
			specialButtons.push_back(k);
		}
		ofLogNotice("ofxInterfaceKeyboard") << "loaded JSON config! " << path;

	}else{
		ofLogError("ofxInterfaceKeyboard") << "Can't load JSON! " << path;
	}
}

void ofxInterfaceKeyboard::toggleState(const string & state){
	if (std::find(knownStates.begin(), knownStates.end(), state) != knownStates.end()){
		auto it = std::find(currentStates.begin(), currentStates.end(), state);
		if ( it != currentStates.end()){ //state was ON
			currentStates.erase(it);
		}else{
			currentStates.push_back(state);
		}
		stateStr = hash(currentStates);
	}else{
		ofLogError("ofxInterfaceKeyboard") << "Cant toggle state!! unknown state! : " << state;
	}
}

void ofxInterfaceKeyboard::handleSpecialKey(const string & key){

	if(key == "backspace"){
		if(content.size()){
			content = content.substr(0, content.size()-1);
		}
	}

	if(key == "esc"){
	}
}

void ofxInterfaceKeyboard::onSpecialKeyClick(TouchEvent & t){
	KeyboardButton * b = (KeyboardButton *)t.receiver;
	ofLogNotice("ofxInterfaceKeyboard") << "user pressed special key '" << b->specialCommand << "'";
	handleSpecialKey(b->specialCommand);
}

void ofxInterfaceKeyboard::onModifierKeyClick(TouchEvent & t){

	KeyboardButton * b = (KeyboardButton *)t.receiver;
	ofLogNotice("ofxInterfaceKeyboard") << "user pressed modifier key '" << b->modifier << "'";
	toggleState(b->modifier);
}

void ofxInterfaceKeyboard::onCharKeyClick(TouchEvent & t){

	KeyboardButton * b = (KeyboardButton *)t.receiver;
	string keyCode = b->keycodeForState(stateStr);
	ofLogNotice("ofxInterfaceKeyboard") << "user pressed char key '" << keyCode << "'";
	content += keyCode;
}

void ofxInterfaceKeyboard::onCharKeyDown(TouchEvent & t){
	pressedChars.push_back((KeyboardButton *)t.receiver);
}

void ofxInterfaceKeyboard::onCharKeyUp(TouchEvent & t){

	auto it = std::find(pressedChars.begin(), pressedChars.end(), (KeyboardButton *)t.receiver);
	if(it != pressedChars.end()){
		pressedChars.erase(it);
	}else{
		ofLogError() << "char key up that was not down???" << t.receiver;
	}
}


void ofxInterfaceKeyboard::update(float dt){}

void ofxInterfaceKeyboard::draw(){
	ofSetColor(255);
	auto it = kbTextures.find(stateStr);
	if(it != kbTextures.end()){
		it->second.depressed->draw(0,0, getWidth(), getHeight());
	}

	if(stateStr != "default"){ //draw currently pressed modifier keys
		for(auto b : modifierButtons){
			string modif = b->modifier;
			auto it = std::find(currentStates.begin(), currentStates.end(), modif);
			if(it != currentStates.end()){
				ofVec2f pos = b->getPosition();
				ofVec2f size = b->getSize();
				kbTextures[b->modifier].pressed->drawSubsection(pos.x, pos.y, size.x, size.y, pos.x, pos.y, size.x, size.y);
			}
		}
	}

	ofTexture * kbOnTex = nullptr;
	it = kbTextures.find(stateStr);
	if(it != kbTextures.end()){
		kbOnTex = it->second.pressed;
	}

	if(kbOnTex){
		for(auto b: pressedChars){
			ofVec2f pos = b->getPosition();
			ofVec2f size = b->getSize();
			kbOnTex->drawSubsection(pos.x, pos.y, size.x, size.y, pos.x, pos.y, size.x, size.y);
		}
	}
}

void ofxInterfaceKeyboard::drawDebug(){
	Node::drawDebug();
	ofDrawBitmapStringHighlight("Content: " + content, 2, -18);
}

void ofxInterfaceKeyboard::drawBounds(){
	Node::drawBounds();
}

bool ofxInterfaceKeyboard::contains(const ofVec3f& globalPoint){
	return Node::contains(globalPoint);
}

void ofxInterfaceKeyboard::onClick(TouchEvent&){
}

void ofxInterfaceKeyboard::onTouchDown(TouchEvent& event){
}

void ofxInterfaceKeyboard::onTouchMove(TouchEvent& event){
}

void ofxInterfaceKeyboard::onTouchUp(TouchEvent& event){
}

