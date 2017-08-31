#include "Component.h"


//helper function
Pair makePair(int ii, int ij, Pair* inext, Pair* iprev)
{
	Pair result(ii, ij, inext, iprev);
	return result;
}
Position3D position(double x, double y, double z)
{
	Position3D result(x, y, z);
	return result;
}
Position3D unit_vector(Position3D vec)
{
	double mag = sqrt(pow(vec.x, 2)+pow(vec.y, 2)+pow(vec.z, 2));
	Position3D unitV;
	unitV.x = vec.x/mag;
	unitV.y = vec.y/mag;
	unitV.z = vec.z/mag;
	return unitV;
}
Position3D crossProduct(Position3D vec1, Position3D vec2)
{
	Position3D vector;
	vector.x = vec1.y*vec2.z-vec1.z*vec2.y;
	vector.y = vec1.z*vec2.x-vec1.x*vec2.z;
	vector.z = vec1.x*vec2.y-vec1.y*vec2.x;
	return vector;
}
double innerProduct(Position3D vec1, Position3D vec2)
{
	return vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z;
}
double returnDegAngle(Position3D vec1, Position3D vec2)
{
	double inner = innerProduct(vec1, vec2);
	double cos_val = inner/MAGNITUDE(vec1)/MAGNITUDE(vec2);
	return 180*acos(cos_val)/CV_PI;
}
Position3D inverseVector(Position3D vec)
{
	vec.x = -vec.x;
	vec.y = -vec.y;
	vec.z = -vec.z;
	return vec;
}


//Class Plane
Plane::Plane()
{
	IsInit = false;
}
Plane::Plane(Position3D a, Position3D vec)
{
	IsInit = initParamByPtVec(a, vec);
}
Plane::~Plane()
{
	IsInit = false;
}
Plane::Plane(const Plane & plane)
{
	IsInit = plane.IsInit;
	nor_dir_vec = plane.nor_dir_vec;
	Stdpoint = plane.Stdpoint;
}
Plane & Plane::operator=(const Plane & rhs)
{
	IsInit = rhs.IsInit;
	nor_dir_vec = rhs.nor_dir_vec;
	Stdpoint = rhs.Stdpoint;

	return *this;
}
//initialize
bool Plane::initParamByPtVec(Position3D a, Position3D vec)
{
	if (abs(vec.x) < SINGULAR_THD && abs(vec.y) < SINGULAR_THD && abs(vec.z) < SINGULAR_THD) return false;
	
	IsInit = true;
	Stdpoint = a;
	nor_dir_vec = unit_vector(vec);
	if (nor_dir_vec.x < 0) nor_dir_vec = inverseVector(nor_dir_vec);

	return true;
}
//functionality
//bool getPosFromXY(double x, double y, Position3D & pt)
//bool getPosFromYZ(double y, double z, Position3D & pt)
//bool getPosFromXZ(double x, double z, Position3D & pt)
bool Plane::isPtOnPlane(Position3D pt) const
{
	if (!IsInit) return false;
	Position3D vec;
	vec.x = pt.x - Stdpoint.x;
	vec.y = pt.y - Stdpoint.y;
	vec.z = pt.z - Stdpoint.z;
	return (abs(innerProduct(vec, nor_dir_vec)/MAGNITUDE(vec)) < SINGULAR_THD);
}
double Plane::getDisFromPt(Position3D pt) const
{
	if (!IsInit) return -10;
	if (isPtOnPlane(pt)) return 0;

	Position3D vec;
	vec.x = pt.x - Stdpoint.x;
	vec.y = pt.y - Stdpoint.y;
	vec.z = pt.z - Stdpoint.z;
	return abs(innerProduct(vec, nor_dir_vec));
}
Position3D Plane::getProjPtFromPt(Position3D pt, double & dist) const
{
	Position3D vec;
	vec.x = pt.x - Stdpoint.x;
	vec.y = pt.y - Stdpoint.y;
	vec.z = pt.z - Stdpoint.z;

	//get project vector along normal vector
	double t = innerProduct(vec, nor_dir_vec);
	Position3D norProjVec;
	norProjVec.x = nor_dir_vec.x*t;
	norProjVec.y = nor_dir_vec.y*t;
	norProjVec.z = nor_dir_vec.z*t;
	dist = abs(t);

	//vec - norProjVec = planeProjVec
	Position3D projPt;
	projPt.x = Stdpoint.x + (vec.x - norProjVec.x);
	projPt.y = Stdpoint.y + (vec.y - norProjVec.y);
	projPt.z = Stdpoint.z + (vec.z - norProjVec.z);
	if (!isPtOnPlane(projPt))
	{
		//problem here
	}
	return projPt;
}

//Class Line3D

Line3D::Line3D()
{
	IsInit = false;
}
Line3D::Line3D(Position3D a, Position3D b, bool dont_care)
{
	IsInit = initParamBy2Pt(a, b);
}
Line3D::Line3D(Position3D a, Position3D vec)
{
	IsInit = initParamByPtVec(a, vec);
}
Line3D::~Line3D()
{
	IsInit = false;
}

Line3D::Line3D(const Line3D & line)
{
	unit_dir_vec = line.unit_dir_vec;
	Stdpoint1 = line.Stdpoint1;
	Stdpoint2 = line.Stdpoint2;
	IsInit = line.IsInit;
}

Line3D & Line3D::operator=(const Line3D & rhs)
{
	//dtor is empty
	unit_dir_vec = rhs.unit_dir_vec;
	Stdpoint1 = rhs.Stdpoint1;
	Stdpoint2 = rhs.Stdpoint2;
	IsInit = rhs.IsInit;

	return *this;
}


//initialize
bool Line3D::initParamBy2Pt(Position3D a, Position3D b)
{
	if (abs(a.x-b.x) + abs(a.y-b.y) + abs(a.z-b.z) < IDENTICAL_THD) return false;

	IsInit = true;
	Stdpoint1 = a;
	Stdpoint2 = b;
	unit_dir_vec.x = a.x - b.x;
	unit_dir_vec.y = a.y - b.y;
	unit_dir_vec.z = a.z - b.z;
	unit_dir_vec = unit_vector(unit_dir_vec);
	if (unit_dir_vec.x < 0) unit_dir_vec = inverseVector(unit_dir_vec);
	return true;
}

bool Line3D::initParamByPtVec(Position3D a, Position3D vec)
{
	if (abs(vec.x) < SINGULAR_THD && abs(vec.y) < SINGULAR_THD && abs(vec.z) < SINGULAR_THD) return false;
	IsInit = true;
	Stdpoint1 = a;
	unit_dir_vec = unit_vector(vec);
	if (unit_dir_vec.x < 0) unit_dir_vec = inverseVector(unit_dir_vec);
	
	Stdpoint2.x = Stdpoint1.x + 50.0*unit_dir_vec.x;
	Stdpoint2.y = Stdpoint1.y + 50.0*unit_dir_vec.y;
	Stdpoint2.z = Stdpoint1.z + 50.0*unit_dir_vec.z;
	return true;
}


//functionality
bool Line3D::getPosFromX(double x, Position3D & pt) const
{
	if (abs(unit_dir_vec.x) < SINGULAR_THD) return false;
	if (!IsInit) return false;

	double t = (x - Stdpoint1.x)/unit_dir_vec.x;
	pt.x = x;
	pt.y = Stdpoint1.y + t*unit_dir_vec.y;
	pt.z = Stdpoint1.z + t*unit_dir_vec.z;

	return true;
}

