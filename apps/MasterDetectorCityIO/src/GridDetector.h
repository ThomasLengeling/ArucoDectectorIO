/*

Thomas Sanchez Lengeling
December, 2021

L3

*/

#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>
#include <glm/gtx/fast_square_root.hpp>

// addons
#include "ofxCv.h"
#include "ofxOpenCv.h"

#include "QRBlock.h"
#include "CommonTypes.h"
#include "ArucoDetector.h"
#include "CamCapture.h"
#include "TangibleMarker.h"

#define RAD_DETECTION 30
#define MOUSE_RADIUS  17
#define MAX_MARKERS 100

class GridDetector;
typedef std::shared_ptr<GridDetector> GridDetectorRef;

class GridDetector{
public:

    GridDetector(glm::vec2 dim);

    static GridDetectorRef create(glm::vec2 dim) {
        return std::make_shared<GridDetector>(dim);
    }

    glm::vec2 getDim(){return mGridDim;}

    void setId(int id);
    int getId(){return mId;}

    int getNumMarkers(){return mNumMarkers;}
    int getNumMarkersAbolute() { return mNumMakersAbsolute; }

    void setMaxMarkers(int max);
    int getMaxMarkers(){return mMaxMarkers;}

    void setupGridJsonPos(std::string filePos);
    void generateGridPos(int startGridX = 100, int startGridY= 100, int stepX= 50, int stepY= 50);
    void setupBlocks();

    void generateMarkers(std::vector<int> & ids, std::vector<QRBlockRef> & blocks, bool sort = false );

    void updateBlockTypes();

    void drawMarkers( );

    //update grid positions
    void enableDebugGrid(bool toggle = true) { mDebugGrid = toggle; }
    void toogleUpdateGrid(){mUpdateGrid = !mUpdateGrid;}

    void gridPosIdInc();
    void gridPosIdDec();

    void calibrateGrid();
    void recordGrid();

    void cleanDuplicatePos();

    void setupCleaner();
    
    void updateProablity();
    void resetProbabilty();

    void calculateProbabilityGrid();
    bool isDoneCleaner(){return mCleanDone;}

    void setGridPos(glm::vec2 mousePos);
    void setGridPos(glm::vec2 mousePos, int index);

    void setHighlightMarkerId(int id) { mHightlightMarkeId = id; }

    //save json files
    void saveGridJson();
    void saveGridJson(std::string fileName);

    //draw detected grid
    void drawDetectedGrid(float posx, float posy, float size = 20, float space = 5);
    void drawDetectedInteraction(int id, float posx, float posy, float sizeW, float sizeH, float spaceX, float spaceY);
    void drawDetectedInteraction(int id, float posx, float posy, float size = 20, float space = 5);
    void drawDetectedGridIn(float posx, float posy, float size = 20, float space = 5);

    void drawBlock(float posx, float posy, float size = 20, float space = 5);

    void drawRotation();

    void setWindowIter(int value) {mWindowIterMax = value;}

    std::map<int, int> getGridInter() {
        return mGridIdPair;
    }

    std::map<int, int > getEmptyGrid() { return mEmptyGrid; }

private:

    //tags
    std::map<int, int> mGridIdPair;
    std::map<int, int> mEmptyGrid;

    //dimentions
    glm::vec2  mGridDim;

    //id;
    int mId;

    //max markers possible
    int mMaxMarkers;

    //number of markers detected
    int mNumMarkers; //raw

    int mNumMakersAbsolute; 

    //debug grid
    bool mDebugGrid;

    //update grid
    bool mUpdateGrid;
    int mCurrentGridId;

    bool mRecordOnce;

    //highlight grid marker
    int mHightlightMarkeId;

    //clearner
    int mWindowIterMax;
    int mWindowCounter;

    bool mCleanDone;

    ///radar
    int mNumRL;
    int mNumRM;
    int mNumRS;

    int mNumOL;
    int mNumOM;
    int mNumOS;
    int mNumPark;

    //movable part of fixed
    // Free piece does not take into account
    bool mFreePiece;

    //radius of detection
    float mRadDetection;

    //calibrate grid
    bool mCalibrateGrid;

    std::map<int, int> mIdsCounter;
    std::map<int, int> mProCounter;
    std::map<int, int> mCenterCounter;

    // check if found marker
    std::vector<TangibleMarkerRef> mMarkers;

    std::vector<QRBlockRef> mCurrBlock;
    std::vector<std::vector<QRBlockRef>> mTmpBlocks;

    std::vector<QRBlockRef> mBlocks;
    std::vector<int> mTagsIds;
    std::vector<int> mFullIds;

    int specialGrid02;
};
