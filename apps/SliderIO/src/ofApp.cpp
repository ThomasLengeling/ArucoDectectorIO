#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(30);
    ofEnableAntiAliasing();

    //json
    loadJsonNet();

    //load gui
    loadJsonGui();

    generateSliders();


}

//--------------------------------------------------------------
void ofApp::loadJsonNet() {
  ofJson js;
  std::string configFile = "config.json";
  ofFile file(configFile);
  if (file.exists()) {
    ofLog(OF_LOG_NOTICE) << " Reading Config File " << configFile;
    file >> js;

    int i = 0;
    for (auto& net : js) {
        if (i == 0) {
            mUrbanSimIp = net["network_" + to_string(i)]["ip"].get<string>();
            mUrbanSimPort = int(net["network_" + to_string(i)]["port"]);
        }
        if (i == 1) {
            mRadarIp = net["network_" + to_string(i)]["ip"].get<string>();
            mRadarPort = int(net["network_" + to_string(i)]["port"]);
        }

        if (i == 2) {
            mAnalysisIp = net["network_" + to_string(i)]["ip"].get<string>();
            mAnalysisPort = int(net["network_" + to_string(i)]["port"]);
        }
        i++;
    }
  }
  else {
      ofLog(OF_LOG_NOTICE) << configFile << " not found";
  }

  // radar
  ofxUDPSettings settingsUrban;
  settingsUrban.sendTo(mUrbanSimIp, mUrbanSimPort);
  settingsUrban.blocking = false;
  mUDPUrbanSim.Setup(settingsUrban);

  // table
  ofxUDPSettings settingsRadar;
  settingsRadar.sendTo(mRadarIp, mRadarPort);
  settingsRadar.blocking = false;
  mUDPRadar.Setup(settingsRadar);

  //analysis
  ofxUDPSettings settingsAnalysis;
  settingsAnalysis.sendTo(mAnalysisIp, mAnalysisPort);
  settingsAnalysis.blocking = false;
  mUDPAnalysis.Setup(settingsAnalysis);

  //SIM PORTS
  ofLog(OF_LOG_NOTICE) << "Urban: ";
  ofLog(OF_LOG_NOTICE) << mUrbanSimIp << " " << mUrbanSimPort;
  ofLog(OF_LOG_NOTICE) << "Radar: ";
  ofLog(OF_LOG_NOTICE) << mRadarIp << " " << mRadarPort;
  ofLog(OF_LOG_NOTICE) << "Analysis: ";
  ofLog(OF_LOG_NOTICE) << mAnalysisIp << " " << mAnalysisPort;
}


//--------------------------------------------------------------
void ofApp::loadJsonGui() {

}

//--------------------------------------------------------------
void ofApp::generateSliders() {


}

//--------------------------------------------------------------
void ofApp::update() {
    // update gui
    updateGui();

}

//--------------------------------------------------------------
void ofApp::draw() {
    //draw gui
    drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {

}
//--------------------------------------------------------------
void ofApp::updateGui() {

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
