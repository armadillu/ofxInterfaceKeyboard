//
//  ofxInterfaceKeyboard.h
//  PortraitMachine
//
//  Created by Oriol Ferrer Mesià on 28/8/16.
//
//

#ifndef __ofxInterfaceKeyboard__
#define __ofxInterfaceKeyboard__

#include "ofMain.h"
#include "ofxInterface.h"
#include "ofxJSON.h"

using namespace ofxInterface;

class KeyboardButton: public Node{
public:
	enum Type{
		CHAR_KEY, //a,b,c,1,2, etc
		MODIFIER_KEY, //alt, cmd
		SPECIAL_KEY //backspace, esc, etc
	};
	Type keyType;
	map<string,string> chars; //map key is a modifier ("default", "alt"), element is keychar
	string modifier; //if i am a modifier key, this will have a value (ie "alt")
	string specialCommand; //special keys will have a value set here: (ie "backspace", "esc")

	string keycodeForState(const string & currentState){
		auto it = chars.find(currentState);
		if(it != chars.end()){
			return it->second;
		}
		ofLogWarning("KeyboardButton") << "key has no char for current state! " << currentState;
		return "";
	}
};



class ofxInterfaceKeyboard: public Node{

public:
	
	ofxInterfaceKeyboard();
	~ofxInterfaceKeyboard();
	
	void loadConfig(const string & path);

	void update(float dt);
	void draw();
	void reset();
	
	void drawDebug();	 // debug debugging stuff (will be called by renderDebug)

	string getContent(){return content;}
	void setContent(const string& str){content=str;}
	void clearContent(){content = "";}


	struct KeyboardEvent{
		KeyboardButton::Type type;
		string keyChar;
		string modifierID;
		string specialCommand;
	};

	ofEvent<KeyboardEvent> onKeyClicked; //triggers event for every click (all keys types!)
	ofEvent<KeyboardEvent> onKeyDown; //todo!
	ofEvent<KeyboardEvent> onKeyUp; //todo!

protected:

	void onCharKeyClick(TouchEvent & t);
	void onModifierKeyClick(TouchEvent & t);
	void onSpecialKeyClick(TouchEvent & t);

	void onCharKeyDown(TouchEvent & t);
	void onCharKeyUp(TouchEvent & t);

	void clearKeyboard();

	//state
	vector<string> knownStates;
	vector<string> currentStates; //if empty, state is "default"
	string stateStr; //a hash of the above, but will be "default" if no special states are on

	string hash(const vector<string> & states); //compress any list of states into a unique key
											//will return "default" if vector is empty

	void toggleState(const string & state); //when a user pressed a modifier key
	virtual void handleSpecialKey(const string & key);

	vector<KeyboardButton*> charButtons;
	vector<KeyboardButton*> modifierButtons;
	vector<KeyboardButton*> specialButtons;

	vector<KeyboardButton*> pressedChars;

	struct KeyboardTextureSet{
		ofTexture* depressed;
		ofTexture* pressed;
	};
	map<string, KeyboardTextureSet> kbTextures; //

	ofTexture offTex;
	ofTexture onTex;

	string content; //text field so far
};

#endif /* defined(__PortraitMachine__ofxInterfaceKeyboard__) */
