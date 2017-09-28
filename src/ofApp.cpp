#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    
    ofxKinectV2 tmp;
    vector<ofxKinectV2::KinectDeviceInfo> deviceList = tmp.getDeviceList();
    
    // gui1
    panel.setup("", "kinectList", 50, 50);
    
    kinect.resize(deviceList.size());
    colorTex.resize(deviceList.size());
    depthTex.resize(deviceList.size());
    
    string kinectId[2];
    kinectId[0] = "004978762247";
    kinectId[1] = "005990562247";
    
    for(int i = 0; i < deviceList.size(); i++){
        cout << i << endl;
        kinect[i] = shared_ptr<ofxKinectV2>(new ofxKinectV2());
        kinect[i]->open(kinectId[i]);
        kinect[i]->minDistance = 1500;
        kinect[i]->maxDistance = 3000;
        
        panel.add(kinect[i]->params);
    }
    
    cout << kinect.size() << endl;
    
    // gui
    gui.setup("", "kinect", 300, 50);
    gui.add(thresh.setup("Threshhold", 127, 0, 255));
    gui.add(minRadius.setup("Min Radius", 80, 0, 255));
    gui.add(maxRadius.setup("Max Radius", 130, 0, 255));
    gui.add(jointMarginLeft.setup("Joint Margin Left", 0, 0, 512 - 30));
    gui.add(jointMarginRight.setup("Joint Margin Right", 0, 0, 512 - 30));
    
    gui.add(cropPositionX.setup("Crop Position X", 0, 0, 512*2));
    gui.add(cropPositionY.setup("Crop Position Y", 0, 0, 424));
    gui.add(cropWidth.setup("Crop Width", 512*2, 50, 512*2));
    gui.add(cropHeight.setup("Crop Height", 424, 50, 424));
}

//--------------------------------------------------------------
void ofApp::update(){
    
    for(int i = 0; i < kinect.size(); i++){
        
        kinect[i]->update();
        
        if(kinect[i]->isFrameNew()){
            
//            colorTex[i].loadData(kinect[i]->getRgbPixels());
//            depthTex[i].loadData(kinect[i]->getDepthPixels());
            
        }else{
            // frame error
            return;
        }
    }
    
    
    // left kinect
    ofPixels pix = kinect[0]->getDepthPixels();
    pix.crop(0, 0, 512 - jointMarginLeft, 424);
    
    // right kinect
    ofPixels pix2 = kinect[1]->getDepthPixels();
    pix2.crop(jointMarginRight, 0, 512 - jointMarginRight, 424);
    
    
    int dataSize = (512 * 2 - jointMarginLeft - jointMarginRight) * 424;
    unsigned char* videoPixels = new unsigned char[dataSize];
    ofPixels* bindPix = new ofPixels();
    bindPix->setFromExternalPixels(videoPixels, 512 * 2 - jointMarginLeft - jointMarginRight, 424, 1);
    pix.pasteInto(*bindPix, 0, 0);
    pix2.pasteInto(*bindPix, 512 - jointMarginLeft, 0);
    
    trimDepth.clear();
    trimDepth.loadData(*bindPix);
    
    ofPixels pixels;
    trimDepth.readToPixels(pixels);
    
    
    // opencv analysis area
    if(pixels.getWidth() < cropPositionX + cropWidth){
        cropWidth = pixels.getWidth() - cropPositionX;
    }
    if(pixels.getHeight() < cropPositionY + cropHeight){
        cropHeight = pixels.getHeight() - cropPositionY;
    }
    pixels.crop(cropPositionX, cropPositionY, cropWidth, cropHeight);
//    cout << pixels.size() << endl;
    
    cv::Mat mat = cv::Mat(pixels.getHeight(), pixels.getWidth(), CV_8UC1, pixels.getData(), 0);
    
    //cv::Mat mat = cv::Mat(414, 512, CV_8UC1, pix.getData(), 0);
    
    contourFinder.setMinAreaRadius(minRadius);
    contourFinder.setMaxAreaRadius(maxRadius);
    contourFinder.setThreshold(thresh);
    contourFinder.findContours(mat);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    ofClear(255);
    
    panel.draw();
    gui.draw();
    
    for(int i = 0; i < kinect.size(); i++){
//        if (colorTex[i].isAllocated()) {
//            colorTex[i].draw(0, 0, 640, 360);
//        }
//        if (depthTex[i].isAllocated()) {
//            depthShader.begin();
//            depthTex[i].draw(512 * i, 360, 512, 424);
//            depthShader.end();
//        }
    }
    
    if (trimDepth.isAllocated()) {
        trimDepth.draw(0, 360, trimDepth.getWidth(), trimDepth.getHeight());
    }
    
    ofPushMatrix();
    ofTranslate(0 + cropPositionX, 360 + cropPositionY);
    ofSetHexColor(0x3399ff);
    ofNoFill();
    ofDrawRectangle(0, 0, cropWidth, cropHeight);
    
    ofSetHexColor(0xffff99);
    contourFinder.draw();
    
    
    for(int i = 0; i < contourFinder.size(); i++){
        if(i < OBJECT){
            ofPoint center = ofxCv::toOf(contourFinder.getCentroid(i));
            ofSetColor(255, 0, 0);
            ofSetLineWidth(4);
            ofDrawLine(center.x, 0, center.x, 424);
            float pos = ofMap(center.x, 0, cropWidth, 0, 1);
            cout << pos << endl;
        }
    }
    ofPopMatrix();
    
    ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 120, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