bool Line3D::getPosFromY(double y, Position3D & pt) const
{
	if (abs(unit_dir_vec.y) < SINGULAR_THD) return false;
	if (!IsInit) return false;

	double t = (y - Stdpoint1.y)/unit_dir_vec.y;
	pt.x = Stdpoint1.x + t*unit_dir_vec.x;
	pt.y = y;
	pt.z = Stdpoint1.z + t*unit_dir_vec.z;

	return true;

}

bool Line3D::getPosFromZ(double z, Position3D & pt) const
{
	if (abs(unit_dir_vec.z) < SINGULAR_THD) return false;
	if (!IsInit) return false;

	double t = (z - Stdpoint1.z)/unit_dir_vec.z;
	pt.x = Stdpoint1.x + t*unit_dir_vec.x;
	pt.y = Stdpoint1.y + t*unit_dir_vec.y;
	pt.z = z;

	return true;
}
bool Line3D::isPtOnLine(Position3D pt) const
{
	if (!IsInit) return false;
	if (abs(unit_dir_vec.x) >= abs(unit_dir_vec.y) && abs(unit_dir_vec.x) >= abs(unit_dir_vec.z))
	{
		double t = (pt.x - Stdpoint1.x)/unit_dir_vec.x;
		return (abs(Stdpoint1.y + t*unit_dir_vec.y - pt.y) < LARGE_IDENTICAL_THD && abs(Stdpoint1.z + t*unit_dir_vec.z - pt.z) < LARGE_IDENTICAL_THD );
	}
	else if (abs(unit_dir_vec.y) >= abs(unit_dir_vec.x) && abs(unit_dir_vec.y) >= abs(unit_dir_vec.z))
	{
		double t = (pt.y - Stdpoint1.y)/unit_dir_vec.y;
		return (abs(Stdpoint1.x + t*unit_dir_vec.x - pt.x) < LARGE_IDENTICAL_THD && abs(Stdpoint1.z + t*unit_dir_vec.z - pt.z) < LARGE_IDENTICAL_THD );
	}
	else if (abs(unit_dir_vec.z) >= abs(unit_dir_vec.x) && abs(unit_dir_vec.z) >= abs(unit_dir_vec.y))
	{
		double t = (pt.z - Stdpoint1.z)/unit_dir_vec.z;
		return (abs(Stdpoint1.x + t*unit_dir_vec.x - pt.x) < LARGE_IDENTICAL_THD && abs(Stdpoint1.y + t*unit_dir_vec.y - pt.y) < LARGE_IDENTICAL_THD );
	}
	else 
	{
		//problem here
		return false;
	}

}

double Line3D::getDisFromPt(Position3D pt) const
{
	if (!IsInit) return -10;
	if (isPtOnLine(pt)) return 0;
	
	Position3D vec;
	vec.x = pt.x - Stdpoint1.x;
	vec.y = pt.y - Stdpoint1.y;
	vec.z = pt.z - Stdpoint1.z;

	return MAGNITUDE(crossProduct(vec, unit_dir_vec))/MAGNITUDE(unit_dir_vec);
}

Position3D Line3D::getProjPtFromPt(Position3D outerPt, double & dist) const
{
	Position3D vec;
	vec.x = Stdpoint1.x - outerPt.x;
	vec.y = Stdpoint1.y - outerPt.y;
	vec.z = Stdpoint1.z - outerPt.z;

	//project vector
	double coeff = innerProduct(vec, unit_dir_vec);
	Position3D projVec;
	projVec.x = coeff*unit_dir_vec.x;
	projVec.y = coeff*unit_dir_vec.y;
	projVec.z = coeff*unit_dir_vec.z;

	//vec - projVec = orthogonal vector
	Position3D orthVec;
	orthVec.x = vec.x - projVec.x;
	orthVec.y = vec.y - projVec.y;
	orthVec.z = vec.z - projVec.z;
	dist = MAGNITUDE(orthVec);

	//orthVec + outerPt = project point on line
	Position3D projPt;
	projPt.x = orthVec.x + outerPt.x;
	projPt.y = orthVec.y + outerPt.y;
	projPt.z = orthVec.z + outerPt.z;

	if (!isPtOnLine(projPt)) 
	{
		//problem here
	}

	return projPt;
}
bool Line3D::getLinePtOnPlane(Plane plane, Position3D & intersect) const
{
	if (abs(innerProduct(plane.nor_dir_vec, unit_dir_vec)) < SINGULAR_THD) return false;

	double dist;
	Position3D planeProjPt = plane.getProjPtFromPt(Stdpoint1, dist);
	Position3D nor_vec;
	nor_vec.x = planeProjPt.x - Stdpoint1.x;
	nor_vec.y = planeProjPt.y - Stdpoint1.y;
	nor_vec.z = planeProjPt.z - Stdpoint1.z;
	double cos_val = innerProduct(nor_vec, unit_dir_vec)/dist;
	double signed_length = dist/cos_val;
	intersect.x = Stdpoint1.x + signed_length*unit_dir_vec.x;
	intersect.y = Stdpoint1.y + signed_length*unit_dir_vec.y;
	intersect.z = Stdpoint1.z + signed_length*unit_dir_vec.z;
	if (!plane.isPtOnPlane(intersect))
	{
		//problem here
	}

	return true;
}
Position3D Line3D::getMinDisPtFromLine(bool & valid, const Line3D line, Position3D & proj_pt) const
{
	//self line1, input line2
	Position3D com_nor_vec = crossProduct(line.unit_dir_vec, unit_dir_vec);
	
	Position3D plane1_nor_vec = crossProduct(com_nor_vec, unit_dir_vec);
	Plane plane1(Stdpoint1, plane1_nor_vec);
	
	//get project point on line2
	valid = line.getLinePtOnPlane(plane1, proj_pt);

	Position3D plane2_nor_vec = crossProduct(com_nor_vec, line.unit_dir_vec);
	Plane plane2(line.Stdpoint1, plane2_nor_vec);

	//gey project point on line1
	Position3D line1ProjPt;
	valid &= getLinePtOnPlane(plane2, line1ProjPt);

	return line1ProjPt;
	
}


//Class CameraLine3D
CameraLine3D::CameraLine3D()
{
	IsInit = false;
	camID = NONDEFINED;
}
CameraLine3D::~CameraLine3D()
{
	IsInit = false;
	camID = NONDEFINED;
}
CameraLine3D::CameraLine3D(const CameraLine3D & camline)
{
	//principle components
	unit_dir_vec = camline.unit_dir_vec;
	Stdpoint1 = camline.Stdpoint1;
	Stdpoint2 = camline.Stdpoint2;
	IsInit = camline.IsInit;


	optic_center = camline.optic_center;
	ref_plane_center = camline.ref_plane_center;
	ref_plane_dist = camline.ref_plane_dist;
	unit_orth_up_vec = camline.unit_orth_up_vec;
	unit_orth_right_vec = camline.unit_orth_right_vec;
	cm2pixelRatio = camline.cm2pixelRatio;
	h2wRatio = camline.h2wRatio;
	refPlane = camline.refPlane;
	imageLoc = camline.imageLoc;
	central_dir_vec = camline.central_dir_vec;
	camID = camline.camID;
}
CameraLine3D & CameraLine3D::operator=(const CameraLine3D & rhs)
{
	//principle components
	unit_dir_vec = rhs.unit_dir_vec;
	Stdpoint1 = rhs.Stdpoint1;
	Stdpoint2 = rhs.Stdpoint2;
	IsInit = rhs.IsInit;

	optic_center = rhs.optic_center;
	ref_plane_center = rhs.ref_plane_center;
	ref_plane_dist = rhs.ref_plane_dist;
	unit_orth_up_vec = rhs.unit_orth_up_vec;
	unit_orth_right_vec = rhs.unit_orth_right_vec;
	cm2pixelRatio = rhs.cm2pixelRatio;
	h2wRatio = rhs.h2wRatio;
	refPlane = rhs.refPlane;
	imageLoc = rhs.imageLoc;
	central_dir_vec = rhs.central_dir_vec;
	camID = rhs.camID;

	return *this;
}

