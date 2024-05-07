#include "CityIO.h"

CityIO::CityIO(std::string table) {
	this->mTable = table;
    mGridSize = glm::vec2(0, 0);
}

//---------------------------------------------------------------------------
void CityIO::setTable(std::string table) {
    this->mTable = table;
}

//---------------------------------------------------------------------------
void CityIO::setBaseUrl(std::string baseUrl) {
    this->mBaseUrl = baseUrl;
}


//---------------------------------------------------------------------------
void CityIO::computeTableProperties() {
    {
        ofFile file("geogrid_properties.json");
        if (file.exists()) {
            ofJson propertiesjson;
            propertiesjson << file;

            ofJson dimjson = propertiesjson["header"];
            //get max rows colums
            if (dimjson.contains("ncols") && dimjson.contains("nrows")) {
                mGridSize.x = dimjson["ncols"];
                mGridSize.y = dimjson["nrows"];
                ofLog(OF_LOG_NOTICE) << "grid: " << mGridSize.x << " " << mGridSize.y << std::endl;
            }

            if (dimjson.contains("latitude") && dimjson.contains("longitude")) {
                mCoorGrid.x = dimjson["latitude"];
                mCoorGrid.y = dimjson["longitude"];
            }
        }
        else {
            ofLog(OF_LOG_NOTICE) << "not found";
        }
    }

    {
        ofFile file("aruco_codes.json");
        if (file.exists()) {
            ofJson arucoJson;
            arucoJson << file;
            ofJson blockCodes = arucoJson["Codes"];

            
            mTypes.insert(std::make_pair("None", -1));
            for (auto& types : blockCodes.items()) {
                mTypes.insert(std::make_pair(types.value()["name"].get<string>(), types.value()["aruco_id"].get<int>()));
            }

            for (auto& types : blockCodes.items()) {
                std::vector<int> height;
                ofLog(OF_LOG_NOTICE) << "type: " << types.value() << " " << types.key() << std::endl;

                string name = types.value()["name"].get<string>();
                //if (types.value()["height"].is_object() || types.value()["height"].is_number_integer()) {
                //    height = types.value()["height"].get<int>();
               // }
                if (types.value()["height"].is_array()) {
                    if (types.value()["height"].size() > 1) {
                        height = { types.value()["height"][0].get<int>(),
                            types.value()["height"][1].get<int>(),
                            types.value()["height"][2].get<int>() };
                    }
                    else {
                        height = { types.value()["height"][0].get<int>(), types.value()["height"][0].get<int>(),  types.value()["height"][0].get<int>() };

                    }
                }
                if (types.value()["height"].is_number_integer()) {
                    height = { types.value()["height"].get<int>(), types.value()["height"].get<int>(),  types.value()["height"].get<int>() };
                }

                ofColor col(types.value()["color"][0].get<int>(),
                    types.value()["color"][1].get<int>(),
                    types.value()["color"][2].get<int>(),
                    types.value()["color"][3].get<int>());

                // int aid = (mTypes.find(name))->second;
                mDefaultColor.insert(std::make_pair(mTypes.find(name)->second, col.getHex()));
                mDefaultHeight.insert(make_pair(mTypes.find(name)->second, height));
                mDefaultTypes.insert(make_pair(name, mTypes.find(name)->second));

                mDefaultTypesRev.insert(make_pair(mTypes.find(name)->second, name));

                //map aruco id with the types on the grid
               // int arucoId = mDefaultTypes.find(types.key())->second;
               // mTypes.insert(std::make_pair(types.key(), arucoId));
            }

            ofLog(OF_LOG_NOTICE) << "filled types";
            for (auto& colors : mDefaultColor) {
                //ofLog(OF_LOG_NOTICE) << (int)ofColor::fromHex(colors.first).r << " " << (int)ofColor::fromHex(colors.first).g << " " << colors.second;
            }
            for (auto& colors : mDefaultTypes) {
                // ofLog(OF_LOG_NOTICE) << colors.first << " " << colors.second;
            }
        }
        else {
            ofLog(OF_LOG_NOTICE) << "not found";
        }
    }

    //load geogriddata 
    {
        ofFile file("geogriddata.json");
        if (file.exists()) {
            geogriddata << file;
        }
        else {
            ofLog(OF_LOG_NOTICE) << "not found "<< geogriddata;
        }
    }

    //coordinates
    int coordinc = 0;

    glm::vec2 coord(0, 0);

    //main grid id;
    int ids = 0;
    int pid = 0;
   

    //get interactive modules
        //interactive grid
    int idP = 0;
    int intGridX = 0;
    int intGridY = 0;
    int intGridIndex = 0;
    int idgrid = 0;

    int minX = 9999;
    int maxX = 0;
    int minY = 999;
    int maxY = 0;



   
    ofFile file("geogrid_features.json");
    if (file.exists()) {
        ofJson geogridFeatures;
        geogridFeatures << file;
        ofLog(OF_LOG_NOTICE) << "Interactive size: " << geogridFeatures.size();
        for (auto& types : geogridFeatures.items()) {
            string name = "";

            int idxy = types.value()["properties"]["id"].get<int>();

            coord.x = int(idxy % (int)mGridSize.x);
            coord.y = int(idxy / (int)mGridSize.y);

            bool interactive = false;
            if (types.value()["properties"]["interactive"].is_boolean()) {
                interactive = types.value()["properties"]["interactive"].get<bool>();
            }
            if (types.value()["properties"]["interactive"].is_string()) {
                if (types.value()["properties"]["interactive"].get<string>() == "Web") {
                    interactive = true;
                    ofLog(OF_LOG_NOTICE) << "web ";
                }
            }

            if (types.value()["properties"]["name"].is_string()) {
                name = types.value()["properties"]["name"].get<string>();
            }

            if (interactive == true) {
                idP = idgrid;
                idgrid = types.value()["properties"]["id"].get<int>();

                if (minX > coord.x) {
                    minX = coord.x;
                }
                if (maxX < coord.x) {
                    maxX = coord.x;
                }
                if (minY > coord.y) {
                    minY = coord.y;
                }
                if (maxY < coord.y) {
                    maxY = coord.y;
                }

                ofLog(OF_LOG_NOTICE) << "id: " << idgrid;
                ofLog(OF_LOG_NOTICE) << "interactive: " << interactive;
                ofLog(OF_LOG_NOTICE) << "name: " << name;

                //got ids;

                intGridX++;
                if (idgrid - idP > 1) { //jump of row
                    intGridY++;
                    intGridX = 0;
                }
                intGridIndex++;

            }
        }
    }

    ofLog(OF_LOG_NOTICE) << "bdox: " << minX << " " << maxX << " - "<< minY << " " << maxY << std::endl;
    ofLog(OF_LOG_NOTICE) << "bdox id: " << minY * mGridSize.x + minX << " " << maxY * mGridSize.x + maxX;
    ofLog(OF_LOG_NOTICE) << maxX - minX + 1 << " " << maxX - minX + 1 << " " << (maxX - minX + 1) * (maxX - minX + 1) << std::endl;

    if (intGridY != 0) {
        intGridX = (intGridIndex / intGridY);
    }
    ofLog(OF_LOG_NOTICE) << "grid: " << intGridIndex<<" "<< intGridX << " " << intGridY << std::endl;
    mInteractiveGrid = glm::vec2(intGridX, intGridY);

    for (auto& v : mInteractiveIds) {
        ofLog(OF_LOG_NOTICE) << v.first << " " << v.second;
    }
}

