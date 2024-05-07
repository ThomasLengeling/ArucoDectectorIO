#pragma once


/*

Thomas Sanchez Lengeling
March, 2019

Living Line

*/

void ofApp::setupValues() {

    mDrawGui = true;
    mRefimentAruco = true;
    mCurrentCamId = 0;
    mNumMarkersRaw = 0;
    mNumMarkers = 0;

    mGridLocation.x = 863;
    mGridLocation.y = 257;
    mGridStep.x = 65;
    mGridStep.y = 65;

    //
    std::string configFile = "config.json";
    ofLog(OF_LOG_NOTICE) << "Loading Config File: " << configFile;
    ofFile file(configFile);
    if (file.exists()) {
        ofJson  jsParams;
        file >> jsParams;
        mCamFps = jsParams["window"]["camfps"].get<int>();
        mNumCams = jsParams["window"]["numCam"].get<int>();

        mCamWidth = jsParams["window"]["cam_width"].get<int>();
        mCamHeight = jsParams["window"]["cam_height"].get<int>();
    }
    else {
        ofLog(OF_LOG_NOTICE) << "Cannot Find File: " << configFile;
        mNumCams = 1;
        mCamFps = 5;
    }


    mConfigureMode = RELEASE;

    mEnableColorPros = true;

    //mGridArea.resize(mNumCams);
   
}
//-----------------------------------------------------------------------------
void ofApp::setupGridInfo() {

    //load imputs for #cameras

    if (mNumCams >= 1) {
        ofLog(OF_LOG_NOTICE) << "Reading Cam Definition griddef.json";
        ofFile file("griddef.json");
        if (file.exists()) {
            ofJson gridjs;
            file >> gridjs;
            for (int j = 0; j < mNumCams; j++) {
                std::string inputGrid("grid_" + to_string(j));
                ofLog(OF_LOG_NOTICE) << inputGrid << " " << j;
                glm::vec2 gridSize = glm::vec2(gridjs[inputGrid]["size_x"], gridjs[inputGrid]["size_y"]);
                glm::vec2 gridPos = glm::vec2(gridjs[inputGrid]["start_x"], gridjs[inputGrid]["start_y"]);
                mGridSizes.push_back(gridSize);
                mGridStart.push_back(gridPos);
                mMaxMarkers += (gridSize.x * gridSize.y);
                ofLog(OF_LOG_NOTICE) << j << ": " << gridSize << " " << (gridSize.x * gridSize.y) << std::endl;
            }
            ofLog(OF_LOG_NOTICE) << "Done grid values JSON";
        }
        else {
            ofLog(OF_LOG_NOTICE) << "File does not exist griddef.json";
        }
    }


    ofLog(OF_LOG_NOTICE) << "done setup init";
}

//-----------------------------------------------------------------------------
void ofApp::setupUDPConnection() {
    std::string jsonNet = "networkUDP.json";
    ofLog(OF_LOG_NOTICE) << "Setup newtwork: " << jsonNet;
    ofFile file(jsonNet);
    mGeoGridUDP = true;
    if (file.exists()) {
        ofJson js;
        file >> js;
        int i = 0;
        for (auto& net : js) {
            std::string netName = "network_" + to_string(i - 1);
            if (i == 0) {
                mGeoGridUDP = (net["geoGridMode"].get<int>() == 1) ? true : false;
            }else if (i == 1) {
                mUDPGeoGridIp = net[netName]["ip"].get<std::string>();
                mUDPGeoGridPort = int(net[netName]["port"]);
            }
            else if (i == 2) {
                mUDPRawIp = net[netName]["ip"].get<std::string>();
                mUDPRawPort = int(net[netName]["port"]);
            }
            else if (i == 3) {
                mUDPTableIp = net[netName]["ip"].get<std::string>();
                mUDTablePort = int(net[netName]["port"]);
            }
            i++;
        }
        ofLog(OF_LOG_NOTICE) << "Loaded: UDP GeoGrid:";
        ofLog(OF_LOG_NOTICE) << "IP: " << mUDPGeoGridIp << " Port: " << mUDPGeoGridPort;

        ofLog(OF_LOG_NOTICE) << "Loaded: UDP Raw:";
        ofLog(OF_LOG_NOTICE) << "IP: " << mUDPRawIp << " Port: " << mUDPRawPort;

        ofLog(OF_LOG_NOTICE) << "Loaded: UDP Receiver:";
        ofLog(OF_LOG_NOTICE) << "IP: " << mUDPTableIp << " Port: " << mUDTablePort;
    }
    else {
        mUDPGeoGridIp = "127.0.0.1";
        mUDPGeoGridPort = 15800;
        ofLog(OF_LOG_NOTICE) << "fail loading newtwork: " << jsonNet
            << " Default: " << mUDPGeoGridIp << " " << mUDPGeoGridPort;
    }

    // table
    ofxUDPSettings settings;
    settings.sendTo(mUDPGeoGridIp, mUDPGeoGridPort);
    settings.blocking = false;
    mUDPConnectionGeoGrid.Setup(settings);

    //radar
    settings.sendTo(mUDPRawIp, mUDPRawPort);
    settings.blocking = false;
    mUDPConnectionRaw.Setup(settings);

    //tablets
    settings.receiveOn(mUDTablePort);
    settings.blocking = false;
    mUDPConnectionTable.Setup(settings);


    //send a simple msg.
    //std::string message = "Connected to Aruco Detector";
    //mUDPConnectionTable.Send(message.c_str(), message.length());

    ofLog(OF_LOG_NOTICE) << "done setup UDP connection ";
}

