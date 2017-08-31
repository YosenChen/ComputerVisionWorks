#pragma once

//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "process.h"

//
//#include <stdio.h>
//#include <fcntl.h>
//#include <io.h>
//
//void showWin32Console()
//{
//	static const WORD MAX_CONSOLE_LINES = 500;
//	int hConHandle;
//	long lStdHandle;
//	CONSOLE_SCREEN_BUFFER_INFO coninfo;
//	FILE *fp;
//	// allocate a console for this app
//	AllocConsole();
//	// set the screen buffer to be big enough to let us scroll text
//	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
//	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
//	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
//	coninfo.dwSize);
//	// redirect unbuffered STDOUT to the console
//	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
//	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
//	fp = _fdopen( hConHandle, "w" );
//	*stdout = *fp;
//	setvbuf( stdout, NULL, _IONBF, 0 );
//	// redirect unbuffered STDIN to the console
//	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
//	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
//	fp = _fdopen( hConHandle, "r" );
//	*stdin = *fp;
//	setvbuf( stdin, NULL, _IONBF, 0 );
//	// redirect unbuffered STDERR to the console
//	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
//	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
//	fp = _fdopen( hConHandle, "w" );
//	*stderr = *fp;
//	setvbuf( stderr, NULL, _IONBF, 0 );
//	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
//	// point to console as well
//	std::ios::sync_with_stdio();
//}


#include "../ClassSources/CPerformanceCounter/CPerformanceCounter.h"
#include "../ClassSources/ClassOpenCVMat/ClassOpenCVMat.h"
#include "../ClassSources/C_ImageSource/C_ImageSourceWebCam.h"
#include "../ClassSources/ClassMotionDetection/OpticalFlow.h"
#include <direct.h>
#include "Vcclr.h"
#include "highgui.h"
#include "Client.hpp"

//OpticalFlow *motionDetection  NULL;
int     globalThreadNr;                    // Number of threads started
HANDLE globalhThread[2];
bool imageSampleReady = false;
bool keepRecFlag = true;
IplImage *recSrc = cvCreateImage(cvSize(320,240),8,3); 
IplImage *recSrcPrev = cvCreateImage(cvSize(320,240),8,3); 
IplImage *recRes = cvCreateImage(cvSize(320,240),8,3);
IplImage *recResPrev = cvCreateImage(cvSize(320,240),8,3);
bool readFromAVI_out;
bool initializeTracking_out;
CvVideoWriter* srcVideo_out;
CvVideoWriter* resVideo_out;
#define REC_SAMPLINGRATE 25

namespace Form_ImageSource {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
    private: System::Windows::Forms::Panel^  connectionPanel;
	private: System::Windows::Forms::Button^  btnSave;
			 
	private: System::Windows::Forms::TextBox^  IPAddress;
	private: System::Windows::Forms::TextBox^  port;
	private: System::Windows::Forms::Button^  btnConnect;
	private: System::Windows::Forms::Button^  btnPlay;
	private: System::Windows::Forms::Button^  btnStep;

	public:
		CPerformanceCounter        *mpCounterForFPS;
		CPerformanceCounter        *mpCounterForProcessingTime;	
		OpticalFlow                *motionDetection;
		//C_ImageSourceManager       *mpImageSourceMgr;
		C_ImageSourceWebCam		   *mImageSourceWebCam;
		ClassOpenCVMat             *matColorDisplayImage;
		ClassOpenCVMat             *matColorDisplayResult;
		ClassOpenCVMat             *matTempBlob;
		ClassOpenCVMat             *matTempImage;

		int                        miFrameIndex;
		int                        miSaveIndex;
		bool                       mbGrab;
		IplImage					*imageTemp;
		IplImage					*test_img;
		IplImage                    *pImageSource320240;
		IplImage					*pImageSource;
		IplImage					*distortImg;
		CvCapture					*srcCapture;
		bool                        readFromAVI;
		bool						takephoto;
		bool						showCalibLines;
		bool						showCalibLines_old;
		CvMat						*CAM1_intrinsic;
		CvMat						*CAM2_intrinsic;
		CvMat						*CAM1_dist_coeffs;
		CvMat						*CAM2_dist_coeffs;
		int							onlineCamNo; 
		bool						initializeTracking;


