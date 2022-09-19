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
    std::vector<int> gridIds01 = { 216,217,
264,265,266,267,268,269,270,271,272,273,274,275,
313,314,315,316,317,318,319,320,321,322,323,324,325,
362,363,364,365,366,367,368,369,370,371,372,373,374,375,
412,413,414,415,416,417,418,419,420,421,422,423,424,425,
461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,
510,511,512,513,514,515,516,517,518,519,520,521,522,523,524,525,
559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,
608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,624,625,
657,658,659,660,661,662,663,664,665,666,667,668,669,670,671,672,673,674,675,
706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,
755,756,757,758,759,760,761,762,763,764,765,766,767,768,769,770,771,772,773,774,775,
800,801,802,803,804,805,806,807,808,809,810,811,812,813,814,815,816,817,818,819,820,821,822,823,824,825 };

    std::vector<int> gridIds02 = { 36,37,
85,86,87,88,96,97,98,99,
135,136,137,138,142,143,144,145,146,147,148,149,
185,186,187,188,189,190,191,192,193,194,195,196,197,198,
234,235,236,237,238,239,240,241,242,243,244,245,246,247,
276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,
326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,
376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,
426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,441,442,443,444,
476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,
526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,
576,577,578,579,580,581,582,583,584,585,586,587,588,589,590,591,
626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,
676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,
726,727,728,729,730,731,732,733,734,735,736,737,738,739,
776,777,778,779,780,781,782,783,784,785,786,787,788 };

    std::vector<int> gridIds03 = { 36,37,
85,86,87,88,96,97,98,99,
135,136,137,138,142,143,144,145,146,147,148,149,
185,186,187,188,189,190,191,192,193,194,195,196,197,198,
234,235,236,237,238,239,240,241,242,243,244,245,246,247,
276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,
326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,
376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,
426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,441,442,443,444,
476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,
526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,
576,577,578,579,580,581,582,583,584,585,586,587,588,589,590,591,
626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,
676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,
726,727,728,729,730,731,732,733,734,735,736,737,738,739,
776,777,778,779,780,781,782,783,784,785,786,787,788 };

    std::vector<int> gridIds04 = { 826,827,828,829,830,831,832,833,834,835,836,837,
876,877,878,879,880,881,882,883,884,885,886,
926,927,928,929,930,931,932,933,934,935,
976,977,978,979,980,981,982,983,984,
1026,1027,1028,1029,1030,1031,1032,1033,
1076,1077,1078,1079,1080,1081,1082,1083,
1126,1127,1128,1129,1130,1131,1132,
1176,1177,1178,1179,1180,1181,
1126,1127,1128,1129,1130,
1276,1277,1278,
1326,1327,
1376,1377,
1426,
1476 };

    std::vector<std::vector<int>> gridPos = { gridIds01, gridIds02, gridIds03, gridIds04 };

    mMarkers.clear();
    int indeY = 0;
    int indeX = 0;
    ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers; //gridPos[mId].size()
    for (int i = 0; i < mMaxMarkers; i++) {
        TangibleMarkerRef m = TangibleMarker::create();

        float x = indeX * stepX + startGridX;
        float y = indeY * stepY + startGridY;

        m->setPos(glm::vec2(x, y));
        m->setGridId(i);  //
        m->setInteractiveId(i);
       // m->setInteractiveId(gridPos[mId].at(i));
        m->setMarkerId(-1);
        mMarkers.push_back(m);
        indeX++;
        if (indeX >= mGridDim.x) {
            indeY++;
            indeX = 0;
        }
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
  mWindowIterMax = 1; ///12
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


            if (proba >= 0.2) {
                mk->enableOn();
                mk->setMarkerId(mIdsCounter[i]);
                mk->updateIdPair(mIdsCounter[i]);
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