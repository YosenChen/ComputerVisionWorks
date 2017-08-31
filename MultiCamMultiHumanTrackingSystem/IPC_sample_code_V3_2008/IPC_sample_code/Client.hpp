#ifndef IPC_CLIENT_H
#define IPC_CLIENT_H

#include "ipc.h"
#include "messages.hpp"

//--------------- the client--------------------
class IPCClientMgr{
public:
	IPCClientMgr():isServerConnected(false){}
	~IPCClientMgr();
	void connectToServer(const char* server_name = "localhost");
	void subscribe(const char*, ...);
	void publish(const char*, ...);
	void listen(unsigned period = 50);
private:
	void register_messages();
	void define_message(const char *msgName, const char *formatString);

	static unsigned WINAPI listenThread(LPVOID);
	static bool keepListening;
	bool isServerConnected;
	HANDLE h_listenThread;
};

// global IPC client manager
extern IPCClientMgr IPCClient;

// get messages
void getLaserPose(LaserPoseMgr& mgr);
void getSLaser(SLaserMgr& mgr);
void getSubgoal(SubgoalMgr& mgr);
void getOdometry(OdometryMgr& mgr);
void getMotor(MotorMgr& mgr);
void getSpeedMode(SpeedModeMgr&mgr);
void getHeadMotor(HeadMotorMgr& mgr);
void getGoal(GoalMgr& mgr);
void getHandPos(HandPosMgr& mgr);
void getFacialExp(FacialExpMgr& mgr);
void getMic19(Mic19Mgr& mgr);
void getMic20(Mic20Mgr& mgr);
void getMic21(Mic21Mgr& mgr);
void getMic22(Mic22Mgr& mgr);

void getHistCrCb(HistCrCbMgr_CAM1_OB1& mgr);
void getHistCrCb(HistCrCbMgr_CAM1_OB2& mgr);
void getHistCrCb(HistCrCbMgr_CAM1_OB3& mgr);
void getHistCrCb(HistCrCbMgr_CAM1_OB4& mgr);
void getHistCrCb(HistCrCbMgr_CAM1_OB5& mgr);
void getHistCrCb(HistCrCbMgr_CAM2_OB1& mgr);
void getHistCrCb(HistCrCbMgr_CAM2_OB2& mgr);
void getHistCrCb(HistCrCbMgr_CAM2_OB3& mgr);
void getHistCrCb(HistCrCbMgr_CAM2_OB4& mgr);
void getHistCrCb(HistCrCbMgr_CAM2_OB5& mgr);
void getHistCrCb(HistCrCbMgr_CAM3_OB1& mgr);
void getHistCrCb(HistCrCbMgr_CAM3_OB2& mgr);
void getHistCrCb(HistCrCbMgr_CAM3_OB3& mgr);
void getHistCrCb(HistCrCbMgr_CAM3_OB4& mgr);
void getHistCrCb(HistCrCbMgr_CAM3_OB5& mgr);

void gettest(test& mgr);
void getBodyInfo(BodyInfoMgr_CAM1_OB1& mgr);
void getBodyInfo(BodyInfoMgr_CAM1_OB2& mgr);
void getBodyInfo(BodyInfoMgr_CAM1_OB3& mgr);
void getBodyInfo(BodyInfoMgr_CAM1_OB4& mgr);
void getBodyInfo(BodyInfoMgr_CAM1_OB5& mgr);
void getBodyInfo(BodyInfoMgr_CAM2_OB1& mgr);
void getBodyInfo(BodyInfoMgr_CAM2_OB2& mgr);
void getBodyInfo(BodyInfoMgr_CAM2_OB3& mgr);
void getBodyInfo(BodyInfoMgr_CAM2_OB4& mgr);
void getBodyInfo(BodyInfoMgr_CAM2_OB5& mgr);
void getBodyInfo(BodyInfoMgr_CAM3_OB1& mgr);
void getBodyInfo(BodyInfoMgr_CAM3_OB2& mgr);
void getBodyInfo(BodyInfoMgr_CAM3_OB3& mgr);
void getBodyInfo(BodyInfoMgr_CAM3_OB4& mgr);
void getBodyInfo(BodyInfoMgr_CAM3_OB5& mgr);

