#pragma once

#pragma once

/*
Class the sends the detected blocks to a UDP port
*/

#include "ofMain.h"
#include "ofxNetwork.h"

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

    bool  updateGridPost(ofJson grid);
    void  printValueDifferences(std::map<int, int>& map1, std::map<int, int>& map2);
  
    void computeTableProperties();

    void executePostGeoGrid(std::map<int, int> tags, bool overide = false);

    std::string getNameFromArucoId(int id);

    ofJson getGeoGridData() {
        return geogriddata;
    }
    ofJson getUpdatedGeoGridData() {
        return geogriddataNew;
    }

    bool isTagExist(int id);


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
    //store JSON information
    ofJson geogriddataNew;
};

