#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sampleRate = 44100;
    bufferSize = 512;
    
    // settings for ofxMaxim.
    maxiSettings::setup(sampleRate, 2, bufferSize);
    
    ofSoundStreamSettings settings;
    
    //Not getting your microhpone correctly?
    //You should read what devices + IDs are printed to your console
    //Especially if you have an external mic
    soundStream.printDeviceList();//prints out devices to console
    //E.G. the device ID is 0 > "[OSX Core Audio: 0]" full example seen below:
    //(CONSOLE PRINT EXAMPLE): ofBaseSoundStream::printDeviceList: [OSX Core Audio: 0] Apple Inc.: MacBook Air Microphone [in:1 out:0]
    
    // by name "default" like this should work for most
    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
        // if not 0 try change the ID to your microphone device ID see comments above
        settings.setInDevice(devices[0]);
    }
    
    
    settings.setInListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 1;//on mac can only do 1, change to 2 on windows
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    //sets up the soundStream object with default devices
    mfft.setup(fftSize, 512, 256); //just a forward FFT
    oct.setup(sampleRate, fftSize/2, nAverages);
    mfccs = (double*) malloc(sizeof(double) * 13);
    //512 bins, 42 filters, 13 coeffs, min/max freq 20/20000
    mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

}

//--------------------------------------------------------------
void ofApp::draw(){
    //MFCCs
    ofDrawBitmapString("MFCC",10,200);
    float xinc = 190.0 / 13;
    
    for(int i=0; i < 13; i++) {
        float height = mfccs[i] * 200.0;
        ofDrawRectangle(50 + (i*xinc),200 - height,10, height);
    }
    
    // microphone input raw audio wave
    ofDrawBitmapString("Audio Wave",10,ofGetHeight()/2 - 5);
    waveLine.draw();
    
    //scale volume
    ofDrawBitmapString("Volume:",10,ofGetHeight()*0.75 - 5);
    ofDrawRectangle(10, ofGetHeight()*0.75, 40, scaledVol*200);
}


//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
    
    waveLine.clear();
    
    
    volume = 0;
    //samples are interleaved
    float numCounted = 0.0;
    double left, right;
   
    for (unsigned int i = 0; i < input.getNumFrames(); i++){
        left = input[i*input.getNumChannels()] * 0.5;
        right = input[i*input.getNumChannels()+1] * 0.5;
                
        //calculate the volume / rms
        volume += left * left;
        volume += right * right;
        numCounted += 2.0;
        
        wave = left + right;
        
        //Calculate the mfccs
        if (mfft.process(wave)) { //is the window buffer full?
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
            mfcc.mfcc(mfft.magnitudes, mfccs);
            for(unsigned int k=0; k < 13; k++) {
                if (isinf(mfccs[k])) mfccs[k]=0;
            }
        }
        
        //viusalise sound wave
        waveLine.addVertex(ofMap(i, 0, input.getNumFrames() - 1, 0, ofGetWidth()), ofMap(input[i*input.getNumChannels()], -1, 1, 0, ofGetHeight()));
    }
    
    
    volume /= numCounted;//mean
    volume = sqrt(volume);//square root
    if (isinf(volume)) volume = 0;
    
    smoothedVol *= 0.93;
    smoothedVol += volume * 0.07;
}

//--------------------------------------------------------------
void ofApp::exit() {
    ofSoundStreamClose();
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
