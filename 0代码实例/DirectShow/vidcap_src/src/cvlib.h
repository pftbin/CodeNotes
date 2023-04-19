
#ifndef CVLIB_h
#define CVLIB_h


void cvInit(unsigned int image_width, unsigned int image_height,
            unsigned int face_width, unsigned int face_height, double zoom);

int cvInitAI(const wchar_t* face_detector,
             const wchar_t* projection_matrix,
             const wchar_t* skin_filter = 0,
             const wchar_t * preface_filter = 0);              //0-OK, <0-err

void cvSetScales(const double* scales, unsigned int size);     //scales = {0.9, 0.8, 0.7, ... N} N = size

const wchar_t* cvInfo();         //get fdetect setup params

int cvDetect(const unsigned char *pBGR);
//int cvDetect(unsigned char *r, unsigned char *g, unsigned char *b);       //detect faces in image

float cvSkinAmount();
const vec2D* cvGetFace(unsigned int i);
void cvFaceRect(unsigned int i, RECT& rect);                                //get face rectangle


void tic();
__int64 toc();

#endif

/*
     calllib('cvlib','cvSetScales',[0.9 0.8 0.7], 3)
     calllib('cvlib','cvInit',640,480,0.125,19,19)
     calllib('cvlib','cvInitNN',face,prj,skin,prefilter)
     calllib('cvlib','cvInfo')

      I = image 480x640x3
       r = I(:,:,1)'
       g = I(:,:,2)'
       b = I(:,:,3)'

     fnum = calllib('cvlib','cvDetect',r,g,b)

      S = struct('x',0,'y',0,'w',0,'h',0)
      pS = libstruct('s_rec',S);
      calllib('cvlib','cvFaceRect',0,pS)

*/