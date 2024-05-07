#include "TangibleMarker.h"

//--------------------------------------------------------------
ProbabiltyAruco::ProbabiltyAruco() {
  mProba = 0.0;
  mInc = 0;
}
//--------------------------------------------------------------
void ProbabiltyAruco::incProba() { mInc++; }
float ProbabiltyAruco::getProba(int maxNum) {

  return ((float)mInc / (float)maxNum);
}
//--------------------------------------------------------------
void ProbabiltyAruco::resetProba() {
  mProba = 0.0;
  mInc = 0;
}

//------------------------------------------------------------------------------
void TangibleMarker::makePairIdType(int id, int type) {
  mIdType = std::make_pair(id, type);
}
//------------------------------------------------------------------------------
void TangibleMarker::updateTypePair(int type) { mIdType.second = type; }
void TangibleMarker::updateIdPair(int id) { mIdType.first = id; }
std::pair<int, int> TangibleMarker::getIdTypePair() { return mIdType; }
