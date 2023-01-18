#include "GridDetector.h"

GridDetector::GridDetector(glm::vec2 dim) {
  mGridDim = dim;
  mDebugGrid = false;
  mRecordOnce = true;
  mCalibrateGrid = false;
  mCleanDone = false;
  mCurrentGridId =0;

  mRadDetection = RAD_DETECTION;
  mMaxMarkers = mGridDim.x * mGridDim.y;

  mNumRL = 0;
  mNumRM = 0;
  mNumRS = 0;
  mNumOL = 0;
  mNumOM = 0;
  mNumOS = 0;
  mNumPark = 0;

  std::vector<int> index = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,23,24,25,26,27,28,29,30,31,32,33,34,35,36 };
  for (auto& ids : index) {
      mEmptyGrid.insert(std::make_pair<int&, int>(ids, -1));
  }
  ofLog(OF_LOG_NOTICE) << "Created Grid Detector: ";
}

//-----------------------------------------------------------------------------
void GridDetector::setMaxMarkers(int max) {
  mMaxMarkers = max;
  ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers;
}

//-----------------------------------------------------------------------------
void GridDetector::setId(int id) { mId = id; }

//-----------------------------------------------------------------------------
void GridDetector::setupBlocks() {
  for (int i = 0; i < mMaxMarkers; i++) {
    QRBlockRef block = QRBlock::create();
    block->setMarkerId(i);
    mBlocks.push_back(block);
  }

  ofLog(OF_LOG_NOTICE) << "setup blocks ";
}



//-----------------------------------------------------------------------------
void GridDetector::generateGridPos(int startGridX, int startGridY, int stepX, int stepY) {
    mMarkers.clear();
    int indeY = -1;
    int indeX = 0;
    ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers; //gridPos[mId].size()
    for (int i = 0; i < mMaxMarkers; i++) {
        TangibleMarkerRef m = TangibleMarker::create();

        float x = indeX * stepX + startGridX;
        float y = indeY * stepY + startGridY;

        m->setPos(glm::vec2(x, y));
        m->setGridId(i);  //
       // m->setInteractiveId(i);

        indeX = i % 16;
        if (i % 16 == 0) {
            indeY++;
        }

        if (mId == 0) {
            int gridId = (indeX + indeY * 16 + 0);
            m->setInteractiveId(gridId);
        }
        else if (mId == 1) {
            int gridId = (indeX + (indeY + 8) * 16);
            m->setInteractiveId(gridId);
        }

        /*
        *         
        
        indeX = i % 8;
        if (i % 8 == 0) {
            indeY++;
        }
        if (mId == 0) {
            int gridId = (indeX + indeY * 16 + 0);
            m->setInteractiveId(gridId);
        }
        else if(mId == 1) {
            int gridId = (indeX + indeY * 16 + 8);
            m->setInteractiveId(gridId);
        }
        else if (mId == 2) {
            int gridId = (indeX + (indeY + 8) * 16);
            m->setInteractiveId(gridId);
        }
        else if (mId == 3) {
            int gridId = (indeX + (indeY + 8) * 16 + 8);
            m->setInteractiveId(gridId);
        }
        */
        m->setMarkerId(-1);
        mMarkers.push_back(m);

    }
}
//-----------------------------------------------------------------------------
void GridDetector::cleanDuplicatePos() {

    for (auto& mk : mMarkers) {
        for (auto & mj : mMarkers) {
            glm::vec2 posK = mk->getPos();
            glm::vec2 posJ = mj->getPos();
            float dist = glm::fastDistance(posK, posJ);
            if (dist >= 0 && dist <=10) {
                mk->setPos(posK + glm::vec2(30, 0));
                mj->setPos(posJ - glm::vec2(0, 30));
            }
        }
    }
}

