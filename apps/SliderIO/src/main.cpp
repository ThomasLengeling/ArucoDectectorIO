#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main(){
    ofGLFWWindowSettings settings;
    settings.setSize(1920, 1200);
    settings.windowMode = OF_WINDOW;
    settings.resizable = false;
    settings.decorated = false;
    settings.setGLVersion(4, 1); // programmable renderer

    auto mainWindow = ofCreateWindow(settings);
    auto mainApp = make_shared<ofApp>();

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();

}
