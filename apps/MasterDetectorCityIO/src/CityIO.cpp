#include "CityIO.h"

CityIO::CityIO(std::string table) {
	this->mTable = table;
    mGridSize = glm::vec2(0, 0);

    //fill default types
    //aruco code + types
    mTypes.insert(std::make_pair("Commercial R&D - lowrise", 30));
    mTypes.insert(std::make_pair("Commercial offices - highrise", 7));
    mTypes.insert(std::make_pair("Commercial offices and R&D  - medium rise", 21));
    mTypes.insert(std::make_pair("Cultural - entertainment", 36));
    mTypes.insert(std::make_pair("MIXED-USE-OFFICE - highrise",10));
    mTypes.insert(std::make_pair("MIXED-USE-OFFICE - medium rise",28));
    mTypes.insert(std::make_pair("MIXED-USE-RESIDENTIAL - highrise",35));
    mTypes.insert(std::make_pair("MIXED-USE-RESIDENTIAL - medium rise",13));
    mTypes.insert(std::make_pair("Public Parks & Open Space",43));
    mTypes.insert(std::make_pair("Residentail - high rise",38));
    mTypes.insert(std::make_pair("Residential - low rise",34));  //or 17
    mTypes.insert(std::make_pair("Residential - medium rise", 17));  //34
    mTypes.insert(std::make_pair("Higher Education", 48));
    mTypes.insert(std::make_pair("Primary Education", 6));

    mTypes.insert(std::make_pair("MIXED-USE-RESIDENTIAL + OFFICE - medium rise", 4));
    mTypes.insert(std::make_pair("MIXED-USE-RESIDENTIAL + OFFICE - highrise", 8));

    //mTypes.insert(std::make_pair("Park", 12));
    //mTypes.insert(std::make_pair("Retail", 13));
    //mTypes.insert(std::make_pair("Light Industrial", 94));
    //mTypes.insert(std::make_pair("Institutional", 15));
    mTypes.insert(std::make_pair("None", -1));
    //mTypes.insert(std::make_pair("Religious", 17));
    //mTypes.insert(std::make_pair("Healthcare", 18));
    //mTypes.insert(std::make_pair("Residential Low Density", 19));
    //mTypes.insert(std::make_pair("Office Tower", 1));
    //mTypes.insert(std::make_pair("Residential Complex", 30));
    //mTypes.insert(std::make_pair("Residential Tower", 10));
    //mTypes.insert(std::make_pair("Residential Single", 37));


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
ofJson  CityIO::excuteGetRequest(std::string getRequestStr) {
    std::string getUrl = mBaseUrl + "/api/table/" + mTable + "/" + getRequestStr;

    ofLog(OF_LOG_NOTICE) << "HTTP GET: " << getUrl << std::endl;

    ofxHTTP::Client client;
    ofJson streamRequest = "";

    // Create a request.
    ofxHTTP::GetRequest request(getUrl);

    // Create a context.
    ofxHTTP::Context context;

    // Set custom session settings.
    //
    // See the class documentation for many additional settings.
    ofxHTTP::ClientSessionSettings sessionSettings;

    // Use an iPhone X user agent string.
    //sessionSettings.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1");

    // Set a 60 second keep-alive timeout (default is 8 seconds).
    //sessionSettings.setKeepAliveTimeout(Poco::Timespan::SECONDS * 60);

    // Save the session settings with the context.
    context.setClientSessionSettings(sessionSettings);

    // Do the query!
    try
    {
        // Execute the request within the given context.
        auto response = client.execute(context, request);

        // Check the response.
        if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            // A successful response.
            ofLogNotice("ofApp::setup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";

            // Buffer the response, or otherwise consume the stream.
            //ofBuffer buffer(response->stream());
            streamRequest = response->json();

           
            ofLogNotice("ofApp::setup") << "Content Begin";

            std::cout << streamRequest.dump(4) << std::endl;

            ofLogNotice("ofApp::setup") << "Content End";
        }
        else
        {
            ofLogError("ofApp::setup") << response->getStatus() << " " << response->getReason();
        }
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("ofApp::setup") << exc.displayText();
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::setup") << exc.what();
    }
    return streamRequest;
}


//---------------------------------------------------------------------------
void CityIO::computeGeoGrid() {
    std::string request = "GEOGRID/properties/header/";
    ofJson streamRequest = excuteGetRequest(request);
    mGeoGrid = streamRequest;
    //std::cout << streamRequest.dump(4) << std::endl;

    //get max rows colums
    if (streamRequest.contains("ncols") && streamRequest.contains("nrows")) {
        mGridSize.x = streamRequest["ncols"];
        mGridSize.y = streamRequest["nrows"];
        ofLog(OF_LOG_NOTICE) << "grid: "<< mGridSize.x<<" "<< mGridSize.y<< std::endl;
    }

    if (streamRequest.contains("latitude") && streamRequest.contains("longitude")) {
        mCoorGrid.x = streamRequest["latitude"];
        mCoorGrid.y = streamRequest["longitude"];
    }

    //obtain types
    request = "GEOGRID/properties/types/";
    streamRequest = excuteGetRequest(request);

    for (auto& types : streamRequest.items()) {
        std::vector<int> height;
        ofLog(OF_LOG_NOTICE) << "type: " << types.value() << " " << types.key() << std::endl;

        string name = types.value()["name"].get<string>();
        //if (types.value()["height"].is_object() || types.value()["height"].is_number_integer()) {
        //    height = types.value()["height"].get<int>();
       // }
        if (types.value()["height"].is_array()) {
            height = { types.value()["height"][0].get<int>(),
                types.value()["height"][1].get<int>(),
                types.value()["height"][2].get<int>() };
        }
        if( types.value()["height"].is_number_integer()) {
            height = { types.value()["height"].get<int>(), types.value()["height"].get<int>(),  types.value()["height"].get<int>()};
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

    

    //coordinates

    int coordinc = 0;

    glm::vec2 coord(0, 0);
    request = "GEOGRID/features/"; // "GEOGRIDDATA/";
    streamRequest = excuteGetRequest(request);
    ofLog(OF_LOG_NOTICE) << "Interactive size: " << streamRequest.size();
    for (auto& types : streamRequest.items()) {

        //convert id to x, y coordinates;
        int idxy = types.value()["properties"]["id"].get<int>();

        coord.x = idxy % (int)mGridSize.x;
        coord.y = idxy / (int)mGridSize.y;

    }
    
    //get interactive data information
    //request = "GEOGRID/features/";
    //streamRequest = excuteGetRequest(request);
    //ofLog(OF_LOG_NOTICE) << "Interactive size: " << streamRequest.size();

    //main grid id;
    int ids = 0;
    int pid = 0;
    
    //int  id = streamRequest.at(0)["properties"]["interactive"].get<int>();;
    //for (auto& types : streamRequest.items()) {
    //    ids++;        
    //}
    //ofLog(OF_LOG_NOTICE) << "Max id: " << ids;

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

    

    request = "GEOGRID/features/"; //"GEOGRIDDATA/";
    streamRequest = excuteGetRequest(request);
    geogriddata = streamRequest;
    ofLog(OF_LOG_NOTICE) << "Interactive size: " << streamRequest.size();
    for (auto & types : streamRequest.items()) {
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
            if ( idgrid - idP > 1) { //jump of row
                intGridY++;
                intGridX = 0;
            }
            intGridIndex++;

        }

        //ofLog(OF_LOG_NOTICE) << "type: " << types.value() << " " << types.key() << std::endl;

        //map aruco id with the types on the grid
        //int arucoId = mDefaultTypes.find(types.key())->second;
        //mTypes.insert(std::make_pair(types.key(), arucoId));
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

//---------------------------------------------------------------------------
void CityIO::executePostGeoGrid(std::map<int, int>  arucotags, bool overide) {
    

    int index = 0;
    if (!geogriddata.empty()) {  //check if geogriddata is not empty
        
        ofJson geogriddatNew = ofJson::array();

        
        ofLog(OF_LOG_NOTICE) << arucotags.size() << std::endl;
        for (auto& title : arucotags) {
            int id = title.first;
            int arucoId = title.second;
            ofJson::value_type piece;

            //only interactive pieces
            bool interactive = false;
            if (geogriddata.at(id)["properties"]["interactive"].is_boolean()) {
                interactive = geogriddata.at(id)["properties"]["interactive"].get<bool>();
            }
            if (geogriddata.at(id)["properties"]["interactive"].is_string()) {
                if (geogriddata.at(id)["properties"]["interactive"].get<string>() == "Web") {
                    interactive = true;
                    ofLog(OF_LOG_NOTICE) << "web";
                }
            }

            if (interactive) {
                ofColor col = ofColor::fromHex(mDefaultColor[arucoId]);
                piece["color"] = ofJson::array({ (int)col.r, (int)col.g, (int)col.b, geogriddata.at(id)["properties"]["color"][3] });

                if (arucoId == -1) {
                    piece["height"] = { 0, 0, 0 };
                }
                else {
                    piece["height"] = { mDefaultHeight[arucoId].at(0),
                                        mDefaultHeight[arucoId].at(1),
                                        mDefaultHeight[arucoId].at(2) };
                }//get aruco value;
                //if (geogriddata.at(id)["properties"]["height"].is_array()) {
                //    piece["height"] = geogriddata.at(id)["properties"]["height"];
               // }


                piece["name"] = std::string(mDefaultTypesRev[arucoId]);
                if (arucoId == 4 || arucoId == 8) {
                    piece["name"] = "None";
                    piece["height"] = 0;
                    ofLog(OF_LOG_NOTICE) << "interactive none " << arucoId;
                }


            }
            else {
                piece["color"] = ofJson::array({ geogriddata.at(id)["properties"]["color"][0], geogriddata.at(id)["properties"]["color"][1],geogriddata.at(id)["properties"]["color"][2], geogriddata.at(id)["properties"]["color"][3] });
                
                //if (geogriddata.at(id)["properties"]["height"].is_array()) {
                    piece["height"] = geogriddata.at(id)["properties"]["height"];
                //}
                piece["name"] = geogriddata.at(id)["properties"]["name"].get<string>();

                
                if (arucoId == 4 || arucoId == 8) {
                    piece["name"] = "None";
                    piece["height"] = 0;
                    ofLog(OF_LOG_NOTICE) << "none none " << arucoId;
                }
               
            }

            piece["interactive"] = geogriddata.at(id)["properties"]["interactive"];
            if (geogriddata.at(id)["properties"]["id"].is_number_integer()) {
                piece["id"] = geogriddata.at(id)["properties"]["id"].get<int>();
            }
            if (geogriddata.at(id)["properties"]["lu"].is_string()) {
                piece["lu"] = geogriddata.at(id)["properties"]["lu"].get<std::string>();
            }

            if (piece["name"].get<std::string>() == "") {
                piece["name"] = "None";
                piece["height"] = 0;
                ofLog(OF_LOG_NOTICE) << "none "<<arucoId;
            }

            geogriddatNew.push_back(piece);

            
        }

        //ofLog(OF_LOG_NOTICE) << geogriddatNew.dump(4) << std::endl;

        //
        std::string postUrl = mBaseUrl + "/api/table/" + mTable + "/GEOGRIDDATA/";

        ofLog(OF_LOG_NOTICE) << "HTTP POST: " << postUrl << std::endl;

        // Create a client.
        ofxHTTP::Client client;

        // Create a request.
        ofxHTTP::JSONRequest request(postUrl);

        // Set the json request data.
        request.setJSON(geogriddatNew);

        try {
            // Execute the request.
            auto response = client.execute(request);

            // Check the response.
            if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
            {
                // A successful response.
                ofLogNotice("ofApp::setup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";

                // Buffer the response, or otherwise consume the stream.
                ofJson responseJson = response->json();
                Poco::StreamCopier::copyStream(response->stream(), std::cout);

                // Flush the input stream.
                std::cout << std::endl;

                ofLogNotice("ofApp::setup") << "Sent Data to CityIO Completed";
                //ofLog(OF_LOG_NOTICE) << geogriddatNew.dump(4) << std::endl;
                ofLog(OF_LOG_NOTICE) << responseJson.dump(4) << std::endl;
               // ofLogNotice("ofApp::setup") << "Content End";
            }
            else
            {
                ofLogError("ofApp::setup") << response->getStatus() << " " << response->getReason();
            }
        }
        catch (const Poco::Exception& exc)
        {
            ofLogError("ofApp::setup") << exc.displayText();
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::setup") << exc.what();
        }
    }
    else {
        
        ofLogError("NOTICE") << "geojson empty, read geogriddata first";
    }

}

void CityIO::excuteGetRequestTUI(ofJson requestTUI) {

    if(!requestTUI.empty()){
        //
        std::string postUrl = mBaseUrl + "/api/table/" + mTable + "/tui/";

        ofLog(OF_LOG_NOTICE) << "HTTP POST: " << postUrl << std::endl;

        // Create a client.
        ofxHTTP::Client client;

        // Create a request.
        ofxHTTP::JSONRequest request(postUrl);

        // Set the json request data.
        request.setJSON(requestTUI);

        try {
            // Execute the request.
            auto response = client.execute(request);

            // Check the response.
            if (response->getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
            {
                // A successful response.
                ofLogNotice("ofApp::setup") << "Response success, expecting " << response->estimatedContentLength() << " bytes.";

                // Buffer the response, or otherwise consume the stream.
                ofJson responseJson = response->json();
                Poco::StreamCopier::copyStream(response->stream(), std::cout);

                // Flush the input stream.
                std::cout << std::endl;

                ofLogNotice("ofApp::setup") << "Sent TUI to CityIO Completed";
                ofLog(OF_LOG_NOTICE) << responseJson.dump(4) << std::endl;
                ofLogNotice("ofApp::setup") << "Content End";
            }
            else
            {
                ofLogError("ofApp::setup") << response->getStatus() << " " << response->getReason();
            }
        }
        catch (const Poco::Exception& exc)
        {
            ofLogError("ofApp::setup") << exc.displayText();
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::setup") << exc.what();
        }
    }
    else {

        ofLogError("NOTICE") << "TUIO empty or not found";
    }

}