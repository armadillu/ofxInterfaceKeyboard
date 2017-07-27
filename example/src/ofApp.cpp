#include "ofApp.h"


using namespace ofxInterface;


//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofBackground(64);

	/******
	 * setup the scene with width and height
	 */
	scene = new Node();
	scene->setSize(ofGetWidth(), ofGetHeight());
	scene->setName("Scene");

	/******
	 * this is the touch manager, give him the scene to set it up
	 */
	TouchManager::one().setup(scene);

	keyboard.loadConfig("keyboard/keyboard.json");
	keyboard.setScale(1.0);
	keyboard.setPosition(50,60);

	// add it to the scene
	scene->addChild(&keyboard);


	keyboard2.loadConfig("keyboard_with_docs/keyboard.json");
	keyboard2.setScale(0.82);
	keyboard2.setPosition(50,460);

	// add it to the scene
	scene->addChild(&keyboard2);


}


//--------------------------------------------------------------
void ofApp::update(){

	TouchManager::one().update();
	scene->updateSubtree(1./60.);
}

//--------------------------------------------------------------
void ofApp::draw(){
	scene->render();

	if (bShowDebug) {
		scene->renderDebug();
	}

	ofSetColor(255);
	ofDrawBitmapString("hit 'd' to toggle debug rendering", 5, ofGetHeight()-8);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
		case 'd':
			bShowDebug = !bShowDebug;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

	/******
	 * pass touch/mouse events to the touch manager
	 */
	TouchManager::one().touchMove(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	/******
	 * pass touch/mouse events to the touch manager
	 */
	TouchManager::one().touchDown(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

	/******
	 * pass touch/mouse events to the touch manager
	 */
	TouchManager::one().touchUp(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