		static const int mWidthCapturedImage = 320;
		static const int mHeightCapturedImage = 240;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel2;


	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Panel^  panel3;

	private: System::Windows::Forms::Button^  btnAutoInitialize;


	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;

	private: System::Windows::Forms::TextBox^  normalMessagePresenter;
    private: System::Windows::Forms::TextBox^  controlMessagePresenter;







		bool                       mbStep;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::CheckBox^  isCAM1;
	private: System::Windows::Forms::CheckBox^  isCAM2;
	private: System::Windows::Forms::CheckBox^  isCAM3;





		bool					   mStartCount;
		void                       mInitializeMyObjects();
		void                       mReleaseMyObjects();

		//Processing things happend for each period
		void                       mProcessLoop();
		void					   mPrintCalibLine(int camid);
		void					   mPrintCalibLine_oldforComparison(int camid);
		IplImage*                  getCurrnentImageFromCam();
		void					   setupServer();
		void					   globaladdThread();

		void                       mShowFPS( int fps );
        void                       mShowTimer();
		void	                   showMessage( string *normalMessage, string *controlMessage );
		System::String^			   stdString2SystemString( string s );

		void                       mStringToChar(System::String^ str,char *chr)
		{
			 pin_ptr<const wchar_t> wch =PtrToStringChars(str);

			 size_t convertedChars = 0;
			 size_t  sizeInBytes = ((str->Length + 1) * 2);
			 wcstombs_s(&convertedChars, 
					chr, sizeInBytes,
					wch, sizeInBytes);
		}


	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::TextBox^  textProcessTime;
	
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::RadioButton^  radBtnSource2;
	private: System::Windows::Forms::RadioButton^  radBtnSource1;
	private: System::Windows::Forms::TrackBar^  barMotionThreshold;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  labMotionThreshold;
	private: System::Drawing::Bitmap^   mBitMapOriginal;
	private: System::Drawing::Bitmap^   mBitMapBlob;
    private: System::Drawing::Bitmap^   mBitMapImage;
    private: System::Drawing::Bitmap^   mBitMapResult;
    private: System::Drawing::Bitmap^   mBitMapHistimg;
    private: System::Drawing::Bitmap^   mBitMapPurifiedHistimg;


    private: System::Windows::Forms::PictureBox^  pictureBox3;
    private: System::Windows::Forms::PictureBox^  pictureBox4;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::TextBox^  txtFrameIndex;

		//--------------------------------------------------------
	public:
		Form1(void)
		{
			// _CrtSetBreakAlloc(276);

			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			mInitializeMyObjects();
			timer1->Start();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}

			mReleaseMyObjects();

