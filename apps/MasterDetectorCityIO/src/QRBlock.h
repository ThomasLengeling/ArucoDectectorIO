#pragma once

/*
Class for each  Physical Block
*/

#include <memory>

#include "ofMain.h"



class QRBlock;
typedef std::shared_ptr<QRBlock> QRBlockRef;

class QRBlock {

public:
  QRBlock() {
    mPos = glm::vec2(0, 0);
    mFirstCorner = glm::vec2(0, 0);
    mRot = 0;
    mEnable = false;
    mIdType = std::make_pair(-1, 0);
  }

  static QRBlockRef create() { return std::make_shared<QRBlock>(); }

  glm::vec2 getPos() { return mPos; }
  void setPos(glm::vec2 p) { mPos = p; }

  glm::vec2 getFirstCorner(){return mFirstCorner;}
  void setFirstCorner(glm::vec2 pos){mFirstCorner = pos;}

  int getMarkerId() { return mIdType.first; }
  void setMarkerId(int id) { mIdType.first = id; }

  int getType() { return mIdType.second; }
  void setType(int type) { mIdType.second = type; }

  void setRot(float rot){mRot = rot;}
  float getRot(){return mRot;}

  void calculateRotation(){
    float dot = mPos.x * mFirstCorner.x + mPos.y * mFirstCorner.y;
    mRot = acos(dot / (glm::length(mPos) * glm::length(mFirstCorner)) );
  }

private:
  glm::vec2 mPos;
  glm::vec2 mFirstCorner;

  //rotation
  float mRot;

  bool mEnable;

  std::pair<int, int> mIdType;
};
///----------------------------------------------------
