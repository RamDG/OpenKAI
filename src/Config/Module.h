/*
 * Module.h
 *
 *  Created on: Nov 22, 2016
 *      Author: root
 */

#ifndef OpenKAI_src_Config_Module_H_
#define OpenKAI_src_Config_Module_H_

#include "../Base/common.h"
#include "../Base/BASE.h"
#include "../Script/Kiss.h"
#include "switch.h"

#include "../Actuator/_ArduServo.h"
#include "../Actuator/_ActuatorSync.h"
#include "../Actuator/_DRV8825_RS485.h"
#include "../Actuator/_OrientalMotor.h"
#include "../Actuator/_S6H4D.h"

#include "../Application/Autopilot/ArduPilot/_AP_base.h"
#include "../Application/Autopilot/ArduPilot/_AP_actuator.h"
#include "../Application/Autopilot/ArduPilot/_AP_CETCUS.h"
#include "../Application/Autopilot/ArduPilot/_AP_distLidar.h"
#include "../Application/Autopilot/ArduPilot/_AP_goto.h"
#include "../Application/Autopilot/ArduPilot/_AP_GPS.h"
#include "../Application/Autopilot/ArduPilot/_AP_link.h"
#include "../Application/Autopilot/ArduPilot/_AP_mission.h"
#include "../Application/Autopilot/ArduPilot/_AP_posCtrl.h"
#include "../Application/Autopilot/ArduPilot/_AP_RTH.h"
#include "../Application/Autopilot/ArduPilot/_AP_relay.h"
#include "../Application/Autopilot/ArduPilot/_AP_servo.h"
#include "../Application/Autopilot/ArduPilot/_AP_takeoff.h"
#include "../Application/Autopilot/ArduPilot/_AProver_drive.h"
#include "../Application/Mission/_MissionControl.h"
#include "../Application/RobotArm/_Sequencer.h"
#include "../Control/PIDctrl.h"

#include "../Navigation/_Path.h"
#include "../Navigation/_GPS.h"
#include "../Navigation/_RTCM3.h"

#include "../IO/_TCPserver.h"
#include "../IO/_TCPclient.h"
#include "../IO/_File.h"
#include "../IO/_SerialPort.h"
#include "../IO/_UDP.h"
#include "../IO/_WebSocket.h"
#include "../Protocol/_Canbus.h"
#include "../Protocol/_CETCUS.h"
#include "../Protocol/_Mavlink.h"
#include "../Protocol/_MOAB.h"
#include "../Protocol/_ProtocolBase.h"
#include "../Protocol/_Modbus.h"
#include "../Sensor/_LeddarVu.h"
#include "../Sensor/_TOFsense.h"
#include "../Sensor/RPLIDAR/_RPLIDAR.h"
#include "../SLAM/_ORB_SLAM2.h"

#include "../UI/Console.h"

#include "../Universe/_Object.h"
#include "../Universe/_Universe.h"

#include "../Vision/Hiphen/_HiphenServer.h"
#include "../Vision/Hiphen/_HiphenCMD.h"

#ifdef USE_OPENCV

#include "../UI/Window.h"
#include "../Vision/_Camera.h"
#include "../Vision/_VideoFile.h"
#include "../Vision/_ImgFile.h"
#include "../Vision/_GStreamer.h"
#include "../Vision/_Raspivid.h"
#include "../Vision/_GPhoto.h"
#include "../Vision/ImgFilter/_Morphology.h"
#include "../Vision/ImgFilter/_Mask.h"
#include "../Vision/ImgFilter/_Threshold.h"
#include "../Vision/ImgFilter/_Contrast.h"
#include "../Vision/ImgFilter/_Crop.h"
#include "../Vision/ImgFilter/_Depth2Gray.h"
#include "../Vision/ImgFilter/_Erode.h"
#include "../Vision/ImgFilter/_Grayscale.h"
#include "../Vision/ImgFilter/_HistEqualize.h"
#include "../Vision/ImgFilter/_Invert.h"
#include "../Vision/ImgFilter/_InRange.h"
#include "../Vision/ImgFilter/_Resize.h"
#include "../Vision/ImgFilter/_Rotate.h"

