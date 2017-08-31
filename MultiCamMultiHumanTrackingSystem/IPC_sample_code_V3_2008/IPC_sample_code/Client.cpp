
#include "Client.hpp"
#include "process.h"
#include <cstdarg>
#include <map>
#include <sstream>

using std::pair;
using std::map;
using std::ostringstream;

class win32CriticalSection{
public:
	win32CriticalSection(){
		InitializeCriticalSection( &cs );
	}
	~win32CriticalSection(){
		DeleteCriticalSection( &cs );
	}
	void lock(){
		EnterCriticalSection( &cs );	
	}
	void unlock(){
		LeaveCriticalSection( &cs );
	}
private:
	CRITICAL_SECTION cs;
};

static win32CriticalSection IPC_CS;

//--------------------------------------------------//
// Global IPC client manager
//--------------------------------------------------//
IPCClientMgr IPCClient;

//--------------------------------------------------//
// messages
//--------------------------------------------------//
static LaserPoseMgr laserposemgr;
static SLaserMgr slasermgr;
static SubgoalMgr subgoalmgr;

static OdometryMgr odometrymgr;
static MotorMgr motormgr;
static SpeedModeMgr SpeedModemgr;
static HeadMotorMgr HeadMotormgr;
static HandPosMgr handposmgr;
static FacialExpMgr facialexpmgr;
static Mic19Mgr mic19mgr;
static Mic20Mgr mic20mgr;
static Mic21Mgr mic21mgr;
static Mic22Mgr mic22mgr;
static StateMgr statemgr;
static GoalMgr goalmgr;
static PeopleMgr peoplemgr;
static WordMgr wordmgr;
static Detect detectmgr;
static LocState loc_state;
static SpeakMgr speakmgr;

static HistCrCbMgr_CAM1_OB1 histcrcbmgr_CAM1_OB1;
static HistCrCbMgr_CAM1_OB2 histcrcbmgr_CAM1_OB2;
static HistCrCbMgr_CAM1_OB3 histcrcbmgr_CAM1_OB3;
static HistCrCbMgr_CAM1_OB4 histcrcbmgr_CAM1_OB4;
static HistCrCbMgr_CAM1_OB5 histcrcbmgr_CAM1_OB5;
static HistCrCbMgr_CAM2_OB1 histcrcbmgr_CAM2_OB1;
static HistCrCbMgr_CAM2_OB2 histcrcbmgr_CAM2_OB2;
static HistCrCbMgr_CAM2_OB3 histcrcbmgr_CAM2_OB3;
static HistCrCbMgr_CAM2_OB4 histcrcbmgr_CAM2_OB4;
static HistCrCbMgr_CAM2_OB5 histcrcbmgr_CAM2_OB5;
static HistCrCbMgr_CAM3_OB1 histcrcbmgr_CAM3_OB1;
static HistCrCbMgr_CAM3_OB2 histcrcbmgr_CAM3_OB2;
static HistCrCbMgr_CAM3_OB3 histcrcbmgr_CAM3_OB3;
static HistCrCbMgr_CAM3_OB4 histcrcbmgr_CAM3_OB4;
static HistCrCbMgr_CAM3_OB5 histcrcbmgr_CAM3_OB5;

static test t;
static BodyInfoMgr_CAM1_OB1 bodyinfomgr_CAM1_OB1;
static BodyInfoMgr_CAM1_OB2 bodyinfomgr_CAM1_OB2;
static BodyInfoMgr_CAM1_OB3 bodyinfomgr_CAM1_OB3;
static BodyInfoMgr_CAM1_OB4 bodyinfomgr_CAM1_OB4;
static BodyInfoMgr_CAM1_OB5 bodyinfomgr_CAM1_OB5;
static BodyInfoMgr_CAM2_OB1 bodyinfomgr_CAM2_OB1;
static BodyInfoMgr_CAM2_OB2 bodyinfomgr_CAM2_OB2;
static BodyInfoMgr_CAM2_OB3 bodyinfomgr_CAM2_OB3;
static BodyInfoMgr_CAM2_OB4 bodyinfomgr_CAM2_OB4;
static BodyInfoMgr_CAM2_OB5 bodyinfomgr_CAM2_OB5;
static BodyInfoMgr_CAM3_OB1 bodyinfomgr_CAM3_OB1;
static BodyInfoMgr_CAM3_OB2 bodyinfomgr_CAM3_OB2;
static BodyInfoMgr_CAM3_OB3 bodyinfomgr_CAM3_OB3;
static BodyInfoMgr_CAM3_OB4 bodyinfomgr_CAM3_OB4;
static BodyInfoMgr_CAM3_OB5 bodyinfomgr_CAM3_OB5;

static Pos3DMgr_CAM1_OB1 pos3dmgr_CAM1_OB1;
static Pos3DMgr_CAM1_OB2 pos3dmgr_CAM1_OB2;
static Pos3DMgr_CAM1_OB3 pos3dmgr_CAM1_OB3;
static Pos3DMgr_CAM1_OB4 pos3dmgr_CAM1_OB4;
static Pos3DMgr_CAM1_OB5 pos3dmgr_CAM1_OB5;
static Pos3DMgr_CAM2_OB1 pos3dmgr_CAM2_OB1;
static Pos3DMgr_CAM2_OB2 pos3dmgr_CAM2_OB2;
static Pos3DMgr_CAM2_OB3 pos3dmgr_CAM2_OB3;
static Pos3DMgr_CAM2_OB4 pos3dmgr_CAM2_OB4;
static Pos3DMgr_CAM2_OB5 pos3dmgr_CAM2_OB5;
static Pos3DMgr_CAM3_OB1 pos3dmgr_CAM3_OB1;
static Pos3DMgr_CAM3_OB2 pos3dmgr_CAM3_OB2;
static Pos3DMgr_CAM3_OB3 pos3dmgr_CAM3_OB3;
static Pos3DMgr_CAM3_OB4 pos3dmgr_CAM3_OB4;
static Pos3DMgr_CAM3_OB5 pos3dmgr_CAM3_OB5;

static CamLineMgr_CAM1_OB1 camlinemgr_CAM1_OB1;
static CamLineMgr_CAM1_OB2 camlinemgr_CAM1_OB2;
static CamLineMgr_CAM1_OB3 camlinemgr_CAM1_OB3;
static CamLineMgr_CAM1_OB4 camlinemgr_CAM1_OB4;
static CamLineMgr_CAM1_OB5 camlinemgr_CAM1_OB5;
static CamLineMgr_CAM2_OB1 camlinemgr_CAM2_OB1;
static CamLineMgr_CAM2_OB2 camlinemgr_CAM2_OB2;
static CamLineMgr_CAM2_OB3 camlinemgr_CAM2_OB3;
static CamLineMgr_CAM2_OB4 camlinemgr_CAM2_OB4;
static CamLineMgr_CAM2_OB5 camlinemgr_CAM2_OB5;
static CamLineMgr_CAM3_OB1 camlinemgr_CAM3_OB1;
static CamLineMgr_CAM3_OB2 camlinemgr_CAM3_OB2;
static CamLineMgr_CAM3_OB3 camlinemgr_CAM3_OB3;
static CamLineMgr_CAM3_OB4 camlinemgr_CAM3_OB4;
static CamLineMgr_CAM3_OB5 camlinemgr_CAM3_OB5;

static AliveStaMgr_CAM1 alivestamgr_CAM1;
static AliveStaMgr_CAM2 alivestamgr_CAM2;
static AliveStaMgr_CAM3 alivestamgr_CAM3;

static LabelIntegrateMgr_CAM1 labelintegratemgr_CAM1;
static LabelIntegrateMgr_CAM2 labelintegratemgr_CAM2;
static LabelIntegrateMgr_CAM3 labelintegratemgr_CAM3;




//--------------------------------------------------//
// message container ( formats and handlers )
//--------------------------------------------------//
typedef pair< const char *, void (*)(MSG_INSTANCE, BYTE_ARRAY, void *)> message_pair;
static map<const char*, message_pair > message_handlers;

