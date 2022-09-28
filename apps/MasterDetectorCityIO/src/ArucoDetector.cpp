#include "ArucoDetector.h"

using namespace std;
using namespace cv;

ArucoDetector::ArucoDetector() {
  mMinFoundId = 100;
  mMaxFoundId = 0;
  mMarkerInfo = true;
  mNumFoundMarkers = 0;


}

void ArucoDetector::resetMinMax() {
  mMinFoundId = 100;
  mMaxFoundId = 0;
}

void ArucoDetector::generateDetectorParams() {
    generateDetectorParams("camera_config.json");
}

void ArucoDetector::generateDetectorParams(std::string sfile) {
    ofLog(OF_LOG_NOTICE) << "Loading Dectector Params: "<< sfile;
    ofFile file(sfile);
    if (file.exists()) {
        ofJson  jsParams;
        file >> jsParams;

        double adaptiveThreshConstant       = jsParams["camera"]["adaptiveThreshConstant"].get<double>();
        int adaptiveThreshWinSizeMax        = jsParams["camera"]["adaptiveThreshWinSizeMax"].get<int>();
        int adaptiveThreshWinSizeMin        = jsParams["camera"]["adaptiveThreshWinSizeMin"].get<int>();
        int adaptiveThreshWinSizeStep       = jsParams["camera"]["adaptiveThreshWinSizeStep"].get<int>();
        
        int cornerRefinementMaxIterations   = jsParams["camera"]["cornerRefinementMaxIterations"].get<int>();
        double cornerRefinementMinAccuracy  = jsParams["camera"]["cornerRefinementMinAccuracy"].get<double>();
        int cornerRefinementWinSize         = jsParams["camera"]["cornerRefinementWinSize"].get<int>();
        
        double errorCorrectionRate          = jsParams["camera"]["errorCorrectionRate"].get<double>();
        int markerBorderBits                = jsParams["camera"]["markerBorderBits"].get<int>();
        double maxErroneousBitsInBorderRate = jsParams["camera"]["maxErroneousBitsInBorderRate"].get<double>();

        double maxMarkerPerimeterRate  = jsParams["camera"]["maxMarkerPerimeterRate"].get<double>();
        double minCornerDistanceRate   = jsParams["camera"]["minCornerDistanceRate"].get<double>();
        int minDistanceToBorder        = jsParams["camera"]["minDistanceToBorder"].get<int>();

        double minMarkerDistanceRate   = jsParams["camera"]["minMarkerDistanceRate"].get<double>();
        double minMarkerPerimeterRate  = jsParams["camera"]["minMarkerPerimeterRate"].get<double>();

        double minOtsuStdDev              = jsParams["camera"]["minOtsuStdDev"].get<double>();

        double perspectiveRemoveIgnoredMarginPerCell = jsParams["camera"]["perspectiveRemoveIgnoredMarginPerCell"].get<double>();
        int perspectiveRemovePixelPerCell            = jsParams["camera"]["perspectiveRemovePixelPerCell"].get<int>();
        double polygonalApproxAccuracyRate           = jsParams["camera"]["polygonalApproxAccuracyRate"].get<double>();



        int dictionaryId = cv::aruco::DICT_4X4_50; //0
        detectorParams = cv::aruco::DetectorParameters::create();

        detectorParams->adaptiveThreshConstant = adaptiveThreshConstant;
        detectorParams->adaptiveThreshWinSizeMax = adaptiveThreshWinSizeMax;
        detectorParams->adaptiveThreshWinSizeMin = adaptiveThreshWinSizeMin;
        detectorParams->adaptiveThreshWinSizeStep = adaptiveThreshWinSizeStep;

        detectorParams->cornerRefinementMaxIterations = cornerRefinementMaxIterations;
        detectorParams->cornerRefinementMinAccuracy = cornerRefinementMinAccuracy;
        detectorParams->cornerRefinementWinSize = cornerRefinementWinSize;

        detectorParams->errorCorrectionRate = errorCorrectionRate;
        detectorParams->markerBorderBits = markerBorderBits;
        detectorParams->maxErroneousBitsInBorderRate = maxErroneousBitsInBorderRate;

        detectorParams->maxMarkerPerimeterRate = maxMarkerPerimeterRate;
        detectorParams->minCornerDistanceRate = minCornerDistanceRate;
        detectorParams->minDistanceToBorder = minDistanceToBorder;

        detectorParams->minMarkerDistanceRate = minMarkerDistanceRate;
        detectorParams->minMarkerPerimeterRate = minMarkerPerimeterRate;

        detectorParams->minOtsuStdDev = minOtsuStdDev;

        detectorParams->perspectiveRemoveIgnoredMarginPerCell = perspectiveRemoveIgnoredMarginPerCell;
        detectorParams->perspectiveRemovePixelPerCell = perspectiveRemovePixelPerCell;
        detectorParams->polygonalApproxAccuracyRate = polygonalApproxAccuracyRate;


        //pedfine marker
     
        dictionary = cv::aruco::getPredefinedDictionary(
            cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    }
    else {
        ofLog(OF_LOG_NOTICE) << "cannot find file, creating default paramas";
    }
}

/*
https://docs.opencv.org/4.5.5/d5/dae/tutorial_aruco_detection.html

https://docs.opencv.org/4.5.5/d1/dcd/structcv_1_1aruco_1_1DetectorParameters.html

*/
void ArucoDetector::setupCalibration(int markersX, int markersY) {
  float markerLength     = 0.0162;     // 0.0165
  float markerSeparation = 0.0042; // 0045
  int dictionaryId       = cv::aruco::DICT_4X4_50; //0
  std::string outputFile = "./cal.txt";

  int calibrationFlags = 0;
  float aspectRatio = 1;

  detectorParams = cv::aruco::DetectorParameters::create();

  detectorParams->adaptiveThreshWinSizeMin = 3; //20
  detectorParams->adaptiveThreshWinSizeMax = 37; //50
  detectorParams->adaptiveThreshWinSizeStep = 4;

  detectorParams->perspectiveRemovePixelPerCell = 10; // 10
  detectorParams->perspectiveRemoveIgnoredMarginPerCell = .04; //.15
  detectorParams->errorCorrectionRate = 0.3;
  detectorParams->maxErroneousBitsInBorderRate = 0.6;//35
  detectorParams->minOtsuStdDev = 5;//
  //detectorParams->maxErroneousBitsInBorderRate = 0.1;
  //detectorParams->maxMarkerPerimeterRate = 5;

  detectorParams->maxErroneousBitsInBorderRate = 15;

  detectorParams->polygonalApproxAccuracyRate = 0.05;//.1

  //detectorParams->markerBorderBits = 0;

  detectorParams->minDistanceToBorder = 5;

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
void ArucoDetector::detectMarkers(cv::Mat &inputVideo, bool refiment) {
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

  //aruco::detectMarkers(input, dictionary, corners, arucoIds, detectorParams);
  // if (refiment) {
  //aruco::refineDetectedMarkers(input, board, corners, arucoIds, rejected);
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
   // ofLog(OF_LOG_NOTICE) << "non size Aruco detector";
  }

  // create video output
  input.copyTo(mVidMat);
  ofxCv::toOf(mVidMat, mVidImg.getPixels());
  mVidImg.update();
}

//calibrate Camera sequence
void ArucoDetector::calibrateCamera(){


}

// Calibrate
//--------------------------------------------------------------
bool ArucoDetector::readDetectorParameters(
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
bool ArucoDetector::saveCameraParams(const std::string &filename,
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
