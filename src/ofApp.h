#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxMultiKinectV2.h"
#include "ofxCv.h"


#define OBJECT 1

class ofApp : public ofBaseApp{

    vector <shared_ptr<ofxMultiKinectV2>> kinect;
//    vector <ofTexture> colorTex;
//    vector <ofTexture> depthTex;
    vector <shared_ptr<ofPixels>> mapPix;
    ofPixels depthPix;
    
    ofxCv::ContourFinder contourFinder;
//    ofShader depthShader;
    ofTexture trimDepth;
    
    
    ofxPanel kinectDistanceGui;
    ofxIntSlider minDistance;
    ofxIntSlider maxDistance;
    
    // GUI
    ofxPanel gui;
    ofxFloatSlider thresh;
    ofxFloatSlider minRadius;
    ofxFloatSlider maxRadius;
    ofxIntSlider trackArea;
    ofxIntSlider jointMarginLeft;
    ofxIntSlider jointMarginRight;
    ofxIntSlider cropPositionX;
    ofxIntSlider cropPositionY;
    ofxIntSlider cropWidth;
    ofxIntSlider cropHeight;
    
    
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
};