//--------------------------------------------------//
// message handlers
//--------------------------------------------------//
static void laserpose_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &laserposemgr, sizeof(LaserPoseMgr));

	IPC_freeByteArray(callData);
}

static void slaser_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &slasermgr, sizeof(SLaserMgr));

	IPC_freeByteArray(callData);
}

static void subgoal_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &subgoalmgr, sizeof(SubgoalMgr));

	IPC_freeByteArray(callData);
}

static void odometry_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &odometrymgr, sizeof(OdometryMgr));

	IPC_freeByteArray(callData);
}
static void motor_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &motormgr, sizeof(MotorMgr));

	IPC_freeByteArray(callData);
}
static void SpeedMode_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &SpeedModemgr, sizeof(SpeedModeMgr));

	IPC_freeByteArray(callData);
}

static void HeadMotor_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &HeadMotormgr, sizeof(HeadMotorMgr));

	IPC_freeByteArray(callData);
}

static void handpos_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &handposmgr, sizeof(HandPosMgr));

	IPC_freeByteArray(callData);
}

static void facialexp_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &facialexpmgr, sizeof(FacialExpMgr));

	IPC_freeByteArray(callData);
}

static void mic19_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &mic19mgr, sizeof(Mic19Mgr));

	IPC_freeByteArray(callData);
}

static void mic20_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &mic20mgr, sizeof(Mic20Mgr));

	IPC_freeByteArray(callData);
}

static void mic21_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &mic21mgr, sizeof(Mic21Mgr));

	IPC_freeByteArray(callData);
}

static void mic22_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &mic22mgr, sizeof(Mic22Mgr));

	IPC_freeByteArray(callData);
}

static void state_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &statemgr, sizeof(StateMgr));

	IPC_freeByteArray(callData);
}

static void goal_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &goalmgr, sizeof(GoalMgr));

	IPC_freeByteArray(callData);
}

static void people_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &peoplemgr, sizeof(PeopleMgr));

	IPC_freeByteArray(callData);
}

static void word_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &wordmgr, sizeof(WordMgr));

	IPC_freeByteArray(callData);
}

static void detect_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &detectmgr, sizeof(Detect));

	IPC_freeByteArray(callData);
}

static void loc_state_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &loc_state, sizeof(LocState));

	IPC_freeByteArray(callData);
}

static void speak_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData){

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &speakmgr, sizeof(SpeakMgr));

	IPC_freeByteArray(callData);
}





static void histCrCb_CAM1_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM1_OB1, sizeof(HistCrCbMgr_CAM1_OB1));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM1_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM1_OB2, sizeof(HistCrCbMgr_CAM1_OB2));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM1_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM1_OB3, sizeof(HistCrCbMgr_CAM1_OB3));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM1_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM1_OB4, sizeof(HistCrCbMgr_CAM1_OB4));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM1_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM1_OB5, sizeof(HistCrCbMgr_CAM1_OB5));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM2_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM2_OB1, sizeof(HistCrCbMgr_CAM2_OB1));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM2_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM2_OB2, sizeof(HistCrCbMgr_CAM2_OB2));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM2_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM2_OB3, sizeof(HistCrCbMgr_CAM2_OB3));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM2_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM2_OB4, sizeof(HistCrCbMgr_CAM2_OB4));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM2_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM2_OB5, sizeof(HistCrCbMgr_CAM2_OB5));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM3_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM3_OB1, sizeof(HistCrCbMgr_CAM3_OB1));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM3_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM3_OB2, sizeof(HistCrCbMgr_CAM3_OB2));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM3_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM3_OB3, sizeof(HistCrCbMgr_CAM3_OB3));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM3_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM3_OB4, sizeof(HistCrCbMgr_CAM3_OB4));

	IPC_freeByteArray(callData);
}
static void histCrCb_CAM3_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &histcrcbmgr_CAM3_OB5, sizeof(HistCrCbMgr_CAM3_OB5));

	IPC_freeByteArray(callData);
}




static void test_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &t, sizeof(test));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM1_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM1_OB1, sizeof(BodyInfoMgr_CAM1_OB1));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM1_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM1_OB2, sizeof(BodyInfoMgr_CAM1_OB2));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM1_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM1_OB3, sizeof(BodyInfoMgr_CAM1_OB3));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM1_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM1_OB4, sizeof(BodyInfoMgr_CAM1_OB4));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM1_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM1_OB5, sizeof(BodyInfoMgr_CAM1_OB5));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM2_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM2_OB1, sizeof(BodyInfoMgr_CAM2_OB1));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM2_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM2_OB2, sizeof(BodyInfoMgr_CAM2_OB2));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM2_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM2_OB3, sizeof(BodyInfoMgr_CAM2_OB3));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM2_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM2_OB4, sizeof(BodyInfoMgr_CAM2_OB4));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM2_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM2_OB5, sizeof(BodyInfoMgr_CAM2_OB5));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM3_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM3_OB1, sizeof(BodyInfoMgr_CAM3_OB1));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM3_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM3_OB2, sizeof(BodyInfoMgr_CAM3_OB2));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM3_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM3_OB3, sizeof(BodyInfoMgr_CAM3_OB3));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM3_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM3_OB4, sizeof(BodyInfoMgr_CAM3_OB4));

	IPC_freeByteArray(callData);
}
static void bodyInfo_CAM3_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &bodyinfomgr_CAM3_OB5, sizeof(BodyInfoMgr_CAM3_OB5));

	IPC_freeByteArray(callData);
}





static void pos3D_CAM1_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM1_OB1, sizeof(Pos3DMgr_CAM1_OB1));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM1_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM1_OB2, sizeof(Pos3DMgr_CAM1_OB2));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM1_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM1_OB3, sizeof(Pos3DMgr_CAM1_OB3));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM1_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM1_OB4, sizeof(Pos3DMgr_CAM1_OB4));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM1_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM1_OB5, sizeof(Pos3DMgr_CAM1_OB5));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM2_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM2_OB1, sizeof(Pos3DMgr_CAM2_OB1));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM2_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM2_OB2, sizeof(Pos3DMgr_CAM2_OB2));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM2_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM2_OB3, sizeof(Pos3DMgr_CAM2_OB3));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM2_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM2_OB4, sizeof(Pos3DMgr_CAM2_OB4));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM2_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM2_OB5, sizeof(Pos3DMgr_CAM2_OB5));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM3_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM3_OB1, sizeof(Pos3DMgr_CAM3_OB1));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM3_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM3_OB2, sizeof(Pos3DMgr_CAM3_OB2));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM3_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM3_OB3, sizeof(Pos3DMgr_CAM3_OB3));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM3_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM3_OB4, sizeof(Pos3DMgr_CAM3_OB4));

	IPC_freeByteArray(callData);
}
static void pos3D_CAM3_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &pos3dmgr_CAM3_OB5, sizeof(Pos3DMgr_CAM3_OB5));

	IPC_freeByteArray(callData);
}






