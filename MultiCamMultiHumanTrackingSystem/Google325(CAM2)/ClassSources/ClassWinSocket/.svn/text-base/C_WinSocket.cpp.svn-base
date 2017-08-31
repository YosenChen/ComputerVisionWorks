#include "C_WinSocket.h"
#include <ctime>

const double C_WinSocketClient::RESEND_PERIOD_TIME = 2.0;
const double C_WinSocketClient::GET_SERVER_REPLY_PERIOD_TIME = 4.0;

int C_WinSocket::mInitialize(int iLength)
{
	// initialize win socket
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	miLength=iLength;
	mData=new char[iLength];
	memset(mData,' ',miLength);
	mpDataTemp=mData;

	if(iResult!=0)
	{
		printf("\n WSAStartup Error %d\n",iResult);
	}
	/*else
	{
		mSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if (mSocket == INVALID_SOCKET) 
		{
			WSACleanup();
			printf("\n Invalid Socket\n");
			iResult=-1;
		}
	}*/

	return iResult;
}

int C_WinSocketServer::mInitialize(int port, int connections,int iLength,int iSndBufLen,int iRcvBufLength)
{
	int iResult=C_WinSocket::mInitialize(iLength);

	if(iSndBufLen>0)
		mSndBufLen=iSndBufLen;

	if(iRcvBufLength>0)
		mRecvBufLen=iRcvBufLength;
	
	if(iResult==0)
	{
		memset(&mAddress, 0, sizeof(mAddress));

		//mAddress.sin_family = AF_INET;
		//mAddress.sin_port = htons(  port );

		mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mSocket == INVALID_SOCKET) 
		{
			printf("\n Invalid Socket\n");
		}
		else
		{

			//nonblocking
			/*unsigned long nNonBlocking=0;
			if (ioctlsocket(mSocket, FIONBIO, &nNonBlocking) == SOCKET_ERROR)
			{
				printf("Unable to set nonblocking mode, ec:%d\n", WSAGetLastError());
				closesocket(mSocket);
				return -1;
			}*/

			mAddress.sin_family = AF_INET;
			mAddress.sin_port = htons (port);
			mAddress.sin_addr.s_addr = htonl (INADDR_ANY);

			if(bind(mSocket, (sockaddr *)&mAddress, sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				closesocket(mSocket);
				printf("\n Invalid Socket\n");
				iResult=-1;
			}
			else
			{
				miMaxConnections=connections;
				dataSocket=new C_WinSocket[connections];
				for(int i=0;i<connections;i++)
				{
					dataSocket[i].mInitialize(iLength);
				}
				listen(mSocket, connections);   
				mbInitialized=true;
			}
		}
	}

	return iResult;
}

int C_WinSocketServer::mAccept(int iSndBufLen,int iRcvBufLen)
{
  //unsigned int msg;
  if(WSAAsyncSelect(mSocket,GetForegroundWindow(),0,FD_CONNECT)!=SOCKET_ERROR)
  {
	  SOCKET new_sock = accept(mSocket, 0, 0);
	  if (new_sock != INVALID_SOCKET && miNumConnected<miMaxConnections) 
	  {
		  dataSocket[miNumConnected].mAssignSOCKET(new_sock);

		  if(mSndBufLen>0)
			iSndBufLen=mSndBufLen;

		  if(mRecvBufLen>0)
			iRcvBufLen=mRecvBufLen;

		  if(iSndBufLen>=0)
		{
			int window_size = iSndBufLen;
			int length;
			int optLen = sizeof(int);
			setsockopt(dataSocket[miNumConnected].mSocket,SOL_SOCKET,SO_SNDBUF,(char *) &window_size, sizeof(window_size));
			getsockopt(dataSocket[miNumConnected].mSocket, SOL_SOCKET, SO_SNDBUF,(char*) &length,&optLen);
			int a=2;
		}

		if(iRcvBufLen>=0)
		{
			int window_size = iSndBufLen;
			int length;
			int optLen = sizeof(int);
			setsockopt(dataSocket[miNumConnected].mSocket,SOL_SOCKET,SO_SNDBUF,(char *) &window_size, sizeof(window_size));
			getsockopt(dataSocket[miNumConnected].mSocket, SOL_SOCKET, SO_SNDBUF,(char*) &length,&optLen);
			int a=2;
		}
		  miNumConnected++;
		  return 0;
	  }
  }

   return -1;
}

int   C_WinSocketServer::mRecv(int iClientIndex)
{
	int iSize=dataSocket[iClientIndex].mRecv();
	return iSize;
}

int C_WinSocketServer::mSend(int iClientIndex,char* data)
{
	sprintf(dataSocket[iClientIndex].mData,data);
	return dataSocket[iClientIndex].mSend();
}

int C_WinSocketServer::mSend(int iClientIndex,char* data,int iLengthOffset)
{
	sprintf(dataSocket[iClientIndex].mData,data);
	return dataSocket[iClientIndex].mSend(iLengthOffset);
}