//initialize
void CameraLine3D::initParamByImageLoc(CvPoint img_loc, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio)
{
	Position3D dir_vec;
	dir_vec.x = refPlaneCtr.x - op_ctr.x;
	dir_vec.y = refPlaneCtr.y - op_ctr.y;
	dir_vec.z = refPlaneCtr.z - op_ctr.z;

	
	central_dir_vec = unit_vector(dir_vec);
	if (central_dir_vec.x < 0) central_dir_vec = inverseVector(central_dir_vec);

	IsInit = true;

	optic_center = op_ctr;
	ref_plane_center = refPlaneCtr;
	ref_plane_dist = MAGNITUDE(dir_vec);
	Position3D upVec;
	upVec.x = 0;
	upVec.y = 0;
	upVec.z = 1;
	unit_orth_right_vec = unit_vector(crossProduct(central_dir_vec, upVec));
	if (unit_orth_right_vec.x < 0) unit_orth_right_vec = inverseVector(unit_orth_right_vec);

	unit_orth_up_vec = unit_vector(crossProduct(unit_orth_right_vec, central_dir_vec));
	if (unit_orth_up_vec.z < 0) unit_orth_up_vec = inverseVector(unit_orth_up_vec);

	cm2pixelRatio = c2p_ratio;
	
	/*int*/double dx = img_loc.x - IMG_WIDTH/2;
	/*int*/double dy = img_loc.y - IMG_HEIGHT/2;
	if (abs(dy) < IMG_HEIGHT/2) h2wRatio = CAM1_H2W_RATIO1*CAM1_H2W_RATIO2;
	else h2wRatio = CAM1_H2W_RATIO1;
	dx = -dx;
	dy = -dy/h2wRatio;

	Stdpoint2.x = ref_plane_center.x + dx*cm2pixelRatio*unit_orth_right_vec.x;
	Stdpoint2.y = ref_plane_center.y + dx*cm2pixelRatio*unit_orth_right_vec.y;
	Stdpoint2.z = ref_plane_center.z + dx*cm2pixelRatio*unit_orth_right_vec.z;
	Stdpoint2.x = Stdpoint2.x + dy*cm2pixelRatio*unit_orth_up_vec.x;
	Stdpoint2.y = Stdpoint2.y + dy*cm2pixelRatio*unit_orth_up_vec.y;
	Stdpoint2.z = Stdpoint2.z + dy*cm2pixelRatio*unit_orth_up_vec.z;


	initParamBy2Pt(op_ctr, Stdpoint2);

	imageLoc = img_loc;

	refPlane.initParamByPtVec(refPlaneCtr, dir_vec);
	
}
void CameraLine3D::initParamBy3DPt(Position3D pt, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio)
{
	initParamBy2Pt(op_ctr, pt);

	Position3D dir_vec;
	dir_vec.x = refPlaneCtr.x - op_ctr.x;
	dir_vec.y = refPlaneCtr.y - op_ctr.y;
	dir_vec.z = refPlaneCtr.z - op_ctr.z;

	
	central_dir_vec = unit_vector(dir_vec);
	if (central_dir_vec.x < 0) central_dir_vec = inverseVector(central_dir_vec);

	IsInit = true;

	optic_center = op_ctr;
	ref_plane_center = refPlaneCtr;
	ref_plane_dist = MAGNITUDE(dir_vec);
	Position3D upVec;
	upVec.x = 0;
	upVec.y = 0;
	upVec.z = 1;
	unit_orth_right_vec = unit_vector(crossProduct(central_dir_vec, upVec));
	if (unit_orth_right_vec.x < 0) unit_orth_right_vec = inverseVector(unit_orth_right_vec);

	unit_orth_up_vec = unit_vector(crossProduct(unit_orth_right_vec, central_dir_vec));
	if (unit_orth_up_vec.z < 0) unit_orth_up_vec = inverseVector(unit_orth_up_vec);

	cm2pixelRatio = c2p_ratio;

	refPlane.initParamByPtVec(refPlaneCtr, dir_vec);

	Position3D ref_plane_vec, ref_plane_pt;
	getLinePtOnPlane(refPlane, ref_plane_pt);

	ref_plane_vec.x = ref_plane_pt.x - refPlaneCtr.x;
	ref_plane_vec.y = ref_plane_pt.y - refPlaneCtr.y;
	ref_plane_vec.z = ref_plane_pt.z - refPlaneCtr.z;
	/*int*/double dx = innerProduct(ref_plane_vec, unit_orth_right_vec)/cm2pixelRatio;
	/*int*/double dy = innerProduct(ref_plane_vec, unit_orth_up_vec)/cm2pixelRatio*CAM1_H2W_RATIO1;
	h2wRatio = CAM1_H2W_RATIO1;

	if (abs(dy)*CAM1_H2W_RATIO2 < IMG_HEIGHT/2)
	{
		dy = dy*CAM1_H2W_RATIO2;
		h2wRatio = h2wRatio*CAM1_H2W_RATIO2;
	}

	imageLoc.x = IMG_WIDTH/2 - dx;
	imageLoc.y = IMG_HEIGHT/2 - dy;

}
void CameraLine3D::setByImgLoc(CvPoint img_loc)
{
	/*int*/double dx = img_loc.x - IMG_WIDTH/2;
	/*int*/double dy = img_loc.y - IMG_HEIGHT/2;

	if (abs(dy) < IMG_HEIGHT/2) h2wRatio = CAM1_H2W_RATIO1*CAM1_H2W_RATIO2;
	else h2wRatio = CAM1_H2W_RATIO1;
	
	dx = -dx;
	dy = -dy/h2wRatio;

	Stdpoint2.x = ref_plane_center.x + dx*cm2pixelRatio*unit_orth_right_vec.x;
	Stdpoint2.y = ref_plane_center.y + dx*cm2pixelRatio*unit_orth_right_vec.y;
	Stdpoint2.z = ref_plane_center.z + dx*cm2pixelRatio*unit_orth_right_vec.z;
	Stdpoint2.x = Stdpoint2.x + dy*cm2pixelRatio*unit_orth_up_vec.x;
	Stdpoint2.y = Stdpoint2.y + dy*cm2pixelRatio*unit_orth_up_vec.y;
	Stdpoint2.z = Stdpoint2.z + dy*cm2pixelRatio*unit_orth_up_vec.z;


	initParamBy2Pt(optic_center, Stdpoint2);

	imageLoc = img_loc;

}
void CameraLine3D::setBy3DPt(Position3D pt)
{
	initParamBy2Pt(optic_center, pt);
	Position3D ref_plane_pt;
	getLinePtOnPlane(refPlane, ref_plane_pt); 

	Position3D ref_plane_vec;
	ref_plane_vec.x = ref_plane_pt.x - ref_plane_center.x;
	ref_plane_vec.y = ref_plane_pt.y - ref_plane_center.y;
	ref_plane_vec.z = ref_plane_pt.z - ref_plane_center.z;
	/*int*/double dx = innerProduct(ref_plane_vec, unit_orth_right_vec)/cm2pixelRatio;
	/*int*/double dy = innerProduct(ref_plane_vec, unit_orth_up_vec)/cm2pixelRatio*CAM1_H2W_RATIO1;
	h2wRatio = CAM1_H2W_RATIO1;

	if (abs(dy)*CAM1_H2W_RATIO2 < IMG_HEIGHT/2)
	{
		dy = dy*CAM1_H2W_RATIO2;
		h2wRatio = h2wRatio*CAM1_H2W_RATIO2;
	}
	imageLoc.x = IMG_WIDTH/2 - dx;
	imageLoc.y = IMG_HEIGHT/2 - dy;
}


