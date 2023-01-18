#pragma once

#pragma once

/*
Class for Maneging communications between the detector and CityIO
*/

#include "ofMain.h"
#include "ofxHTTP.h"


class CityIO;
typedef std::shared_ptr<CityIO> CityIORef;


class CityIO {
public:
    CityIO(std::string table = "test");
    ~CityIO() {}

    static CityIORef create() {
        return std::make_shared<CityIO>();
    }

    void setTable(std::string t);
    void setBaseUrl(std::string url );

    ofJson excuteGetRequest(std::string request);

    bool  updateGridPost(ofJson grid);

  
    void computeGeoGrid();

    void executePostGeoGrid(std::map<int, int> tags, bool overide = false);

    void excuteGetRequestTUI(ofJson request);

private:

    std::string mBaseUrl;
    std::string mTable;

    //geogrid json
    glm::vec2 mGridSize;
    glm::vec2 mInteractiveGrid;
    ofJson mGeoGrid;
    glm::vec2 mCoorGrid;

    std::map< std::string, int > mTypes;

    std::map<std::string, int > mDefaultTypes;
    std::map<int, std::string > mDefaultTypesRev;
    std::map<int, int > mDefaultColor;
    //std::map<int, int > mDefaultHeight;

    std::map<int, std::vector<int> > mDefaultHeight;


    std::map<int, int> mInteractiveIds;

    //store JSON information
    ofJson geogrid;
    ofJson geogriddata;
};