//-----------------------------------------------------------------------------
void ofApp::setupGUI() {
    int sliderStartX =  250;
    int sliderStartY = ofGetWindowHeight() - 200;
   

    mBSingleGrid = ofxDatButton::create();
    mBSingleGrid->button = new ofxDatGuiToggle("Single Input", false);
    mBSingleGrid->setActivation(false);
    mBSingleGrid->button->setPosition(sliderStartX, sliderStartY);
    mBSingleGrid->button->setWidth(100, .5);
    mBSingleGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
        mBSingleGrid->toggle();
        });

    mBFullGrid = ofxDatButton::create();
    mBFullGrid->button = new ofxDatGuiToggle("Full Input", true);
    mBFullGrid->setActivation(true);
    mBFullGrid->button->setPosition(sliderStartX, sliderStartY + 30);
    mBFullGrid->button->setWidth(100, .5);
    mBFullGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
        mBFullGrid->toggle();

        mConfigureMode = RELEASE;
        if (mBFullGrid->isActive()) {
            std::cout << "done allocated full img";
        }
        else {
            std::cout << "done allocated single img";
        }
        });


    mCamRaw = ofxDatButton::create();
    mCamRaw->setActivation(false);
    mCamRaw->button = new ofxDatGuiToggle("Cameras");
    mCamRaw->button->setPosition(sliderStartX, sliderStartY + 60);
    mCamRaw->button->setWidth(100, .5);
    mCamRaw->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
        mCamRaw->toggle();
        });

    mCamCalibration = ofxDatButton::create();
    mCamCalibration->setActivation(false);
    mCamCalibration->button = new ofxDatGuiToggle("Cam Calibrations");
    mCamCalibration->button->setPosition(sliderStartX, sliderStartY + 90);
    mCamCalibration->button->setWidth(100, .5);
    mCamCalibration->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
        if (mCamCalibration->isActive()) {
            setupCamCalibration();
            ofLog(OF_LOG_NOTICE) << "Setup Camera Calibration: " << std::endl;
        }
        });


    mBGridSelect = ofxDatMatrix::create();
    mBGridSelect->matrix = new ofxDatGuiMatrix("Grid Matrix", mNumCams, true);
    mBGridSelect->matrix->setRadioMode(true);
    mBGridSelect->matrix->setOpacity(0.8);
    mBGridSelect->matrix->setWidth(390, .5);
    mBGridSelect->matrix->setPosition(sliderStartX, sliderStartY + 120);
    mBGridSelect->matrix->onMatrixEvent([&](ofxDatGuiMatrixEvent v) {
        mCurrentCamId = v.child;
        mHighlightMarkerId = 0;
        ofLog(OF_LOG_NOTICE) << "Index: " << mCurrentCamId << std::endl;

       // mGammaValue->setValue(mCamGrabber.at(mCurrentCamId)->getGamma());
        //mAlphaValue->setValue(mCamGrabber.at(mCurrentCamId)->getAlpha());
        //mBetaValue->setValue(mCamGrabber.at(mCurrentCamId)->getBeta());

        });


    mGammaValue = ofxDatSlider::create();
    mGammaValue->slider =
        new ofxDatGuiSlider(mGammaValue->ofParam.set("gamma", 0.65, 0.0, 3.0));
    mGammaValue->slider->setWidth(390, .5);
    mGammaValue->slider->setPosition(sliderStartX + 30, sliderStartY + 30);
    mGammaValue->slider->onSliderEvent(
        [&](ofxDatGuiSliderEvent v) { mGammaValue->ofParam = v.value; });

    //alpha
    mAlphaValue = ofxDatSlider::create();
    mAlphaValue->slider =
        new ofxDatGuiSlider(mAlphaValue->ofParam.set("alpha", 0.65, 0.0, 3.0));
    mAlphaValue->slider->setWidth(390, .5);
    mAlphaValue->slider->setPosition(sliderStartX + 30, sliderStartY + 60);
    mAlphaValue->slider->onSliderEvent(
        [&](ofxDatGuiSliderEvent v) { mAlphaValue->ofParam = v.value; });

    //beta
    mBetaValue = ofxDatSlider::create();
    mBetaValue->slider =
        new ofxDatGuiSlider(mBetaValue->ofParam.set("beta", 50, 0.0, 100.0));
    mBetaValue->slider->setWidth(390, .5);
    mBetaValue->slider->setPosition(sliderStartX + 30, sliderStartY + 90);
    mBetaValue->slider->onSliderEvent(
        [&](ofxDatGuiSliderEvent v) { mBetaValue->ofParam = v.value; });

    ofLog(OF_LOG_NOTICE) << "done setup gui";
}
//-----------------------------------------------------------------------------
void ofApp::setupArucoDetector() {
    for (int i = 0; i < mNumCams; i++) {
        ArucoDetectorRef detector = ArucoDetector::create();
        detector->setupCalibration(mGridSizes.at(i).x, mGridSizes.at(i).y);
        mArucoDetector.push_back(detector);
    }

    ofLog(OF_LOG_NOTICE) << "done setup detector: " << mArucoDetector.size();
}