int C_WinSocketClient::mInitialize(const char* ip,int port,int iLength,int iSndBufLen,int iRcvBufLen)
{
	int iResult=C_WinSocket::mInitialize(iLength);

	mSocket = socket(AF_INET,SOCK_STREAM,0);

	if (mSocket == INVALID_SOCKET)
	{
		WSACleanup();
		printf("\n Invalid Socket\n");
		iResult=-1;
	}
	else
	{
		//nonblocking
		unsigned long nNonBlocking=1;
		if (ioctlsocket(mSocket, FIONBIO, &nNonBlocking) == SOCKET_ERROR)
		{
			printf("Unable to set nonblocking mode, ec:%d\n", WSAGetLastError());
			closesocket(mSocket);
			return -1;
		}
	}

	if(iSndBufLen>=0)
	{
		mSndBufLen=iSndBufLen;
		int window_size = iSndBufLen;
		int length;
		int optLen = sizeof(int);
		setsockopt(mSocket,SOL_SOCKET,SO_SNDBUF,(char *) &window_size, sizeof(window_size));
		getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF,(char*) &length,&optLen);
		int a=2;
	}

	if(iRcvBufLen>=0)
	{
		mRecvBufLen=iRcvBufLen;
		int window_size = iSndBufLen;
		int length;
		int optLen = sizeof(int);
		setsockopt(mSocket,SOL_SOCKET,SO_SNDBUF,(char *) &window_size, sizeof(window_size));
		getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF,(char*) &length,&optLen);
		int a=2;
	}

	
    mAddress.sin_family = AF_INET;
    mAddress.sin_addr.s_addr = inet_addr( ip );
    mAddress.sin_port = htons( port );
	
	if( connect( mSocket, (SOCKADDR*) &mAddress, sizeof(sockaddr_in) ) == SOCKET_ERROR) 
	{
		//need select (Non Blocking May not receive connection immediately)
		struct timeval oTV;
		oTV.tv_sec = 3000 / 1000;
		oTV.tv_usec = 3000;
		fd_set oRead, oWrite;
		FD_ZERO(&oRead);
		FD_ZERO(&oWrite);
		int nResult;//, nError;

		FD_SET(mSocket, &oRead);
        oWrite = oRead;


		nResult = select(mSocket+1, &oRead, &oWrite, 0, &oTV);

		if (nResult == 0 || nResult==SOCKET_ERROR ) 
        {
            //printf( "\nFailed to connect.\n" );
			closesocket( mSocket );
			WSACleanup();
			return -1;
        }
		else
		{
			mbInitialized = true;
		}
	}
	else
	{
		mbInitialized = true;
	}

	if( mbInitialized )
		time (&mStartTimeStamp);

	return iResult;
}

bool C_WinSocketClient::disConnect()
{
	// disable sending & receiving datum
	shutdown (mSocket, 2);
	
	LINGER lig;
	int   iLen;

	lig.l_onoff = 1;  
	lig.l_linger = 0;  
	iLen = sizeof(struct   linger); 
	// discard all data waiting for sending immediatly
	setsockopt( mSocket, SOL_SOCKET, SO_LINGER, (char*)&lig, iLen );  

	// release resources occupied by this socket
    closesocket(mSocket);
    WSACleanup(); // correspoding resource releaser associated with WSAStartup()

	return true;
}

bool C_WinSocketClient::isServerAlive()
{	
	if( !mbInitialized )
		return false;

	time (&mEndTimeStamp);

	if( mHasReceive == true ) 
	{
		if( difftime( mEndTimeStamp, mStartTimeStamp ) >= RESEND_PERIOD_TIME ) // send test alive message periodicity
		{
			mWriteInt( 0 );
			mWriteInt( 0 );
			mWriteInt( 0 );
			mWriteInt( 0 );
			mWriteInt( 3 );
			//_cprintf( "ask server alive\n" );
			mSend();
			mHasSend = true;
			mHasReceive = false;
			time (&mStartTimeStamp); // recount
			return true;
		}

		return true;
	}
	if( mHasSend == true ) // send test alive message
	{
		int size = mRecv();

		if( size > 0 )
		{
			int value = mGetInt();
			//_cprintf("%d\n", value);

			if( value == 1 ) // OK, server reply
			{
				mHasReceive = true;
				mHasSend = false;

				time (&mStartTimeStamp);
				//_cprintf( "OK server reply\n" );
				return true;
			}
		}
		if( difftime( mEndTimeStamp, mStartTimeStamp ) >= GET_SERVER_REPLY_PERIOD_TIME ) // server is dead
		{
			//_cprintf( "server is dead\n" );
			mHasReceive = true;
			mHasSend = false;
			return false;
		}
		//_cprintf( "missed" );
	}

	return true;
}