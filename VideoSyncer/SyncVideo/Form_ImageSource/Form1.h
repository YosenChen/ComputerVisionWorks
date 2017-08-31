#pragma once

//#define _CRTDBG_MAP_ALLOC

#include "CPerformanceCounter.h"
#include "ClassOpenCVMat.h"
#include "C_ImageSourceWebCam.h"
#include "VideoSyncer.h"

#include <fstream>

#include "windows.h"

#include <ctype.h>
#include <stdio.h>

#include <direct.h>

#include "Vcclr.h"
#include <vector>

#include "highgui.h"

#define REC_SAMPLINGRATE 22

using namespace std;


/*VideoSyncer	*mVideoSyncer = new VideoSyncer;*//*("srcVideo201004202324_cam1_sync2_cvtd.avi", "test_cam1.avi",
											"srcVideo201004202324_cam2_sync_cvtd.avi", "test_cam2.avi",
											REC_SAMPLINGRATE);*/
CvFont	frameFont;

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

	private:
		CPerformanceCounter        *mpCounterForFPS;
		CPerformanceCounter        *mpCounterForProcessingTime;	
		C_ImageSourceWebCam		   *mImageSourceWebCam;
		ClassOpenCVMat             *matColorDisplayImage;
		ClassOpenCVMat             *matColorDisplayResult;
		ClassOpenCVMat             *matTempBlob;
		ClassOpenCVMat             *matTempImage;
		ClassOpenCVMat			   *matTemp;
		bool			           mRecognizing;
		bool			           mTraining;
		bool			           mSaving;

		int                        miFrameIndex;
		int                        miSaveIndex;
		bool                       mbGrab;
		IplImage					*imageDisp1;
		IplImage					*imageDisp2;
		int                        take_A_image;

		// object for Video sync
		VideoSyncer				   *mVideoSyncer;
		bool					   start_sync_video;
		
		//debug printing tool
		fstream					   *txtOut;




		static const int mWidthCapturedImage = 320;
		static const int mHeightCapturedImage = 240;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;


	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TextBox^  normalMessagePresenter;
    private: System::Windows::Forms::TextBox^  controlMessagePresenter;

		bool                       mbStep;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TabPage^  PatternRecognition;
	private: System::Windows::Forms::Button^  button_save;
	private: System::Windows::Forms::Label^  label_directiveName;
	private: System::Windows::Forms::CheckBox^  chkCircular;
	private: System::Windows::Forms::TextBox^  textArea;
	private: System::Windows::Forms::Button^  button_train;
	private: System::Windows::Forms::Button^  button_test;
	private: System::Windows::Forms::TabPage^  VisionTracking;
	private: System::Windows::Forms::Button^  btn_StartSyncVideo;

	private: System::Windows::Forms::TabControl^  TabControlPanel;
	private: System::Windows::Forms::Button^  btn_checkCapVideoDetails;

	private: System::Windows::Forms::Button^  btn_CheckRecVideoDetails;
	private: System::Windows::Forms::Button^  releaseVideo;















		bool				       mStartCount;
		void                       mInitializeMyObjects();
		void                       mReleaseMyObjects();

		//Processing things happend for each period
		void                       mProcessLoop();
		IplImage*                  getCurrnentImageFromCam();
		void	                   MarshalString( String ^ s, string& os );
		void                       mShowFPS( int fps );
        void                       mShowTimer();
		void	                   showTrackingMessage( string *normalMessage, string *controlMessage );
		void                       showPMMessage( const string& result);
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
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->controlMessagePresenter = (gcnew System::Windows::Forms::TextBox());
			this->PatternRecognition = (gcnew System::Windows::Forms::TabPage());
			this->button_save = (gcnew System::Windows::Forms::Button());
			this->label_directiveName = (gcnew System::Windows::Forms::Label());
			this->chkCircular = (gcnew System::Windows::Forms::CheckBox());
			this->textArea = (gcnew System::Windows::Forms::TextBox());
			this->button_train = (gcnew System::Windows::Forms::Button());
			this->button_test = (gcnew System::Windows::Forms::Button());
			this->VisionTracking = (gcnew System::Windows::Forms::TabPage());
			this->btn_CheckRecVideoDetails = (gcnew System::Windows::Forms::Button());
			this->btn_checkCapVideoDetails = (gcnew System::Windows::Forms::Button());
			this->btn_StartSyncVideo = (gcnew System::Windows::Forms::Button());
			this->TabControlPanel = (gcnew System::Windows::Forms::TabControl());
			this->releaseVideo = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->barMotionThreshold))->BeginInit();
			this->connectionPanel->SuspendLayout();
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->PatternRecognition->SuspendLayout();
			this->VisionTracking->SuspendLayout();
			this->TabControlPanel->SuspendLayout();
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
			this->textProcessTime->Location = System::Drawing::Point(119, 2);
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
			this->label1->Size = System::Drawing::Size(111, 16);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Processing Time";
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
			this->connectionPanel->Size = System::Drawing::Size(318, 200);
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
			this->normalMessagePresenter->Size = System::Drawing::Size(287, 142);
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
			this->IPAddress->Text = L"127.0.0.1";
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
			this->txtFrameIndex->Location = System::Drawing::Point(118, 48);
			this->txtFrameIndex->Margin = System::Windows::Forms::Padding(2);
			this->txtFrameIndex->Name = L"txtFrameIndex";
			this->txtFrameIndex->Size = System::Drawing::Size(46, 27);
			this->txtFrameIndex->TabIndex = 11;
			// 
			// btnPlay
			// 
			this->btnPlay->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
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
			this->btnStep->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
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
			this->label5->Location = System::Drawing::Point(46, 51);
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
			// PatternRecognition
			// 
			this->PatternRecognition->Controls->Add(this->button_save);
			this->PatternRecognition->Controls->Add(this->label_directiveName);
			this->PatternRecognition->Controls->Add(this->chkCircular);
			this->PatternRecognition->Controls->Add(this->textArea);
			this->PatternRecognition->Controls->Add(this->button_train);
			this->PatternRecognition->Controls->Add(this->button_test);
			this->PatternRecognition->Location = System::Drawing::Point(4, 19);
			this->PatternRecognition->Name = L"PatternRecognition";
			this->PatternRecognition->Padding = System::Windows::Forms::Padding(3);
			this->PatternRecognition->Size = System::Drawing::Size(221, 276);
			this->PatternRecognition->TabIndex = 1;
			this->PatternRecognition->Text = L"PatternRecognition";
			this->PatternRecognition->UseVisualStyleBackColor = true;
			// 
			// button_save
			// 
			this->button_save->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->button_save->Location = System::Drawing::Point(10, 179);
			this->button_save->Name = L"button_save";
			this->button_save->Size = System::Drawing::Size(199, 43);
			this->button_save->TabIndex = 14;
			this->button_save->Text = L"SAVE";
			this->button_save->UseVisualStyleBackColor = true;
			this->button_save->Click += gcnew System::EventHandler(this, &Form1::button_save_Click);
			// 
			// label_directiveName
			// 
			this->label_directiveName->AutoSize = true;
			this->label_directiveName->Font = (gcnew System::Drawing::Font(L"PMingLiU", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->label_directiveName->Location = System::Drawing::Point(68, 22);
			this->label_directiveName->Name = L"label_directiveName";
			this->label_directiveName->Size = System::Drawing::Size(77, 12);
			this->label_directiveName->TabIndex = 13;
			this->label_directiveName->Text = L"Directive Name";
			// 
			// chkCircular
			// 
			this->chkCircular->AutoSize = true;
			this->chkCircular->Location = System::Drawing::Point(21, 5);
			this->chkCircular->Name = L"chkCircular";
			this->chkCircular->Size = System::Drawing::Size(82, 14);
			this->chkCircular->TabIndex = 12;
			this->chkCircular->Text = L"Check Circular";
			this->chkCircular->UseVisualStyleBackColor = true;
			// 
			// textArea
			// 
			this->textArea->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->textArea->Location = System::Drawing::Point(7, 37);
			this->textArea->Name = L"textArea";
			this->textArea->Size = System::Drawing::Size(199, 30);
			this->textArea->TabIndex = 11;
			// 
			// button_train
			// 
			this->button_train->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->button_train->Location = System::Drawing::Point(8, 73);
			this->button_train->Name = L"button_train";
			this->button_train->Size = System::Drawing::Size(201, 43);
			this->button_train->TabIndex = 10;
			this->button_train->Text = L"TRAIN";
			this->button_train->UseVisualStyleBackColor = true;
			this->button_train->Click += gcnew System::EventHandler(this, &Form1::button_train_Click);
			// 
			// button_test
			// 
			this->button_test->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->button_test->Location = System::Drawing::Point(8, 126);
			this->button_test->Name = L"button_test";
			this->button_test->Size = System::Drawing::Size(201, 42);
			this->button_test->TabIndex = 9;
			this->button_test->Text = L"TEST";
			this->button_test->UseVisualStyleBackColor = true;
			this->button_test->Click += gcnew System::EventHandler(this, &Form1::button_test_Click);
			// 
			// VisionTracking
			// 
			this->VisionTracking->Controls->Add(this->releaseVideo);
			this->VisionTracking->Controls->Add(this->btn_CheckRecVideoDetails);
			this->VisionTracking->Controls->Add(this->btn_checkCapVideoDetails);
			this->VisionTracking->Controls->Add(this->btn_StartSyncVideo);
			this->VisionTracking->Location = System::Drawing::Point(4, 19);
			this->VisionTracking->Name = L"VisionTracking";
			this->VisionTracking->Padding = System::Windows::Forms::Padding(3);
			this->VisionTracking->Size = System::Drawing::Size(221, 276);
			this->VisionTracking->TabIndex = 0;
			this->VisionTracking->Text = L"VisionTracking";
			this->VisionTracking->UseVisualStyleBackColor = true;
			// 
			// btn_CheckRecVideoDetails
			// 
			this->btn_CheckRecVideoDetails->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->btn_CheckRecVideoDetails->Location = System::Drawing::Point(-1, 174);
			this->btn_CheckRecVideoDetails->Margin = System::Windows::Forms::Padding(2);
			this->btn_CheckRecVideoDetails->Name = L"btn_CheckRecVideoDetails";
			this->btn_CheckRecVideoDetails->Size = System::Drawing::Size(217, 31);
			this->btn_CheckRecVideoDetails->TabIndex = 14;
			this->btn_CheckRecVideoDetails->Text = L"Check Rec Details";
			this->btn_CheckRecVideoDetails->UseVisualStyleBackColor = true;
			this->btn_CheckRecVideoDetails->Click += gcnew System::EventHandler(this, &Form1::btn_checkRecVideoDetails_Click);
			// 
			// btn_checkCapVideoDetails
			// 
			this->btn_checkCapVideoDetails->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->btn_checkCapVideoDetails->Location = System::Drawing::Point(-1, 130);
			this->btn_checkCapVideoDetails->Margin = System::Windows::Forms::Padding(2);
			this->btn_checkCapVideoDetails->Name = L"btn_checkCapVideoDetails";
			this->btn_checkCapVideoDetails->Size = System::Drawing::Size(217, 31);
			this->btn_checkCapVideoDetails->TabIndex = 13;
			this->btn_checkCapVideoDetails->Text = L"Check Cap Details";
			this->btn_checkCapVideoDetails->UseVisualStyleBackColor = true;
			this->btn_checkCapVideoDetails->Click += gcnew System::EventHandler(this, &Form1::btn_checkCapVideoDetails_Click);
			// 
			// btn_StartSyncVideo
			// 
			this->btn_StartSyncVideo->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->btn_StartSyncVideo->Location = System::Drawing::Point(-1, 90);
			this->btn_StartSyncVideo->Margin = System::Windows::Forms::Padding(2);
			this->btn_StartSyncVideo->Name = L"btn_StartSyncVideo";
			this->btn_StartSyncVideo->Size = System::Drawing::Size(217, 31);
			this->btn_StartSyncVideo->TabIndex = 12;
			this->btn_StartSyncVideo->Text = L"Start Sync Video";
			this->btn_StartSyncVideo->UseVisualStyleBackColor = true;
			this->btn_StartSyncVideo->Click += gcnew System::EventHandler(this, &Form1::btn_StartSyncVideo_Click);
			// 
			// TabControlPanel
			// 
			this->TabControlPanel->AccessibleName = L"";
			this->TabControlPanel->Controls->Add(this->VisionTracking);
			this->TabControlPanel->Controls->Add(this->PatternRecognition);
			this->TabControlPanel->Location = System::Drawing::Point(672, 168);
			this->TabControlPanel->Name = L"TabControlPanel";
			this->TabControlPanel->SelectedIndex = 0;
			this->TabControlPanel->Size = System::Drawing::Size(229, 299);
			this->TabControlPanel->TabIndex = 17;
			this->TabControlPanel->Tag = L"";
			// 
			// releaseVideo
			// 
			this->releaseVideo->Font = (gcnew System::Drawing::Font(L"PMingLiU", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(136)));
			this->releaseVideo->Location = System::Drawing::Point(2, 221);
			this->releaseVideo->Margin = System::Windows::Forms::Padding(2);
			this->releaseVideo->Name = L"releaseVideo";
			this->releaseVideo->Size = System::Drawing::Size(217, 31);
			this->releaseVideo->TabIndex = 15;
			this->releaseVideo->Text = L"releaseVideo";
			this->releaseVideo->UseVisualStyleBackColor = true;
			this->releaseVideo->Click += gcnew System::EventHandler(this, &Form1::releaseVideo_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(5, 9);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(910, 475);
			this->Controls->Add(this->TabControlPanel);
			this->Controls->Add(this->controlMessagePresenter);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
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
			this->PatternRecognition->ResumeLayout(false);
			this->PatternRecognition->PerformLayout();
			this->VisionTracking->ResumeLayout(false);
			this->TabControlPanel->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
								
				 cvDestroyAllWindows();

			 }

	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
				if( mStartCount == true )
				{
					mpCounterForFPS->ResetPeriodController( 1.0 );
					mStartCount = false;
				}
				if( mpCounterForFPS->CheckPeriodController() == true )
				{
					mStartCount = true;
					mShowFPS( miFrameIndex );
					miFrameIndex = 0;
				}

				mProcessLoop();


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
					 
				 if(btnConnect->Text=="Connect")
				 {
					 if( IPAddress->Text->Length > 0 && port->Text->Length > 0 )
					 {
						 char ip[64];
						 mStringToChar( IPAddress->Text, ip );
/*
						 if( motionDetection->connect( ip, System::Int32::Parse(port->Text) ) == true )
						 {
							 btnConnect->Text="Disconnect";
							 btnConnect->ForeColor=System::Drawing::Color::Gray;
						 }
*/
					 }
				 }
				 else if(btnConnect->Text=="Disconnect")
				 {
					 btnConnect->Text="Connect";
					 btnConnect->ForeColor=System::Drawing::Color::Green;
///					 motionDetection->disConnect();
				 }
///				 showTrackingMessage( motionDetection->getNormalMessage(), motionDetection->getControlMessage() );
			 }

	private: System::Void btn_StartSyncVideo_Click(System::Object^  sender, System::EventArgs^  e) {
///				motionDetection->activateMotionDetection();
				take_A_image = 1;
				start_sync_video = true;
				std::string nmlString = "";
				std::string crlString = "start show sync video\n";
				showTrackingMessage(&nmlString, &crlString);
			 }

	private: System::Void button_test_Click(System::Object^  sender, System::EventArgs^  e) {
				 if( button_test->Text=="TEST" && button_train->Text=="TRAIN" )
				 {
					 button_test->Text="STOP";
					 mRecognizing=true;
					 this->button_train->Enabled = false;
				 }
				 else if( button_test->Text=="STOP" )
				 {
					 this->button_train->Enabled = true;
					 button_test->Text="TEST";
					 mRecognizing=false;
				 }
			 }
	 private: System::Void button_train_Click(System::Object^  sender, System::EventArgs^  e) {
				 if( button_test->Text=="TEST" && button_train->Text=="TRAIN" )
				 {
					 this->button_save->Enabled = true;
					 this->button_test->Enabled = false;
					 button_train->Text="STOP";
					 mTraining=true;
				 }
				 else if( button_train->Text=="STOP" )
				 {
					 this->button_test->Enabled = true;
					 button_train->Text="TRAIN";
					 mTraining=false;
				 }
			 }

	private: System::Void button_save_Click(System::Object^  sender, System::EventArgs^  e) {
				 if(button_test->Text=="TEST" && button_train->Text=="TRAIN" )
				 {
					 this->button_save->Enabled = true;
					 mSaving = true;
				 }
			 }
	private: System::Void btn_checkCapVideoDetails_Click(System::Object^  sender, System::EventArgs^  e) {
				std::string nmlString = "";
				//showTrackingMessage(&nmlString, mVideoSyncer->get_2capVideo_details());		
				showTrackingMessage(&nmlString, mVideoSyncer->get_2capVideo_details(txtOut));
			 }
	private: System::Void btn_checkRecVideoDetails_Click(System::Object^  sender, System::EventArgs^  e) {
				std::string nmlString = "";
				//showTrackingMessage(&nmlString, mVideoSyncer->get_2recVideo_details());		
				showTrackingMessage(&nmlString, mVideoSyncer->get_2recVideo_details(txtOut));
			 }
private: System::Void releaseVideo_Click(System::Object^  sender, System::EventArgs^  e) {
			 CvVideoWriter* vw_ptr1 = mVideoSyncer->get_camSet_videoWriter(1);
			 CvVideoWriter* vw_ptr2 = mVideoSyncer->get_camSet_videoWriter(2);
			 cvReleaseVideoWriter(&vw_ptr1);
			 cvReleaseVideoWriter(&vw_ptr2);
			 *txtOut << "videos released!\n";
		 }
};

}