//precise calibrated version:
//after cvUndistort2 with first round intrinsic parameter (CAM1_FX,...., CAM2_FX,....)
//now assume that the image plane is not distorted by distortion coeffs 
//but we still need to deal with CX, CY for second round
void CameraLine3D::initParamByImageLoc_precise(CvPoint img_loc, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio, int cam_id)
{
	camID = cam_id;
	Position3D dir_vec;
	dir_vec.x = refPlaneCtr.x - op_ctr.x;
	dir_vec.y = refPlaneCtr.y - op_ctr.y;
	dir_vec.z = refPlaneCtr.z - op_ctr.z;

	
	central_dir_vec = unit_vector(dir_vec);
	if (central_dir_vec.x < 0) central_dir_vec = inverseVector(central_dir_vec);

	IsInit = true;

	optic_center = op_ctr;
	ref_plane_center = refPlaneCtr;
	ref_plane_dist = MAGNITUDE(dir_vec);
	Position3D upVec;
	upVec.x = 0;
	upVec.y = 0;
	upVec.z = 1;
	unit_orth_right_vec = unit_vector(crossProduct(central_dir_vec, upVec));
	if (unit_orth_right_vec.x < 0) unit_orth_right_vec = inverseVector(unit_orth_right_vec);

	unit_orth_up_vec = unit_vector(crossProduct(unit_orth_right_vec, central_dir_vec));
	if (unit_orth_up_vec.z < 0) unit_orth_up_vec = inverseVector(unit_orth_up_vec);

	cm2pixelRatio = c2p_ratio;
	
	double dx, dy;
	if (cam_id == CAM1)
	{
		dx = img_loc.x - CAM1_2CX;//IMG_WIDTH/2;
		dy = img_loc.y - CAM1_2CY;//IMG_HEIGHT/2;
		h2wRatio = CAM1_H2W_RATIO_AFTER_CALIB;//CAM1_2FY/CAM1_2FX;
	}
	else //cam_id == CAM2
	{
		dx = img_loc.x - CAM2_2CX;//IMG_WIDTH/2;
		dy = img_loc.y - CAM2_2CY;//IMG_HEIGHT/2;
		h2wRatio = CAM2_H2W_RATIO_AFTER_CALIB;//CAM2_2FY/CAM2_2FX;
	}

	dx = -dx;
	dy = -dy/h2wRatio;

	Stdpoint2.x = ref_plane_center.x + dx*cm2pixelRatio*unit_orth_right_vec.x;
	Stdpoint2.y = ref_plane_center.y + dx*cm2pixelRatio*unit_orth_right_vec.y;
	Stdpoint2.z = ref_plane_center.z + dx*cm2pixelRatio*unit_orth_right_vec.z;
	Stdpoint2.x = Stdpoint2.x + dy*cm2pixelRatio*unit_orth_up_vec.x;
	Stdpoint2.y = Stdpoint2.y + dy*cm2pixelRatio*unit_orth_up_vec.y;
	Stdpoint2.z = Stdpoint2.z + dy*cm2pixelRatio*unit_orth_up_vec.z;


	initParamBy2Pt(op_ctr, Stdpoint2);

	imageLoc = img_loc;

	refPlane.initParamByPtVec(refPlaneCtr, dir_vec);
	
}
void CameraLine3D::initParamBy3DPt_precise(Position3D pt, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio, int cam_id)
{
	camID = cam_id;
	initParamBy2Pt(op_ctr, pt);

	Position3D dir_vec;
	dir_vec.x = refPlaneCtr.x - op_ctr.x;
	dir_vec.y = refPlaneCtr.y - op_ctr.y;
	dir_vec.z = refPlaneCtr.z - op_ctr.z;

	
	central_dir_vec = unit_vector(dir_vec);
	if (central_dir_vec.x < 0) central_dir_vec = inverseVector(central_dir_vec);

	IsInit = true;

	optic_center = op_ctr;
	ref_plane_center = refPlaneCtr;
	ref_plane_dist = MAGNITUDE(dir_vec);
	Position3D upVec;
	upVec.x = 0;
	upVec.y = 0;
	upVec.z = 1;
	unit_orth_right_vec = unit_vector(crossProduct(central_dir_vec, upVec));
	if (unit_orth_right_vec.x < 0) unit_orth_right_vec = inverseVector(unit_orth_right_vec);

	unit_orth_up_vec = unit_vector(crossProduct(unit_orth_right_vec, central_dir_vec));
	if (unit_orth_up_vec.z < 0) unit_orth_up_vec = inverseVector(unit_orth_up_vec);

	cm2pixelRatio = c2p_ratio;

	refPlane.initParamByPtVec(refPlaneCtr, dir_vec);

	Position3D ref_plane_vec, ref_plane_pt;
	getLinePtOnPlane(refPlane, ref_plane_pt);

	ref_plane_vec.x = ref_plane_pt.x - refPlaneCtr.x;
	ref_plane_vec.y = ref_plane_pt.y - refPlaneCtr.y;
	ref_plane_vec.z = ref_plane_pt.z - refPlaneCtr.z;

	h2wRatio = ((cam_id == CAM1) ? CAM1_H2W_RATIO_AFTER_CALIB/*CAM1_2FY/CAM1_2FX*/ : CAM2_H2W_RATIO_AFTER_CALIB/*CAM2_2FY/CAM2_2FX*/);

	double dx = innerProduct(ref_plane_vec, unit_orth_right_vec)/cm2pixelRatio;
	double dy = innerProduct(ref_plane_vec, unit_orth_up_vec)/cm2pixelRatio*h2wRatio;

	imageLoc.x = 0.0 - dx + ((cam_id == CAM1) ? CAM1_2CX : CAM2_2CX);
	imageLoc.y = 0.0 - dy + ((cam_id == CAM1) ? CAM1_2CY : CAM2_2CY);

}
void CameraLine3D::setByImgLoc_precise(CvPoint img_loc)
{
	double dx, dy;
	if (camID == CAM1)
	{
		dx = img_loc.x - CAM1_2CX;//IMG_WIDTH/2;
		dy = img_loc.y - CAM1_2CY;//IMG_HEIGHT/2;
		h2wRatio = CAM1_H2W_RATIO_AFTER_CALIB;//CAM1_2FY/CAM1_2FX;
	}
	else //cam_id == CAM2
	{
		dx = img_loc.x - CAM2_2CX;//IMG_WIDTH/2;
		dy = img_loc.y - CAM2_2CY;//IMG_HEIGHT/2;
		h2wRatio = CAM2_H2W_RATIO_AFTER_CALIB;//CAM2_2FY/CAM2_2FX;
	}

	dx = -dx;
	dy = -dy/h2wRatio;

	Stdpoint2.x = ref_plane_center.x + dx*cm2pixelRatio*unit_orth_right_vec.x;
	Stdpoint2.y = ref_plane_center.y + dx*cm2pixelRatio*unit_orth_right_vec.y;
	Stdpoint2.z = ref_plane_center.z + dx*cm2pixelRatio*unit_orth_right_vec.z;
	Stdpoint2.x = Stdpoint2.x + dy*cm2pixelRatio*unit_orth_up_vec.x;
	Stdpoint2.y = Stdpoint2.y + dy*cm2pixelRatio*unit_orth_up_vec.y;
	Stdpoint2.z = Stdpoint2.z + dy*cm2pixelRatio*unit_orth_up_vec.z;


	initParamBy2Pt(optic_center, Stdpoint2);

	imageLoc = img_loc;

}
void CameraLine3D::setBy3DPt_precise(Position3D pt)
{
	initParamBy2Pt(optic_center, pt);
	Position3D ref_plane_pt;
	getLinePtOnPlane(refPlane, ref_plane_pt); 

	Position3D ref_plane_vec;
	ref_plane_vec.x = ref_plane_pt.x - ref_plane_center.x;
	ref_plane_vec.y = ref_plane_pt.y - ref_plane_center.y;
	ref_plane_vec.z = ref_plane_pt.z - ref_plane_center.z;

	h2wRatio = ((camID == CAM1) ? CAM1_H2W_RATIO_AFTER_CALIB/*CAM1_2FY/CAM1_2FX*/ : CAM2_H2W_RATIO_AFTER_CALIB/*CAM2_2FY/CAM2_2FX*/);

	double dx = innerProduct(ref_plane_vec, unit_orth_right_vec)/cm2pixelRatio;
	double dy = innerProduct(ref_plane_vec, unit_orth_up_vec)/cm2pixelRatio*h2wRatio;

	imageLoc.x = 0.0 - dx + ((camID == CAM1) ? CAM1_2CX : CAM2_2CX);
	imageLoc.y = 0.0 - dy + ((camID == CAM1) ? CAM1_2CY : CAM2_2CY);

}




