#pragma once

#include "ofMain.h"
#include "DatGui.h"


class ModuleLayer;
typedef std::shared_ptr<ModuleLayer> ModuleLayerRef;


class ModuleLayer {
public:
    ModuleLayer() {
        ofTrueTypeFont::setGlobalDpi(96*2);

        verdana14.load("ofxbraitsch/fonts/NotoSansTC-Regular.otf", 13, true, true);
       // verdana14.setLineHeight(18.0f);
        verdana14.setLetterSpacing(1.037);
       // verdana14.setGlobalDpi(96 * 2);

        verdana18.load("ofxbraitsch/fonts/NotoSansTC-Regular.otf", 19, true, true);
        // verdana14.setLineHeight(18.0f);
        verdana18.setLetterSpacing(1.037);

         mActiveToggle = false;
         mActiveTogglePrev = false;

    }

    static ModuleLayerRef create() {
        return std::make_shared<ModuleLayer>();
    }

    void draw() {

        if (mActive) {
            if (button != NULL)
                button->draw();

            if (toggle != NULL)
                toggle->draw();

            if (slider != NULL)
                slider->draw();

            if (matrix != NULL) {
                matrixLabel->draw();
                matrix->draw();
            }

            ofSetColor(255);
            verdana14.drawString(name, pos.x, pos.y);
        }
    }

    void update() {
        if (mActive) {
            if (button != NULL)
                button->update();

            if (slider != NULL)
                slider->update();

            if (toggle != NULL) {


                toggle->update();
                mActiveTogglePrev = mActiveToggle;
                bool active = toggle->mToggle->getChecked();
                //cout << active;
                mActiveToggle = active;
                if (mActiveTogglePrev != mActiveToggle) {
                    mGuiJson["toggle"]["active"] = mActiveToggle;
                }
            }

            if (matrix != NULL) {
                matrix->update();
                matrixLabel->update();



                auto labelName = matrix->mMatrix->getHoverLabel();
                if (labelName != "") {
                    matrixLabel->mLabel->setLabel(labelName);
                    int v = matrix->mMatrix->getHoverId();
                   // matrix->mMatrix->setSelected(v);

                    mGuiJson["toggle_array"]["curr_active"] = v;
                    mInteraction = true;
                }
                else {
                    matrixLabel->mLabel->setLabel(matrix->mCurrentName);
                }

                if (toggle != NULL) {
                    bool checked = toggle->mToggle->getChecked();
                    if (!checked) {
                        for (int i = 0; i < matrix->mMatrix->getNumButtons(); i++) {
                            matrix->mMatrix->setHover(i, false);

                        }
                        matrix->mMatrix->dissableAll();
                        matrixLabel->mLabel->setLabel("");
                    }
                }

            }
        }
    }
    
    glm::vec2 pos;

    ofxDatButtonRef     button;
    ofxDatToggleRef     toggle;

    ofxDatSliderRef     slider;
    ofxDatMatrixRef     matrix;
    ofxDatLabelRef      matrixLabel;

    ofTrueTypeFont	verdana14;
    ofTrueTypeFont	verdana18;
    std::string name;

    bool mActiveToggle;
    bool mActiveTogglePrev;

    bool mActive;

    ofJson mGuiJson;
    std::string jsonName;

    bool mInteraction;
};