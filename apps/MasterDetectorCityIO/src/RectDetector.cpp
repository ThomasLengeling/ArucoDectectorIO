#include "RectDetector.h"

//--------------------------------------------------------------
RectDetector::RectDetector() {
  for (int i = 0; i < 4; i++) {
    mCornerVertex.push_back(glm::vec2(0, 0));
  }
}
//-----------------------------------------------------------------------------
void RectDetector::setRectPos(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2,
                              glm::vec2 p3) {
  mCornerVertex.at(0) = p0;
  mCornerVertex.at(1) = p1;
  mCornerVertex.at(2) = p2;
  mCornerVertex.at(3) = p3;
}
//-----------------------------------------------------------------------------
void RectDetector::setRectPos(std::vector<glm::vec2> &vec) {
  // setPos
  mCornerVertex = vec;
}
//-----------------------------------------------------------------------------
void RectDetector::drawRect() {
  ofSetColor(255, 50);
  ofBeginShape();
  for (auto &v : mCornerVertex) {
    ofVertex(v);
  }
  ofEndShape();

  ofSetColor(255, 150);
  for (auto &v : mCornerVertex) {
    ofDrawCircle(v.x, v.y, 2);
  }
}

//-----------------------------------------------------------------------------
void RectDetector::setPos(int i, glm::vec2 pos) {
  if (i >= 0 && i <= 3) {
    mCornerVertex.at(i) = pos;
  }
}
//-----------------------------------------------------------------------------
void RectDetector::subdivde() {}
