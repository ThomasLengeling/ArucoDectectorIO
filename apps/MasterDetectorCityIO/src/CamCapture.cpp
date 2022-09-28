#include "CamCapture.h"

using namespace std;
using namespace cv;


//----------------------------------------------------------------------------
CamCapture::~CamCapture(){
  mCam.close();
  cout<<"cam closed "<<mId<<std::endl;
}

//----------------------------------------------------------------------------
CamCapture::CamCapture(glm::vec2 dims) {
  mDim = dims;
  mGamma = 0.65;
  mActivateCrop = false;
  mActivateCam = true;
  mEnableRemote = false;
  mFps = 30;

  mCornerUp = glm::vec2(0, 0);
  mCornerDown = glm::vec2(dims.x, dims.y);
  mDisp = glm::vec2(23, 23);

  mFboResolution.allocate(mDim.x, mDim.y, GL_RGBA);
  mPixs.allocate(mDim.x, mDim.y, OF_PIXELS_RGBA);

  // clean start with Fbos
  mFboResolution.begin();
  ofClear(0, 0, 0, 255);
  mFboResolution.end();

  //setup NDI

  //mNdiReceiver.SetSenderName("ndicam");
  mSpoutReceiver.init("remotecam");
    // ofImage
  mSpoutTex.allocate(dims.x, dims.y, GL_RGB);

  // ofPixels
  mSpoutPixels.allocate(dims.x, dims.y, GL_RGB);
  temp.allocate(dims.x, dims.y, GL_RGB);

  temp.begin();
  ofClear(0);
  temp.end();

}

//-----------------------------------------------------------------------------
void CamCapture::setupCam(int id, int fps) {
  mCamId = id;
  mFps = fps;

  mCam.setDeviceID(mCamId);
  mCam.setVerbose(true);
  mCam.setDesiredFrameRate(mFps);
  mCam.initGrabber(mDim.x, mDim.y);
  mCam.setUseTexture(true);

  ofLog(OF_LOG_NOTICE) << "loaded Cam: " << mCamId << " " << mId << " "
                       << mDim.x << " " << mDim.y << "  " << mCam.getWidth()
                       << " " << mCam.getHeight()<<" fps: "<<mFps<<std::endl;
}

//-----------------------------------------------------------------------------
void CamCapture::setupGUISwap(float x, float y) {
  mSwapCamId = ofxDatMatrix::create();
  mSwapCamId->matrix =new ofxDatGuiMatrix("Cam Selector: " + to_string(mId), 4, true);
  mSwapCamId->matrix->setRadioMode(true);
  mSwapCamId->matrix->setOpacity(0.7);
  mSwapCamId->matrix->setWidth(390, .4);
  mSwapCamId->matrix->setPosition(x, y);
  mSwapCamId->matrix->onMatrixEvent([&](ofxDatGuiMatrixEvent v) {
    ofLog(OF_LOG_NOTICE) << "Id: " << mCamId << " New Index: " << v.child<< std::endl;
    mCam.close();

    mCamId = v.child;
    mCam.setDeviceID(mCamId);
    // mCam.setVerbose(true);
    mCam.setDesiredFrameRate(mFps);
    mCam.initGrabber(mDim.x, mDim.y);
  });
}

//-----------------------------------------------------------------------------
void CamCapture::setupVideo(std::string name) {
  // mVideoName = name;
  // mVideoInput.load(mVideoName);
  // mVideoInput.play();

  ofLog(OF_LOG_NOTICE) << "loaded Video: " << mVideoName << " " << mId;
}

//-----------------------------------------------------------------------------
bool CamCapture::updateImage() {
  bool newFrame = false;
  if (mActivateCam) {
    mCam.update();
    newFrame = mCam.isFrameNew();
    if (newFrame) {
      // mFboResolution.begin();
      // mCam.draw(0, 0, mDim.x, mDim.y);
      // mFboResolution.end();
      // ofLog(OF_LOG_NOTICE) << " " << newFrame << " ";
    }
  } else if(mEnableRemote) {
      mSpoutReceiver.receive(mSpoutTex);
      
      temp.begin();
      mSpoutTex.draw(0, 0);
      temp.end();

      ofTexture tex = temp.getTexture();
      tex.readToPixels(mSpoutPixels);

     //mNdiReceiver.ReceiveImage(mNdiImage);
     //mNdiReceiver.ReceiveImage(mNdiPixels);
    // mVideoInput.update();
    // newFrame = mVideoInput.isFrameNew();
  }
  return newFrame;
}

//-----------------------------------------------------------------------------
ofPixels & CamCapture::getImgPixels() {
    if (mActivateCam) {
        return mCam.getPixels();
    }
    else if(mEnableRemote) {
       // auto pixels = mNdiImage.getPixels();
       // mNdiImage.update();
        
     
        
        return mSpoutPixels;
    }
  //return mCam.getPixels();
}

//-----------------------------------------------------------------------------
void CamCapture::setCropUp(glm::vec2 up) {
  mCornerUp = up;
}

//-----------------------------------------------------------------------------
void CamCapture::setCropDown(glm::vec2 down) {
  mCornerDown = down;
}

