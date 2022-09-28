//
//  DatGui.h
//  SliderDensity
//
//  Created by tom on 8/2/18.
//
//
#pragma once

class ofxDatSlider;
typedef std::shared_ptr<ofxDatSlider> ofxDatSliderRef;

class ofxDatButton;
typedef std::shared_ptr<ofxDatButton> ofxDatButtonRef;

class ofxDatToggle;
typedef std::shared_ptr<ofxDatToggle> ofxDatToggleRef;

class ofxDatMatrix;
typedef std::shared_ptr<ofxDatMatrix> ofxDatMatrixRef;



class ofxDatSlider {
public:
    ofxDatSlider() {
        mValue = 50;
    }
    static ofxDatSliderRef create() {
        return std::make_shared<ofxDatSlider>();
    }
    
    void draw(){
        mSlider->draw();
    }
    
    void update(){
        mSlider->update();
    }
    
    ofxDatGuiSlider *   mSlider;
    ofParameter<int>    ofParamInt;
    ofEventListener     mSliderListener;
    int                 mValue;
    
};


class ofxDatButton {
public:
    ofxDatButton() {
        mActivate =false;
    }
    static ofxDatButtonRef create() {
        return std::make_shared<ofxDatButton>();
    }
    
    void draw(){
        mButton->draw();
    }
    
    void update(){
        mButton->update();
    }
    
    ofxDatGuiButton     * mButton;
    ofParameter<bool>     ofParamBoolean;
    ofEventListener       mButtonListener;
    bool                  mActivate;
    
};

class ofxDatToggle {
public:
    ofxDatToggle() {
        mActivate = false;
    }
    static ofxDatToggleRef create() {
        return std::make_shared<ofxDatToggle>();
    }

    void draw() {
        mToggle->draw();
    }

    void update() {
        mToggle->update();
    }

    ofxDatGuiToggle  *    mToggle;
    ofParameter<bool>     ofParamBoolean;
    ofEventListener       mToggleListener;
    bool                  mActivate;

};

class ofxDatMatrix {
public:
    ofxDatMatrix() {
        mActivate = false;
    }
    static ofxDatMatrixRef create() {
        return std::make_shared<ofxDatMatrix>();
    }

    void draw() {
        mMatrix->draw();
    }

    void update() {
        mMatrix->update();
    }

    ofxDatGuiMatrix*      mMatrix;
    ofParameter<bool>     ofParamBoolean;
    ofEventListener       mMatrixListener;
    bool                  mActivate;

};