static void camLine_CAM1_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM1_OB1, sizeof(CamLineMgr_CAM1_OB1));

	IPC_freeByteArray(callData);
}
static void camLine_CAM1_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM1_OB2, sizeof(CamLineMgr_CAM1_OB2));

	IPC_freeByteArray(callData);
}
static void camLine_CAM1_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM1_OB3, sizeof(CamLineMgr_CAM1_OB3));

	IPC_freeByteArray(callData);
}
static void camLine_CAM1_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM1_OB4, sizeof(CamLineMgr_CAM1_OB4));

	IPC_freeByteArray(callData);
}
static void camLine_CAM1_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM1_OB5, sizeof(CamLineMgr_CAM1_OB5));

	IPC_freeByteArray(callData);
}
static void camLine_CAM2_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM2_OB1, sizeof(CamLineMgr_CAM2_OB1));

	IPC_freeByteArray(callData);
}
static void camLine_CAM2_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM2_OB2, sizeof(CamLineMgr_CAM2_OB2));

	IPC_freeByteArray(callData);
}
static void camLine_CAM2_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM2_OB3, sizeof(CamLineMgr_CAM2_OB3));

	IPC_freeByteArray(callData);
}
static void camLine_CAM2_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM2_OB4, sizeof(CamLineMgr_CAM2_OB4));

	IPC_freeByteArray(callData);
}
static void camLine_CAM2_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM2_OB5, sizeof(CamLineMgr_CAM2_OB5));

	IPC_freeByteArray(callData);
}
static void camLine_CAM3_OB1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM3_OB1, sizeof(CamLineMgr_CAM3_OB1));

	IPC_freeByteArray(callData);
}
static void camLine_CAM3_OB2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM3_OB2, sizeof(CamLineMgr_CAM3_OB2));

	IPC_freeByteArray(callData);
}
static void camLine_CAM3_OB3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM3_OB3, sizeof(CamLineMgr_CAM3_OB3));

	IPC_freeByteArray(callData);
}
static void camLine_CAM3_OB4_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM3_OB4, sizeof(CamLineMgr_CAM3_OB4));

	IPC_freeByteArray(callData);
}
static void camLine_CAM3_OB5_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &camlinemgr_CAM3_OB5, sizeof(CamLineMgr_CAM3_OB5));

	IPC_freeByteArray(callData);
}








static void aliveSta_CAM1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &alivestamgr_CAM1, sizeof(AliveStaMgr_CAM1));

	IPC_freeByteArray(callData);
}
static void aliveSta_CAM2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &alivestamgr_CAM2, sizeof(AliveStaMgr_CAM2));

	IPC_freeByteArray(callData);
}
static void aliveSta_CAM3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &alivestamgr_CAM3, sizeof(AliveStaMgr_CAM3));

	IPC_freeByteArray(callData);
}






static void labelIntegrate_CAM1_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &labelintegratemgr_CAM1, sizeof(LabelIntegrateMgr_CAM1));

	IPC_freeByteArray(callData);
}
static void labelIntegrate_CAM2_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &labelintegratemgr_CAM2, sizeof(LabelIntegrateMgr_CAM2));

	IPC_freeByteArray(callData);
}
static void labelIntegrate_CAM3_message_handler(MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData)
{

	FORMATTER_PTR formatter;

	formatter = IPC_msgInstanceFormatter(msgRef);
	IPC_unmarshallData(formatter, callData, &labelintegratemgr_CAM3, sizeof(LabelIntegrateMgr_CAM3));

	IPC_freeByteArray(callData);
}



//--------------------------------------------------//
// IPC client manager
//--------------------------------------------------//

// initialize static member data
bool IPCClientMgr::keepListening = false;

IPCClientMgr::~IPCClientMgr()
{
	if( keepListening ){
		keepListening = false;
		WaitForSingleObject( h_listenThread, INFINITE );
		CloseHandle( h_listenThread );
		printf( "close listen thread\n" );
	}
}

void IPCClientMgr::connectToServer(const char* server_name)
{
	ostringstream ipc_name;
	ipc_name << "Client" << GetCurrentProcessId();

	IPC_CS.lock();
	IPC_RETURN_TYPE err = IPC_connectModule(ipc_name.str().c_str(), server_name);

	/*
	if(err == IPC_Error) {
		printf("Could not connect to server.\n\n");
		printf("Did you remember to start server?\n");
		printf("Did you remember to set your CENTRALHOST variable?");
	}
	*/

	IPC_setVerbosity(IPC_Print_Errors);
	IPC_setVerbosity(IPC_Print_Warnings);
	IPC_CS.unlock();

	register_messages();
}

void IPCClientMgr::subscribe(const char* first, ...)
{
	const char* msgName = first;
	va_list arg_list;
	va_start( arg_list, first );

	while( msgName != NULL ){
		printf( "subscribe %s\n", msgName );
		message_pair mp = message_handlers[msgName];
		IPC_CS.lock();
		define_message( msgName, mp.first );
		IPC_subscribeData(msgName, mp.second, NULL);
		IPC_CS.unlock();
		msgName = va_arg( arg_list, const char* );
	}

	va_end( arg_list );
}

void IPCClientMgr::publish(const char* first, ...)
{
	const char* msgName = first;
	va_list arg_list;
	va_start( arg_list, first );

	while( msgName != NULL ){
		printf( "publish %s\n", msgName );
		IPC_CS.lock();
		define_message( msgName, message_handlers[msgName].first );
		IPC_CS.unlock();
		msgName = va_arg( arg_list, const char* );
	}

	va_end( arg_list );
}

unsigned period;

void IPCClientMgr::listen(unsigned _period)
{
	unsigned threadID;
	keepListening = true;
	period = _period;
	h_listenThread = (HANDLE)_beginthreadex( NULL, 0, &IPCClientMgr::listenThread, &period, 0, &threadID );
}

unsigned WINAPI IPCClientMgr::listenThread(LPVOID para)
{
	unsigned ms = *(unsigned*)para;
	IPC_CS.lock();
	IPC_listenClear(1000);
	IPC_CS.unlock();

	while( keepListening ){
		IPC_CS.lock();
		IPC_listen(1);
		IPC_CS.unlock();
		Sleep(ms);
	}
	return 0;
}

