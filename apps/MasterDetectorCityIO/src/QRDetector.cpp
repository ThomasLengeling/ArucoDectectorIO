#include "QRDetector.h"

using namespace std;
using namespace cv;

QRDetector::QRDetector() {
  mMinFoundId = 50;
  mMaxFoundId = 0;
  mMarkerInfo = false;
}

void QRDetector::resetMinMax() {
  mMinFoundId = 50;
  mMaxFoundId = 0;
}

void QRDetector::setupCalibration(int markersX, int markersY) {
  float markerLength     = 0.0162;     // 0.0165
  float markerSeparation = 0.0042; // 0045
  int dictionaryId       = cv::aruco::DICT_4X4_50; //0
  std::string outputFile = "./cal.txt";

  int calibrationFlags = 0;
  float aspectRatio = 1;

  detectorParams = cv::aruco::DetectorParameters::create();

  detectorParams->adaptiveThreshWinSizeMin = 25; //20
  detectorParams->adaptiveThreshWinSizeMax = 80; //50
  detectorParams->adaptiveThreshWinSizeStep = 5;

  detectorParams->perspectiveRemovePixelPerCell = 10; // 10
  detectorParams->perspectiveRemoveIgnoredMarginPerCell = 0.3;
  detectorParams->errorCorrectionRate = 0.53;
  detectorParams->maxErroneousBitsInBorderRate = 0.3;
  detectorParams->minOtsuStdDev = 2;

  bool refindStrategy = false;

  //pedfine marker
  dictionary = cv::aruco::getPredefinedDictionary(
      cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));


  // create board object
  cv::Ptr<cv::aruco::GridBoard> gridboard = cv::aruco::GridBoard::create(
      markersX, markersY, markerLength, markerSeparation, dictionary);
  board = gridboard.staticCast<cv::aruco::Board>();
}

//---------------------------------------------------------------------------
void QRDetector::detectMarkers(cv::Mat &inputVideo, bool refiment) {
  // detect markers
  std::vector<int> arucoIds;
  std::vector<std::vector<cv::Point2f>> corners;
  std::vector<std::vector<cv::Point2f>> rejected;

  cv::Mat input;
  inputVideo.copyTo(input);
  inputVideo.copyTo(mVidCopy);

  mTagsIds.clear();
  mBlock.clear();

  // ofxCv::imitate(input, inputVideo);

  aruco::detectMarkers(input, dictionary, corners, arucoIds, detectorParams);
  // if (refiment) {
  aruco::refineDetectedMarkers(input, board, corners, arucoIds, rejected);
  //}

  if (arucoIds.size() > 0) {

    if (mMarkerInfo) {
      aruco::drawDetectedMarkers(input, corners, arucoIds);
    }
    InputArrayOfArrays cornersDetected = corners;
    InputArray idsDetected = arucoIds;

    mNumFoundMarkers = cornersDetected.total();

    // analize which markers are activated in the grid.
    for (int i = 0; i < cornersDetected.total(); i++) {
      cv::Mat currentMarker = cornersDetected.getMat(i);
      cv::Point2f cent(0, 0);

      for (int p = 0; p < 4; p++) {
        cent += currentMarker.ptr<cv::Point2f>(0)[p];
      }

      cent = cent / 4.;
      QRBlockRef cva = QRBlock::create();
      cva->setPos(glm::vec2(cent.x, cent.y));
      cva->setFirstCorner(glm::vec2(currentMarker.ptr<cv::Point2f>(0)[0].x, currentMarker.ptr<cv::Point2f>(0)[0].y));

      // get ids
      if (idsDetected.total() != 0) {
        int id = idsDetected.getMat().ptr<int>(0)[i];
        mTagsIds.push_back(id);
        cva->setMarkerId(id);

        if (mMinFoundId > id) {
          mMinFoundId = id;
        }
        if (mMaxFoundId < id) {
          mMaxFoundId = id;
        }
      }

      mBlock.push_back(cva);
    }

  } else {
    ofLog(OF_LOG_NOTICE) << "non size Aruco detector";
  }

  // create video output
  input.copyTo(mVidMat);
  ofxCv::toOf(mVidMat, mVidImg.getPixels());
  mVidImg.update();
}

//calibrate Camera sequence
void QRDetector::calibrateCamera(){


}

// Calibrate
//--------------------------------------------------------------
bool QRDetector::readDetectorParameters(
    std::string filename, cv::Ptr<cv::aruco::DetectorParameters> &params) {
  FileStorage fs(filename, FileStorage::READ);
  if (!fs.isOpened())
    return false;
  fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
  fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
  fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
  fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
  fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
  fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
  fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
  fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
  fs["minDistanceToBorder"] >> params->minDistanceToBorder;
  fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
  //fs["cornerRefinementMethod"] >> params->cornerRefinementMethod;
  fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
  fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
  fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
  fs["markerBorderBits"] >> params->markerBorderBits;
  fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
  fs["perspectiveRemoveIgnoredMarginPerCell"] >>
      params->perspectiveRemoveIgnoredMarginPerCell;
  fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
  fs["minOtsuStdDev"] >> params->minOtsuStdDev;
  fs["errorCorrectionRate"] >> params->errorCorrectionRate;
  return true;
}

//--------------------------------------------------------------
bool QRDetector::saveCameraParams(const std::string &filename,
                                  cv::Size imageSize, float aspectRatio,
                                  int flags, const cv::Mat &cameraMatrix,
                                  const cv::Mat &distCoeffs,
                                  double totalAvgErr) {

  FileStorage fs(filename, FileStorage::WRITE);
  if (!fs.isOpened())
    return false;

  time_t tt;
  time(&tt);
  struct tm *t2 = localtime(&tt);
  char buf[1024];
  strftime(buf, sizeof(buf) - 1, "%c", t2);

  fs << "calibration_time" << buf;

  fs << "image_width" << imageSize.width;
  fs << "image_height" << imageSize.height;

  if (flags & CALIB_FIX_ASPECT_RATIO)
    fs << "aspectRatio" << aspectRatio;

  if (flags != 0) {
    sprintf(buf, "flags: %s%s%s%s",
            flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
  }

  fs << "flags" << flags;

  fs << "camera_matrix" << cameraMatrix;
  fs << "distortion_coefficients" << distCoeffs;

  fs << "avg_reprojection_error" << totalAvgErr;

  return true;
}
