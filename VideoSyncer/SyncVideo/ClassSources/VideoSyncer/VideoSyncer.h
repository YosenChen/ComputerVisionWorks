#ifndef VIDEOSYNCER_H
#define VIDEOSYNCER_H

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>

#include <fstream>
#include <sstream> //for std::string operator "+" and "+="
using namespace std;

#define IMG_WIDTH 320
#define IMG_HEIGHT 240

class VideoSyncer
{
public:
	class VideoSet
	{
	public:
		int vC_frame_S, vC_frame_F, vC_frame_cnt;
		CvCapture *videoCapture;
		int vW_frame_S, vW_frame_F, vW_frame_cnt;
		CvVideoWriter *videoWriter;
		IplImage *cur_img_ptr;

		VideoSet():videoWriter(NULL),videoCapture(NULL), cur_img_ptr(NULL)
		{
			vC_frame_S = vC_frame_F = vC_frame_cnt = vW_frame_S = vW_frame_F = vW_frame_cnt = 0;
		}

		//since swallow copy of VideoSet is OK, so no need to specify cctor & assignment operator
		
		~VideoSet()
		{
			cvReleaseVideoWriter(&videoWriter);
			cvReleaseCapture(&videoCapture);
			cur_img_ptr = NULL;
		}
	};

private:
//public:
	VideoSet camSet1, camSet2;
	VideoSet *S_Vid, *L_Vid;
	bool is_cam1_longerThan_cam2;
	double L2S_ratio;
	int sync_frame_idx;
	const char *m_vCap_name1, *m_vCap_name2, *m_vW_name1, *m_vW_name2;
	std::string capVideo_details;
	std::string recVideo_details;
	int set_rec_fps;
	bool videoReachTheEnd;
	
	
public:
	VideoSyncer():is_cam1_longerThan_cam2(true),S_Vid(NULL),L_Vid(NULL),
		L2S_ratio(1.0),sync_frame_idx(0), 
		m_vCap_name1(NULL), m_vCap_name2(NULL), m_vW_name1(NULL), m_vW_name2(NULL), set_rec_fps(0),
		videoReachTheEnd(false)
	{
	}
	
	VideoSyncer(const char* vCap_name1, const char* vW_name1,
				const char* vCap_name2, const char* vW_name2, 
				int rec_fps);

	// this function can make object(by default ctor) initialize at another time  
	void initialize_VideoSyncer(const char* vCap_name1, const char* vW_name1,
								const char* vCap_name2, const char* vW_name2, 
								int rec_fps);

	//since swallow copy of VideoSyncer is OK, so no need to specify cctor & assignment operator

	~VideoSyncer();

	// initialize & return total frame number
	void initialize_LinearInterpolation(int vC_frame_S1, int vC_frame_F1, int vC_frame_S2, int vC_frame_F2, fstream* txtOut = NULL);
	
	// given (synchronized) frame index, return 2 cam image ptrs with format: [NULL cam1ptr cam2ptr] 
	void syncByShorterFrame_LinearInterp(fstream* txtOut = NULL);

	// given 2 images (ptr array: IplImage**), record into 2 sync-ed videos
	bool record2Videos(bool rec_en, fstream* txtOut = NULL);

	bool syncByShorterFrame_LinearInterp_thenRec(fstream* txtOut = NULL)
	{
		syncByShorterFrame_LinearInterp(txtOut);
		return record2Videos(!videoReachTheEnd, txtOut);
	}

	// function to return private member values
	int get_sync_frame_idx(void) const;
	int get_camSet_vC_frame_cnt(int cam_id) const;
	int get_camSet_vC_frame_F(int cam_id) const;
	int get_camSet_vC_frame_S(int cam_id) const;
	const IplImage* get_camSet_cur_img_ptr(int cam_id) const
	{
		return (cam_id == 1) ? camSet1.cur_img_ptr : (cam_id == 2) ? camSet2.cur_img_ptr : NULL;
	}
	CvVideoWriter* get_camSet_videoWriter(int cam_id) const
	{
		return (cam_id == 1) ? camSet1.videoWriter : (cam_id == 2) ? camSet2.videoWriter : NULL;
	}
	double get_L2S_ratio(void) const;
	

	// function (and its necessary setup) to check video total length
	int get_video_total_length(int cam_id);

	std::string* get_2capVideo_details(fstream* txtOut = NULL);
	std::string* get_2recVideo_details(fstream* txtOut = NULL);
};



#endif