//-----------------------------------------------------------------------------
void ofApp::setupCamCalibration() {
    if (mArucoDetector.size() > 0) {
    mArucoDetector.at(0)->toggleCamCalibration(true);
    }
    else {
        ofLog(OF_LOG_NOTICE) << "Error setup calibration";
    }
    mConfigureMode = CAMERA_CALIBRATION;
    ofLog(OF_LOG_NOTICE) << "done setup calibration";
}

//-----------------------------------------------------------------------------
void ofApp::setupCams() {

    // setup inputs
    ofLog(OF_LOG_NOTICE) << "setting inputs: " << mNumCams;

    for (int i = 0; i < mNumCams; i++) {
        CamCaptureRef gridImage = CamCapture::create(glm::vec2(mCamWidth, mCamHeight));
        gridImage->setId(i);
        mCamGrabber.push_back(gridImage);
    }

    // Print available devices
    ofLog(OF_LOG_NOTICE) << "Cam devices:";
    ofVideoGrabber mVideoGrabber;
    mVideoGrabber.listDevices();

    if (mNumCams >= 1) {
        ofLog(OF_LOG_NOTICE) << "Loading cam and crop positions " << mCamGrabber.size();
        ofFile file("img.json");
        bool foundFile = false;
        if (file.exists()) {
            ofJson camjs;
            file >> camjs;
            int j = 0;
            for (auto& cam : camjs) {
                if (j < mNumCams) {
                    std::string inputImg("cam" + to_string(j));
                    int camId = cam[inputImg]["camId"];
                    ofLog(OF_LOG_NOTICE) << "Loading: " << j << ": CamId: " << camId << " " << std::endl;


                    mCamGrabber.at(j)->setCropUp(glm::vec2(cam[inputImg]["x1"], cam[inputImg]["y1"]));
                    mCamGrabber.at(j)->setCropDown(glm::vec2(cam[inputImg]["x2"], cam[inputImg]["y2"]));
                   // mCamGrabber.at(j)->setCropDisp(glm::vec2(cam[inputImg]["disX"], cam[inputImg]["disY"]));

                    float gamma = cam[inputImg]["gamma"].get<float>();
                    float alpha = cam[inputImg]["alpha"].get<float>();
                    float beta  = cam[inputImg]["beta"].get<float>();
                    

                    mCamGrabber.at(j)->setGamma(gamma);
                    mCamGrabber.at(j)->setAlpha(alpha);
                    mCamGrabber.at(j)->setBeta(beta);

                    //enable ndi
                    bool remote = false;
                    if (cam[inputImg]["remote"].is_boolean()) {
                        remote = cam[inputImg]["remote"].get<bool>();
                        mCamGrabber.at(j)->enableRemote(remote);
                        remote == true ? mCamGrabber.at(j)->enableCam(false) : mCamGrabber.at(j)->enableCam(true);
                        ofLog(OF_LOG_NOTICE) << "Enable remote " << remote;
                    }
                    else {
                        ofLog(OF_LOG_NOTICE) << "No json object remote";
                    }

                   // mGammaValue->setValue(gamma);
                    //mAlphaValue->setValue(alpha);
                    //mBetaValue->setValue(beta);

                    ofLog(OF_LOG_NOTICE) << "Loading cam devices:";
                    mCamGrabber.at(j)->setupCam(camId, mCamFps);

                    ofLog(OF_LOG_NOTICE) << "Gamma: " << gamma << " a: " << alpha << " b: " << beta << std::endl;
                    j++;
                }
            }
            ofLog(OF_LOG_NOTICE) << "Done crop values JSON";
            foundFile = true;
        }
        else {
            ofLog(OF_LOG_NOTICE) << "File does not exist img.json";
        }
    }

    // fill the fbos with the appropiate dimentions
    mFboSingle.allocate(mCamWidth, mCamHeight, GL_RGBA);

    // clean start with Fbos
    mFboSingle.begin();
    ofClear(0, 0, 0, 255);
    mFboSingle.end();

    ofLog(OF_LOG_NOTICE) << "done setup video";
}

