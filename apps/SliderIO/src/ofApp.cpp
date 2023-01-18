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

   
    //font
    verdana18.load("ofxbraitsch/fonts/NotoSansTC-Regular.otf", 5, true, true);
    // verdana14.setLineHeight(18.0f);
    verdana18.setLetterSpacing(1.037);
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
            mTableName = net["network_" + to_string(i)]["name"].get<string>();
            mDesTxt = net["network_" + to_string(i)]["txt"].get<string>();
            ofLog(OF_LOG_NOTICE) << mDesTxt ;
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

    string  inputs []  = { "MESH", "ACCESS", 
                          "GRID", "ABM",
                            "AGGREGATED_TRIPS", "MAP_STYLE", 
                            "GEOJSON", "TEXTUAL"};

    int px = 150;
    int py = 125;
    int spaceX = 875;
    int spaceY = 420;
   
    glm::vec2  pos2[] = {
        glm::vec2(px + spaceX * 0, py + spaceY * 0 + 175), glm::vec2(px + spaceX * 1, py + spaceY * 0),
        glm::vec2(px + spaceX * 0, py + spaceY * 1 + 0), glm::vec2(px + spaceX * 1, py + spaceY * 1),
        glm::vec2(px + spaceX * 0, py + spaceY * 2 + 0), glm::vec2(px + spaceX * 1, py + spaceY * 2- 75),
        glm::vec2(px + spaceX * 0, py + spaceY * 3 - 175), glm::vec2(px + spaceX * 1, py + spaceY * 3- 25) };


    glm::vec2  pos[8];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            pos[j + i * 2] = glm::vec2(150 + j*800, 125 + 400*i);
        }
    }

    //pos2[0] = glm::vec2(px + spaceX * 800 * 0, 125 + 300 * spaceY * 0); pos2[1] = glm::vec2(px + spaceX * 800 * 1, 125 + 400 * spaceY * 0);
    //pos2[2] = glm::vec2(px + spaceX * 800 * 0, 125 + 300 * spaceY * 1); pos2[3] = glm::vec2(px + spaceX * 800 * 1, 125 + 400 * spaceY * 1);
    //pos2[4] = glm::vec2(px + spaceX * 800 * 0, 125 + 300 * spaceY * 2); pos2[5] = glm::vec2(px + spaceX * 800 * 1, 125 + 400 * spaceY * 2);
    //pos2[6] = glm::vec2(px + spaceX * 800 * 0, 125 + 300 * spaceY * 3); pos2[7] = glm::vec2(px + spaceX * 800 * 1, 125 + 400 * spaceY * 3);

   // pos[6] = glm::vec2(150, 150+650);



 

    if (file.exists()) {
        ofLog(OF_LOG_NOTICE) << " Reading Config File " << configFile;
        file >> js;

        int i = 0;
        ofLog(OF_LOG_NOTICE) << js << std::endl;
        for (auto& inputName : inputs) {
            auto tui = js[inputName];

            ofLog(OF_LOG_NOTICE) << tui["display"] << " " << inputName;
            bool active = tui["display"].get<bool>();
            std::string name = tui["display_name"].get<string>();

            float x = pos2[i].x;
            float y = pos2[i].y;

            auto modulegui = ModuleLayer::create();
            modulegui->mActive  = active;

            //json
            modulegui->mGuiJson = js[inputName];

            cout << "sat: " << inputName << std::endl;
            cout << modulegui->mGuiJson.dump(4) << std::endl;
            modulegui->jsonName = inputName;

            modulegui->name = name;
            modulegui->pos = glm::vec2(x + 10, y);

            if (tui["toggle"].is_object()) {
                std::string tooggle_name = tui["toggle"]["name"].get<string>();
                bool active = tui["toggle"]["active"].get<bool>();
                bool default = tui["toggle"]["default"].get<bool>();
                ofLog(OF_LOG_NOTICE) << " Name: " << name;

                modulegui->toggle = ofxDatToggle::create();
                modulegui->toggle->mToggle = new ofxDatGuiToggle("");


                modulegui->toggle->mToggle->setWidth(name.size() * 27, .4);
                modulegui->toggle->mToggle->setPosition(x, y - 45);
                modulegui->toggle->mToggle->setStripeColor(ofColor(17, 90, 230));
               

                modulegui->toggle->mToggle->setBackgroundColorOnMouseOver(ofColor::fromHex(0x2FA1D6));


                /* modulegui->button->mButton->onButtonEvent([&](ofxDatGuiButtonEvent v) {
                     cout << " button msg"<< endl;
                     }
                 );
                 */

            }

            if (tui["toggle_array"].is_object()) {
                std::string nameToggle = tui["toggle_array"]["name"].get<string>();
                int num = tui["toggle_array"]["num"].get<int>();

                int curr_active = tui["toggle_array"]["curr_active"].get<int>();

                ofLog(OF_LOG_NOTICE) << " Matrix Name: " << nameToggle << " " << num;
                if (tui["toggle_array"]["names"].is_array()) {

                    std::vector<string> labels;
                    for (auto& togglesNames : tui["toggle_array"]["names"]) {
                        ofLog(OF_LOG_NOTICE) << " Name: " << togglesNames;
                        labels.push_back(togglesNames);
                    }

                    //matrix
                    modulegui->matrix = ofxDatMatrix::create(labels);
                    modulegui->matrix->mMatrix = new ofxDatGuiMatrix(name, num * 1, false);

                    modulegui->matrix->mMatrix->setLabels(labels);
                    modulegui->matrix->mMatrix->setWidth(850, 0.33);


                    // if (num == 4) {
                     //    modulegui->matrix->mMatrix->setWidth(50, 0.33);
                    // }

                    modulegui->matrix->mMatrix->setPosition(x, y + 75);
                    modulegui->matrix->mMatrix->setRadioMode(true);


                    //labels
                    modulegui->matrixLabel = ofxDatLabel::create();
                    if (curr_active >= 0 && curr_active < num) {
                        modulegui->matrixLabel->mLabel = new ofxDatGuiLabel(labels.at(curr_active));
                        modulegui->matrix->mCurrentName = labels.at(curr_active);
                        modulegui->matrix->mCurrentActive = curr_active;
                    }
                    else {
                        modulegui->matrixLabel->mLabel = new ofxDatGuiLabel("");
                    }
                    modulegui->matrixLabel->mLabel->setPosition(x, y + 25);

                    //
                  //  modulegui->matrix->mMatrix->setHover(0);
                    //modulegui->matrix->mMatrix->setSelected(0);


                    modulegui->matrix->mMatrix->onMatrixEvent([modulegui](ofxDatGuiMatrixEvent v) {
                        int id = v.child;
                        int sizelb = modulegui->matrix->mLabels.size();
                        if (sizelb >= 0 && id < sizelb) {
                            modulegui->matrixLabel->mLabel->setLabel(modulegui->matrix->mLabels.at(id));
                        }
                        modulegui->matrix->mCurrentActive = id;
                        modulegui->matrix->mCurrentName = modulegui->matrix->mLabels.at(id);

                        
                        modulegui->matrix->mMatrix->setHover(id);


                        cout << "onMatrixEvent : " << v.child << " was clicked : " << modulegui->matrix->mLabels.at(id) << endl;
                        });
                }
            }



            if (tui["slider"].is_object()) {
                std::string sliderName = tui["slider"]["name"].get<string>();
                int min = tui["slider"]["min"].get<int>();
                int max = tui["slider"]["max"].get<int>();
                int def = tui["slider"]["default"].get<int>();
                ofLog(OF_LOG_NOTICE) << " Name: " << sliderName;

                modulegui->slider = ofxDatSlider::create();
                modulegui->slider->mSlider = new ofxDatGuiSlider(sliderName, min, max);
                modulegui->slider->mSlider->setPrecision(0);
                modulegui->slider->mSlider->setPosition(x, y + 45);
                modulegui->slider->mSlider->setWidth(600, 0.25);
                modulegui->slider->mSlider->setValue(def);

                modulegui->slider->mSlider->onSliderEvent([modulegui](ofxDatGuiSliderEvent v) {
                    cout << "Slider : " << v.value;
                    modulegui->mGuiJson["slider"]["value"] = (int)v.value;
                    cout << modulegui->mGuiJson.dump(4) << std::endl;

                    modulegui->mInteraction = true;

                    });


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



    for (auto& guis : mModulesGui) { 
        if (guis->mInteraction) {
            guis->mInteraction = false;
            mInteraction = true;
        }
    }

    if (mInteraction) {
        mTUIJson.clear();
        cout << "new tui json:";
        for (auto& guis : mModulesGui) {
            mTUIJson[guis->jsonName] = guis->mGuiJson;
        }

        for (auto& guis : mModulesGui) {
            mTUIJson[guis->jsonName]["active"] = guis->mGuiJson["toggle"]["active"];
        }

        mInteraction = false;
        cout << mTUIJson.dump(4);


        //udp
        string strjson = mTUIJson.dump();
        mUDPUrbanSim.Send(strjson.c_str(), strjson.length());
    }

    
}

//--------------------------------------------------------------
void ofApp::draw() {
    //draw gui

    mModulesGui.at(0)->verdana18.drawString(mTableName, 70, 100);


   // verdana18.drawString(mDesTxt, 75, 150);

    auto lines = ofSplitString(mDesTxt, "\n");
    float y = 175;// (verdana18.stringHeight(mDesTxt)) / 2 + verdana18.getAscenderHeight();

    for (auto& l : lines) {
        verdana18.drawString(l, 120, y);

        y += verdana18.getLineHeight();
    }
    

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
void ofApp::keyPressed(int key) {
    if (key == 't') {
        loadJsonNet();
    }


}

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
