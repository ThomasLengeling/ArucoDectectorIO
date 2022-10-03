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
    ofJson js;
    std::string configFile = "TUIO.json";
    ofFile file(configFile);

    string  inputs []  = { "GRID", "ABM", "AGGREGATED_TRIPS", "ACCESS", "TEXTUAL", "GEOJSON", "MESH"};
    if (file.exists()) {
        ofLog(OF_LOG_NOTICE) << " Reading Config File " << configFile;
        file >> js;

        int i = 0;
        ofLog(OF_LOG_NOTICE) << js << std::endl;
        for (auto & inputName : inputs) {
            auto tui = js[inputName];

            ofLog(OF_LOG_NOTICE) << tui["active"] << " " << inputName;
            bool active = tui["active"].get<bool>();
            std::string name = tui["display_name"].get<string>();

            float x = 50;
            float y = i * 100 + 100;

            auto modulegui = ModuleLayer::create();
            modulegui->name = name;

            modulegui->pos = glm::vec2(x, y);


            if (tui["toggle"].is_object()) {
                std::string tooggle_name = tui["toggle"]["name"].get<string>();
                bool active = tui["toggle"]["active"].get<bool>();
                bool default = tui["toggle"]["default"].get<bool>();
                ofLog(OF_LOG_NOTICE) << " Name: " << name;

                modulegui->button = ofxDatButton::create();
                modulegui->button->mButton = new ofxDatGuiButton(name);
               
                
                modulegui->button->mButton->setWidth(250, .4);
                modulegui->button->mButton->setPosition(x, y+ 25);

                modulegui->button->mButton->onButtonEvent([&](ofxDatGuiButtonEvent v) {

                    });

            }

            if (tui["toggle_array"].is_object()) {
                std::string nameToggle = tui["toggle_array"]["name"].get<string>();
                int num = tui["toggle_array"]["num"].get<int>();


                ofLog(OF_LOG_NOTICE) << " Name: " << nameToggle << " " << num;
                if (tui["toggle_array"]["names"].is_array()) {
                    for (auto& toggles : tui["toggle_array"]["names"]) {
                        ofLog(OF_LOG_NOTICE) << " Name: " << toggles;
                    }

                    modulegui->matrix = ofxDatMatrix::create();
                    modulegui->matrix->mMatrix = new ofxDatGuiMatrix(name, num, false);


                    modulegui->button->mButton->setWidth(250, .4);
                    modulegui->button->mButton->setPosition(x, y + 25);

                    modulegui->button->mButton->onButtonEvent([&](ofxDatGuiButtonEvent v) {

                        });
                }
                
            }

            if (tui["slider"].is_object()) {
                std::string sliderName = tui["slider"]["name"].get<string>();
                ofLog(OF_LOG_NOTICE) << " Name: " << sliderName;

            }

            mModulesGui.push_back(modulegui);

            i++;
        }

    }
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
    for (auto& modules : mModulesGui) {
        modules->draw();
    }
}
//--------------------------------------------------------------
void ofApp::updateGui() {
    for (auto& modules : mModulesGui) {
        modules->update();
    }
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