//-----------------------------------------------------------------------------
void GridDetector::setupCleaner() {

  // cleaner
  mWindowCounter = 0;
  mWindowIterMax = 12; ///12
  mCleanDone = false;

  for (int i = 0; i < mMaxMarkers; i++) {
    mIdsCounter.emplace(i, 0); // mFullIds.at(i), 0);
    mProCounter.emplace(i, 0);
  }

  // ids from 0 -1000, max number of counters
  for (int i = 0; i < MAX_MARKERS; i++) {
    mCenterCounter.emplace(i, 0);
  }

  ofLog(OF_LOG_NOTICE) << "setup clean";
}

//-----------------------------------------------------------------------------
void GridDetector::setupGridJsonPos(std::string filePos) {
  ofLog(OF_LOG_NOTICE) << "Loading gridpos json: " << filePos;
  ofFile file(filePos); //"gridpos.json");
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    for (auto &gridPos : js) {
      if (i < mMaxMarkers) {
        TangibleMarkerRef m = TangibleMarker::create();
        m->setMarkerId(-1);

        int type = gridPos[to_string(i)]["type"];

        if (type == BlockType::grid) {

          float posx = gridPos[to_string(i)]["posx"];
          float posy = gridPos[to_string(i)]["posy"];
          int interactiveId = gridPos[to_string(i)]["interid"].get<int>();

          m->setRectPos(
              glm::vec2(posx - 20, posy - 20), glm::vec2(posx + 20, posy - 20),
              glm::vec2(posx + 20, posy + 20), glm::vec2(posx - 20, posy + 20));

          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);//i
          m->setInteractiveId(interactiveId);
          mMarkers.push_back(m);
        }
      }
      i++;
    }
  } else {
    ofLog(OF_LOG_NOTICE) << "cannot find file, creating default positions";
    ofLog(OF_LOG_NOTICE) << "Creating find file: "<<mId;
    generateGridPos(1200, 50, 10, 10); //generate default pos and save them

    ofJson writer;
    int i = 0;

    for (auto& mk : mMarkers) {
        ofJson pt;

        pt[to_string(i)]["posx"] = mk->getPos().x;
        pt[to_string(i)]["posy"] = mk->getPos().y;
        pt[to_string(i)]["interid"] = (int)mk->getInteractiveId();
        pt[to_string(i)]["type"] = (int)mk->getBlockType();

        writer.push_back(pt);
        i++;
    }
    ofLog(OF_LOG_NOTICE) << "json write: 0" + to_string(mId) + " - " + to_string(mId);
    ofSaveJson(filePos, writer);
  }
}

//-----------------------------------------------------------------------------
void GridDetector::generateMarkers(std::vector<int> &ids,
                                   std::vector<QRBlockRef> &blocks, bool sort) {
  mTagsIds = ids;
  mCurrBlock = blocks;

  // clasification of ids and blocks
  if (sort) {
    std::sort(mCurrBlock.begin(), mCurrBlock.end(),
              [](const QRBlockRef &lhs, const QRBlockRef &rhs) -> bool {
                return lhs->getPos().x < rhs->getPos().x;
              });
    ofLog(OF_LOG_NOTICE) << "sorted";
  }
  mNumMarkers = mCurrBlock.size();
  mTmpBlocks.push_back(mCurrBlock);
}