//Class BodyInfo
BodyInfo::BodyInfo()
{
	int hdims = Cr_BINS;//16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	face = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		body[k] = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges,1);
		bodyH[k] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	}

	delete [] Cr_ranges;
	delete [] Cb_ranges;

	IsHeightKnown = false;
}
BodyInfo::~BodyInfo()
{
	IsHeightKnown = false;
	cvReleaseHist(&face);
	for (int k=0; k<NUM_OF_BLOCKS; k++)
		cvReleaseHist(&body[k]);
}
BodyInfo::BodyInfo(const BodyInfo & bi)
{
	int hdims = Cr_BINS;//16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	face = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		body[k] = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges,1);
		bodyH[k] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	}

	delete [] Cr_ranges;
	delete [] Cb_ranges;


	height = bi.height;
	cvCopyHist(bi.face, &face);
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		cvCopyHist(bi.body[k], &body[k]);
		cvCopyHist(bi.bodyH[k], &bodyH[k]);
		Blocks[k] = bi.Blocks[k];
		IsBodySeen[k] = bi.IsBodySeen[k];
	}
	IsHeightKnown = bi.IsHeightKnown;

}
BodyInfo & BodyInfo::operator=(const BodyInfo & rhs)
{
	height = rhs.height;
	cvCopyHist(rhs.face, &face);
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		cvCopyHist(rhs.body[k], &body[k]);
		cvCopyHist(rhs.bodyH[k], &bodyH[k]);
		Blocks[k] = rhs.Blocks[k];
		IsBodySeen[k] = rhs.IsBodySeen[k];
	}
	IsHeightKnown = rhs.IsHeightKnown;

	return *this;
}
//set block locatons from image plane
bool BodyInfo::LocateBlocksFromImage(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h )
{
	groundline = *headline;

	if (h_is_got)
	{
		height = h;
		IsHeightKnown = true;
	}
	else
	{
		height = DEFAULT_HEIGHT;
		IsHeightKnown = false;
	}
	
	Position3D headCtr;
	headline->getPosFromZ(height, headCtr);
	
	//use ground point to set groundline
	Position3D groundPt(headCtr.x, headCtr.y, 0);
	groundline.setBy3DPt_precise(groundPt);

	double img_h = abs(headline->imageLoc.y - groundline.imageLoc.y);
	CvPoint bodyPt[NUM_OF_BLOCKS];
	double block_height = img_h/(NUM_OF_BLOCKS + 1);
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		bodyPt[k] = cvPoint(headline->imageLoc.x, groundline.imageLoc.y - block_height*((double)(NUM_OF_BLOCKS -1 - k)+1.0));
		if (bodyPt[k].x >= 0 && bodyPt[k].x < IMG_WIDTH && bodyPt[k].y >= 0 && bodyPt[k].y < IMG_HEIGHT)
		{
			Blocks[k] = cvRect(bodyPt[k].x - BLOCK_WIDTH*0.5, bodyPt[k].y - block_height*0.5, BLOCK_WIDTH, block_height);
			IsBodySeen[k] = true;
		}
		else
			IsBodySeen[k] = false;
	}

	return IsHeightKnown;

}
//set block locatons from 3D space
bool BodyInfo::LocateBlocksFrom3D(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h )
{
	groundline = *headline;

	if (h_is_got)
	{
		height = h;
		IsHeightKnown = true;
	}
	else
	{
		height = DEFAULT_HEIGHT;
		IsHeightKnown = false;
	}
	
	Position3D headCtr;
	headline->getPosFromZ(height, headCtr);
	
	//use ground point to set groundline
	Position3D groundPt(headCtr.x, headCtr.y, 0);
	groundline.setBy3DPt_precise(groundPt);
	double img_h = abs(headline->imageLoc.y - groundline.imageLoc.y);
	double blockImageHeight = img_h/(NUM_OF_BLOCKS + 1);
	CvPoint bodyPt[NUM_OF_BLOCKS];
	double block_height = height/(NUM_OF_BLOCKS + 1);
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		groundline.setBy3DPt_precise(position(headCtr.x, headCtr.y, block_height*(double)((NUM_OF_BLOCKS -1 - k)+1)));
		bodyPt[k] = groundline.imageLoc;
		if (bodyPt[k].x >= 0 && bodyPt[k].x < IMG_WIDTH && bodyPt[k].y >= 0 && bodyPt[k].y < IMG_HEIGHT)
		{
			Blocks[k] = cvRect(bodyPt[k].x - BLOCK_WIDTH*0.5, bodyPt[k].y - blockImageHeight*0.5, BLOCK_WIDTH, blockImageHeight);
			IsBodySeen[k] = true;
		}
		else
			IsBodySeen[k] = false;
	}
	groundline.setBy3DPt_precise(groundPt);
	return IsHeightKnown;

}