// register messages
void IPCClientMgr::register_messages()
{
	message_handlers[LASERPOSE_NAME] = message_pair( LASERPOSE_FORMAT, &laserpose_message_handler );
	message_handlers[SLASER_NAME] = message_pair( SLASER_FORMAT, &slaser_message_handler );
	message_handlers[SUBGOAL_NAME] = message_pair( SUBGOAL_FORMAT, &subgoal_message_handler );
	message_handlers[ODOMETRY_NAME] = message_pair( ODOMETRY_FORMAT, &odometry_message_handler );
	message_handlers[MOTOR_NAME] = message_pair( MOTOR_FORMAT, &motor_message_handler );
	message_handlers[SPEEDMODE_NAME] = message_pair( SPEEDMODE_FORMAT, &SpeedMode_message_handler );
	message_handlers[HEADMOTOR_NAME] = message_pair( HEADMOTOR_FORMAT, &HeadMotor_message_handler );
	message_handlers[HANDPOS_NAME] = message_pair( HANDPOS_FORMAT, &handpos_message_handler );
	message_handlers[FACIAlEXP_NAME] = message_pair( FACIAlEXP_FORMAT, &facialexp_message_handler );
	message_handlers[MIC19_NAME] = message_pair( MIC19_FORMAT, &mic19_message_handler );
	message_handlers[MIC20_NAME] = message_pair( MIC20_FORMAT, &mic20_message_handler );
	message_handlers[MIC21_NAME] = message_pair( MIC21_FORMAT, &mic21_message_handler );
	message_handlers[MIC22_NAME] = message_pair( MIC22_FORMAT, &mic22_message_handler );
	message_handlers[STATE_NAME] = message_pair( STATE_FORMAT, &state_message_handler );
	message_handlers[GOAL_NAME] = message_pair( GOAL_FORMAT, &goal_message_handler );
	message_handlers[PEOPLE_NAME] = message_pair( PEOPLE_FORMAT, &people_message_handler );
	message_handlers[WORD_NAME] = message_pair( WORD_FORMAT, &word_message_handler );
	message_handlers[DETECT_NAME] = message_pair( DETECT_FORMAT, &detect_message_handler );
	message_handlers[LOCSTATE_NAME] = message_pair( LOCSTATE_FORMAT, &loc_state_message_handler );
	message_handlers[SPEAK_NAME] = message_pair( SPEAK_FORMAT, &speak_message_handler );

	message_handlers[HISTCRCB_CAM1_OB1_NAME] = message_pair( HISTCRCB_CAM1_OB1_FORMAT, &histCrCb_CAM1_OB1_message_handler );
	message_handlers[HISTCRCB_CAM1_OB2_NAME] = message_pair( HISTCRCB_CAM1_OB2_FORMAT, &histCrCb_CAM1_OB2_message_handler );
	message_handlers[HISTCRCB_CAM1_OB3_NAME] = message_pair( HISTCRCB_CAM1_OB3_FORMAT, &histCrCb_CAM1_OB3_message_handler );
	message_handlers[HISTCRCB_CAM1_OB4_NAME] = message_pair( HISTCRCB_CAM1_OB4_FORMAT, &histCrCb_CAM1_OB4_message_handler );
	message_handlers[HISTCRCB_CAM1_OB5_NAME] = message_pair( HISTCRCB_CAM1_OB5_FORMAT, &histCrCb_CAM1_OB5_message_handler );
	message_handlers[HISTCRCB_CAM2_OB1_NAME] = message_pair( HISTCRCB_CAM2_OB1_FORMAT, &histCrCb_CAM2_OB1_message_handler );
	message_handlers[HISTCRCB_CAM2_OB2_NAME] = message_pair( HISTCRCB_CAM2_OB2_FORMAT, &histCrCb_CAM2_OB2_message_handler );
	message_handlers[HISTCRCB_CAM2_OB3_NAME] = message_pair( HISTCRCB_CAM2_OB3_FORMAT, &histCrCb_CAM2_OB3_message_handler );
	message_handlers[HISTCRCB_CAM2_OB4_NAME] = message_pair( HISTCRCB_CAM2_OB4_FORMAT, &histCrCb_CAM2_OB4_message_handler );
	message_handlers[HISTCRCB_CAM2_OB5_NAME] = message_pair( HISTCRCB_CAM2_OB5_FORMAT, &histCrCb_CAM2_OB5_message_handler );
	message_handlers[HISTCRCB_CAM3_OB1_NAME] = message_pair( HISTCRCB_CAM3_OB1_FORMAT, &histCrCb_CAM3_OB1_message_handler );
	message_handlers[HISTCRCB_CAM3_OB2_NAME] = message_pair( HISTCRCB_CAM3_OB2_FORMAT, &histCrCb_CAM3_OB2_message_handler );
	message_handlers[HISTCRCB_CAM3_OB3_NAME] = message_pair( HISTCRCB_CAM3_OB3_FORMAT, &histCrCb_CAM3_OB3_message_handler );
	message_handlers[HISTCRCB_CAM3_OB4_NAME] = message_pair( HISTCRCB_CAM3_OB4_FORMAT, &histCrCb_CAM3_OB4_message_handler );
	message_handlers[HISTCRCB_CAM3_OB5_NAME] = message_pair( HISTCRCB_CAM3_OB5_FORMAT, &histCrCb_CAM3_OB5_message_handler );

	message_handlers[TEST_NAME] = message_pair( TEST_FORMAT, &test_message_handler );
	message_handlers[BODYINFO_CAM1_OB1_NAME] = message_pair( BODYINFO_CAM1_OB1_FORMAT, &bodyInfo_CAM1_OB1_message_handler );
	message_handlers[BODYINFO_CAM1_OB2_NAME] = message_pair( BODYINFO_CAM1_OB2_FORMAT, &bodyInfo_CAM1_OB2_message_handler );
	message_handlers[BODYINFO_CAM1_OB3_NAME] = message_pair( BODYINFO_CAM1_OB3_FORMAT, &bodyInfo_CAM1_OB3_message_handler );
	message_handlers[BODYINFO_CAM1_OB4_NAME] = message_pair( BODYINFO_CAM1_OB4_FORMAT, &bodyInfo_CAM1_OB4_message_handler );
	message_handlers[BODYINFO_CAM1_OB5_NAME] = message_pair( BODYINFO_CAM1_OB5_FORMAT, &bodyInfo_CAM1_OB5_message_handler );
	message_handlers[BODYINFO_CAM2_OB1_NAME] = message_pair( BODYINFO_CAM2_OB1_FORMAT, &bodyInfo_CAM2_OB1_message_handler );
	message_handlers[BODYINFO_CAM2_OB2_NAME] = message_pair( BODYINFO_CAM2_OB2_FORMAT, &bodyInfo_CAM2_OB2_message_handler );
	message_handlers[BODYINFO_CAM2_OB3_NAME] = message_pair( BODYINFO_CAM2_OB3_FORMAT, &bodyInfo_CAM2_OB3_message_handler );
	message_handlers[BODYINFO_CAM2_OB4_NAME] = message_pair( BODYINFO_CAM2_OB4_FORMAT, &bodyInfo_CAM2_OB4_message_handler );
	message_handlers[BODYINFO_CAM2_OB5_NAME] = message_pair( BODYINFO_CAM2_OB5_FORMAT, &bodyInfo_CAM2_OB5_message_handler );
	message_handlers[BODYINFO_CAM3_OB1_NAME] = message_pair( BODYINFO_CAM3_OB1_FORMAT, &bodyInfo_CAM3_OB1_message_handler );
	message_handlers[BODYINFO_CAM3_OB2_NAME] = message_pair( BODYINFO_CAM3_OB2_FORMAT, &bodyInfo_CAM3_OB2_message_handler );
	message_handlers[BODYINFO_CAM3_OB3_NAME] = message_pair( BODYINFO_CAM3_OB3_FORMAT, &bodyInfo_CAM3_OB3_message_handler );
	message_handlers[BODYINFO_CAM3_OB4_NAME] = message_pair( BODYINFO_CAM3_OB4_FORMAT, &bodyInfo_CAM3_OB4_message_handler );
	message_handlers[BODYINFO_CAM3_OB5_NAME] = message_pair( BODYINFO_CAM3_OB5_FORMAT, &bodyInfo_CAM3_OB5_message_handler );


	message_handlers[POS3D_CAM1_OB1_NAME] = message_pair( POS3D_CAM1_OB1_FORMAT, &pos3D_CAM1_OB1_message_handler );
	message_handlers[POS3D_CAM1_OB2_NAME] = message_pair( POS3D_CAM1_OB2_FORMAT, &pos3D_CAM1_OB2_message_handler );
	message_handlers[POS3D_CAM1_OB3_NAME] = message_pair( POS3D_CAM1_OB3_FORMAT, &pos3D_CAM1_OB3_message_handler );
	message_handlers[POS3D_CAM1_OB4_NAME] = message_pair( POS3D_CAM1_OB4_FORMAT, &pos3D_CAM1_OB4_message_handler );
	message_handlers[POS3D_CAM1_OB5_NAME] = message_pair( POS3D_CAM1_OB5_FORMAT, &pos3D_CAM1_OB5_message_handler );
	message_handlers[POS3D_CAM2_OB1_NAME] = message_pair( POS3D_CAM2_OB1_FORMAT, &pos3D_CAM2_OB1_message_handler );
	message_handlers[POS3D_CAM2_OB2_NAME] = message_pair( POS3D_CAM2_OB2_FORMAT, &pos3D_CAM2_OB2_message_handler );
	message_handlers[POS3D_CAM2_OB3_NAME] = message_pair( POS3D_CAM2_OB3_FORMAT, &pos3D_CAM2_OB3_message_handler );
	message_handlers[POS3D_CAM2_OB4_NAME] = message_pair( POS3D_CAM2_OB4_FORMAT, &pos3D_CAM2_OB4_message_handler );
	message_handlers[POS3D_CAM2_OB5_NAME] = message_pair( POS3D_CAM2_OB5_FORMAT, &pos3D_CAM2_OB5_message_handler );
	message_handlers[POS3D_CAM3_OB1_NAME] = message_pair( POS3D_CAM3_OB1_FORMAT, &pos3D_CAM3_OB1_message_handler );
	message_handlers[POS3D_CAM3_OB2_NAME] = message_pair( POS3D_CAM3_OB2_FORMAT, &pos3D_CAM3_OB2_message_handler );
	message_handlers[POS3D_CAM3_OB3_NAME] = message_pair( POS3D_CAM3_OB3_FORMAT, &pos3D_CAM3_OB3_message_handler );
	message_handlers[POS3D_CAM3_OB4_NAME] = message_pair( POS3D_CAM3_OB4_FORMAT, &pos3D_CAM3_OB4_message_handler );
	message_handlers[POS3D_CAM3_OB5_NAME] = message_pair( POS3D_CAM3_OB5_FORMAT, &pos3D_CAM3_OB5_message_handler );



	message_handlers[CAMLINE_CAM1_OB1_NAME] = message_pair( CAMLINE_CAM1_OB1_FORMAT, &camLine_CAM1_OB1_message_handler );
	message_handlers[CAMLINE_CAM1_OB2_NAME] = message_pair( CAMLINE_CAM1_OB2_FORMAT, &camLine_CAM1_OB2_message_handler );
	message_handlers[CAMLINE_CAM1_OB3_NAME] = message_pair( CAMLINE_CAM1_OB3_FORMAT, &camLine_CAM1_OB3_message_handler );
	message_handlers[CAMLINE_CAM1_OB4_NAME] = message_pair( CAMLINE_CAM1_OB4_FORMAT, &camLine_CAM1_OB4_message_handler );
	message_handlers[CAMLINE_CAM1_OB5_NAME] = message_pair( CAMLINE_CAM1_OB5_FORMAT, &camLine_CAM1_OB5_message_handler );
	message_handlers[CAMLINE_CAM2_OB1_NAME] = message_pair( CAMLINE_CAM2_OB1_FORMAT, &camLine_CAM2_OB1_message_handler );
	message_handlers[CAMLINE_CAM2_OB2_NAME] = message_pair( CAMLINE_CAM2_OB2_FORMAT, &camLine_CAM2_OB2_message_handler );
	message_handlers[CAMLINE_CAM2_OB3_NAME] = message_pair( CAMLINE_CAM2_OB3_FORMAT, &camLine_CAM2_OB3_message_handler );
	message_handlers[CAMLINE_CAM2_OB4_NAME] = message_pair( CAMLINE_CAM2_OB4_FORMAT, &camLine_CAM2_OB4_message_handler );
	message_handlers[CAMLINE_CAM2_OB5_NAME] = message_pair( CAMLINE_CAM2_OB5_FORMAT, &camLine_CAM2_OB5_message_handler );
	message_handlers[CAMLINE_CAM3_OB1_NAME] = message_pair( CAMLINE_CAM3_OB1_FORMAT, &camLine_CAM3_OB1_message_handler );
	message_handlers[CAMLINE_CAM3_OB2_NAME] = message_pair( CAMLINE_CAM3_OB2_FORMAT, &camLine_CAM3_OB2_message_handler );
	message_handlers[CAMLINE_CAM3_OB3_NAME] = message_pair( CAMLINE_CAM3_OB3_FORMAT, &camLine_CAM3_OB3_message_handler );
	message_handlers[CAMLINE_CAM3_OB4_NAME] = message_pair( CAMLINE_CAM3_OB4_FORMAT, &camLine_CAM3_OB4_message_handler );
	message_handlers[CAMLINE_CAM3_OB5_NAME] = message_pair( CAMLINE_CAM3_OB5_FORMAT, &camLine_CAM3_OB5_message_handler );
	

	message_handlers[ALIVESTA_CAM1_NAME] = message_pair( ALIVESTA_CAM1_FORMAT, &aliveSta_CAM1_message_handler );
	message_handlers[ALIVESTA_CAM2_NAME] = message_pair( ALIVESTA_CAM2_FORMAT, &aliveSta_CAM2_message_handler );
	message_handlers[ALIVESTA_CAM3_NAME] = message_pair( ALIVESTA_CAM3_FORMAT, &aliveSta_CAM3_message_handler );
	
	message_handlers[LABELINTEGRATE_CAM1_NAME] = message_pair( LABELINTEGRATE_CAM1_FORMAT, &labelIntegrate_CAM1_message_handler );
	message_handlers[LABELINTEGRATE_CAM2_NAME] = message_pair( LABELINTEGRATE_CAM2_FORMAT, &labelIntegrate_CAM2_message_handler );
	message_handlers[LABELINTEGRATE_CAM3_NAME] = message_pair( LABELINTEGRATE_CAM3_FORMAT, &labelIntegrate_CAM3_message_handler );

}

