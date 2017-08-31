#include "VideoSyncer.h"


// format transferring
template <class T>
inline std::string to_string(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}




VideoSyncer::VideoSyncer(const char* vCap_name1, const char* vW_name1,
				const char* vCap_name2, const char* vW_name2, 
				int rec_fps)
{
	initialize_VideoSyncer(vCap_name1, vW_name1, vCap_name2, vW_name2, rec_fps);
}

void VideoSyncer::initialize_VideoSyncer(const char* vCap_name1, const char* vW_name1,
				const char* vCap_name2, const char* vW_name2, 
				int rec_fps)
{
	camSet1.videoCapture = cvCaptureFromAVI(vCap_name1);
	camSet1.videoWriter = cvCreateVideoWriter(vW_name1,-1,rec_fps,cvSize(IMG_WIDTH, IMG_HEIGHT),1);
	camSet2.videoCapture = cvCaptureFromAVI(vCap_name2);
	camSet2.videoWriter = cvCreateVideoWriter(vW_name2,-1,rec_fps,cvSize(IMG_WIDTH, IMG_HEIGHT),1);
	S_Vid = L_Vid = NULL;
	is_cam1_longerThan_cam2 = true;
	L2S_ratio = 1.0;
	sync_frame_idx = 0;
	set_rec_fps = rec_fps;
	videoReachTheEnd = false;

	// for video length check & its setup & displaying cap video detailed info
	VideoSyncer::m_vCap_name1 = vCap_name1;
	VideoSyncer::m_vCap_name2 = vCap_name2;

	// for displaying rec video detailed info
	VideoSyncer::m_vW_name1 = vW_name1;
	VideoSyncer::m_vW_name2 = vW_name2;
}

VideoSyncer::~VideoSyncer()
{
	//camSet1 & camSet2 will be released in ~VideoSet()
	S_Vid = L_Vid = NULL;
}

void VideoSyncer::initialize_LinearInterpolation(int vC_frame_S1, int vC_frame_F1, int vC_frame_S2, int vC_frame_F2, fstream* txtOut)
{
	camSet1.vC_frame_cnt = camSet1.vC_frame_S = vC_frame_S1;
	camSet1.vC_frame_F = vC_frame_F1;
	camSet2.vC_frame_cnt = camSet2.vC_frame_S = vC_frame_S2;
	camSet2.vC_frame_F = vC_frame_F2;

	if((vC_frame_F1-vC_frame_S1) >= (vC_frame_F2-vC_frame_S2))
	{
		is_cam1_longerThan_cam2 = true;
		L_Vid = &camSet1;
		S_Vid = &camSet2;
		L2S_ratio = (double(vC_frame_F1-vC_frame_S1))/(double(vC_frame_F2-vC_frame_S2));
	}
	else
	{
		is_cam1_longerThan_cam2 = false;
		S_Vid = &camSet1;
		L_Vid = &camSet2;
		L2S_ratio = (double(vC_frame_F2-vC_frame_S2))/(double(vC_frame_F1-vC_frame_S1));
	}
}