bool BodyInfo::LocateAndMeasureInfoH(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h, IplImage* src_Cr, IplImage* src_Cb, IplImage* msk)
{
	bool result = LocateBlocksFrom3D(headline, groundline, h_is_got, h);
	float max_val;
	IplImage * sample_planes[] = {src_Cr, src_Cb};
	cvZero(msk);
	cvRectangle(msk, cvPoint(headline->imageLoc.x - 0.5*FACE_WIDTH, headline->imageLoc.y - 0.5*FACE_HEIGHT), cvPoint(headline->imageLoc.x + 0.5*FACE_WIDTH, headline->imageLoc.y + 0.5*FACE_HEIGHT), cvScalar(255), -1);
	cvCalcHist(sample_planes, face, 0, msk);
	max_val = 0;
	cvGetMinMaxHistValue( face, 0, &max_val, 0, 0 );
	cvConvertScale( face->bins, face->bins, max_val ? 255. / max_val : 0., 0 );
	
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		if (IsBodySeen[k])
		{
			cvZero(msk);
			cvRectangle(msk, cvPoint(Blocks[k].x + 0.2*Blocks[k].width, Blocks[k].y + 0.2*Blocks[k].height), cvPoint(Blocks[k].x+0.8*Blocks[k].width, Blocks[k].y+0.8*Blocks[k].height), cvScalar(255), -1);
			cvCalcHist(sample_planes, body[k], 0, msk);
			cvCalcHist(&src_Cr, bodyH[k], 0, msk);
			max_val = 0;
			cvGetMinMaxHistValue( body[k], 0, &max_val, 0, 0 );
			cvConvertScale( body[k]->bins, body[k]->bins, max_val ? 255. / max_val : 0., 0 );
			max_val = 0;
			cvGetMinMaxHistValue( bodyH[k], 0, &max_val, 0, 0 );
			cvConvertScale( bodyH[k]->bins, bodyH[k]->bins, max_val ? 255. / max_val : 0., 0 );
			//cvNormalizeHist(body[k], 1);
		}
	}

	return result;
}
bool BodyInfo::LocateAndMeasureInfo(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h, IplImage* src_Cr, IplImage* src_Cb, IplImage* msk)
{
	bool result = LocateBlocksFrom3D(headline, groundline, h_is_got, h);
	float max_val;
	IplImage * sample_planes[] = {src_Cr, src_Cb};
	cvZero(msk);
	cvRectangle(msk, cvPoint(headline->imageLoc.x - 0.5*FACE_WIDTH, headline->imageLoc.y - 0.5*FACE_HEIGHT), cvPoint(headline->imageLoc.x + 0.5*FACE_WIDTH, headline->imageLoc.y + 0.5*FACE_HEIGHT), cvScalar(255), -1);
	cvCalcHist(sample_planes, face, 0, msk);
	max_val = 0;
	cvGetMinMaxHistValue( face, 0, &max_val, 0, 0 );
	cvConvertScale( face->bins, face->bins, max_val ? 255. / max_val : 0., 0 );
	
	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		if (IsBodySeen[k])
		{
			cvZero(msk);
			cvRectangle(msk, cvPoint(Blocks[k].x, Blocks[k].y), cvPoint(Blocks[k].x+Blocks[k].width, Blocks[k].y+Blocks[k].height), cvScalar(255), -1);
			cvCalcHist(sample_planes, body[k], 0, msk);
			max_val = 0;
			cvGetMinMaxHistValue( body[k], 0, &max_val, 0, 0 );
			cvConvertScale( body[k]->bins, body[k]->bins, max_val ? 255. / max_val : 0., 0 );
			//cvNormalizeHist(body[k], 1);
		}
	}

	return result;
}
////update
//void BodyInfo::UpdateFaceHist(CvHistogram *fh)
//void BodyInfo::UpdateBodyHist(CvHistogram *bh, int i)
//void BodyInfo::UpdateHeight(double h)
//void BodyInfo::UpdateAndShow(CameraLine3D * headline, IplImage * Cr, IplImage * Cb, IplImage* src, bool IsInGroup)


//Geometry match function
bool CheckLineIntersect(const CameraLine3D* line1, const CameraLine3D* line2, Position3D & intersect, Position3D & line1pt, Position3D & line2pt)
{
	bool valid;
	line1pt = line1->getMinDisPtFromLine(valid, *line2, line2pt);
	intersect = line1pt + line2pt;
	intersect = position(intersect.x/2, intersect.y/2, intersect.z/2);

	double dist = MAGNITUDE((line1pt-line2pt));
	if (dist > LARGE_IDENTICAL_THD) valid = (valid && false);// false
	else valid = (valid && true); //unchanged

	return valid;
}


//Appearance comparison function
double AppearanceComparison(const BodyInfo* ob1, const BodyInfo* ob2, int & matchCnt, int & level, double & best_color_score, int & best_block_index)
{
	double total_score = 0;
	matchCnt = 0;
	best_color_score = 10000;
	best_block_index = -1;


	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		if (ob1->IsBodySeen[k] && ob2->IsBodySeen[k])
		{
			cvNormalizeHist(ob1->body[k], 1);
			cvNormalizeHist(ob2->body[k], 1);
			double dist = cvCompareHist(ob1->body[k], ob2->body[k], CV_COMP_BHATTACHARYYA);
			total_score += pow(dist, 2);
			matchCnt++; 

			if (dist < best_color_score)
			{
				best_color_score = dist;
				best_block_index = k;
			}

		}
		else
		{
			;
		}
	}
	level = matchCnt;
	//if (NUM_OF_BLOCKS - matchCnt <= 1) level = OBJ_HIGH_CONF;
	//else if (NUM_OF_BLOCKS - matchCnt == 2) level = OBJ_MID_CONF;
	//else level = OBJ_LOW_CONF;

	return sqrt(total_score/matchCnt);
}

double AppearanceComparison_powWeight(const BodyInfo* ob1, const BodyInfo* ob2, int & matchCnt, int & level, double & best_color_log_score, int & best_block_index)
{
	double total_log_score = 0;
	matchCnt = 0;
	best_color_log_score = 10000;
	best_block_index = -1;


	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		if (ob1->IsBodySeen[k] && ob2->IsBodySeen[k])
		{
			cvNormalizeHist(ob1->body[k], 1);
			cvNormalizeHist(ob2->body[k], 1);
			double logDist = log(cvCompareHist(ob1->body[k], ob2->body[k], CV_COMP_BHATTACHARYYA));
			total_log_score += logDist;
			matchCnt++; 

			if (logDist < best_color_log_score)
			{
				best_color_log_score = logDist;
				best_block_index = k;
			}

		}
		else
		{
			;
		}
	}
	level = matchCnt;
	//if (NUM_OF_BLOCKS - matchCnt <= 1) level = OBJ_HIGH_CONF;
	//else if (NUM_OF_BLOCKS - matchCnt == 2) level = OBJ_MID_CONF;
	//else level = OBJ_LOW_CONF;

	return (total_log_score/matchCnt);
}
double AppearanceComparison_powWeight_H(const BodyInfo* ob1, const BodyInfo* ob2, int & matchCnt, int & level, double & best_color_log_score, int & best_block_index)
{
	double total_log_score = 0;
	matchCnt = 0;
	best_color_log_score = 10000;
	best_block_index = -1;


	for (int k=0; k<NUM_OF_BLOCKS; k++)
	{
		if (ob1->IsBodySeen[k] && ob2->IsBodySeen[k])
		{
			cvNormalizeHist(ob1->bodyH[k], 1);
			cvNormalizeHist(ob2->bodyH[k], 1);
			double logDist = log(cvCompareHist(ob1->bodyH[k], ob2->bodyH[k], CV_COMP_BHATTACHARYYA));
			total_log_score += logDist;
			matchCnt++; 

			if (logDist < best_color_log_score)
			{
				best_color_log_score = logDist;
				best_block_index = k;
			}

		}
		else
		{
			;
		}
	}
	level = matchCnt;
	//if (NUM_OF_BLOCKS - matchCnt <= 1) level = OBJ_HIGH_CONF;
	//else if (NUM_OF_BLOCKS - matchCnt == 2) level = OBJ_MID_CONF;
	//else level = OBJ_LOW_CONF;

	return (total_log_score/matchCnt);
}