//---------------------------------------------------------------------------
bool CityIO::updateGridPost(ofJson grid) {
    return false;
}


void CityIO::printValueDifferences(std::map<int, int>& map1, std::map<int, int>& map2) {
    // Assuming map1 and map2 have the same keys
    for (const auto& pair : map1) {
        int key = pair.first;
        int value1 = pair.second;
        int value2 = map2.at(key); // Safe to use at() since keys are the same in both maps

        if (value1 != value2) {
            std::cout << "Key " << key << " has different values: "
                << value1 << " (map1) vs " << value2 << " (map2)." << std::endl;
        }
    }
}

//---------------------------------------------------------------------------
void CityIO::executePostGeoGrid(std::map<int, int>  arucotags, bool overide) {

    int index = 0;
    geogriddataNew.clear();
    if (!geogriddata.empty()) {  //check if geogriddata is not empty

        geogriddataNew = ofJson::array();

        ofLog(OF_LOG_NOTICE) << arucotags.size() << std::endl;
        for (auto& title : arucotags) {
            int id = title.first;
            int arucoId = title.second;
            ofJson::value_type piece = geogriddata.at(id);

            //ofLog(OF_LOG_NOTICE) << "ArucoId: " << arucoId;

            //only interactive pieces
            bool interactive = false;
            if (geogriddata.at(id)["interactive"].is_boolean()) {
                interactive = geogriddata.at(id)["interactive"].get<bool>();
            }
            if (geogriddata.at(id)["interactive"].is_string()) {
                if (geogriddata.at(id)["interactive"].get<string>() == "Web") {
                    interactive = true;
                    //ofLog(OF_LOG_NOTICE) << "web";
                }
            }
            if (interactive) {
                if (isTagExist(arucoId)) {

                    ofColor col = ofColor::fromHex(mDefaultColor[arucoId]);
                    piece["color"] = ofJson::array({ (int)col.r, (int)col.g, (int)col.b, geogriddata.at(id)["color"][3] });

                    if (arucoId == -1) {
                        piece["height"] = { 0, 0, 0 };
                    }
                    else {
                        piece["height"] = { mDefaultHeight[arucoId].at(0),
                                            mDefaultHeight[arucoId].at(1),
                                            mDefaultHeight[arucoId].at(2) };
                    }

                    piece["name"] = getNameFromArucoId(arucoId);
                }
            }
            geogriddataNew.push_back(piece);
        }
    }
}

bool CityIO::isTagExist(int id) {
    if (mDefaultTypesRev.find(id) != mDefaultTypesRev.end()) {
        return true;
    }
    return false;
}


std::string CityIO::getNameFromArucoId(int id) {
    if (mDefaultTypesRev.find(id) != mDefaultTypesRev.end()) {
        return string(mDefaultTypesRev[id]);
    }
    else {
        return string(mDefaultTypesRev[-1]);;
    }
}