#include "ofApp.h"
#include "setup.h"

using namespace glm;
using namespace ofxCv;


void ofApp::setup()
{

    // init video grabber
    //get back a list of devices.
    // Print available devices
    ofLog(OF_LOG_NOTICE) << "Cam devices:";
    ofVideoGrabber mVideoGrabber;
    mVideoGrabber.listDevices();

    camlist = mVideoGrabber.listDevices();

    // oF app
    ofSetVerticalSync(false);
    ofSetFrameRate(30);
    ofSetBackgroundAuto(false);
    //ofBackground(0);

    //detector
    //ofSetBackgroundAuto(false);
    //ofBackground(0);

    setupValues();
    setupGUI();
    setupCityIO();

    setupGridInfo();

    setupArucoDetector();
    setupGridDetector();
    //setupCamCalibration();
    setupCams();
    setupUDPConnection();



    ofLog(OF_LOG_NOTICE) << "finished setup";

    mHighlightMarkerId = 0;


   // spaecialGridInter01 = {37, 38, 39, 40, 41, 42, 60, 61, 62, 63, 64, 65};

}


void ofApp::update()
{

    bool newFrame = false;
    int currentId = mCurrentCamId;

    std::vector<cv::Mat> vidMatImgs;

    std::vector<cv::Mat> imageCopys;
    std::vector<std::string> UDPStrings;


    int j = 0;
    for (auto& mGrabber : mCamGrabber) {
        newFrame = mGrabber->updateImage();
        j++;
    }

    //update frame
    if (mBSingleGrid->isActive()) {

        cv::Mat imageCopy;
        ofPixels pixs = mCamGrabber.at(mCurrentCamId)->getImgPixels();
        if (pixs.getHeight() > 0) {
            //pixs.rotate90(2);
            cv::Mat copyColor;
            cv::Mat input = ofxCv::toCv(pixs).clone();
            mCamGrabber.at(currentId)->cropImg(input);
            cv::Mat copMat = mCamGrabber.at(currentId)->getCropMat();


            if (mConfigureMode == DEBUG_COLOR || mEnableColorPros) {
                copMat.copyTo(copyColor);
                mCamGrabber.at(currentId)->setGamma(mGammaValue->getValue());
                mCamGrabber.at(currentId)->setAlpha(mAlphaValue->getValue());
                mCamGrabber.at(currentId)->setBeta(mBetaValue->getValue());

                // calculate Gamma
                mCamGrabber.at(currentId)->adjustGamma(copyColor);

                copyColor.copyTo(imageCopy);
            }
            else {
                copMat.copyTo(imageCopy);
            }
            if (!imageCopy.empty()) {

                mArucoDetector.at(currentId)->detectMarkers(imageCopy, mRefimentAruco);

                // calculate the number of total markers
                mNumMarkersRaw = mArucoDetector.at(currentId)->getNumMarkers();

                // get the marker image output
                mImageDetector = mArucoDetector.at(currentId)->getOfImg();
                //vidMat = mArucoDetector.at(currentId)->getMatImg();

                mCamGrabber.at(currentId)->updateDetectImg(mImageDetector);

                // save the positions and id from the detected markers.
                mGridDetector.at(currentId)->generateMarkers(mArucoDetector.at(currentId)->getTagIds(), mArucoDetector.at(currentId)->getBoard());

                //update error check
                mGridDetector.at(currentId)->updateProablity();
            }

        }
    }
    else if (mBFullGrid->isActive()) {
        int i = 0;
        mNumMarkersRaw = 0;
        for (auto& cam : mCamGrabber) {
            cv::Mat imageCopy;
            ofPixels pixelsImg =  cam->getImgPixels();
            if (pixelsImg.getHeight() > 0) {
                cv::Mat copyColor;
                // pixs.rotate90(2);
                //pixelsImg.u
                cv::Mat input = ofxCv::toCv(pixelsImg);
                if (input.empty()) {
                    ofLog(OF_LOG_NOTICE) << "empty input";
                }

                cam->cropImg(input);
               // pixelsImg.up)
                cv::Mat copMat = cam->getCropMat();
                if (mConfigureMode == DEBUG_COLOR || mEnableColorPros) {
                    copMat.copyTo(copyColor);
                    cam->adjustGamma(copyColor);
                    copyColor.copyTo(imageCopy);
                }
                else {
                    copMat.copyTo(imageCopy);
                }

                if (!imageCopy.empty()) {
                    // detect the markers
                    mArucoDetector.at(i)->detectMarkers(imageCopy, mRefimentAruco);

                    // calculate the number of total markers
                    mNumMarkersRaw += mArucoDetector.at(i)->getNumMarkers();
                    

                    // get the marker image output
                    mImageDetector = mArucoDetector.at(i)->getOfImg();
                    //vidMat = mArucoDetector.at(i)->getMatImg();

                    cam->updateDetectImg(mImageDetector);

                    // save the positions and id from the detected markers.
                    mGridDetector.at(i)->generateMarkers(mArucoDetector.at(i)->getTagIds(), mArucoDetector.at(i)->getBoard());

                    //update errors
                    mGridDetector.at(i)->updateProablity();
                    i++;
                }

            }
        }
    }



    updateGUI();

    offScreenMarkers();

    updateUDP();

    updateTabletJson();

    if (mCalculateGrid) {
        mGridDetector.at(mCurrentCamId)->generateGridPos(mGridLocation.x, mGridLocation.y, mGridStep.x, mGridStep.y);
        ofLog(OF_LOG_NOTICE) << mGridLocation.x << " " << mGridLocation.y << " " << mGridStep.x<< " " << mGridStep.y;
        mCalculateGrid = false;
    }


}
//---------------------------------------------------------------------------
void ofApp::updateUDP() {
    // calculate probabilyt and clean nois
    if (mBSingleGrid->isActive()) {

        int doneClean = 0;
        if (mGridDetector.at(mCurrentCamId)->isDoneCleaner()) {
            mGridDetector.at(mCurrentCamId)->calculateProbabilityGrid();
            doneClean = 1;
        }

        if (doneClean >= 1) {

            //send a single grid
            if (mGridDetector.at(mCurrentCamId)->isDoneCleaner()) {

                std::map<int, int> gridInterSingle = mGridDetector.at(mCurrentCamId)->getGridInter();
               // mUDPConnectionTable.Send(udpMsg.c_str(), udpMsg.length());

                mPrevGridAreaS = mGridAreaS;
                mGridAreaS = gridInterSingle;

                if (mPrevGridAreaS != mGridAreaS) {

                    mCityIo->executePostGeoGrid(mGridAreaS, false); //overide interative

                    ofLog(OF_LOG_NOTICE) << "sent data " << std::endl;

                }
            }

            mGridDetector.at(mCurrentCamId)->resetProbabilty();
        }
    }
    else if (mBFullGrid->isActive()) {

        //if we have clean succesfully 4 times the.
        int doneClean = 0;
        for (auto& gridDetector : mGridDetector) {

            if (gridDetector->isDoneCleaner()) {
                gridDetector->calculateProbabilityGrid();
                   doneClean++;
            }
        }

        // send UDP in the correct format.
        if (doneClean >= mNumCams) {
            mNumMarkers = 0;
           
            mPrevGridArea = mGridArea;
            mGridArea.clear();

            for (auto& gridInter : mGridDetector) {
                auto gridInt = gridInter->getGridInter();
                mGridArea.insert(gridInt.begin(), gridInt.end());

            }
            //std::map<int, int> gridInter01 = mGridDetector.at(0)->getGridInter();
            //std::map<int, int> gridInter02 = mGridDetector.at(1)->getGridInter();
            //std::map<int, int> gridInter03 = mGridDetector.at(2)->getGridInter();
            //std::map<int, int> gridInter04 = mGridDetector.at(3)->getGridInter();

            //mGridArea/

            //mPrevGridArea = mGridArea;
            //mGridArea.clear();

           // mGridArea.insert(gridInter01.begin(), gridInter01.end());
           // mGridArea.insert(gridInter02.begin(), gridInter02.end());
           // mGridArea.insert(gridInter03.begin(), gridInter03.end());
           // mGridArea.insert(gridInter04.begin(), gridInter04.end());


            if (mPrevGridArea != mGridArea) {
                /*ofLog(OF_LOG_NOTICE) << std::endl;
                for (auto& val : mGridArea) {
                    ofLog(OF_LOG_NOTICE) << val.first << " " << val.second;
                }
                */

                mCityIo->executePostGeoGrid(mGridArea, false); //overide interative
                ofLog(OF_LOG_NOTICE) << "ready to send data to cityIO" << std::endl;
            }
            /*
            std::string compandStr;
            compandStr += "i ";


            std::map<int, int> gridInter03 = mGridDetector.at(2)->getGridInter();
            std::map<int, int> gridInter04 = mGridDetector.at(3)->getGridInter();


            for (auto& grid : gridInter02) {

            }

            mPrevGridArea = mGridArea;

            mGridArea.clear();
            mGridArea.resize(NUM_CAM_INPUTS);

            //area 1
            mGridArea.at(0).insert(empthGrid.begin(), empthGrid.end());
            mGridArea.at(0).insert(gridInter01.begin(), gridInter01.end());
            mGridArea.at(0).insert(gridInter02.begin(), gridInter02.end());
            mGridArea.at(0).insert(gridInter03.begin(), gridInter03.find(276));

            //area 2
            auto in = gridInter03.find(276);
            mGridArea.at(1).insert(make_pair<const int&, int&>(in->first, in->second));

            //area 3
            mGridArea.at(2).insert(gridInter03.find(277), gridInter03.end());

            //area 4
            mGridArea.at(3).insert(gridInter04.begin(), gridInter04.end());


            //send grid areas if there was a change in the grid
            for (int i = 0; i < mPrevGridArea.size(); i++) {
                if (mPrevGridArea.at(i) != mGridArea.at(i)) {
                    std::string mUdpMsg = "i" + to_string(i + 1) + " ";
                    for (auto& gridInter : mGridArea.at(i)) {
                        mUdpMsg += std::to_string(gridInter.second) + " ";
                    }
                    mUDPConnectionTable.Send(mUdpMsg.c_str(), mUdpMsg.length());
                    mUDPConnectionRadar.Send(mUdpMsg.c_str(), mUdpMsg.length());
                    mUDPConnectionGrid.Send(mUdpMsg.c_str(), mUdpMsg.length());
                    //ofLog(OF_LOG_NOTICE) << "Msg: " << mUdpMsg;
                }
            }
            */

            for (auto& gridDetector : mGridDetector) {
                mNumMarkers += gridDetector->getNumMarkersAbolute();
                gridDetector->resetProbabilty();
            }

        }
    }
}
//----------------------------------------------
void ofApp::updateTabletJson() {
    char udpMessage[10000];
    mUDPConnectionTablet.Receive(udpMessage, 10000);

    string message = udpMessage;
    if (message != "") {
        if (udpMessage[0] == '{') {
            ofJson tuijson = ofJson::parse(message);
            //std::string tuijson = tuijson.dump();
            ofLog(OF_LOG_NOTICE) << tuijson.dump(4);
            mCityIo->excuteGetRequestTUI(tuijson);
        }
        else {
            ofLog(OF_LOG_NOTICE) << "error UDP JSON tui";
        }
    }

}
//---------------------------------------------------------------------------
void ofApp::offScreenMarkers() {
    mFboSingle.begin();
    ofSetColor(0, 0, 0, 255);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());

    switch (mConfigureMode) {
    case INPUT_IMG:
        ofSetColor(255);
        mImageDetector.draw(0, 0);
        break;

    case CUT_IMG:
        ofSetColor(255);
        mCamGrabber.at(mCurrentCamId)->drawCropImg();
        mCamGrabber.at(mCurrentCamId)->drawCropRoi();
        break;
    case CAMERA_CALIBRATION:
        ofSetColor(255);
        mImageDetector.draw(0, 0);
        break;

    case DEBUG_COLOR:
    {
        ofSetColor(255);
        mImageDetector.draw(0, 0);
    }
    break;
    case GRID_POS:
    {
        ofSetColor(255);


        mImageDetector.draw(0, 0);
        mGridDetector.at(mCurrentCamId)->drawMarkers();

        ofSetColor(255);

        float sqsize = 36;
        float sqspace = 5;

        //draw squares
        glm::vec2 dim = mGridDetector.at(mCurrentCamId)->getDim();
        int spaceX = dim.x * (sqsize + sqspace);
        int spaceY = dim.y * (sqsize + sqspace);

        int camWidth = mCamGrabber.at(mCurrentCamId)->getROI().width;

        mGridDetector.at(mCurrentCamId)->drawDetectedGridIn(camWidth + 30, 20, sqsize, sqspace);
    }
        break;

    case RELEASE:
    {
        //draw grids
        ofSetColor(255);
       // mImageDetector.draw(0, 0);


        //glm::vec2 dim = mGridDetector.at(mCurrentCamId)->getDim();
        //int spaceX = dim.x * (sqsize + sqspace);
        //int spaceY = dim.y * (sqsize + sqspace);

        //cut RIO
        int camWidth = mCamGrabber.at(mCurrentCamId)->getROI().width;

        //mGridDetector.at(mCurrentCamId)->drawDetectedGridIn(camWidth + 30, 20, sqsize, sqspace);

        ofSetColor(255);
        ofDrawRectangle(0, 1080 - 980, 1920, 1080 * 0.65);

        ofSetColor(0);
        ofDrawRectangle(0, 0, 1920, 1080);

        ofSetColor(255);
        ofPushMatrix();
        //mBaseGrid.draw(200, 1080 - 950, 1920 * 0.9, 1080 * 0.9);

        mBaseGrid.draw(0, 40, 1920, 1080);// , 1920, 1080);
        ofPopMatrix();

        //black rectangle to hide cams
        ofSetColor(0);
        ofDrawRectangle(0, 0, 1920, 240);


        int j = 0;
        glm::vec2 pos;
        float sqsizeW = 42;
        float sqsizeH = 37;

        float sqspaceX = 47.2;
        float sqspaceY = 41;

        std::vector<glm::vec2> gridPos = {
    glm::vec2(620, 275),
    glm::vec2(620 , 275 + 8 * sqspaceY) };

        /*
        std::vector<glm::vec2> gridPos = {
            glm::vec2(620, 275),
            glm::vec2(620 + 8 * sqspaceX, 275),
            glm::vec2(620 , 275 + 8 * sqspaceY), 
            glm::vec2(620 + 8 * sqspaceX, 275 + 8 * sqspaceY) };
            */
        for (auto& grid : mGridDetector) {
            pos = gridPos[j];
            grid->drawDetectedInteraction(j, pos.x, pos.y, sqsizeW, sqsizeH, sqspaceX, sqspaceY);
            j++;
        }


        //draw cam feed images

       // ofSetColor(225);
       // ofDrawRectangle(0, 0, 1920, 240);

        if (mCamRaw->isActive()) {
            int index = 0;
            ofSetColor(255, 255);
            for (auto& gridImage : mCamGrabber) {
                if (mCurrentCamId == index) {
                    ofSetColor(0, 100, 120);
                }
                else {
                    ofSetColor(255);
                }
                int imageW = 1920 / (float)mCamGrabber.size();
                gridImage->drawImage((imageW) * index, 0, imageW, 270);
                index++;
            }
        }
    }
        break;

    }


    mFboSingle.end();

}