ObjectInfo createObjectInfo(BodyInfo* biptr, int obsIndex_cam1, int obsIndex_cam2, int obj_conf/* = OBJ_NON_DEFINED*/)
{
	ObjectInfo result(biptr, obsIndex_cam1, obsIndex_cam2, obj_conf);
	return result;
}

//functions that deal with consistent indices for observation integration table
//return: RETURN_DONE_NO_ERROR, RETURN_CONTRADICT, RETURN_ALREADY_DONE
//output: confMap
//input: cam1Alive, cam2Alive(already updated)(they must be updated every time index from all local tracking systems)
RETURN_STATUS observationIntegrate(int** confMap, int obsIndex_cam1, int obsIndex_cam2, int Lv, bool* cam1Alive, bool* cam2Alive)
{
	//check self entry
	if (Lv <= 0) return RETURN_CONTRADICT;
	if (confMap[obsIndex_cam1][obsIndex_cam2] >= Lv) return RETURN_ALREADY_DONE;
	if (confMap[obsIndex_cam1][obsIndex_cam2] <= (0-Lv)) return RETURN_ALREADY_DONE;
	if (!(cam1Alive[obsIndex_cam1] && cam2Alive[obsIndex_cam2])) return RETURN_CONTRADICT;
	
	//check other entries
	for (int t=0; t<MAX_TARGET_NUM; t++)
	{
		if (cam2Alive[t]) 
		{
			if ((confMap[obsIndex_cam1][t] > 0) && (t != obsIndex_cam2)) //already integrated with others
			{
				if (confMap[obsIndex_cam1][t] >= Lv) //the same level or higher level of match
					return RETURN_ALREADY_DONE;
			}
		}
		if (cam1Alive[t]) 
		{
			if ((confMap[t][obsIndex_cam2] > 0) && (t != obsIndex_cam1)) //already integrated with others
			{
				if (confMap[t][obsIndex_cam2] >= Lv) //the same level or higher level of match
					return RETURN_ALREADY_DONE;
			}
		}
	}	


	//update otehr entries
	for (int t=0; t<MAX_TARGET_NUM; t++)
	{
		if (cam2Alive[t]) 
		{
			if (confMap[obsIndex_cam1][t] > 0 && (t != obsIndex_cam2)) //already integrated with others
			{
				if (confMap[obsIndex_cam1][t] < Lv) //lower level match
					confMap[obsIndex_cam1][t] = (0-Lv); //break with this level of collision
				else //the same level or higher level of match
					return RETURN_CONTRADICT;
			}
			else //not integrated or in collision 
			{
				if (confMap[obsIndex_cam1][t] <= (0-Lv)) // equal or higher level of collision
					;
				else //lower level of collision
					confMap[obsIndex_cam1][t] = (0-Lv);
			}
		}
		if (cam1Alive[t]) 
		{
			if (confMap[t][obsIndex_cam2] > 0 && (t != obsIndex_cam1)) //already integrated with others
			{
				if (confMap[t][obsIndex_cam2] < Lv) //lower level match
					confMap[t][obsIndex_cam2] = (0-Lv); //break with this level of collision
				else //the same level or higher level of match
					return RETURN_CONTRADICT;
			}
			else //not integrated or in collision 
			{
				if (confMap[t][obsIndex_cam2] <= (0-Lv)) // equal or higher level of collision
					;
				else //lower level of collision
					confMap[t][obsIndex_cam2] = (0-Lv);
			}
		}
	}
	confMap[obsIndex_cam1][obsIndex_cam2] = Lv;

	return RETURN_DONE_NO_ERROR;
}
RETURN_STATUS observationDisappear(int** confMap, int cam_id, int obsIndex_camX, bool* cam1Alive, bool* cam2Alive)
{
	if (cam_id == CAM1) 
	{
		if (cam1Alive[obsIndex_camX]) return RETURN_CONTRADICT;
		for (int t=0; t<MAX_TARGET_NUM; t++)
		{
			confMap[obsIndex_camX][t] = 0;
		}
		return RETURN_DONE_NO_ERROR;
	}
	else if (cam_id == CAM2)
	{
		if (cam2Alive[obsIndex_camX]) return RETURN_CONTRADICT;
		for (int t=0; t<MAX_TARGET_NUM; t++)
		{
			confMap[t][obsIndex_camX] = 0;
		}
		return RETURN_DONE_NO_ERROR;
	}
	else return RETURN_CONTRADICT;
}
RETURN_STATUS observationShowIndep(int** confMap, int cam_id, int obsIndex_camX, bool* cam1Alive, bool* cam2Alive)
{
	bool valid = true;
	if (cam_id == CAM1)
	{
		for (int t=0; t<MAX_TARGET_NUM; t++)
		{
			valid &= (confMap[obsIndex_camX][t] == 0);
		}
		if (!valid) 
			return RETURN_CONTRADICT;
		if (!cam1Alive[obsIndex_camX]) 
			return RETURN_CONTRADICT;
		
		for (int t=0; t<MAX_TARGET_NUM; t++)
		{
			if (!cam2Alive[t]) 
				continue;
			
			int Itg_Cnt = 0;
			int Itg_Lv;
			for (int j=0; j<MAX_TARGET_NUM; j++)
			{
				if (confMap[j][t] > 0) 
				{
					Itg_Cnt++;
					Itg_Lv = confMap[j][t];
				}
			}

			if (Itg_Cnt == 1) //normal case of integration
			{
				confMap[obsIndex_camX][t] = (0-Itg_Lv);
			}
			else if (Itg_Cnt > 1) 
				return RETURN_CONTRADICT;
			else if (Itg_Cnt == 0)
			{
				//Nice, do nothing
				confMap[obsIndex_camX][t] = 0;
			}
			else 
				return RETURN_CONTRADICT;
		}
		return RETURN_DONE_NO_ERROR;
	}
	else if (cam_id == CAM2)
	{
		for (int t=0; t<MAX_TARGET_NUM; t++)
		{
			valid &= (confMap[t][obsIndex_camX] == 0);
		}
		if (!valid) 
			return RETURN_CONTRADICT;
		if (!cam2Alive[obsIndex_camX]) 
			return RETURN_CONTRADICT;

		for (int t=0; t<MAX_TARGET_NUM; t++)
		{
			if (!cam1Alive[t]) 
				continue;
			
			int Itg_Cnt = 0;
			int Itg_Lv;
			for (int j=0; j<MAX_TARGET_NUM; j++)
			{
				if (confMap[t][j] > 0) 
				{
					Itg_Cnt++;
					Itg_Lv = confMap[t][j];
				}
			}

			if (Itg_Cnt == 1) //normal case of high conf integration
			{
				confMap[t][obsIndex_camX] = (0-Itg_Lv);
			}
			else if (Itg_Cnt > 1) 
				return RETURN_CONTRADICT; //error case
			else if (Itg_Cnt == 0) //normal case of no high conf integration
			{
				//Nice, do nothing
				confMap[t][obsIndex_camX] = 0;
			}
			else 
				return RETURN_CONTRADICT; //error case
		}
		return RETURN_DONE_NO_ERROR;
	}
	else 
		return RETURN_CONTRADICT;

}

