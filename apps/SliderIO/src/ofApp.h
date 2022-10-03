#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxNetwork.h"
#include "DatGui.h"
#include "CityIO.h"
#include "ModuleLayer.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        //upd
        void generateSliders();
        void drawGui();
        void updateGui();

        void loadJsonNet();
        void loadJsonGui();

        //layers

        
        //upd msg
        ofxUDPManager mUDPUrbanSim;
        ofxUDPManager mUDPRadar;
        ofxUDPManager mUDPAnalysis;

        std::string mUrbanSimIp;
        std::string mRadarIp;
        std::string mAnalysisIp;

        int mUrbanSimPort;
        int mRadarPort;
        int mAnalysisPort;

        ofxDatButtonRef mButtonReset;


        std::vector<ModuleLayerRef> mModulesGui;

};