void IPCClientMgr::define_message(const char *msgName, const char *formatString)
{
	if(!IPC_isMsgDefined(msgName)){
		printf("defining message(%s)!\n", msgName);
		IPC_defineMsg(msgName, IPC_VARIABLE_LENGTH, formatString);
	}
}

//--------------------------------------------------//
// get messages
//--------------------------------------------------//
void getLaserPose(LaserPoseMgr& mgr)
{
	mgr = laserposemgr;
}

void getSLaser(SLaserMgr& mgr)
{
	mgr = slasermgr;
}

void getSubgoal(SubgoalMgr& mgr)
{
	mgr = subgoalmgr;
}
void getOdometry(OdometryMgr& mgr)
{
	mgr = odometrymgr;
}

void getMotor(MotorMgr& mgr)
{
	mgr = motormgr;
}

void getSpeedMode(SpeedModeMgr& mgr)
{
	mgr = SpeedModemgr;
}

void getHeadMotor(HeadMotorMgr& mgr){
	mgr = HeadMotormgr;
}

void getHandPos(HandPosMgr& mgr){
	mgr = handposmgr;
}

void getFacialExp(FacialExpMgr& mgr){
	mgr = facialexpmgr;
}

void getMic19(Mic19Mgr& mgr){
	mgr = mic19mgr;
}

void getMic20(Mic20Mgr& mgr){
	mgr = mic20mgr;
}

void getMic21(Mic21Mgr& mgr){
	mgr = mic21mgr;
}

void getMic22(Mic22Mgr& mgr){
	mgr = mic22mgr;
}

void getState(StateMgr& mgr){
	mgr = statemgr;
}

void getGoal(GoalMgr& mgr){
	mgr = goalmgr;
}

void getPeople(PeopleMgr& mgr){
	mgr = peoplemgr;
}

void getWord(WordMgr& mgr){
	mgr = wordmgr;
}

void getDetect(Detect& mgr){
	mgr = detectmgr;
}

void getLocState(LocState& mgr){
	mgr = loc_state;
}

void getSpeak(SpeakMgr& mgr){
	mgr = speakmgr;
}



void getHistCrCb(HistCrCbMgr_CAM1_OB1& mgr)
{mgr = histcrcbmgr_CAM1_OB1;}
void getHistCrCb(HistCrCbMgr_CAM1_OB2& mgr)
{mgr = histcrcbmgr_CAM1_OB2;}
void getHistCrCb(HistCrCbMgr_CAM1_OB3& mgr)
{mgr = histcrcbmgr_CAM1_OB3;}
void getHistCrCb(HistCrCbMgr_CAM1_OB4& mgr)
{mgr = histcrcbmgr_CAM1_OB4;}
void getHistCrCb(HistCrCbMgr_CAM1_OB5& mgr)
{mgr = histcrcbmgr_CAM1_OB5;}
void getHistCrCb(HistCrCbMgr_CAM2_OB1& mgr)
{mgr = histcrcbmgr_CAM2_OB1;}
void getHistCrCb(HistCrCbMgr_CAM2_OB2& mgr)
{mgr = histcrcbmgr_CAM2_OB2;}
void getHistCrCb(HistCrCbMgr_CAM2_OB3& mgr)
{mgr = histcrcbmgr_CAM2_OB3;}
void getHistCrCb(HistCrCbMgr_CAM2_OB4& mgr)
{mgr = histcrcbmgr_CAM2_OB4;}
void getHistCrCb(HistCrCbMgr_CAM2_OB5& mgr)
{mgr = histcrcbmgr_CAM2_OB5;}
void getHistCrCb(HistCrCbMgr_CAM3_OB1& mgr)
{mgr = histcrcbmgr_CAM3_OB1;}
void getHistCrCb(HistCrCbMgr_CAM3_OB2& mgr)
{mgr = histcrcbmgr_CAM3_OB2;}
void getHistCrCb(HistCrCbMgr_CAM3_OB3& mgr)
{mgr = histcrcbmgr_CAM3_OB3;}
void getHistCrCb(HistCrCbMgr_CAM3_OB4& mgr)
{mgr = histcrcbmgr_CAM3_OB4;}
void getHistCrCb(HistCrCbMgr_CAM3_OB5& mgr)
{mgr = histcrcbmgr_CAM3_OB5;}