void ofApp::draw()
{
    ofSetColor(0, 0, 0, 255);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());


    switch (mConfigureMode) {

        //display displays in square mode
    case DEBUG:
    {

    }
        break;
    case CAMERA_CALIBRATION:
        ofSetColor(255);
        mFboSingle.draw(0, 0);

        mArucoDetector.at(mCurrentCamId)->calibrateCameraProcess();
        break;

    case INPUT_IMG:
    case CUT_IMG:
        ofSetColor(255);
        mFboSingle.draw(0, 0);

        break;
    case GRID_POS:
    case DEBUG_COLOR:
        ofSetColor(255);
        mCamGrabber.at(mCurrentCamId)->drawImage(0, 0, 640, 360);
        mImageDetector.draw(0, 680, 640, 360);

        mFboSingle.draw(0, 0);
        break;
    case RELEASE:


        ofSetColor(255);
        mCamGrabber.at(mCurrentCamId)->drawImage(0, 0, 640, 360);
        mImageDetector.draw(0, 640, 640, 360);

        mFboSingle.draw(0, 0);



        //draw bottom rect for drawing GUI
        ofSetColor(225);
        ofDrawRectangle(0, 1080 - 145, 1920, 1080);
        break;

    }

    if (mDrawGui) {
        drawGUI();

        mGridDetector.at(mCurrentCamId)->recordGrid();
        int guiInfoY = 900;
        int guiInfoX = 15;
        if (mBSingleGrid->isActive()) {
            guiInfoX = 1500;
        }

        ofSetColor(255);
        ofDrawBitmapStringHighlight("app fps: " + ofToString(ofGetFrameRate(), 0), guiInfoX, guiInfoY);
        ofDrawBitmapStringHighlight("Current Cam: " + ofToString(mCurrentCamId) + " - " + ofToString(mGridDetector.at(mCurrentCamId)->getMaxMarkers()), guiInfoX, guiInfoY + 15);
        ofDrawBitmapStringHighlight("Detected Markers Raw: " + ofToString(mNumMarkersRaw), guiInfoX, guiInfoY + 15 * 2);
        ofDrawBitmapStringHighlight("Detected Markers: " + ofToString(mNumMarkers) + "  Max:" + ofToString(mMaxMarkers), guiInfoX, guiInfoY + 15 * 3);
        ofDrawBitmapStringHighlight("Highlight Marker Id: " + ofToString(mHighlightMarkerId), guiInfoX, guiInfoY + 15 * 4);
        ofDrawBitmapStringHighlight("Cam Calibration Frame: " + ofToString(mArucoDetector.at(0)->getCalibrationCount()), guiInfoX, guiInfoY + 15 * 5);

        if (!mBSingleGrid->isActive()) {
            int i = 0;
            for (auto& videoName : camlist) {
                if (i == mCurrentCamId)
                    ofSetColor(255);
                else
                    ofSetColor(100);
                ofDrawBitmapStringHighlight("Video: " + to_string(videoName.id) + " - " + videoName.deviceName, 1650, 900 + i * 15);
                i++;
            }
        }

        ofDrawBitmapStringHighlight(mTableLocation, 1650, 840);
        ofDrawBitmapStringHighlight(mTableName, 1650, 860);
        ofDrawBitmapStringHighlight(mUrl, 1650, 880);
    }

  // ofDrawBitmapStringHighlight("cam fps: " + ofToString(grabber.getGrabber(, 0), 20, ofGetHeight() - 40);
}