//-----------------------------------------------------------------------------
void ofApp::setupGridDetector() {

    //generate detector information
    for (int i = 0; i < mNumCams; i++) {
        ofLog(OF_LOG_NOTICE) << "setup grid: " << i << " " << mGridSizes.at(i);
        GridDetectorRef griD = GridDetector::create(mGridSizes.at(i));
        griD->setGridStart(mGridStart.at(i));
        griD->setId(i);
        std::string gridPosFile = "gridpos_0" + to_string(i) + ".json";
        griD->setupGridJsonPos(gridPosFile);

        griD->setMaxMarkers(mGridSizes.at(i).x * mGridSizes.at(i).y);
        griD->setupCleaner();
        griD->setupBlocks();

        mGridDetector.push_back(griD);

        // allocate fbo for drawing
        ofFbo fboTemp;
        fboTemp.allocate(mCamWidth, mCamHeight, GL_RGB);
        fboTemp.begin();
        ofClear(0, 0, 0);
        fboTemp.end();
        mFboGrid.push_back(fboTemp);
    }

    ofLog(OF_LOG_NOTICE) << "done setup grid :" << mNumCams;
}

//-----------------------------------------------------------------------------
void ofApp::setupCityIO() {
    ofLog(OF_LOG_NOTICE) << "Loading CityIO Data" << std::endl;
    std::string configFile = "config.json";
    ofLog(OF_LOG_NOTICE) << "Loading Config File: " << configFile;
    ofFile file(configFile);
    if (file.exists()) {
        ofJson  jsParams;
        file >> jsParams;

        mTableLocation = jsParams["deploy"]["location"].get<string>();
        mTableName = jsParams["deploy"]["tableName"].get<string>();
        mUrl = jsParams["deploy"]["url"].get<string>();
    }
    else {
        ofLog(OF_LOG_NOTICE) << "Cannot Find File: " << configFile;
        mTableLocation = "test";
        mTableName = "test";
        mUrl = "https://cityscope.media.mit.edu";
    }

    if (mBaseGrid.load("imgs/"+mTableLocation+".png")) {
        ofLog(OF_LOG_NOTICE) << "Loaded Base Interactive Img";
    }
    else {
        ofLog(OF_LOG_NOTICE) << "Error Base Interactive Img";
    }

    mCityIo = CityIO::create();
    mCityIo->setTable(mTableName);
    mCityIo->setBaseUrl(mUrl);
    
    //"corktown"
    //https://cityscope.media.mit.edu/CS_cityscopeJS/?cityscope=volpe_rd
    ///mCityIo->setBaseUrl("http://127.0.0.1:5000");

}
