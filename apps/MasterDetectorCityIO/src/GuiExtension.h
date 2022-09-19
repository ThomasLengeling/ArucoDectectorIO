#pragma once

/*

Thomas Sanchez Lengeling
December, 2021a

l3

*/

#include "ofMain.h"
#include "ofxDatGui.h"


class ofxDatSlider;
typedef std::shared_ptr<ofxDatSlider> ofxDatSliderRef;

class ofxDatButton;
typedef std::shared_ptr<ofxDatButton> ofxDatButtonRef;

class ofxDatMatrix;
typedef std::shared_ptr<ofxDatMatrix> ofxDatMatrixRef;

class ofxDatButton {
public:
    ofxDatButton() { ofParamBoolean = false; }
    static ofxDatButtonRef create() {
        return std::make_shared<ofxDatButton>();
    }

    void setActivation(bool active) { ofParamBoolean = active; }

    void update() { button->update(); }
    void draw() { button->draw(); }
    bool isActive() { return ofParamBoolean; }
    void toggle() {ofParamBoolean = !ofParamBoolean;}



    ofxDatGuiButton* button;
    ofParameter<bool>     ofParamBoolean;
    ofEventListener       buttonListener;

};

//Matrix Slider
class ofxDatMatrix {
public:
    ofxDatMatrix() {}
    static ofxDatMatrixRef create() {
        return std::make_shared<ofxDatMatrix>();
    }
    void update() { matrix->update(); }
    void draw() { matrix->draw(); }
    float getValue() { return mCurretValue; }

    ofxDatGuiMatrix* matrix;
    ofEventListener      matrixListener;
    int                  mCurretValue;
};

class ofxDatSlider {
public:
    ofxDatSlider() {}
    static ofxDatSliderRef create() {
        return std::make_shared<ofxDatSlider>();
    }
    void update() { slider->update(); }
    void draw() { slider->draw(); }
    float getValue() { return ofParam; }
    void setValue(float value) { ofParam = value; }

    ofxDatGuiSlider* slider;
    ofParameter<float>   ofParam;
    ofEventListener      sliderListener;

};