// cannot jump & reverse, only incremental increase
void VideoSyncer::syncByShorterFrame_LinearInterp(fstream* txtOut)
{

	if (sync_frame_idx == 0) //first call of this function
	{
		sync_frame_idx++;// update to current frame idx
		for (int i=1; i<=L_Vid->vC_frame_S; i++)
		{
			// get new frame & deal with run-out case
			if (!(L_Vid->cur_img_ptr = cvQueryFrame(L_Vid->videoCapture))) return;
		}
		L_Vid->vC_frame_cnt = L_Vid->vC_frame_S;
		for (int i=1; i<=S_Vid->vC_frame_S; i++)
		{
			// get new frame & deal with run-out case
			if (!(S_Vid->cur_img_ptr = cvQueryFrame(S_Vid->videoCapture))) return;
		}
		S_Vid->vC_frame_cnt = S_Vid->vC_frame_S;

		// sync_frame_idx = 1, L_Vid->vC_frame_S, S_Vid->vC_frame_S correspond to the same sync time index
	}
	else if (sync_frame_idx >= (S_Vid->vC_frame_F - S_Vid->vC_frame_S + 1)) // haven't updated to current frame idx
	{
		videoReachTheEnd = true;
		return;
	}
	else
	{
		// update to current frame idx
		sync_frame_idx++;
		S_Vid->vC_frame_cnt++;

		// get new frame & deal with run-out case
		// update shorter video once
		if (!(S_Vid->cur_img_ptr = cvQueryFrame(S_Vid->videoCapture))) return;


		//sync_frame_idx-1 == S_Vid->vC_frame_cnt - S_Vid->vC_frame_S
		double linInterp_result = (double(sync_frame_idx-1))*L2S_ratio + (double)(L_Vid->vC_frame_S);
		int linInterp_L_Vid_idx = 
			(linInterp_result >= (int)linInterp_result + 0.5) ? (int)linInterp_result+1 : (int)linInterp_result;


		for (int i=L_Vid->vC_frame_cnt+1; i<=linInterp_L_Vid_idx; i++)
		{
			// get new frame & deal with run-out case
			if (!(L_Vid->cur_img_ptr = cvQueryFrame(L_Vid->videoCapture))) return;
		}
		L_Vid->vC_frame_cnt = linInterp_L_Vid_idx;
	}
	


	if (txtOut)
	{
		*txtOut << "Enter syncByShorterFrame_LinearInterp \n";
		*txtOut << "sync_frame_idx = " << sync_frame_idx << "\n";
		*txtOut << "S_Vid->vC_frame_S = " << S_Vid->vC_frame_S << "\n";
		*txtOut << "S_Vid->vC_frame_F = " << S_Vid->vC_frame_F << "\n";
		*txtOut << "S_Vid->vC_frame_cnt = " << S_Vid->vC_frame_cnt << "\n";
		*txtOut << "L_Vid->vC_frame_S = " << L_Vid->vC_frame_S << "\n";
		*txtOut << "L_Vid->vC_frame_F = " << L_Vid->vC_frame_F << "\n";
		*txtOut << "L_Vid->vC_frame_cnt = " << L_Vid->vC_frame_cnt << "\n";
	}
}

bool VideoSyncer::record2Videos(bool rec_en, fstream* txtOut)
{
	if (!rec_en) return false;

	if (( camSet1.cur_img_ptr == NULL) || ( camSet2.cur_img_ptr == NULL))
	{
		if (txtOut)
		{
			*txtOut << "Video(s) reach the end, stop recording...\n";
			*txtOut << "actual recorded length of video = " << sync_frame_idx << "\n";
		}
		return false;
	}

	*txtOut << "camSet1.videoWriter = " << (camSet1.videoWriter) << "\n";
	*txtOut << "camSet2.videoWriter = " << (camSet2.videoWriter) << "\n";

	cvWriteFrame(camSet1.videoWriter, camSet1.cur_img_ptr);
	cvWriteFrame(camSet2.videoWriter, camSet2.cur_img_ptr);
	*txtOut << "Videos are recording...\n";
	return true;
}


// function to return private member values
int VideoSyncer::get_sync_frame_idx(void) const
{
	return sync_frame_idx;
}
int VideoSyncer::get_camSet_vC_frame_cnt(int cam_id) const
{
	switch (cam_id)
	{
	case 1:
		return camSet1.vC_frame_cnt; 
		// no need to use "break"
	case 2:
		return camSet2.vC_frame_cnt;
		// no need to use "break"
	default:
		return -1;
		//error
	}
}
int VideoSyncer::get_camSet_vC_frame_F(int cam_id) const
{
	switch (cam_id)
	{
	case 1:
		return camSet1.vC_frame_F; 
		// no need to use "break"
	case 2:
		return camSet2.vC_frame_F;
		// no need to use "break"
	default:
		return -1;
		//error
	}
}
int VideoSyncer::get_camSet_vC_frame_S(int cam_id) const
{
	switch (cam_id)
	{
	case 1:
		return camSet1.vC_frame_S; 
		// no need to use "break"
	case 2:
		return camSet2.vC_frame_S;
		// no need to use "break"
	default:
		return -1;
		//error
	}
}
double VideoSyncer::get_L2S_ratio(void) const
{
	return L2S_ratio;
}