//-----------------------------------------------------------------------------
void GridDetector::drawBlock(float posx, float posy, float size, float space) {
  int i = 0;
  int j = 0;
  float squareSize = size;
  float squareSpace = space;
  for (auto &block : mBlocks) {

    ofSetColor(0, 255, 255);

    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    ofDrawBitmapString(block->getMarkerId(), x + squareSize / 3.0, y + squareSize * (1.0 / 3.0));
    ofDrawBitmapString(block->getType(), x + squareSize / 3.0, y + squareSize * (2.0 / 3.0));


    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::drawDetectedGridIn(float posx, float posy, float size,
                                      float space) {
  int i = 0;
  int j = 0; // mGridDim.y - 1;
  float squareSize = size;
  float squareSpace = space;
  for (auto &mk : mMarkers) {
    if (mk->isEnable()) {
      ofSetColor(13, 170, 255);
    } else {
      ofSetColor(255, 215, 13);
    }
    float x = i * space + posx;
    float y = j * space + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    ofDrawBitmapString(mk->getGridId(), x + squareSize / 3.0, y + squareSize * (1.0 / 3.0));
    ofDrawBitmapString(mk->getInteractiveId(), x + squareSize / 3.0, y + squareSize * (2.0 / 3.0));
    ofDrawBitmapString(mk->getMarkerId(), x + squareSize / 3.0, y + squareSize * (3.0 / 3.0));

    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}

void GridDetector::drawDetectedInteraction(int id, float posx, float posy, float sizeW, float sizeH, float spaceX, float spaceY) {
    int i = 0;
    int j = 0; // mGridDim.y - 1;

    for (auto& mk : mMarkers) {
        if (mk->isEnable()) {
            ofSetColor(13, 170, 255, 150);
        }
        else {
            ofSetColor(255, 215, 13, 150);
        }

        float x = i * spaceX + posx;
        float y = j * spaceY + posy;
        ofDrawRectangle(glm::vec2(x, y), sizeW, sizeH);

        ofSetColor(255);
        //ofDrawBitmapString(mk->getGridId(), x + squareSize / 4.0, y + squareSize * (1.0 / 3.0));
        ofDrawBitmapString(mk->getInteractiveId(), x + sizeW / 4.0, y + sizeH * (3.0 / 3.0));
        ofDrawBitmapString(mk->getMarkerId(), x + sizeW / 3.0, y + sizeH * (1.0 / 3.0));
        i++;
        if (i >= mGridDim.x) {
            i = 0;
            j++;
        }
    }
}
//-----------------------------------------------------------------------------
void GridDetector::drawDetectedInteraction(int id, float posx, float posy, float size, float space) {
    drawDetectedInteraction(id, posx, posy, size, size, space, space);
}

//-----------------------------------------------------------------------------
void GridDetector::drawDetectedGrid(float posx, float posy, float size,
                                    float space) {
  int i = 0;
  int j = 0;
  float squareSize = size;
  float squareSpace = space;
  for (auto &block : mCurrBlock) {
    if (block->getType() == -1) {
        ofSetColor(13, 170, 255);
    }
    else {
        ofSetColor(255, 215, 13);
    }
    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);//red
    float strx = x + squareSize / 3.0;
    float stry1 = y + squareSize * (1.0 / 3.0);
    float stry2 = y + squareSize * (2.0 / 3.0);
    ofDrawBitmapString(block->getType(), strx, stry1);
    ofDrawBitmapString(block->getMarkerId(), strx, stry2);
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::updateBlockTypes() {
  // update blocks and types
  for (auto &block : mBlocks) {
    int id = block->getMarkerId();
    for (auto &mk : mMarkers) {
      if (mk->getIdTypePair().first == id) {
        mk->updateTypePair(block->getType());
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::setGridPos(glm::vec2 mousePos) {
  if (mDebugGrid) {
    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk->getPos();
      float dist = glm::fastDistance(pos, mousePos);
      if (dist >= 0.0 && dist <= MOUSE_RADIUS) {
        mk->setPos(mousePos);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::setGridPos(glm::vec2 mousePos, int index) {
    if (mDebugGrid) {
        if (index < mMarkers.size() && index >= 0) {
            mMarkers.at(index)->setPos(mousePos);
        }
    }
}

//-----------------------------------------------------------------------------
void GridDetector::drawMarkers() {
  for (auto &mk : mMarkers) {
    glm::vec2 pos = mk->getPos();

    if (mk->isEnable()) {
      ofSetColor(255);
      ofDrawCircle(pos.x, pos.y, 7);
      ofSetColor(200, 80);
      ofDrawCircle(pos.x, pos.y, mRadDetection / 2.0);
    } else {
      ofSetColor(255, 170, 255, 150);
      ofDrawCircle(pos.x, pos.y, 4);
      ofSetColor(255, 170, 255, 75);
      ofDrawCircle(pos.x, pos.y, mRadDetection / 2.0);
    }

    //update grid position
    if (mDebugGrid) {
      ofSetColor(0, 100, 200, 100);
      ofDrawCircle(pos.x, pos.y, mRadDetection);
    }
    if(mUpdateGrid){
      if(mk->isDebugPos()){
        ofSetColor(80, 80, 150, 150);
        ofDrawCircle(pos.x, pos.y, mRadDetection);
      }
    }

    if (mk->getGridId() == mHightlightMarkeId) {
        ofSetColor(200, 13, 50);
        ofDrawCircle(pos.x, pos.y, mRadDetection/1.5);
    }

    ofSetColor(255, 13, 90);
    //ofDrawBitmapString(mk->getMarkerId(), pos.x - 25, pos.y - 2);
    ofDrawBitmapString(mk->getGridId(), pos.x - 27, pos.y - 4);
    ofDrawBitmapString(mk->getInteractiveId(), pos.x - 27, pos.y - 16);
  }
}

//-----------------------------------------------------------------------------
void GridDetector::drawRotation(){
  //draw rotation of the marker
  for (auto & detectedMk : mCurrBlock) {
    detectedMk->calculateRotation();
    glm::vec2 corner = detectedMk->getFirstCorner();
    glm::vec2 pos = detectedMk->getPos();

    ofSetColor(255);
    ofDrawLine(corner, pos);

    float angle = detectedMk->getRot();

    float px = cos(angle * TWO_PI) * 20 + pos.x;
    float py = sin(angle * TWO_PI) * 20 + pos.y;

    ofDrawLine(pos.x, pos.y, px, py);
  }
}

//-----------------------------------------------------------------------------
void GridDetector::gridPosIdInc(){
  //disable current maker
  mMarkers.at(mCurrentGridId)->disableDebugPos();

  mCurrentGridId++;
  if(mCurrentGridId >= mMaxMarkers){
    mCurrentGridId = 0;
  }

  //update current marker
  mMarkers.at(mCurrentGridId)->enableDebugPos();
}

//-----------------------------------------------------------------------------
void GridDetector::gridPosIdDec(){
  //disable current maker
  mMarkers.at(mCurrentGridId)->disableDebugPos();

  mCurrentGridId--;
  if(mCurrentGridId <= 0){
    mCurrentGridId = mMaxMarkers - 1;
  }

    //update current marker
    mMarkers.at(mCurrentGridId)->enableDebugPos();
}

//-----------------------------------------------------------------------------
// save json
void GridDetector::saveGridJson() {
  saveGridJson("gridpos_0" + to_string(mId) + ".json");
}

//-----------------------------------------------------------------------------
void GridDetector::saveGridJson(std::string fileName){
  ofJson writer;
  int i = 0;
  for (auto &mk : mMarkers) {
    ofJson pt;
    pt[to_string(i)]["posx"] = mk->getPos().x;
    pt[to_string(i)]["posy"] = mk->getPos().y;
    pt[to_string(i)]["interid"] = (int)mk->getInteractiveId();
    pt[to_string(i)]["type"] = mk->getBlockType();
    writer.push_back(pt);
    i++;
  }
  ofLog(OF_LOG_NOTICE) << "json write: 0" + to_string(mId) + " - " + to_string(mId);
  ofSaveJson(fileName, writer);
}

//-----------------------------------------------------------------------------
void GridDetector::calibrateGrid() {
  // draw grid
}

//-----------------------------------------------------------------------------
void GridDetector::recordGrid() {
  if (mRecordOnce) {
    // ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " " << mMarkers.size();
    if (mCurrBlock.size() == mMaxMarkers) {
      ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " markes = " << mMaxMarkers;

      ofLog(OF_LOG_NOTICE) << "Detect";
      // set ids
      mFullIds.clear();
      for (auto &mk : mMarkers) {
        glm::vec2 pos = mk->getPos();
        int k = 0;
        for (auto &cblock : mCurrBlock) {
          glm::vec2 cenPos = cblock->getPos();
          float dis = glm::fastDistance(cenPos, pos);
          if (dis >= 0.0 && dis <= mRadDetection) {
            mk->setMarkerId(mTagsIds.at(k));

            // got ids/
            mFullIds.push_back(mTagsIds.at(k));
            break;
          }
          k++;
        }
      }

      ofLog(OF_LOG_NOTICE) << "Done update fullids";
      ofLog(OF_LOG_NOTICE) << "Num Uniques: " << mFullIds.size();
      ofLog(OF_LOG_NOTICE) << "Done Recording";

      mRecordOnce = false;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::updateProablity() {
  // update clenaer variables
  mWindowCounter++;
  if (mWindowCounter >= mWindowIterMax) {
     // ofLog(OF_LOG_NOTICE) << "update C" << mWindowCounter << " "<<mId;
      mWindowCounter = 0;
      mCleanDone = true;
  }

  for (auto& blocks : mTmpBlocks) {
      for (auto& block : blocks) {

          glm::vec2 blockPos = block->getPos();

          int k = 0;
          for (auto& mk : mMarkers) {
              glm::vec2 boardPos = mk->getPos();
              float dis = glm::fastDistance(blockPos, boardPos);
              if (dis >= 0 && dis <= mRadDetection) {
                  mIdsCounter[k] = block->getMarkerId(); // block.mId
                  mk->incProba();
                  // not sure i need it break;
                  //break;
              }
              k++;
          }
      }
  }


}
//-----------------------------------------------------------------------------
void GridDetector::resetProbabilty() {
  // reset
  if (mCleanDone) {
    mWindowCounter = 0;
    mNumMakersAbsolute = 0;
    mCleanDone = false;
  }

  for (auto& mk : mMarkers) {
      mk->resetProba();
  }
}
//------------------------------------
void GridDetector::calculateProbabilityGrid() {
    if (mCleanDone) {
        // clasical probabilty of ocurance

        // ofLog(OF_LOG_NOTICE) << "reset proba";
   

        // ofLog(OF_LOG_NOTICE) << "calculate freq";
        // calculate the frequency of ocurance


        // ofLog(OF_LOG_NOTICE) << "Update";


        // send upd data and activations;
        int i = 0;
  
        mGridIdPair.clear();
        //ofLog(OF_LOG_NOTICE) << "Proba";
        for (auto& mk : mMarkers) {
            float proba = mk->getProba(mWindowIterMax);
            //ofLog(OF_LOG_NOTICE) << proba <<" "<< mk->getInc();


            if (proba >= 0.1) {
                mk->enableOn();
                mk->setMarkerId(mIdsCounter[i]);
                mk->updateIdPair(mIdsCounter[i]);
                mNumMakersAbsolute++;
            }
            else {
                mk->enableOff();
                mk->setMarkerId(-1);
            }
            i++;

            //either -id or off
            mGridIdPair.insert(std::make_pair <int, int >((int)mk->getInteractiveId(), (int)mk->getMarkerId()));
          //  ofLog(OF_LOG_NOTICE) << to_string(mk->getInteractiveId())<< " ";

        }

        //ofLog(OF_LOG_NOTICE) << " "+ to_string(mGridIdPair.size())<< " "<<to_string(i);
        // std::sort(gridArea01.begin(), gridArea01.end(), [](const auto & a, const auto & b) {return a.second < b.second; });

        // done activation and disactivation





        mTmpBlocks.clear();
        //ofLog(OF_LOG_NOTICE) << "done";
    }
    
}