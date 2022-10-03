#pragma once

#include "ofMain.h"
#include "DatGui.h"


class ModuleLayer;
typedef std::shared_ptr<ModuleLayer> ModuleLayerRef;


class ModuleLayer {
public:
    ModuleLayer() {
        ofTrueTypeFont::setGlobalDpi(72);

        verdana14.load("ofxbraitsch/fonts/NotoSansTC-Regular.otf", 16, true, true);
        verdana14.setLineHeight(18.0f);
        verdana14.setLetterSpacing(1.037);
    }

    static ModuleLayerRef create() {
        return std::make_shared<ModuleLayer>();
    }

    void draw() {

        ofSetColor(255);
        verdana14.drawString(name, pos.x, pos.y);

        if(button != NULL)
        button->draw();

        if (slider != NULL)
        slider->draw();

        if (matrix != NULL)
        matrix->draw();
    }
    void update() {
        if (button != NULL)
        button->update();

        if (slider != NULL)
        slider->update();

        if (matrix != NULL)
        matrix->update();
    }
    
    glm::vec2 pos;

    ofxDatButtonRef     button;
    ofxDatSliderRef     slider;
    ofxDatMatrixRef     matrix;

    ofTrueTypeFont	verdana14;
    std::string name;
};