void getPos3D(Pos3DMgr_CAM1_OB1& mgr);
void getPos3D(Pos3DMgr_CAM1_OB2& mgr);
void getPos3D(Pos3DMgr_CAM1_OB3& mgr);
void getPos3D(Pos3DMgr_CAM1_OB4& mgr);
void getPos3D(Pos3DMgr_CAM1_OB5& mgr);
void getPos3D(Pos3DMgr_CAM2_OB1& mgr);
void getPos3D(Pos3DMgr_CAM2_OB2& mgr);
void getPos3D(Pos3DMgr_CAM2_OB3& mgr);
void getPos3D(Pos3DMgr_CAM2_OB4& mgr);
void getPos3D(Pos3DMgr_CAM2_OB5& mgr);
void getPos3D(Pos3DMgr_CAM3_OB1& mgr);
void getPos3D(Pos3DMgr_CAM3_OB2& mgr);
void getPos3D(Pos3DMgr_CAM3_OB3& mgr);
void getPos3D(Pos3DMgr_CAM3_OB4& mgr);
void getPos3D(Pos3DMgr_CAM3_OB5& mgr);

void getCamLine(CamLineMgr_CAM1_OB1& mgr);
void getCamLine(CamLineMgr_CAM1_OB2& mgr);
void getCamLine(CamLineMgr_CAM1_OB3& mgr);
void getCamLine(CamLineMgr_CAM1_OB4& mgr);
void getCamLine(CamLineMgr_CAM1_OB5& mgr);
void getCamLine(CamLineMgr_CAM2_OB1& mgr);
void getCamLine(CamLineMgr_CAM2_OB2& mgr);
void getCamLine(CamLineMgr_CAM2_OB3& mgr);
void getCamLine(CamLineMgr_CAM2_OB4& mgr);
void getCamLine(CamLineMgr_CAM2_OB5& mgr);
void getCamLine(CamLineMgr_CAM3_OB1& mgr);
void getCamLine(CamLineMgr_CAM3_OB2& mgr);
void getCamLine(CamLineMgr_CAM3_OB3& mgr);
void getCamLine(CamLineMgr_CAM3_OB4& mgr);
void getCamLine(CamLineMgr_CAM3_OB5& mgr);

void getAliveSta(AliveStaMgr_CAM1& mgr);
void getAliveSta(AliveStaMgr_CAM2& mgr);
void getAliveSta(AliveStaMgr_CAM3& mgr);

void getLabelIntegrate(LabelIntegrateMgr_CAM1& mgr);
void getLabelIntegrate(LabelIntegrateMgr_CAM2& mgr);
void getLabelIntegrate(LabelIntegrateMgr_CAM3& mgr);







// set messages
IPC_RETURN_TYPE setLaserPose(LaserPoseMgr& mgr);
IPC_RETURN_TYPE setSLaser(SLaserMgr& mgr);
IPC_RETURN_TYPE setSubgoal(SubgoalMgr& mgr);
IPC_RETURN_TYPE setOdometry(OdometryMgr& mgr);
IPC_RETURN_TYPE setMotor(MotorMgr& mgr);
IPC_RETURN_TYPE setSpeedMode(SpeedModeMgr& mgr);
IPC_RETURN_TYPE setHeadMotor(HeadMotorMgr& mgr);
IPC_RETURN_TYPE setGoal(GoalMgr& mgr);
IPC_RETURN_TYPE setHandPos(HandPosMgr& mgr);
IPC_RETURN_TYPE setFacialExp(FacialExpMgr& mgr);
IPC_RETURN_TYPE setMic19(Mic19Mgr& mgr);
IPC_RETURN_TYPE setMic20(Mic20Mgr& mgr);
IPC_RETURN_TYPE setMic21(Mic21Mgr& mgr);
IPC_RETURN_TYPE setMic22(Mic22Mgr& mgr);

IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB1& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB2& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB3& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB4& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB5& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB1& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB2& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB3& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB4& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB5& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB1& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB2& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB3& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB4& mgr);
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB5& mgr);

IPC_RETURN_TYPE settest(test& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB1& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB2& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB3& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB4& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB5& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB1& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB2& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB3& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB4& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB5& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB1& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB2& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB3& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB4& mgr);
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB5& mgr);

IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB1& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB2& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB3& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB4& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB5& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB1& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB2& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB3& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB4& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB5& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB1& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB2& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB3& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB4& mgr);
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB5& mgr);

IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB1& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB2& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB3& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB4& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB5& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB1& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB2& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB3& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB4& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB5& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB1& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB2& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB3& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB4& mgr);
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB5& mgr);

IPC_RETURN_TYPE setAliveSta(AliveStaMgr_CAM1& mgr);
IPC_RETURN_TYPE setAliveSta(AliveStaMgr_CAM2& mgr);
IPC_RETURN_TYPE setAliveSta(AliveStaMgr_CAM3& mgr);

IPC_RETURN_TYPE setLabelIntegrate(LabelIntegrateMgr_CAM1& mgr);
IPC_RETURN_TYPE setLabelIntegrate(LabelIntegrateMgr_CAM2& mgr);
IPC_RETURN_TYPE setLabelIntegrate(LabelIntegrateMgr_CAM3& mgr);



//transforming functions
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB1* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB2* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB3* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB4* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB5* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB1* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB2* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB3* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB4* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB5* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB1* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB2* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB3* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB4* src, CvHistogram* dst);
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB5* src, CvHistogram* dst);


bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB1* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB2* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB3* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB4* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB5* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB1* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB2* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB3* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB4* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB5* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB1* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB2* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB3* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB4* dst, int timeIndex);
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB5* dst, int timeIndex);


bool trans2BodyInfo(BodyInfoMgr_CAM1_OB1* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB2* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB3* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB4* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB5* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB1* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB2* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB3* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB4* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB5* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB1* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB2* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB3* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB4* src, BodyInfo* dst);
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB5* src, BodyInfo* dst);


bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB1* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB2* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB3* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB4* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB5* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB1* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB2* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB3* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB4* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB5* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB1* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB2* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB3* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB4* dst, int timeIndex);
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB5* dst, int timeIndex);


bool trans2Pos3D(Pos3DMgr_CAM1_OB1* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM1_OB2* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM1_OB3* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM1_OB4* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM1_OB5* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM2_OB1* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM2_OB2* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM2_OB3* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM2_OB4* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM2_OB5* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM3_OB1* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM3_OB2* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM3_OB3* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM3_OB4* src, Position3D* dst);
bool trans2Pos3D(Pos3DMgr_CAM3_OB5* src, Position3D* dst);


bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB1* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB2* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB3* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB4* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB5* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB1* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB2* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB3* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB4* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB5* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB1* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB2* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB3* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB4* dst, int timeIndex);
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB5* dst, int timeIndex);


bool trans2CamLine(CamLineMgr_CAM1_OB1* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM1_OB2* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM1_OB3* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM1_OB4* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM1_OB5* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM2_OB1* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM2_OB2* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM2_OB3* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM2_OB4* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM2_OB5* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM3_OB1* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM3_OB2* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM3_OB3* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM3_OB4* src, CameraLine3D* dst);
bool trans2CamLine(CamLineMgr_CAM3_OB5* src, CameraLine3D* dst);


bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB1* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB2* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB3* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB4* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB5* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB1* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB2* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB3* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB4* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB5* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB1* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB2* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB3* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB4* dst, int timeIndex);
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB5* dst, int timeIndex);

#endif