void gettest(test& mgr)
{
	mgr = t;
}
void getBodyInfo(BodyInfoMgr_CAM1_OB1& mgr)
{mgr = bodyinfomgr_CAM1_OB1;}
void getBodyInfo(BodyInfoMgr_CAM1_OB2& mgr)
{mgr = bodyinfomgr_CAM1_OB2;}
void getBodyInfo(BodyInfoMgr_CAM1_OB3& mgr)
{mgr = bodyinfomgr_CAM1_OB3;}
void getBodyInfo(BodyInfoMgr_CAM1_OB4& mgr)
{mgr = bodyinfomgr_CAM1_OB4;}
void getBodyInfo(BodyInfoMgr_CAM1_OB5& mgr)
{mgr = bodyinfomgr_CAM1_OB5;}
void getBodyInfo(BodyInfoMgr_CAM2_OB1& mgr)
{mgr = bodyinfomgr_CAM2_OB1;}
void getBodyInfo(BodyInfoMgr_CAM2_OB2& mgr)
{mgr = bodyinfomgr_CAM2_OB2;}
void getBodyInfo(BodyInfoMgr_CAM2_OB3& mgr)
{mgr = bodyinfomgr_CAM2_OB3;}
void getBodyInfo(BodyInfoMgr_CAM2_OB4& mgr)
{mgr = bodyinfomgr_CAM2_OB4;}
void getBodyInfo(BodyInfoMgr_CAM2_OB5& mgr)
{mgr = bodyinfomgr_CAM2_OB5;}
void getBodyInfo(BodyInfoMgr_CAM3_OB1& mgr)
{mgr = bodyinfomgr_CAM3_OB1;}
void getBodyInfo(BodyInfoMgr_CAM3_OB2& mgr)
{mgr = bodyinfomgr_CAM3_OB2;}
void getBodyInfo(BodyInfoMgr_CAM3_OB3& mgr)
{mgr = bodyinfomgr_CAM3_OB3;}
void getBodyInfo(BodyInfoMgr_CAM3_OB4& mgr)
{mgr = bodyinfomgr_CAM3_OB4;}
void getBodyInfo(BodyInfoMgr_CAM3_OB5& mgr)
{mgr = bodyinfomgr_CAM3_OB5;}



void getPos3D(Pos3DMgr_CAM1_OB1& mgr)
{mgr = pos3dmgr_CAM1_OB1;}
void getPos3D(Pos3DMgr_CAM1_OB2& mgr)
{mgr = pos3dmgr_CAM1_OB2;}
void getPos3D(Pos3DMgr_CAM1_OB3& mgr)
{mgr = pos3dmgr_CAM1_OB3;}
void getPos3D(Pos3DMgr_CAM1_OB4& mgr)
{mgr = pos3dmgr_CAM1_OB4;}
void getPos3D(Pos3DMgr_CAM1_OB5& mgr)
{mgr = pos3dmgr_CAM1_OB5;}
void getPos3D(Pos3DMgr_CAM2_OB1& mgr)
{mgr = pos3dmgr_CAM2_OB1;}
void getPos3D(Pos3DMgr_CAM2_OB2& mgr)
{mgr = pos3dmgr_CAM2_OB2;}
void getPos3D(Pos3DMgr_CAM2_OB3& mgr)
{mgr = pos3dmgr_CAM2_OB3;}
void getPos3D(Pos3DMgr_CAM2_OB4& mgr)
{mgr = pos3dmgr_CAM2_OB4;}
void getPos3D(Pos3DMgr_CAM2_OB5& mgr)
{mgr = pos3dmgr_CAM2_OB5;}
void getPos3D(Pos3DMgr_CAM3_OB1& mgr)
{mgr = pos3dmgr_CAM3_OB1;}
void getPos3D(Pos3DMgr_CAM3_OB2& mgr)
{mgr = pos3dmgr_CAM3_OB2;}
void getPos3D(Pos3DMgr_CAM3_OB3& mgr)
{mgr = pos3dmgr_CAM3_OB3;}
void getPos3D(Pos3DMgr_CAM3_OB4& mgr)
{mgr = pos3dmgr_CAM3_OB4;}
void getPos3D(Pos3DMgr_CAM3_OB5& mgr)
{mgr = pos3dmgr_CAM3_OB5;}




void getCamLine(CamLineMgr_CAM1_OB1& mgr)
{mgr = camlinemgr_CAM1_OB1;}
void getCamLine(CamLineMgr_CAM1_OB2& mgr)
{mgr = camlinemgr_CAM1_OB2;}
void getCamLine(CamLineMgr_CAM1_OB3& mgr)
{mgr = camlinemgr_CAM1_OB3;}
void getCamLine(CamLineMgr_CAM1_OB4& mgr)
{mgr = camlinemgr_CAM1_OB4;}
void getCamLine(CamLineMgr_CAM1_OB5& mgr)
{mgr = camlinemgr_CAM1_OB5;}
void getCamLine(CamLineMgr_CAM2_OB1& mgr)
{mgr = camlinemgr_CAM2_OB1;}
void getCamLine(CamLineMgr_CAM2_OB2& mgr)
{mgr = camlinemgr_CAM2_OB2;}
void getCamLine(CamLineMgr_CAM2_OB3& mgr)
{mgr = camlinemgr_CAM2_OB3;}
void getCamLine(CamLineMgr_CAM2_OB4& mgr)
{mgr = camlinemgr_CAM2_OB4;}
void getCamLine(CamLineMgr_CAM2_OB5& mgr)
{mgr = camlinemgr_CAM2_OB5;}
void getCamLine(CamLineMgr_CAM3_OB1& mgr)
{mgr = camlinemgr_CAM3_OB1;}
void getCamLine(CamLineMgr_CAM3_OB2& mgr)
{mgr = camlinemgr_CAM3_OB2;}
void getCamLine(CamLineMgr_CAM3_OB3& mgr)
{mgr = camlinemgr_CAM3_OB3;}
void getCamLine(CamLineMgr_CAM3_OB4& mgr)
{mgr = camlinemgr_CAM3_OB4;}
void getCamLine(CamLineMgr_CAM3_OB5& mgr)
{mgr = camlinemgr_CAM3_OB5;}





void getAliveSta(AliveStaMgr_CAM1& mgr)
{mgr = alivestamgr_CAM1;}
void getAliveSta(AliveStaMgr_CAM2& mgr)
{mgr = alivestamgr_CAM2;}
void getAliveSta(AliveStaMgr_CAM3& mgr)
{mgr = alivestamgr_CAM3;}




void getLabelIntegrate(LabelIntegrateMgr_CAM1& mgr)
{mgr = labelintegratemgr_CAM1;}
void getLabelIntegrate(LabelIntegrateMgr_CAM2& mgr)
{mgr = labelintegratemgr_CAM2;}
void getLabelIntegrate(LabelIntegrateMgr_CAM3& mgr)
{mgr = labelintegratemgr_CAM3;}

