#include "ArucoDetector.h"

using namespace std;
using namespace cv;

ArucoDetector::ArucoDetector() {
  mMinFoundId = 50;
  mMaxFoundId = 0;
  mMarkerInfo = true;
  mNumFoundMarkers = 0;

  resetCalibration();
  cameraMatrix = 0;
  distCoeffs = 0;
}

void ArucoDetector::resetMinMax() {
  mMinFoundId = 50;
  mMaxFoundId = 0;
}

void ArucoDetector::generateDetectorParams() {
    generateDetectorParams("aruco_config.json");
}

void ArucoDetector::generateDetectorParams(std::string sfile) {
    ofLog(OF_LOG_NOTICE) << "Loading Dectector Params: "<< sfile;
    ofFile file(sfile);
    if (file.exists()) {
        ofJson  jsParams;
        file >> jsParams;
        ofJson arucoConfig = jsParams["aruco"];

        double adaptiveThreshConstant       = arucoConfig["adaptiveThreshConstant"].get<double>();
        int adaptiveThreshWinSizeMax        = arucoConfig["adaptiveThreshWinSizeMax"].get<int>();
        int adaptiveThreshWinSizeMin        = arucoConfig["adaptiveThreshWinSizeMin"].get<int>();
        int adaptiveThreshWinSizeStep       = arucoConfig["adaptiveThreshWinSizeStep"].get<int>();

        int cornerRefinementMaxIterations   = arucoConfig["cornerRefinementMaxIterations"].get<int>();
        double cornerRefinementMinAccuracy  = arucoConfig["cornerRefinementMinAccuracy"].get<double>();
        int cornerRefinementWinSize         = arucoConfig["cornerRefinementWinSize"].get<int>();

        double errorCorrectionRate          = arucoConfig["errorCorrectionRate"].get<double>();
        int markerBorderBits                = arucoConfig["markerBorderBits"].get<int>();
        double maxErroneousBitsInBorderRate = arucoConfig["maxErroneousBitsInBorderRate"].get<double>();

        double maxMarkerPerimeterRate  = arucoConfig["maxMarkerPerimeterRate"].get<double>();
        double minCornerDistanceRate   = arucoConfig["minCornerDistanceRate"].get<double>();
        int minDistanceToBorder        = arucoConfig["minDistanceToBorder"].get<int>();

        double minMarkerDistanceRate   = arucoConfig["minMarkerDistanceRate"].get<double>();
        double minMarkerPerimeterRate  = arucoConfig["minMarkerPerimeterRate"].get<double>();

        double minOtsuStdDev              = arucoConfig["minOtsuStdDev"].get<double>();

        double perspectiveRemoveIgnoredMarginPerCell = arucoConfig["perspectiveRemoveIgnoredMarginPerCell"].get<double>();
        int perspectiveRemovePixelPerCell            = arucoConfig["perspectiveRemovePixelPerCell"].get<int>();
        double polygonalApproxAccuracyRate           = arucoConfig["polygonalApproxAccuracyRate"].get<double>();



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

  generateDetectorParams();

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
  if (useCalibration) {
      aruco::detectMarkers(input, dictionary, corners, arucoIds, detectorParams, rejected, cameraMatrix, distCoeffs);
  }
  else {

      aruco::detectMarkers(input, dictionary, corners, arucoIds, detectorParams);
  }
  // if (refiment) {
  //aruco::refineDetectedMarkers(input, board, corners, arucoIds, rejected);
  //}

  if (arucoIds.size() > 0) {

    if (mMarkerInfo) {
      aruco::drawDetectedMarkers(input, corners, arucoIds);
    }

    if (useCalibration) {
        std::vector<cv::Vec3d> rvecs, tvecs;
       // cv::aruco::estimatePoseSingleMarkers(corners, 0.03, cameraMatrix, distCoeffs, rvecs, tvecs);

       // for (int i = 0; i < arucoIds.size(); i++) {
           // cv::drawFrameAxes(input, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.02);
       // }
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

    //calibration
  if (calibrationProcess) {
      // collected frames for calibration

      if (captureFrame) {
          if (arucoIds.size() > 0) {
              cout << "Frame captured" << endl;
              caliAllCorners.push_back(corners);
              caliAllIds.push_back(arucoIds);
              imgSize = input.size();
              captureFrame = false;
              captureCount++;
          }
      }
  }


  // create video output
  input.copyTo(mVidMat);
  ofxCv::toOf(mVidMat, mVidImg.getPixels());
  mVidImg.update();
}

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

//---------------------------------------------------------------------
// calibrate Camera sequence
void ArucoDetector::resetCalibration() {
    caliAllCorners.clear();
    caliAllIds.clear();

    calibrationFlags = 0;
    useCalibration = false;
    captureFrame = false;
    calibrationProcess = false;


    captureCount = 0;
}
//---------------------------------------------------------------------
void ArucoDetector::calibrateCameraProcess() {
    if (captureCount > 10) {
        std::vector< cv::Mat > rvecs, tvecs;
        double repError;
        float aspectRatio = 1;



        if (calibrationFlags & CALIB_FIX_ASPECT_RATIO) {
            cameraMatrix = Mat::eye(3, 3, CV_64F);
            cameraMatrix.at< double >(0, 0) = aspectRatio;
        }

        calibrationFlags |= CALIB_ZERO_TANGENT_DIST;

        // prepare data for calibration
        vector< vector< Point2f > > allCornersConcatenated;
        vector< int > allIdsConcatenated;
        vector< int > markerCounterPerFrame;
        markerCounterPerFrame.reserve(caliAllCorners.size());
        for (unsigned int i = 0; i < caliAllCorners.size(); i++) {
            markerCounterPerFrame.push_back((int)caliAllCorners[i].size());
            for (unsigned int j = 0; j < caliAllCorners[i].size(); j++) {
                allCornersConcatenated.push_back(caliAllCorners[i][j]);
                allIdsConcatenated.push_back(caliAllIds[i][j]);
            }
        }

        // calibrate camera
        repError = aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
            markerCounterPerFrame, board, imgSize, cameraMatrix,
            distCoeffs, rvecs, tvecs, calibrationFlags);

        //float aspectRatio = 1;

        string outputFile = ofToDataPath("aruco_calibration.yml");

        bool saveOk = saveCameraParams(outputFile, imgSize, aspectRatio, calibrationFlags, cameraMatrix,
            distCoeffs, repError);

        resetCalibration();

        useCalibration = true;
    }

}

//--------------------------------------------------------------
//calibrate Camera sequence
void ArucoDetector::loadCameraCalibration() {

    string readFile = ofToDataPath("aruco_calibration.yml");
    bool readOk = readCameraCalibration(readFile, cameraMatrix, distCoeffs);

    if (!readOk) {
        ofLog(OF_LOG_NOTICE) << "cannot find file for Cam Matrix";
    }
    else {
        ofLog(OF_LOG_NOTICE) << "Read Calibration for Cam Matrix";
        useCalibration = true;
    }
}


//--------------------------------------------------------------
bool ArucoDetector::readCameraCalibration(std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
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
