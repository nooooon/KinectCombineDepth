#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    
    ofxMultiKinectV2 tmp;
    int count = ofxMultiKinectV2::getDeviceCount();
    
    // gui1
    kinectDistanceGui.setup("", "kinectList", 50, 50);
    kinectDistanceGui.add(minDistance.setup("minDistance", 1500, 0, 12000));
    kinectDistanceGui.add(maxDistance.setup("maxDistance", 4000, 0, 12000));
    
    kinect.resize(count);
//    colorTex.resize(count);
//    depthTex.resize(count);
    mapPix.resize(count);
    
//    string kinectId[2];
//    kinectId[0] = "004978762247";
//    kinectId[1] = "005990562247";
    
    for(int i = 0; i < count; i++){
        
        kinect[i] = shared_ptr<ofxMultiKinectV2>(new ofxMultiKinectV2());
        kinect[i]->open(false, true, i);
        
        kinect[i]->start();
    }
    
    
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
    
    if(!kinect.size()){
        return;
    }
    
    for(int i = 0; i < kinect.size(); i++){
        
        kinect[i]->update();
        
        if(kinect[i]->isFrameNew()){
            
            ofFloatPixels rawDepthPixels = kinect[i]->getDepthPixelsRef();
            
            if(0 < rawDepthPixels.size()){
                
                if(depthPix.getWidth() != rawDepthPixels.getWidth()){
                    depthPix.allocate(rawDepthPixels.getWidth(), rawDepthPixels.getHeight(), 1);
                }
                
                unsigned char * pixels = depthPix.getData();
                
                for(int i = 0; i < depthPix.size(); i++){
                    pixels[i] = ofMap(rawDepthPixels[i], minDistance, maxDistance, 255, 0, true);
                    if( pixels[i] == 255 ){
                        pixels[i] = 0;
                    }
                }
                mapPix[i] = shared_ptr<ofPixels>(new ofPixels());
                mapPix[i]->setFromPixels(pixels, rawDepthPixels.getWidth(), rawDepthPixels.getHeight(), 1);
                //                depthTex[i].loadData(*mapPix[i]);
            }
            
        }else{
            // frame error
            return;
        }
    }
    
    
    // left kinect
    ofPixels pix = *mapPix[0];
    pix.crop(0, 0, 512 - jointMarginLeft, 424);
    
    // right kinect
    ofPixels pix2 = *mapPix[1];
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
    
    cv::Mat mat = cv::Mat(pixels.getHeight(), pixels.getWidth(), CV_8UC1, pixels.getData(), 0);
    
    
    contourFinder.setMinAreaRadius(minRadius);
    contourFinder.setMaxAreaRadius(maxRadius);
    contourFinder.setThreshold(thresh);
    contourFinder.findContours(mat);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    ofClear(255);
    
    kinectDistanceGui.draw();
    gui.draw();
    
//    for(int i = 0; i < kinect.size(); i++){
//        if (colorTex[i].isAllocated()) {
//            colorTex[i].draw(0, 0, 640, 360);
//        }
//        if (depthTex[i].isAllocated()) {
//            depthShader.begin();
//            depthTex[i].draw(512 * i, 360, 512, 424);
//            depthShader.end();
//        }
//    }
    
    if (trimDepth.isAllocated()) {
        trimDepth.draw(0, 360, trimDepth.getWidth(), trimDepth.getHeight());
    }
    
    ofPushMatrix();
    ofTranslate(0 + cropPositionX, 360 + cropPositionY);
    ofSetHexColor(0x3399ff);
    ofNoFill();
    ofDrawRectangle(0, 0, cropWidth, cropHeight);
    
    ofSetHexColor(0xffff99);
    ofFill();
    contourFinder.draw();
    
    
    for(int i = 0; i < contourFinder.size(); i++){
        if(i < OBJECT){
            ofPoint center = ofxCv::toOf(contourFinder.getCentroid(i));
            ofSetColor(255, 0, 0);
            ofSetLineWidth(4);
            ofDrawLine(center.x, 0, center.x, 424);
            float pos = ofMap(center.x, 0, cropWidth, 0, 1);
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
