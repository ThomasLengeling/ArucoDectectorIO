#include "ofApp.h"

int main(){
    ofGLWindowSettings settings;
    settings.setSize(1920, 1080);
    settings.windowMode = OF_WINDOW;
    settings.setGLVersion(4, 1); // programmable renderer

    auto mainWindow = ofCreateWindow(settings);
    auto mainApp = make_shared<ofApp>();

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}