// function (and its necessary setup) to check video total length
int VideoSyncer::get_video_total_length(int cam_id)
{
	// we cannot repeat declaring variables in "case"s of "switch"  
	CvCapture *camCap = NULL;
	IplImage *frame = NULL;
	int frameCnt = 0;

	switch (cam_id)
	{
	case 1:
		camCap = cvCaptureFromAVI(m_vCap_name1); //take cam1
		while (frame = cvQueryFrame(camCap)){frameCnt++;}
		cvReleaseCapture(&camCap);
		return frameCnt;
		// no need to use "break"
	case 2:
		camCap = cvCaptureFromAVI(m_vCap_name2); //take cam2
		while (frame = cvQueryFrame(camCap)){frameCnt++;}
		cvReleaseCapture(&camCap);
		return frameCnt;
		// no need to use "break"		
	default:
		return -1;
	}
}

std::string* VideoSyncer::get_2capVideo_details(fstream* txtOut)
{
	capVideo_details = "\nshow captrued video details: \n";
	capVideo_details += "===================================== \n";
	
	capVideo_details += ("CAM1 cap name: " + to_string(m_vCap_name1) + "\n");
	capVideo_details += ("camSet1.vC_frame_S = " + to_string(get_camSet_vC_frame_S(1)) + "\n");
	capVideo_details += ("camSet1.vC_frame_F = " + to_string(get_camSet_vC_frame_F(1)) + "\n");
	capVideo_details += ("cam1_video_total_length = " + to_string(get_video_total_length(1)) + "\n");
	
	capVideo_details += ("CAM2 cap name: " + to_string(m_vCap_name2) + "\n");
	capVideo_details += ("camSet2.vC_frame_S = " + to_string(get_camSet_vC_frame_S(2)) + "\n");
	capVideo_details += ("camSet2.vC_frame_F = " + to_string(get_camSet_vC_frame_F(2)) + "\n");
	capVideo_details += ("cam2_video_total_length = " + to_string(get_video_total_length(2)) + "\n");

	capVideo_details += "-------------- \n";
	capVideo_details += ("L2S_ratio = " + to_string(get_L2S_ratio()) + "\n\n");

	if (txtOut) *txtOut << capVideo_details;
	return &capVideo_details; 
	// we cannot return a local variable's ptr!! (local variable memory will be freed after retrun)
	// so declare capVideo_details as a member variable (once the object VideoSyncer exists, so does it)
}

std::string* VideoSyncer::get_2recVideo_details(fstream* txtOut)
{
	recVideo_details = "\nshow recording video details: \n";
	recVideo_details += "===================================== \n";
	recVideo_details += ("CAM1 rec name: " + to_string(m_vW_name1) + "\n");
	recVideo_details += ("CAM2 rec name: " + to_string(m_vW_name2) + "\n");
	recVideo_details += ("est. total number of recording frame = " + to_string(S_Vid->vC_frame_F-S_Vid->vC_frame_S+1) + "\n");
	recVideo_details += ("recording frame per sec = " + to_string(set_rec_fps) + "\n\n");

	if (txtOut) *txtOut << recVideo_details;
	return &recVideo_details; 
	// we cannot return a local variable's ptr!! (local variable memory will be freed after retrun)
	// so declare recVideo_details as a member variable (once the object VideoSyncer exists, so does it)
}