void ofApp::mouseMoved(int x, int y) {
    posgrid.x = x;
    posgrid.y = y;
   // cout << posgrid.x << " " << posgrid.y << std::endl;
}

void ofApp::mousePressed(int x, int y, int button) {
    if (mConfigureMode == GRID_POS && ofGetKeyPressed(OF_KEY_SHIFT)) {

        if (mHighlightMarkerId >= 0 && mHighlightMarkerId < mGridDetector.at(mCurrentCamId)->getMaxMarkers()) {
            mGridDetector.at(mCurrentCamId)->setGridPos(glm::vec2(x, y), mHighlightMarkerId);
            mGridDetector.at(mCurrentCamId)->setHighlightMarkerId(mHighlightMarkerId);
            mHighlightMarkerId++;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if (mConfigureMode == GRID_POS) {
        mGridDetector.at(mCurrentCamId)->setGridPos(glm::vec2(x, y));
    }

    //crop input camera information
    if (mConfigureMode == CUT_IMG) {
        {
            float distUp = ofDist(mCamGrabber.at(mCurrentCamId)->getCropUp().x+15, mCamGrabber.at(mCurrentCamId)->getCropUp().y + 15, x, y);
            if (distUp >= 0.0 && distUp <= 50) {
                mCamGrabber.at(mCurrentCamId)->setCropUp(glm::vec2(x, y));
            }

            float distDown =
                ofDist(mCamGrabber.at(mCurrentCamId)->getCropDown().x -15, mCamGrabber.at(mCurrentCamId)->getCropDown().y-15, x, y);
            if (distDown >= 0.0 && distDown <= 70) {
                mCamGrabber.at(mCurrentCamId)->setCropDown(glm::vec2(x, y));
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}
//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    switch (key)
    {
    case '1':
        mConfigureMode = INPUT_IMG;
        mBSingleGrid->setActivation(true);
        mBFullGrid->setActivation(false);
        break;
    case '2':
        mBSingleGrid->setActivation(true);
        mBFullGrid->setActivation(false);
        mConfigureMode = CUT_IMG;
        break;
    case '3':
        mBSingleGrid->setActivation(true);
        mBFullGrid->setActivation(false);
        mConfigureMode = GRID_POS;
        mGridDetector.at(mCurrentCamId)->enableDebugGrid(true);
        break;
    case '4':
        mGammaValue->setValue(mCamGrabber.at(mCurrentCamId)->getGamma());
        mAlphaValue->setValue(mCamGrabber.at(mCurrentCamId)->getAlpha());
        mBetaValue->setValue(mCamGrabber.at(mCurrentCamId)->getBeta());

        mBSingleGrid->setActivation(true);
        mBFullGrid->setActivation(false);
        mConfigureMode = DEBUG_COLOR;
        mEnableColorPros = true;

        ofLog(OF_LOG_NOTICE) << mCamGrabber.at(mCurrentCamId)->getGamma() << std::endl;
        break;
    case '5':
        mBSingleGrid->setActivation(false);
        mBFullGrid->setActivation(true);
        mConfigureMode = RELEASE;
        break;
    case 'd':
        mBSingleGrid->setActivation(true);
        mBFullGrid->setActivation(false);
        mConfigureMode = DEBUG;
        break;

    case 'a':
        mArucoDetector.at(mCurrentCamId)->toggleMarkerInfo();
        break;

    case 'r':
        mRefimentAruco = !mRefimentAruco;
        break;
    case '8':
    case 's': //save configurations
    {
        ofLog(OF_LOG_NOTICE) << "saved json grid positions: " << mCurrentCamId;
        mGridDetector.at(mCurrentCamId)->saveGridJson();

        ofLog(OF_LOG_NOTICE) << "saved json Cam values " << std::endl;
        ofJson writer;
        int i = 0;
        for (auto& gridImage : mCamGrabber) {
            ofJson pt;
            std::string inputImg("cam" + to_string(i));
            pt[inputImg]["x1"] = gridImage->getCropUp().x;
            pt[inputImg]["y1"] = gridImage->getCropUp().y;
            pt[inputImg]["x2"] = gridImage->getCropDown().x;
            pt[inputImg]["y2"] = gridImage->getCropDown().y;
            pt[inputImg]["camId"] = gridImage->getCamId();
            pt[inputImg]["gamma"] = gridImage->getGamma();
            pt[inputImg]["alpha"] = gridImage->getAlpha();
            pt[inputImg]["beta"] = gridImage->getBeta();

            writer.push_back(pt);
            i++;
        }


        ofSaveJson("img.json", writer);
        break;
    }
    case '9':
    case 'c':
        mEnableColorPros = !mEnableColorPros;
        break;
    case 'o':
        mGridDetector.at(mCurrentCamId)->cleanDuplicatePos();
        break;

    case 'j':
        setupCamCalibration();
        break;
    case 'k':
        mArucoDetector.at(mCurrentCamId)->captureCalibrate(); //frame capture 
        break;
    case 'l':
        for (auto& arucoDector : mArucoDetector) {
            arucoDector->loadCameraCalibration();
        }
        break;

    case 'z':
        if (mHighlightMarkerId < mGridDetector.at(mCurrentCamId)->getMaxMarkers()) {
            mHighlightMarkerId++;
            mGridDetector.at(mCurrentCamId)->setHighlightMarkerId(mHighlightMarkerId);
        }
        break;
    case 'x':
        if (mHighlightMarkerId > 0) {
            mHighlightMarkerId--;
            mGridDetector.at(mCurrentCamId)->setHighlightMarkerId(mHighlightMarkerId);
        }
        break;

    case 'v':
        mCityIo->excuteGetRequest("GEOGRID/properties/header/");
        break;
    case 'b':
        mCityIo->computeGeoGrid();
        break;
    case 'n':
        mCityIo->executePostGeoGrid(mGridArea);
        break;
    case 'm':
    {
        std::map<int, int> randomv;
        for (int i = 0; i < 256; i++) {
            int v = i;
            randomv.insert(std::pair<int, int>(i, 30));
        }

        mCityIo->executePostGeoGrid(randomv);

        break;
    }
    case 'g' :
        mDrawGui = !mDrawGui;
        break;
    case 't':
        //send json 
        
    {
        ofFile file("tui.json");
        if (file.exists()) {
            ofJson tuijson;
            tuijson  << file;
            //std::string tuijson = tuijson.dump();
            ofLog(OF_LOG_NOTICE) << tuijson.dump(4);
            mCityIo->excuteGetRequestTUI(tuijson);
        }
        else {
            ofLog(OF_LOG_NOTICE) <<"not found";
        }

        break;
    }
    default:
        break;
    }



}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    /*
    int stepInc = 1;
    if (ofGetKeyPressed(OF_KEY_TAB))
        stepInc = 10;

    if (key == OF_KEY_UP) {
        mGridLocation.y += stepInc;
        mCalculateGrid = true;
    }
    else if (key == OF_KEY_DOWN) {
        mGridLocation.y -= stepInc;
        mCalculateGrid = true;
    }
    else if (key == OF_KEY_LEFT) {
        mGridLocation.x -= stepInc;
        mCalculateGrid = true;
    }
    else if (key == OF_KEY_RIGHT) {
        mGridLocation.x += stepInc;
        mCalculateGrid = true;
    }

     if (key == OF_KEY_UP && ofGetKeyPressed(OF_KEY_SHIFT)) {
        mGridStep.y -= stepInc;
        mCalculateGrid = true;
    }
    else if (key == OF_KEY_DOWN && ofGetKeyPressed(OF_KEY_SHIFT)) {
        mGridStep.y += stepInc;
        mCalculateGrid = true;
    }

    else if (key == OF_KEY_LEFT && ofGetKeyPressed(OF_KEY_SHIFT)) {
        mGridStep.x -= stepInc;
        mCalculateGrid = true;
    }
    else if (key == OF_KEY_RIGHT && ofGetKeyPressed(OF_KEY_SHIFT)) {
        mGridStep.x += stepInc;
        mCalculateGrid = true;
    }
    */
}
//--------------------------------------------------------------
void ofApp::drawGUI() {

    switch (mConfigureMode) {

    case DEBUG_COLOR:
        mGammaValue->draw();
        mAlphaValue->draw();
        mBetaValue->draw();
        break;
    case DEBUG:
    case RELEASE:
    case INPUT_IMG:
        mAccurancy->draw();
        mBSingleGrid->draw();
        mBFullGrid->draw();
        mCamRaw->draw();
        mBGridSelect->draw();
        mCamCalibration->draw();
        break;
    }
}
//--------------------------------------------------------------
void ofApp::updateGUI() {
    switch (mConfigureMode) {
    case DEBUG_COLOR:
        mGammaValue->update();
        mAlphaValue->update();
        mBetaValue->update();
        break;

    case DEBUG:
    case RELEASE:
    case INPUT_IMG:
        mAccurancy->update();
        mBSingleGrid->update();
        mBFullGrid->update();
        mCamRaw->update();
        mBGridSelect->update();
        mCamCalibration->update();
        break;
    }

    if (mAccurancy->isActive()) {
        /*
        for (auto& grid : mGridDetector) {
            //grid->setWindowIter(12);
        }
       // updateUDP();
    }
    else {
        for (auto& grid : mGridDetector) {
            //grid->setWindowIter(2);
        }
        */
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    mUDPConnectionRadar.Close();
    mUDPConnectionTablet.Close();
    mUDPConnectionTable.Close();
}
