#pragma once

/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/

#include "ofMain.h"

#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//addons
#include "ofxCv.h"
#include "ofxOpenCv.h"


#include "QRBlock.h"

class ArucoDetector;
typedef std::shared_ptr<ArucoDetector> ArucoDetectorRef;

#define QR_4x4_50   0

class ArucoDetector{
public:
    ArucoDetector();

    static ArucoDetectorRef create() {
        return std::make_shared<ArucoDetector>();
    }

    void setupCalibration(int markersX, int markersY);

    void detectMarkers(cv::Mat  & inputVideo, bool refiment = true);

    cv::Mat getInput(){return mVidCopy;}

    void toggleMarkerInfo(){mMarkerInfo = !mMarkerInfo;}

    void resetMinMax();

    std::vector<int> & getTagIds(){return mTagsIds;}
    int getNumMarkers(){return mNumFoundMarkers;}

    std::vector< QRBlockRef > & getBoard(){return mBlock;}

    cv::Mat & getMatImg(){return mVidMat;}
    ofImage & getOfImg(){return mVidImg;}

    int getMinId(){return mMinFoundId;}
    int getMaxId(){return mMaxFoundId;}

    //params
    void generateDetectorParams();
    void generateDetectorParams(std::string file);
    bool readDetectorParameters(std::string filename, cv::Ptr< cv::aruco::DetectorParameters>& params);

    //calibration
    void toggleCamCalibration(bool value = true) {
        calibrationProcess = value;
    }

    void captureCalibrate() {
        captureFrame = true;
    }
    int getCalibrationCount() { return captureCount; }


    void calibrateCameraProcess();

    void resetCalibration();

    void loadCameraCalibration();

    bool saveCameraParams(const std::string& filename, cv::Size imageSize,
        float aspectRatio, int flags,
        const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
        double totalAvgErr);

    bool readCameraCalibration(std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs);

private:

    //qr aruco
    cv::Ptr<cv::aruco::Board>              board;
    cv::Ptr<cv::aruco::Dictionary>         dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams;

    std::vector< QRBlockRef >               mBlock;

    std::vector< int >                      mTagsIds;

    int mMinFoundId;
    int mMaxFoundId;

    int mNumFoundMarkers;

    //video output in two formats
    cv::Mat mVidMat;
    cv::Mat mVidCopy;
    ofImage mVidImg;

    bool mMarkerInfo;


    //calibration
    bool calibrationProcess;
    bool captureFrame;

    bool calibratedCam;

    bool useCalibration;
    int  captureCount;

    cv::Size imgSize;
    int calibrationFlags;

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;

    vector < vector < vector < cv::Point2f > > > caliAllCorners;
    vector< vector< int > > caliAllIds;
};