			_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		}

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->textProcessTime = (gcnew System::Windows::Forms::TextBox());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox4 = (gcnew System::Windows::Forms::PictureBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->radBtnSource2 = (gcnew System::Windows::Forms::RadioButton());
			this->radBtnSource1 = (gcnew System::Windows::Forms::RadioButton());
			this->labMotionThreshold = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->barMotionThreshold = (gcnew System::Windows::Forms::TrackBar());
			this->connectionPanel = (gcnew System::Windows::Forms::Panel());
			this->normalMessagePresenter = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->IPAddress = (gcnew System::Windows::Forms::TextBox());
			this->port = (gcnew System::Windows::Forms::TextBox());
			this->btnConnect = (gcnew System::Windows::Forms::Button());
			this->btnSave = (gcnew System::Windows::Forms::Button());
			this->txtFrameIndex = (gcnew System::Windows::Forms::TextBox());
			this->btnPlay = (gcnew System::Windows::Forms::Button());
			this->btnStep = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->btnAutoInitialize = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->controlMessagePresenter = (gcnew System::Windows::Forms::TextBox());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->isCAM1 = (gcnew System::Windows::Forms::CheckBox());
			this->isCAM2 = (gcnew System::Windows::Forms::CheckBox());
			this->isCAM3 = (gcnew System::Windows::Forms::CheckBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->barMotionThreshold))->BeginInit();
			this->connectionPanel->SuspendLayout();
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			this->SuspendLayout();
			// 
			// timer1
			// 
			this->timer1->Interval = 1;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// textProcessTime
			// 
			this->textProcessTime->Font = (gcnew System::Drawing::Font(L"PMingLiU", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->textProcessTime->Location = System::Drawing::Point(102, 3);
			this->textProcessTime->Margin = System::Windows::Forms::Padding(2);
			this->textProcessTime->Name = L"textProcessTime";
			this->textProcessTime->Size = System::Drawing::Size(45, 25);
			this->textProcessTime->TabIndex = 1;
			// 
			// pictureBox3
			// 
			this->pictureBox3->Location = System::Drawing::Point(334, 22);
			this->pictureBox3->Margin = System::Windows::Forms::Padding(2);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(320, 240);
			this->pictureBox3->TabIndex = 4;
			this->pictureBox3->TabStop = false;
			this->pictureBox3->Click += gcnew System::EventHandler(this, &Form1::pictureBox3_Click);
			// 
			// pictureBox4
			// 
			this->pictureBox4->Location = System::Drawing::Point(9, 22);
			this->pictureBox4->Margin = System::Windows::Forms::Padding(2);
			this->pictureBox4->Name = L"pictureBox4";
			this->pictureBox4->Size = System::Drawing::Size(320, 240);
			this->pictureBox4->TabIndex = 5;
			this->pictureBox4->TabStop = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label1->Location = System::Drawing::Point(5, 7);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(91, 16);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Process Time";
			// 
			// radBtnSource2
			// 
			this->radBtnSource2->Location = System::Drawing::Point(0, 0);
			this->radBtnSource2->Name = L"radBtnSource2";
			this->radBtnSource2->Size = System::Drawing::Size(104, 24);
			this->radBtnSource2->TabIndex = 0;
			// 
			// radBtnSource1
			// 
			this->radBtnSource1->Location = System::Drawing::Point(0, 0);
			this->radBtnSource1->Name = L"radBtnSource1";
			this->radBtnSource1->Size = System::Drawing::Size(104, 24);
			this->radBtnSource1->TabIndex = 0;
			// 
			// labMotionThreshold
			// 
			this->labMotionThreshold->Location = System::Drawing::Point(0, 0);
			this->labMotionThreshold->Name = L"labMotionThreshold";
			this->labMotionThreshold->Size = System::Drawing::Size(100, 23);
			this->labMotionThreshold->TabIndex = 0;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(32, 30);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(0, 12);
			this->label2->TabIndex = 1;
			// 
			// barMotionThreshold
			// 
			this->barMotionThreshold->Location = System::Drawing::Point(160, 30);
			this->barMotionThreshold->Maximum = 100;
			this->barMotionThreshold->Name = L"barMotionThreshold";
			this->barMotionThreshold->Size = System::Drawing::Size(136, 45);
			this->barMotionThreshold->TabIndex = 0;
			this->barMotionThreshold->TickFrequency = 25;
			// 
			// connectionPanel
			// 
			this->connectionPanel->Controls->Add(this->normalMessagePresenter);
			this->connectionPanel->Controls->Add(this->label3);
			this->connectionPanel->Controls->Add(this->label4);
			this->connectionPanel->Controls->Add(this->IPAddress);
			this->connectionPanel->Controls->Add(this->port);
			this->connectionPanel->Controls->Add(this->btnConnect);
			this->connectionPanel->Location = System::Drawing::Point(11, 267);
			this->connectionPanel->Margin = System::Windows::Forms::Padding(2);
			this->connectionPanel->Name = L"connectionPanel";
			this->connectionPanel->Size = System::Drawing::Size(318, 195);
			this->connectionPanel->TabIndex = 10;
			// 
			// normalMessagePresenter
			// 
			this->normalMessagePresenter->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->normalMessagePresenter->Location = System::Drawing::Point(8, 55);
			this->normalMessagePresenter->MaxLength = 7692;
			this->normalMessagePresenter->Multiline = true;
			this->normalMessagePresenter->Name = L"normalMessagePresenter";
			this->normalMessagePresenter->ReadOnly = true;
			this->normalMessagePresenter->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->normalMessagePresenter->Size = System::Drawing::Size(287, 137);
			this->normalMessagePresenter->TabIndex = 14;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label3->Location = System::Drawing::Point(58, 4);
			this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(21, 16);
			this->label3->TabIndex = 13;
			this->label3->Text = L"IP";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label4->Location = System::Drawing::Point(129, 4);
			this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(33, 16);
			this->label4->TabIndex = 13;
			this->label4->Text = L"port";
			// 
			// IPAddress
			// 
			this->IPAddress->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->IPAddress->Location = System::Drawing::Point(8, 22);
			this->IPAddress->Margin = System::Windows::Forms::Padding(2);
			this->IPAddress->MaxLength = 15;
			this->IPAddress->Name = L"IPAddress";
			this->IPAddress->Size = System::Drawing::Size(116, 27);
			this->IPAddress->TabIndex = 0;
			this->IPAddress->Text = L"140.112.91.196";
			// 
			// port
			// 
			this->port->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->port->Location = System::Drawing::Point(128, 22);
			this->port->Margin = System::Windows::Forms::Padding(2);
			this->port->MaxLength = 5;
			this->port->Name = L"port";
			this->port->Size = System::Drawing::Size(51, 27);
			this->port->TabIndex = 0;
			this->port->Text = L"22556";
			this->port->TextChanged += gcnew System::EventHandler(this, &Form1::port_TextChanged);
			// 
			// btnConnect
			// 
			this->btnConnect->Font = (gcnew System::Drawing::Font(L"PMingLiU", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->btnConnect->ForeColor = System::Drawing::Color::Green;
			this->btnConnect->Location = System::Drawing::Point(193, 21);
			this->btnConnect->Margin = System::Windows::Forms::Padding(2);
			this->btnConnect->Name = L"btnConnect";
			this->btnConnect->Size = System::Drawing::Size(81, 29);
			this->btnConnect->TabIndex = 12;
			this->btnConnect->Text = L"Connect";
			this->btnConnect->UseVisualStyleBackColor = true;
			this->btnConnect->Click += gcnew System::EventHandler(this, &Form1::btnConnect_Click);
			// 
			// btnSave
			// 
			this->btnSave->Location = System::Drawing::Point(0, 0);
			this->btnSave->Name = L"btnSave";
			this->btnSave->Size = System::Drawing::Size(75, 23);
			this->btnSave->TabIndex = 0;
			// 
			// txtFrameIndex
			// 
			this->txtFrameIndex->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->txtFrameIndex->Location = System::Drawing::Point(101, 48);
			this->txtFrameIndex->Margin = System::Windows::Forms::Padding(2);
			this->txtFrameIndex->Name = L"txtFrameIndex";
			this->txtFrameIndex->Size = System::Drawing::Size(46, 27);
			this->txtFrameIndex->TabIndex = 11;
			// 
			// btnPlay
			// 
			this->btnPlay->Font = (gcnew System::Drawing::Font(L"Times New Roman", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnPlay->ForeColor = System::Drawing::Color::Green;
			this->btnPlay->Location = System::Drawing::Point(12, 16);
			this->btnPlay->Margin = System::Windows::Forms::Padding(2);
			this->btnPlay->Name = L"btnPlay";
			this->btnPlay->Size = System::Drawing::Size(75, 29);
			this->btnPlay->TabIndex = 12;
			this->btnPlay->Text = L"Grab";
			this->btnPlay->UseVisualStyleBackColor = true;
			this->btnPlay->Click += gcnew System::EventHandler(this, &Form1::btnPlay_Click);
			// 
			// btnStep
			// 
			this->btnStep->Font = (gcnew System::Drawing::Font(L"Times New Roman", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnStep->Location = System::Drawing::Point(102, 16);
			this->btnStep->Margin = System::Windows::Forms::Padding(2);
			this->btnStep->Name = L"btnStep";
			this->btnStep->Size = System::Drawing::Size(62, 29);
			this->btnStep->TabIndex = 13;
			this->btnStep->Text = L"Step";
			this->btnStep->UseVisualStyleBackColor = true;
			this->btnStep->Click += gcnew System::EventHandler(this, &Form1::btnStep_Click);
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->textProcessTime);
			this->panel1->Controls->Add(this->txtFrameIndex);
			this->panel1->Location = System::Drawing::Point(672, 2);
			this->panel1->Margin = System::Windows::Forms::Padding(2);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(229, 86);
			this->panel1->TabIndex = 14;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label5->Location = System::Drawing::Point(37, 51);
			this->label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(32, 16);
			this->label5->TabIndex = 4;
			this->label5->Text = L"FPS";
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->btnStep);
			this->panel2->Controls->Add(this->btnPlay);
			this->panel2->Location = System::Drawing::Point(672, 92);
			this->panel2->Margin = System::Windows::Forms::Padding(2);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(180, 56);
			this->panel2->TabIndex = 15;
			// 
			// panel3
			// 
			this->panel3->Controls->Add(this->button3);
			this->panel3->Controls->Add(this->button2);
			this->panel3->Controls->Add(this->button1);
			this->panel3->Controls->Add(this->btnAutoInitialize);
			this->panel3->Location = System::Drawing::Point(672, 239);
			this->panel3->Margin = System::Windows::Forms::Padding(2);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(252, 165);
			this->panel3->TabIndex = 16;
			// 
			// button3
			// 
			this->button3->Font = (gcnew System::Drawing::Font(L"Times New Roman", 15.75F));
			this->button3->Location = System::Drawing::Point(12, 119);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(217, 32);
			this->button3->TabIndex = 15;
			this->button3->Text = L"Take Seq Photo";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button2
			// 
			this->button2->Font = (gcnew System::Drawing::Font(L"Times New Roman", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button2->Location = System::Drawing::Point(12, 81);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(217, 31);
			this->button2->TabIndex = 14;
			this->button2->Text = L"Take Picture";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// button1
			// 
			this->button1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(12, 45);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(217, 30);
			this->button1->TabIndex = 13;
			this->button1->Text = L"ReadFromAVI";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// btnAutoInitialize
			// 
			this->btnAutoInitialize->Font = (gcnew System::Drawing::Font(L"Times New Roman", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnAutoInitialize->Location = System::Drawing::Point(12, 11);
			this->btnAutoInitialize->Margin = System::Windows::Forms::Padding(2);
			this->btnAutoInitialize->Name = L"btnAutoInitialize";
			this->btnAutoInitialize->Size = System::Drawing::Size(217, 29);
			this->btnAutoInitialize->TabIndex = 12;
			this->btnAutoInitialize->Text = L"Initialize Tracking";
			this->btnAutoInitialize->UseVisualStyleBackColor = true;
			this->btnAutoInitialize->Click += gcnew System::EventHandler(this, &Form1::btnAutoInitialize_Click);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label6->Location = System::Drawing::Point(483, 2);
			this->label6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(47, 16);
			this->label6->TabIndex = 13;
			this->label6->Text = L"Image";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label7->Location = System::Drawing::Point(117, 2);
			this->label7->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(47, 16);
			this->label7->TabIndex = 13;
			this->label7->Text = L"Result";
			this->label7->Click += gcnew System::EventHandler(this, &Form1::label7_Click);
			// 
			// controlMessagePresenter
			// 
			this->controlMessagePresenter->Font = (gcnew System::Drawing::Font(L"PMingLiU", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->controlMessagePresenter->Location = System::Drawing::Point(334, 267);
			this->controlMessagePresenter->MaxLength = 7692;
			this->controlMessagePresenter->Multiline = true;
			this->controlMessagePresenter->Name = L"controlMessagePresenter";
			this->controlMessagePresenter->ReadOnly = true;
			this->controlMessagePresenter->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->controlMessagePresenter->Size = System::Drawing::Size(319, 200);
			this->controlMessagePresenter->TabIndex = 14;
			// 
			// button4
			// 
			this->button4->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button4->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->button4->Location = System::Drawing::Point(684, 429);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(96, 33);
			this->button4->TabIndex = 17;
			this->button4->Text = L"Calib Lines";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click);
			// 
			// button5
			// 
			this->button5->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9.75F));
			this->button5->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->button5->Location = System::Drawing::Point(805, 429);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(96, 33);
			this->button5->TabIndex = 18;
			this->button5->Text = L"Old Calib Lines";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &Form1::button5_Click);
			// 
			// isCAM1
			// 
			this->isCAM1->AutoSize = true;
			this->isCAM1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->isCAM1->Location = System::Drawing::Point(672, 186);
			this->isCAM1->Name = L"isCAM1";
			this->isCAM1->Size = System::Drawing::Size(68, 21);
			this->isCAM1->TabIndex = 19;
			this->isCAM1->Text = L"CAM1";
			this->isCAM1->UseVisualStyleBackColor = true;
			this->isCAM1->CheckedChanged += gcnew System::EventHandler(this, &Form1::isCAM1_CheckedChanged);
			// 
			// isCAM2
			// 
			this->isCAM2->AutoSize = true;
			this->isCAM2->Font = (gcnew System::Drawing::Font(L"Times New Roman", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->isCAM2->Location = System::Drawing::Point(746, 186);
			this->isCAM2->Name = L"isCAM2";
			this->isCAM2->Size = System::Drawing::Size(68, 21);
			this->isCAM2->TabIndex = 20;
			this->isCAM2->Text = L"CAM2";
			this->isCAM2->UseVisualStyleBackColor = true;
			this->isCAM2->CheckedChanged += gcnew System::EventHandler(this, &Form1::isCAM2_CheckedChanged);
			// 
			// isCAM3
			// 
			this->isCAM3->AutoSize = true;
			this->isCAM3->Font = (gcnew System::Drawing::Font(L"Times New Roman", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->isCAM3->Location = System::Drawing::Point(820, 186);
			this->isCAM3->Name = L"isCAM3";
			this->isCAM3->Size = System::Drawing::Size(68, 21);
			this->isCAM3->TabIndex = 21;
			this->isCAM3->Text = L"CAM3";
			this->isCAM3->UseVisualStyleBackColor = true;
			this->isCAM3->CheckedChanged += gcnew System::EventHandler(this, &Form1::isCAM3_CheckedChanged);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(5, 9);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(936, 495);
			this->Controls->Add(this->isCAM3);
			this->Controls->Add(this->isCAM2);
			this->Controls->Add(this->isCAM1);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->controlMessagePresenter);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->pictureBox3);
			this->Controls->Add(this->pictureBox4);
			this->Controls->Add(this->connectionPanel);
			this->Font = (gcnew System::Drawing::Font(L"PMingLiU", 7, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->Margin = System::Windows::Forms::Padding(2);
			this->Name = L"Form1";
			this->Text = L"ControlPanel";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->barMotionThreshold))->EndInit();
			this->connectionPanel->ResumeLayout(false);
			this->connectionPanel->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel3->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				 //if(!chkMotionThreshold->Checked)
					//barMotionThreshold->Enabled=false;
			 }
	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
								
				 cvDestroyAllWindows();
 				
				 keepRecFlag = false;


				 WaitForMultipleObjects( 1, globalhThread, TRUE, INFINITE );
				 CloseHandle( globalhThread[0] );

				 cvReleaseVideoWriter(&(srcVideo_out));
				 cvReleaseVideoWriter(&(resVideo_out));

				 //mpImageSourceMgr->mpSource->mFinalize();

			 }

	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
				//mpCounter->StartCount();
				if( mStartCount == true )
				{
					mpCounterForFPS->ResetPeriodController( 1.0 );
					mStartCount = false;
				}
				//mpCounter->EndCount();
				if( mpCounterForFPS->CheckPeriodController() == true )
				{
					mStartCount = true;
					mShowFPS( miFrameIndex );
					miFrameIndex = 0;
				}

				mProcessLoop();


				//mElapsedTime += getElapsedTime();
				//if( mElapsedTime >= 1.0 )
				//{
				//	mShowFPS( miFrameIndex );
				//	miFrameIndex = 0;
				//	mElapsedTime = 0.0;
				//}
				
				//show information
			 }

private: System::Void btnPlay_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(btnPlay->Text=="Grab")
			 {
				 btnPlay->Text="Stop";
				 btnPlay->ForeColor=System::Drawing::Color::Red;
				 mbGrab=true;
			 }
			 else if(btnPlay->Text=="Stop")
			 {
				 btnPlay->Text="Grab";
				 btnPlay->ForeColor=System::Drawing::Color::Green;
				 mbGrab=false;
			 }
		 }
private: System::Void btnStep_Click(System::Object^  sender, System::EventArgs^  e) {
			 btnPlay->Text="Grab";
			 btnPlay->ForeColor=System::Drawing::Color::Green;
			 mbGrab=false;
			 mbStep=true;
		 }

	System::Void btnConnect_Click(System::Object^  sender, System::EventArgs^  e) {
					 
			  //infoPresenter->ReadOnly = true;
			 if(btnConnect->Text=="Connect")
			 {
				 if( IPAddress->Text->Length > 0 && port->Text->Length > 0 )
				 {
					 char ip[64];
				     mStringToChar( IPAddress->Text, ip );
					// mStringToChar( port->Text, portNum );

					 //if( motionDetection->connect( ip, System::Int32::Parse(port->Text) ) == true )
					 //{
						// btnConnect->Text="Disconnect";
						// btnConnect->ForeColor=System::Drawing::Color::Gray;
					 //}
				 }
			 }
			 else if(btnConnect->Text=="Disconnect")
			 {
				 btnConnect->Text="Connect";
				 btnConnect->ForeColor=System::Drawing::Color::Green;
//				 motionDetection->disConnect();
			 }
			 showMessage( motionDetection->getNormalMessage(), motionDetection->getControlMessage() );
		 }
private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label4_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void port_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void pictureBox3_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void btnAutoInitialize_Click(System::Object^  sender, System::EventArgs^  e) {
			 initializeTracking_out = initializeTracking = true;
			 motionDetection->activateMotionDetection();
		 }
private: System::Void btnDeletcPoints_Click(System::Object^  sender, System::EventArgs^  e) {
			motionDetection->activateDeletingPoints();
		 }
private: System::Void btnNightMode_Click(System::Object^  sender, System::EventArgs^  e) {
			motionDetection->activateNightMode();
		 }
private: System::Void label7_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 readFromAVI_out = readFromAVI = true;
			 cvWaitKey(1000);
			 if (onlineCamNo == CAM1)
				 srcCapture = cvCaptureFromAVI(/*"srcVideo[CAM1]201005071811_short_sync_cvtd.avi"*/"srcVideo201004202324_cam1_sync2_cvtd.avi"/*"srcVideo201004270017_cam1_short_cvtd.avi"*/);
			 else if (onlineCamNo == CAM2)
				 srcCapture = cvCaptureFromAVI(/*"srcVideo[CAM2]201005071811_short_sync_cvtd.avi"*/"srcVideo201004202324_cam2_sync_cvtd.avi"/*"srcVideo201004270017_cam2_short_cvtd.avi"*/);
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			 takephoto = true;
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			 motionDetection->takeSeqPhoto = !motionDetection->takeSeqPhoto;
		 }
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
			 showCalibLines = !showCalibLines;
		 }
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
			 showCalibLines_old = !showCalibLines_old;
		 }
private: System::Void isCAM1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (onlineCamNo == NONDEFINED)			
			 {
				 onlineCamNo = CAM1;
				 globaladdThread();
				 motionDetection = OpticalFlow::getSingletonPtr( mWidthCapturedImage, mHeightCapturedImage, CAM1 );
				 motionDetection->initialize( matColorDisplayImage, matColorDisplayResult );
				 //motionDetection->srcVideo=cvCreateVideoWriter("srcVideo[CAM1].avi",-1,22,cvSize(320,240),1);
				 //motionDetection->resVideo=cvCreateVideoWriter("resVideo[CAM1].avi",-1,22,cvSize(320,240),1);
				 //setup server
				 setupServer();
			 }
		 }
private: System::Void isCAM2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (onlineCamNo == NONDEFINED)
			 {	
				 onlineCamNo = CAM2;
				 globaladdThread();
				 motionDetection = OpticalFlow::getSingletonPtr( mWidthCapturedImage, mHeightCapturedImage, CAM2 );
				 motionDetection->initialize( matColorDisplayImage, matColorDisplayResult );
				 //motionDetection->srcVideo=cvCreateVideoWriter("srcVideo[CAM2].avi",-1,22,cvSize(320,240),1);
				 //motionDetection->resVideo=cvCreateVideoWriter("resVideo[CAM2].avi",-1,22,cvSize(320,240),1);
				 //setup server
				 setupServer();
			 }
		 }
private: System::Void isCAM3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (onlineCamNo == NONDEFINED)
			 {	
				 onlineCamNo = CAM3;
				 globaladdThread();
				 motionDetection = OpticalFlow::getSingletonPtr( mWidthCapturedImage, mHeightCapturedImage, CAM3 );
				 motionDetection->initialize( matColorDisplayImage, matColorDisplayResult );
				 //motionDetection->srcVideo=cvCreateVideoWriter("srcVideo[CAM3].avi",-1,22,cvSize(320,240),1);
				 //motionDetection->resVideo=cvCreateVideoWriter("resVideo[CAM3].avi",-1,22,cvSize(320,240),1);
				 //setup server
				 setupServer();
			 }
		 }
};

}