//-----------------------------------------------------------------------------
void CamCapture::drawImage(int x, int y, int w, int h) {
  if (mActivateCam) {
    mCam.draw(x, y, w, h);
  }
  else if (mEnableRemote) {
    mSpoutTex.draw(x, y, w, h);
  }
}

//-----------------------------------------------------------------------------
void CamCapture::drawImage(int x, int y) {
  if (mActivateCam) {
    mCam.draw(x, y, mDim.x, mDim.y);
  } else if (mEnableRemote) {
      mSpoutTex.draw(x, y, mDim.x, mDim.y);
  }
}

//-----------------------------------------------------------------------------
void CamCapture::drawGUISwap() {
  // draw GUI
  mSwapCamId->draw();
}

//-----------------------------------------------------------------------------
void CamCapture::updateGUISwap() {
  // update GUI
  mSwapCamId->update();
}

//-----------------------------------------------------------------------------
void CamCapture::adjustGamma(cv::Mat &img) {
  if (!img.empty()) {

    cv::Mat imgGamma;
    img.convertTo(imgGamma, -1, mAlpha, mBeta);

    cv::Mat lookUpTable(1, 256, CV_8U);
    unsigned char *p = lookUpTable.ptr();
    for (int i = 0; i < 256; i++) {
      p[i] = saturate_cast<unsigned char>(pow(i / 255.0, mGamma) * 255.0);
    }
    cv::LUT(imgGamma, lookUpTable, imgGamma);

    imgGamma.copyTo(img);
  }


}
//-----------------------------------------------------------------------------
void CamCapture::resetCrop() {

  mCornerUp = glm::vec2(0, 0);
  mCornerDown = glm::vec2(mDim.x, mDim.y);
  mDisp = glm::vec2(5, 5);

}
//-----------------------------------------------------------------------------
void CamCapture::cropImg(cv::Mat &inputVideo) {

  mRoi.x = abs(mCornerUp.x);
  mRoi.y = abs(mCornerUp.y);
  mRoi.width = abs(mCornerDown.x - mCornerUp.x);//  mLength.x;// +mDisp.x;
  mRoi.height = abs(mCornerDown.y - mCornerUp.y);  //mLength.y;// +mDisp.y;


  //ofLog(OF_LOG_NOTICE) << mRoi.x << " " << mRoi.y << " - " << mRoi.width << " " << mRoi.height;
  // Copy the data into new matrix
  if (mRoi.width <= mDim.x && mRoi.height <= mDim.y &&
      (mRoi.x >= 0 && mRoi.y >= 0 && mRoi.width <= mDim.x &&  mRoi.height <= mDim.y)) {
      cv::Mat cutMat(inputVideo, mRoi);
      cutMat.copyTo(mCropMat);
  }
  else {
      inputVideo.copyTo(mCropMat);
      ofLog(OF_LOG_NOTICE) << "error crop dims Dims";
  }

}
//-----------------------------------------------------------------------------
void CamCapture::drawCropRoi() {
  if (mActivateCrop) {
    ofSetColor(0, 200, 125, 50);
  } else {
    ofSetColor(0, 220, 195, 155);
  }

  ofBeginShape();
  ofVertex(mCornerUp);
  ofVertex(glm::vec2(mCornerDown.x, mCornerUp.y));
  ofVertex(mCornerDown);
  ofVertex(glm::vec2(mCornerUp.x, mCornerDown.y));
  ofEndShape();

  ofSetColor(255, 50, 50, 155);

  ofDrawCircle(mCornerDown.x- 30, mCornerDown.y-30, 30, 30);
  ofDrawCircle(mCornerUp.x +30, mCornerUp.y+30, 30, 30);
}
//-----------------------------------------------------------------------------
void CamCapture::drawCropImg() {
  ofImage imgCut;
  ofxCv::toOf(mCropMat, imgCut.getPixels());
  imgCut.update();

  ofSetColor(200, 200);
  imgCut.draw(mCornerUp.x, mCornerUp.y, abs(mCornerDown.x - mCornerUp.x), abs(mCornerDown.y - mCornerUp.y));

  //ofxCv::drawMat(mCropMat, 0, 200, 200, 200);
  //imgCut.draw(0, 400, 200, 200);
}

//-----------------------------------------------------------------------------
void CamCapture::updateCorners(){

  //update corners
  //mInputQuad[0]
  //mInputQuad[1]
  //mInputQuad[2]
  //mInputQuad[3]
}

//
void CamCapture::calculatePerspective(){

  //mCropMat
  // Input Quadilateral or Image plane coordinates
   Point2f inputQuad[4];

   inputQuad[0] = Point2f(0, 0);
   inputQuad[1] = Point2f(0, 0);
   inputQuad[2] = Point2f(0, 0);
   inputQuad[3] = Point2f(0, 0);

   // Output Quadilateral or World plane coordinates
   Point2f outputQuad[4];
   outputQuad[0] = Point2f(0, 0);
   outputQuad[1] = Point2f(0, 0);
   outputQuad[2] = Point2f(0, 0);
   outputQuad[3] = Point2f(0, 0);

   // Lambda Matrix
   Mat lambda;
   lambda = getPerspectiveTransform( inputQuad, outputQuad );

   // Apply the Perspective Transform just found to the src image
   warpPerspective(mCropMat, mPerspectiveMat, lambda, mPerspectiveMat.size());

   //
}