//--------------------------------------------------//
// set messages
//--------------------------------------------------//
IPC_RETURN_TYPE setLaserPose(LaserPoseMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(LASERPOSE_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setSLaser(SLaserMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(SLASER_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setSubgoal(SubgoalMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(SUBGOAL_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setOdometry(OdometryMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(ODOMETRY_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setMotor(MotorMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(MOTOR_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setSpeedMode(SpeedModeMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(SPEEDMODE_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setHeadMotor(HeadMotorMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HEADMOTOR_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setHandPos(HandPosMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HANDPOS_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setFacialExp(FacialExpMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(FACIAlEXP_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setMic19(Mic19Mgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(MIC19_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setMic20(Mic20Mgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(MIC20_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setMic21(Mic21Mgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(MIC21_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setMic22(Mic22Mgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(MIC22_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setState(StateMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(STATE_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setGoal(GoalMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(GOAL_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setPeople(PeopleMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(PEOPLE_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setWord(WordMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(WORD_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setDetect(Detect& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(DETECT_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setLocState(LocState& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(LOCSTATE_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}

IPC_RETURN_TYPE setSpeak(SpeakMgr& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(SPEAK_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}




IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM1_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM1_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM1_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM1_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM1_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM1_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM2_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM2_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM2_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM2_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM2_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM2_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM3_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM3_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM3_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM3_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setHistCrCb(HistCrCbMgr_CAM3_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(HISTCRCB_CAM3_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}





IPC_RETURN_TYPE settest(test& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(TEST_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM1_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM1_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM1_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM1_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM1_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM1_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM2_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM2_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM2_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM2_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM2_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM2_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM3_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM3_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM3_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM3_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setBodyInfo(BodyInfoMgr_CAM3_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(BODYINFO_CAM3_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}






IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM1_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM1_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM1_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM1_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM1_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM1_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM2_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM2_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM2_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM2_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM2_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM2_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM3_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM3_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM3_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM3_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setPos3D(Pos3DMgr_CAM3_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(POS3D_CAM3_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}







IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM1_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM1_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM1_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM1_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM1_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM1_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM2_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM2_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM2_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM2_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM2_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM2_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM3_OB1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM3_OB2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM3_OB3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB4& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM3_OB4_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setCamLine(CamLineMgr_CAM3_OB5& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(CAMLINE_CAM3_OB5_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}






IPC_RETURN_TYPE setAliveSta(AliveStaMgr_CAM1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(ALIVESTA_CAM1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setAliveSta(AliveStaMgr_CAM2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(ALIVESTA_CAM2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setAliveSta(AliveStaMgr_CAM3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(ALIVESTA_CAM3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}




IPC_RETURN_TYPE setLabelIntegrate(LabelIntegrateMgr_CAM1& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(LABELINTEGRATE_CAM1_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setLabelIntegrate(LabelIntegrateMgr_CAM2& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(LABELINTEGRATE_CAM2_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}
IPC_RETURN_TYPE setLabelIntegrate(LabelIntegrateMgr_CAM3& mgr){
	IPC_CS.lock();
	IPC_RETURN_TYPE ret = IPC_publishData(LABELINTEGRATE_CAM3_NAME, &mgr);
	IPC_CS.unlock();
	return ret;
}




bool trans2HistCrCb(HistCrCbMgr_CAM1_OB1* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB2* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB3* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB4* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM1_OB5* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB1* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB2* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB3* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB4* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM2_OB5* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB1* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB2* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB3* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB4* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}
bool trans2HistCrCb(HistCrCbMgr_CAM3_OB5* src, CvHistogram* dst)
{
	if (!dst) return false;
	if (!src) return false;

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst, id0, id1)) = src->binCrCb[Cr_BINS*id0 + id1];
		}
	}
	return true;

}









bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB1* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB2* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB3* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB4* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM1_OB5* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB1* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB2* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB3* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB4* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM2_OB5* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB1* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB2* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB3* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB4* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}
bool trans2HistCrCbMgr(CvHistogram* src, HistCrCbMgr_CAM3_OB5* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src, id0, id1));
		}
	}

	dst->timeIndex = timeIndex;

	return true;
}





bool trans2BodyInfo(BodyInfoMgr_CAM1_OB1* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB2* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB3* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB4* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM1_OB5* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB1* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB2* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB3* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB4* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM2_OB5* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB1* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB2* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB3* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB4* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}
bool trans2BodyInfo(BodyInfoMgr_CAM3_OB5* src, BodyInfo* dst)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = ((src->IsHeightKnown == 0) ? false : true);
	dst->height = src->height;

	dst->Blocks[0].x = src->Blocks0xywh[0];
	dst->Blocks[0].y = src->Blocks0xywh[1];
	dst->Blocks[0].width = src->Blocks0xywh[2];
	dst->Blocks[0].height = src->Blocks0xywh[3];

	dst->Blocks[1].x = src->Blocks1xywh[0];
	dst->Blocks[1].y = src->Blocks1xywh[1];
	dst->Blocks[1].width = src->Blocks1xywh[2];
	dst->Blocks[1].height = src->Blocks1xywh[3];

	dst->Blocks[2].x = src->Blocks2xywh[0];
	dst->Blocks[2].y = src->Blocks2xywh[1];
	dst->Blocks[2].width = src->Blocks2xywh[2];
	dst->Blocks[2].height = src->Blocks2xywh[3];

	dst->Blocks[3].x = src->Blocks3xywh[0];
	dst->Blocks[3].y = src->Blocks3xywh[1];
	dst->Blocks[3].width = src->Blocks3xywh[2];
	dst->Blocks[3].height = src->Blocks3xywh[3];

	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			*(cvGetHistValue_2D(dst->body[0], id0, id1)) = src->binCrCb0[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[1], id0, id1)) = src->binCrCb1[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[2], id0, id1)) = src->binCrCb2[Cr_BINS*id0 + id1];
			*(cvGetHistValue_2D(dst->body[3], id0, id1)) = src->binCrCb3[Cr_BINS*id0 + id1];
		}
	}
	
	return true;
}











bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB1* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB2* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB3* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB4* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM1_OB5* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB1* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB2* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB3* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB4* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM2_OB5* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB1* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB2* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB3* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB4* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}
bool trans2BodyInfoMgr(BodyInfo* src, BodyInfoMgr_CAM3_OB5* dst, int timeIndex)
{
	if (!dst) return false;
	if (!src) return false;

	dst->IsHeightKnown = src->IsHeightKnown;
	dst->height = src->height;

	dst->Blocks0xywh[0] = src->Blocks[0].x;
	dst->Blocks0xywh[1] = src->Blocks[0].y;
	dst->Blocks0xywh[2] = src->Blocks[0].width;
	dst->Blocks0xywh[3] = src->Blocks[0].height;

	dst->Blocks1xywh[0] = src->Blocks[1].x;
	dst->Blocks1xywh[1] = src->Blocks[1].y;
	dst->Blocks1xywh[2] = src->Blocks[1].width;
	dst->Blocks1xywh[3] = src->Blocks[1].height;

	dst->Blocks2xywh[0] = src->Blocks[2].x;
	dst->Blocks2xywh[1] = src->Blocks[2].y;
	dst->Blocks2xywh[2] = src->Blocks[2].width;
	dst->Blocks2xywh[3] = src->Blocks[2].height;

	dst->Blocks3xywh[0] = src->Blocks[3].x;
	dst->Blocks3xywh[1] = src->Blocks[3].y;
	dst->Blocks3xywh[2] = src->Blocks[3].width;
	dst->Blocks3xywh[3] = src->Blocks[3].height;


	for (int k=0; k<4; k++)
	{
		dst->IsBodySeen[k] = src->IsBodySeen[k];
	}

	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			dst->binCrCb0[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[0], id0, id1));
			dst->binCrCb1[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[1], id0, id1));
			dst->binCrCb2[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[2], id0, id1));
			dst->binCrCb3[Cr_BINS*id0 + id1] = *(cvGetHistValue_2D(src->body[3], id0, id1));
		}
	}
	
	dst->timeIndex = timeIndex;

	return true;
}









bool trans2Pos3D(Pos3DMgr_CAM1_OB1* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM1_OB2* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM1_OB3* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM1_OB4* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM1_OB5* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM2_OB1* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM2_OB2* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM2_OB3* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM2_OB4* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM2_OB5* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM3_OB1* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM3_OB2* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM3_OB3* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM3_OB4* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}
bool trans2Pos3D(Pos3DMgr_CAM3_OB5* src, Position3D* dst)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;

	return true;
}







bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB1* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB2* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB3* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB4* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM1_OB5* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB1* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB2* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB3* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB4* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM2_OB5* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB1* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB2* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB3* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB4* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}
bool trans2Pos3DMgr(Position3D* src, Pos3DMgr_CAM3_OB5* dst, int timeIndex)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->timeIndex = timeIndex;
	return true;
}





bool trans2CamLine(CamLineMgr_CAM1_OB1* src, CameraLine3D* dst)
{
	dst->camID = CAM1;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM1_OB2* src, CameraLine3D* dst)
{
	dst->camID = CAM1;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM1_OB3* src, CameraLine3D* dst)
{
	dst->camID = CAM1;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM1_OB4* src, CameraLine3D* dst)
{
	dst->camID = CAM1;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM1_OB5* src, CameraLine3D* dst)
{
	dst->camID = CAM1;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM2_OB1* src, CameraLine3D* dst)
{
	dst->camID = CAM2;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM2_OB2* src, CameraLine3D* dst)
{
	dst->camID = CAM2;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM2_OB3* src, CameraLine3D* dst)
{
	dst->camID = CAM2;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM2_OB4* src, CameraLine3D* dst)
{
	dst->camID = CAM2;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM2_OB5* src, CameraLine3D* dst)
{
	dst->camID = CAM2;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM3_OB1* src, CameraLine3D* dst)
{
	dst->camID = CAM3;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM3_OB2* src, CameraLine3D* dst)
{
	dst->camID = CAM3;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM3_OB3* src, CameraLine3D* dst)
{
	dst->camID = CAM3;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM3_OB4* src, CameraLine3D* dst)
{
	dst->camID = CAM3;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}
bool trans2CamLine(CamLineMgr_CAM3_OB5* src, CameraLine3D* dst)
{
	dst->camID = CAM3;

	dst->IsInit = true;

	dst->central_dir_vec.x = src->central_dir_vec[0];
	dst->central_dir_vec.y = src->central_dir_vec[1];
	dst->central_dir_vec.z = src->central_dir_vec[2];
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc.x = src->imageLoc[0];
	dst->imageLoc.y = src->imageLoc[1];

	dst->optic_center.x = src->optic_center[0];
	dst->optic_center.y = src->optic_center[1];
	dst->optic_center.z = src->optic_center[2];

	dst->ref_plane_center.x = src->ref_plane_center[0];
	dst->ref_plane_center.y = src->ref_plane_center[1];
	dst->ref_plane_center.z = src->ref_plane_center[2];

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refPlane.nor_dir_vec.x = src->refplane_nor_dir_vec[0];
	dst->refPlane.nor_dir_vec.y = src->refplane_nor_dir_vec[1];
	dst->refPlane.nor_dir_vec.z = src->refplane_nor_dir_vec[2];

	dst->refPlane.Stdpoint.x = src->refplane_Stdpoint[0];
	dst->refPlane.Stdpoint.y = src->refplane_Stdpoint[1];
	dst->refPlane.Stdpoint.z = src->refplane_Stdpoint[2];

	dst->refPlane.IsInit = true;

	dst->Stdpoint1.x = src->Stdpoint1[0];
	dst->Stdpoint1.y = src->Stdpoint1[1];
	dst->Stdpoint1.z = src->Stdpoint1[2];

	dst->Stdpoint2.x = src->Stdpoint2[0];
	dst->Stdpoint2.y = src->Stdpoint2[1];
	dst->Stdpoint2.z = src->Stdpoint2[2];

	dst->unit_dir_vec.x = src->unit_dir_vec[0];
	dst->unit_dir_vec.y = src->unit_dir_vec[1];
	dst->unit_dir_vec.z = src->unit_dir_vec[2];

	dst->unit_orth_right_vec.x = src->unit_orth_right_vec[0];
	dst->unit_orth_right_vec.y = src->unit_orth_right_vec[1];
	dst->unit_orth_right_vec.z = src->unit_orth_right_vec[2];

	dst->unit_orth_up_vec.x = src->unit_orth_up_vec[0];
	dst->unit_orth_up_vec.y = src->unit_orth_up_vec[1];
	dst->unit_orth_up_vec.z = src->unit_orth_up_vec[2];

	return true;
}










bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB1* dst, int timeIndex)
{
	if (src->camID != CAM1) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB2* dst, int timeIndex)
{
	if (src->camID != CAM1) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB3* dst, int timeIndex)
{
	if (src->camID != CAM1) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB4* dst, int timeIndex)
{
	if (src->camID != CAM1) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM1_OB5* dst, int timeIndex)
{
	if (src->camID != CAM1) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB1* dst, int timeIndex)
{
	if (src->camID != CAM2) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB2* dst, int timeIndex)
{
	if (src->camID != CAM2) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB3* dst, int timeIndex)
{
	if (src->camID != CAM2) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB4* dst, int timeIndex)
{
	if (src->camID != CAM2) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM2_OB5* dst, int timeIndex)
{
	if (src->camID != CAM2) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB1* dst, int timeIndex)
{
	if (src->camID != CAM3) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB2* dst, int timeIndex)
{
	if (src->camID != CAM3) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB3* dst, int timeIndex)
{
	if (src->camID != CAM3) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB4* dst, int timeIndex)
{
	if (src->camID != CAM3) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}
bool trans2CamLineMgr(CameraLine3D* src, CamLineMgr_CAM3_OB5* dst, int timeIndex)
{
	if (src->camID != CAM3) return false;

	dst->timeIndex = timeIndex;

	dst->central_dir_vec[0] = src->central_dir_vec.x;
	dst->central_dir_vec[1] = src->central_dir_vec.y;
	dst->central_dir_vec[2] = src->central_dir_vec.z;
	
	dst->cm2pixelRatio = src->cm2pixelRatio;
	dst->h2wRatio = src->h2wRatio;

	dst->imageLoc[0] = src->imageLoc.x;
	dst->imageLoc[1] = src->imageLoc.y;

	dst->optic_center[0] = src->optic_center.x;
	dst->optic_center[1] = src->optic_center.y;
	dst->optic_center[2] = src->optic_center.z;

	dst->ref_plane_center[0] = src->ref_plane_center.x;
	dst->ref_plane_center[1] = src->ref_plane_center.y;
	dst->ref_plane_center[2] = src->ref_plane_center.z;

	dst->ref_plane_dist = src->ref_plane_dist;

	dst->refplane_nor_dir_vec[0] = src->refPlane.nor_dir_vec.x;
	dst->refplane_nor_dir_vec[1] = src->refPlane.nor_dir_vec.y;
	dst->refplane_nor_dir_vec[2] = src->refPlane.nor_dir_vec.z;

	dst->refplane_Stdpoint[0] = src->refPlane.Stdpoint.x;
	dst->refplane_Stdpoint[1] = src->refPlane.Stdpoint.y;
	dst->refplane_Stdpoint[2] = src->refPlane.Stdpoint.z;

	dst->Stdpoint1[0] = src->Stdpoint1.x;
	dst->Stdpoint1[1] = src->Stdpoint1.y;
	dst->Stdpoint1[2] = src->Stdpoint1.z;

	dst->Stdpoint2[0] = src->Stdpoint2.x;
	dst->Stdpoint2[1] = src->Stdpoint2.y;
	dst->Stdpoint2[2] = src->Stdpoint2.z;

	dst->unit_dir_vec[0] = src->unit_dir_vec.x;
	dst->unit_dir_vec[1] = src->unit_dir_vec.y;
	dst->unit_dir_vec[2] = src->unit_dir_vec.z;

	dst->unit_orth_right_vec[0] = src->unit_orth_right_vec.x;
	dst->unit_orth_right_vec[1] = src->unit_orth_right_vec.y;
	dst->unit_orth_right_vec[2] = src->unit_orth_right_vec.z;

	dst->unit_orth_up_vec[0] = src->unit_orth_up_vec.x;
	dst->unit_orth_up_vec[1] = src->unit_orth_up_vec.y;
	dst->unit_orth_up_vec[2] = src->unit_orth_up_vec.z;

	return true;
}