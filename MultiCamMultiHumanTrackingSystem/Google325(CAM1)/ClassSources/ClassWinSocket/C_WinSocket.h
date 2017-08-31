
#include "Windows.h"
#pragma comment( lib, "Wsock32.lib" )
#pragma once
#include <stdio.h>

class C_WinSocket
{
public:
	char*         mData;
	bool          mbInitialized;
	char*         mpDataTemp;

	int           miDataCount;
	SOCKET        mSocket;

	int           mRecvBufLen;  //No. of Bytes
	int           mSndBufLen;   //No. of Bytes


protected:
	WSADATA       wsaData;
	sockaddr_in   mAddress;

	int           miLength;



public:
	C_WinSocket()
	{
		mbInitialized=false;
		miLength=0;
		miDataCount=0;
		mRecvBufLen=8192;
		mSndBufLen=8192;
	}

	virtual ~C_WinSocket()
	{
		//delete []mData;
		closesocket(mSocket);
		WSACleanup();
		
	}
	
	virtual int   mInitialize(int iDataLength);
	virtual int   mSend(){
						  int iResult=0;
						  if(miDataCount>0)
							  iResult=send(mSocket,mData,miDataCount,0);
						  mpDataTemp=mData;
						  miDataCount=0;
						  memset(mData,' ',miLength);
		          return iResult;}  //send whole buffer

	virtual int   mRecv(){
							u_long lenRecvData = 0;
							memset(mData,' ',miLength);
							mpDataTemp=mData;
							miDataCount=0;
							if (ioctlsocket(mSocket, FIONREAD, &lenRecvData) != 0)
								return -1;

							if (lenRecvData == 0)
								return -1;

							
						  int iExtractLength=lenRecvData;
						  if(iExtractLength>miLength)
							 iExtractLength=miLength;

		                  int iSize=recv(mSocket,mData,iExtractLength,0);  //receive whole buffer
						  
	                      return iSize;}

	void          mWriteInt(int value)
	{
		int* p=&value;
		char* c=(char*)p;
		mpDataTemp[0]=c[0];
		mpDataTemp[1]=c[1];
		mpDataTemp[2]=c[2];
		mpDataTemp[3]=c[3];
		mpDataTemp+=4;
		miDataCount+=4;
	}

	int           mGetInt(){
		miDataCount+=4;
		int* p=(int*)mpDataTemp;
		mpDataTemp=mpDataTemp+4;
		return (*p);
	}

	void          mWriteFloat(float value){
		float* p=&value;
		char* c=(char*)p;
		mpDataTemp[0]=c[0];
		mpDataTemp[1]=c[1];
		mpDataTemp[2]=c[2];
		mpDataTemp[3]=c[3];
		mpDataTemp+=4;
		miDataCount+=4;
	}
	float         mGetFloat(){
		miDataCount+=4;
		float* p=(float*)mpDataTemp;
		mpDataTemp=mpDataTemp+4;
		return (*p);
	}

	void          mWriteChar(char value){
		mpDataTemp[0]=value;
		
		mpDataTemp+=1;
		miDataCount+=1;
	}

	char         mGetChar(){
		miDataCount+=1;
		char c=*mpDataTemp;
		mpDataTemp=mpDataTemp+1;
		return c;
	}


	//virtual int   mSend(int iLengthOffset){return send(mSocket,mData,((int)strlen(mData))+iLengthOffset,0);}  //send whole buffer
	virtual int   mSend(int iLengthOffset){int iSize=send(mSocket,mData,strlen(mData)+1,0);
	                                       return iSize;}
	void          mAssignSOCKET(SOCKET sock){mSocket=sock;}

	
};


class C_WinSocketServer : public C_WinSocket
{
public:
	int   miMaxConnections;
	int   miNumConnected;
private:
	C_WinSocket*  dataSocket; 

public:
	C_WinSocketServer()
	{
		miNumConnected=0;
		dataSocket=NULL;
	}

	~C_WinSocketServer()
	{
		if(dataSocket!=NULL)
			delete []dataSocket;
	}

	int   mInitialize(int port, int connections,int iLength=32,int iSndBufLen=-1,int iRcvBufLength=-1);
	int   mAccept(int iSndBufLen=-1,int iRcvBufLen=-1);
	int   mSend(int iClientIndex,char* data);            //send whole buffer
	int   mSend(int iClientIndex,char* data,int iLengthOffset);  //depend on the string length + offset
	int   mRecv(int iClientIndex);
	C_WinSocket*  mGetDataSocket(int iClientIndex){return &dataSocket[iClientIndex];}

};

class C_WinSocketClient : public C_WinSocket
{
private:
	static const double RESEND_PERIOD_TIME;
	static const double GET_SERVER_REPLY_PERIOD_TIME;
	bool mHasSend;
	bool mHasReceive;
	time_t mStartTimeStamp;
	time_t mEndTimeStamp;

public:
	C_WinSocketClient()
	{
		mHasSend = false;
		mHasReceive = true;
	}

	~C_WinSocketClient()
	{
	}

	int mInitialize(const char* ip,int port,int iLength=32,int iSndBufLen=-1,int iRcvBufLength=-1);
	bool disConnect();
	bool isServerAlive();
};