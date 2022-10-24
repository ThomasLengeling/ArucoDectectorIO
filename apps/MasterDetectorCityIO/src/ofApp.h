#pragma once

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxOpenCv.h"

#include <opencv2/aruco.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "ArucoDetector.h"
#include "QRBlock.h"
#include "CamCapture.h"
#include "GridDetector.h"
#include "CityIO.h"


#include <map>
#include <utility>

enum debugConfiguration {
    INPUT_IMG = 0,
    CUT_IMG = 1,
    GRID_POS = 2,
    DEBUG_COLOR = 3,
    RELEASE = 4,
    DEBUG = 5,
    PERSPECTIVE_IMG = 6,
    CAMERA_CALIBRATION = 7
};



class ofApp: public ofBaseApp
{
public:

    void setup();
	void update();
	void draw();
    void exit();

    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseMoved(int x, int y);
    void keyPressed(int key);
    void keyReleased(int key);

    void offScreenMarkers();

    //setup
    void setupValues();
    void setupGridInfo();
    void setupCamCalibration();
    void setupUDPConnection();
    void setupCams();
    void setupGUI();
    void setupArucoDetector();
    void setupGridDetector();
    void setupCityIO();

    //update
    void updateUDP();
    void updateGUI();

    //draw
    void drawGUI();


    //cams
    int mNumCams;
    int mCamFps;


    // send commands
    ofxUDPManager mUDPConnectionTable;
    std::string   mUDPIp;
    int           mUDPPort;

    //network for Radar communication
    ofxUDPManager mUDPConnectionRadar;
    std::string   mUDPRadarIp;
    int           mUDPRadarPort;

    //
    ofxUDPManager mUDPConnectionTablet;
    std::string   mUDPTabletIp;
    int           mUDTabletPort;
    void          updateTabletJson();


    // 4 camera render
    ofFbo mFboSingle;
    std::vector<ofFbo> mFboGrid;
    ofFbo mFboFullGrid;

    //camera grabber
    std::vector < CamCaptureRef> mCamGrabber;
    int mCurrentCamId;
    ofImage mImageDetector;
    int mCamWidth;
    int mCamHeight;
    

    // aruco detector
    std::vector < ArucoDetectorRef> mArucoDetector;
    int mNumMarkers;
    int mNumMarkersRaw;
    bool mRefimentAruco;
    bool mEnableCrop;

    //grid values
    std::vector < GridDetectorRef>  mGridDetector;
    glm::vec2 mGridLocation;
    glm::vec2  mGridStep;
    bool mCalculateGrid;
    bool mEnableGridPos;

    //
    std::vector<glm::vec2> mGridSizes;

    //grid calculations

    glm::vec2 posgrid;
    int mMaxMarkers;



    //GUI
    bool            mDrawGui;
    ofxDatSliderRef mGammaValue;
    ofxDatSliderRef mAlphaValue;
    ofxDatSliderRef mBetaValue;

    ofxDatButtonRef mBSingleGrid;
    ofxDatButtonRef mBFullGrid;
    ofxDatButtonRef mCamRaw;
    ofxDatButtonRef mCamCalibration;
    ofxDatButtonRef mAccurancy;
    ofxDatMatrixRef mBGridSelect;

    bool mEnableColorPros;

    debugConfiguration mConfigureMode;

    int mHighlightMarkerId;
    ofImage mBaseGrid;

    std::vector<ofVideoDevice> camlist;

    std::map<int, int>  mPrevGridArea;
    std::map<int, int>  mGridArea;

    std::map<int, int>  mPrevGridAreaS;
    std::map<int, int>  mGridAreaS;

    int   spaecialGridInter01[13];

    //city io http requests 
    CityIORef mCityIo;
    std::string mTableName;
    std::string mTableLocation;
    std::string mUrl;



};