void printConfMapOnConsole(int** confMap, bool* cam1Alive, bool* cam2Alive)
{
	fprintf(stderr, "Confidence Table between CAM1 and CAM2 \n" 
					"===========================================\n");
	fprintf(stderr, "                     CAM2 STATUS          \n");

	fprintf(stderr, "            ");
	fprintf(stderr, "   " " ");
	for (int t=0; t<MAX_TARGET_NUM; t++)
	{
		if (cam2Alive[t]) fprintf(stderr, " A " " ");
		else fprintf(stderr, " D " " ");
	}
	fprintf(stderr, "\n");
	for (int t=0; t<MAX_TARGET_NUM; t++)//cam1 index
	{
		if (t == (int)MAX_TARGET_NUM/2) fprintf(stderr, "CAM1 STATUS ");
		else fprintf(stderr, "            ");

		if (cam1Alive[t]) fprintf(stderr, " A " " ");
		else fprintf(stderr, " D " " ");
		for (int g=0; g<MAX_TARGET_NUM; g++)//cam2 index
		{
			cout << setw(2) << confMap[t][g] << " " << " ";
			//if (confMap[t][g] == OBJ_HIGH_CONF) fprintf(stderr, " H " " ");
			//else if (confMap[t][g] == OBJ_NON_DEFINED) fprintf(stderr, " - " " ");
			//else if (confMap[t][g] == OBJ_HIGH_BLOCK) fprintf(stderr, " X " " ");
			//else 
			//	;
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

//construct all pair configurations from confidence map
//initialize: t=0, lt are assigned NULL, well-constructed Confidence Map confMap[][], pairModeList is allocated and empty.
//input Rule rl can not be set to be ptr since List is created and stored at pairModeList(we can delete externally), but rule is passed and use for each leaf
//if we don't create (by "new") rule, but just use ptr instead, then different leaf will share the same rule, all leaves will modify the same rule because of passing by pointer
//this is one big trouble, another is that even if we create rule at every iteration, but we do not have the external pointer to delete those rules. MEMORY LEAK
//if you want to pass ptr in your recursive function, please make sure that you won't change the pointed object. Since you may use the same ptr for another resursion line.
//(one recursive function may have more than one recursion line(leaf line))
//Pass by value is a good idea!
void findAllPairMode(int t, Rule rl, List* lt, double** scoreMap, int** confMap, int* pairNumCtr, double* bestScore, vector<List*>* pairModeList)
{
	//initialize the recursive
	if (lt == NULL) 
	{
		lt = new List;
		pairModeList->push_back(lt);
	}

	//reach the end of this pair mode
	if (t == MAX_TARGET_NUM)
	{
		//summarize here
		//we can summarize score, length

		return;
	}


	int count = 0;
	for (int j=0; j<MAX_TARGET_NUM; j++)
	{
		if (confMap[t][j] == 0 || !(rl.checkValidPairFromRule(t, j))) continue;

		count++;

		Rule rule(&rl, makePair(t,j,NULL, NULL));
		List* newLt = new List(lt, makePair(t,j,NULL, NULL), scoreMap[t][j]);
		pairModeList->push_back(newLt);
		findAllPairMode(t+1, rule, newLt, scoreMap, confMap, pairNumCtr, bestScore, pairModeList);
	}
	
	//create a pair mode which takes no pairs at this iteration
	findAllPairMode(t+1, rl, lt, scoreMap, confMap, pairNumCtr, bestScore, pairModeList);

}

//if want to print points, just set firstPt = secondPt
void printCalibLine_3Dto2D(Position3D pt1, Position3D pt2, CameraLine3D cameraLine, IplImage* srcImg)
{
	Line3D line3d(pt1, pt2, true);

	//cameraLine should be initialized already.
	if (MAGNITUDE((pt1 - pt2)) < IDENTICAL_THD) //identical pt1 == pt2
	{
		cameraLine.setBy3DPt_precise(pt1);
		cvCircle(srcImg, cameraLine.imageLoc, 2, CV_RGB(200,100,20), -1);
		return;
	}
	
	if (abs((pt1.x - pt2.x)) > IDENTICAL_THD)
	{
		double delta_x = (pt2.x - pt1.x)/500;
		Position3D printPt;
		for (double t=0; t<=500; t++)
		{
			line3d.getPosFromX(pt1.x + t*delta_x, printPt);
			cameraLine.setBy3DPt_precise(printPt);
			cvCircle(srcImg, cameraLine.imageLoc, 1, CV_RGB(200,100,20), -1);
		}
	}
	else if (abs((pt1.y - pt2.y)) > IDENTICAL_THD)
	{
		double delta_y = (pt2.y - pt1.y)/500;
		Position3D printPt;
		for (double t=0; t<=500; t++)
		{
			line3d.getPosFromY(pt1.y + t*delta_y, printPt);
			cameraLine.setBy3DPt_precise(printPt);
			cvCircle(srcImg, cameraLine.imageLoc, 1, CV_RGB(200,100,20), -1);
		}
	}
	else if (abs((pt1.z - pt2.z)) > IDENTICAL_THD)
	{
		double delta_z = (pt2.z - pt1.z)/500;
		Position3D printPt;
		for (double t=0; t<=500; t++)
		{
			line3d.getPosFromZ(pt1.z + t*delta_z, printPt);
			cameraLine.setBy3DPt_precise(printPt);
			cvCircle(srcImg, cameraLine.imageLoc, 1, CV_RGB(200,100,20), -1);
		}
	}
	else 
	{
		//problem here
	}
	
}
void printCalibLine_2Dto3D(CvPoint pt1, CvPoint pt2, CameraLine3D cameraLine, IplImage* srcImg)
{
	;
}

//calculate standard deviation and mean for each iteration
//input: newSample, oldMean, oldStdDev
//output: sampleNum, newMean, newStdDev
void calcMeanStdDev(double newSample, int & sampleNum, double & newMean, double & newStdDev, double oldMean, double oldStdDev)
{
	double squareSum = pow(oldStdDev, 2)*(sampleNum-1) + sampleNum*oldMean*oldMean;
	squareSum = squareSum + newSample*newSample;
	double Sum = oldMean*sampleNum;
	Sum = Sum + newSample;
	sampleNum++;
	newMean = Sum/sampleNum;
	newStdDev = sqrt((squareSum-newMean*newMean*sampleNum)/(sampleNum-1));
}