#include "../Data/Image/_GDimgUploader.h"
#include "../Data/Image/_BBoxCutOut.h"
#include "../Data/Image/_CutOut.h"
#include "../Data/Video/_FrameCutOut.h"

#include "../Detector/_Lane.h"
#include "../Detector/_DNNdetect.h"
#include "../Detector/_DNNclassifier.h"
#include "../Detector/_DNNtext.h"
#include "../Detector/_DepthSegment.h"
#include "../Detector/_IRLock.h"
#include "../Detector/_Line.h"
#include "../Detector/_OpenPose.h"
#include "../Detector/_SlideWindow.h"
#include "../Detector/_Thermal.h"

#include "../Application/Autopilot/ArduPilot/_AP_avoid.h"
#include "../Application/Autopilot/ArduPilot/_AP_depthVision.h"
#include "../Application/Autopilot/ArduPilot/_AP_descent.h"
#include "../Application/Autopilot/ArduPilot/_AP_follow.h"
#include "../Application/Autopilot/ArduPilot/_AP_followClient.h"
#include "../Application/Autopilot/ArduPilot/_APcopter_photo.h"
#include "../Application/Autopilot/ArduPilot/_AProver_photo.h"
#include "../Application/Autopilot/ArduPilot/_AProver_field.h"

#include "../Application/Observation/_HiphenRGB.h"
#include "../Application/Surveillance/_ANR.h"
#include "../Application/Surveillance/_GDcam.h"
#include "../Application/Surveillance/_ShopCam.h"

#include "../Application/RobotArm/Articulated/_PickingArm.h"
#include "../Application/RobotArm/Cartesian/_SortingArm.h"
#include "../Application/RobotArm/Cartesian/_SortingCtrlServer.h"
#include "../Application/RobotArm/Cartesian/_SortingCtrlClient.h"

#ifdef USE_OPENCV_CONTRIB
#include "../Detector/_ArUco.h"
#include "../Detector/_MotionDetector.h"
#include "../Tracker/_SingleTracker.h"
#endif

#ifdef USE_CUDA
#include "../Detector/_Bullseye.h"
#include "../Detector/_Cascade.h"
#include "../Vision/_DenseFlow.h"
#endif

#ifdef USE_REALSENSE
#include "../Vision/_RealSense.h"
#endif

#ifdef USE_OPEN3D
#ifdef USE_REALSENSE
#include "../PointCloud/_RealSensePC.h"
#endif
#endif	//USE_OPEN3D

#ifdef USE_MYNTEYE
#include "../Vision/_Mynteye.h"
#endif

#ifdef USE_JETSON_INFERENCE
#include "../DNN/JetsonInference/_ImageNet.h"
#include "../DNN/JetsonInference/_DetectNet.h"
#endif

#ifdef USE_OCR
#include "../Detector/OCR.h"
#endif

#ifdef USE_OPENALPR
#include "../Detector/_OpenALPR.h"
#endif

#ifdef USE_CAFFE
#include "../DNN/Caffe/_Caffe.h"
#include "../Regression/_CaffeRegressionTrain.h"
#include "../Regression/_CaffeRegressionInf.h"
#endif

#ifdef USE_DARKNET
#include "../DNN/Darknet/_YOLO.h"
#endif

#ifdef USE_PYLON
#include "../Vision/_Pylon.h"
#endif

#endif	//USE_OPENCV


#ifdef USE_OPEN3D
#include "../PointCloud/_PointCloudViewer.h"
#endif

#ifdef USE_REALSENSE
#include "../SLAM/_RStracking.h"
#endif

#ifdef USE_DYNAMIXEL
#include "../Actuator/_DeltaArm.h"
#include "../Actuator/_LabArm.h"
#endif

#ifdef USE_LIVOX
#include "../Sensor/_Livox.h"
#endif


#define ADD_MODULE(x) if(pK->m_class == #x){return createInst<x>(pK);}

namespace kai
{

class Module
{
public:
	Module();
	virtual ~Module();
	BASE* createInstance(Kiss* pK);
private:
	template <typename T> BASE* createInst(Kiss* pKiss);
};

}

#endif
