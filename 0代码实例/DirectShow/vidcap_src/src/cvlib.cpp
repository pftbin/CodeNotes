// cvlib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "cvlib.h"

#include "cvLib\vec2D.h"
#include "cvLib\imageresize.h"
#include "cvLib\facedetector.h"



wchar_t mess[256] = L"";

float m_zoom = 0.125f;
vector<float> m_scales;

ImageResize resize;
FaceDetector fdetect;



void cvInit(unsigned int image_width, unsigned int image_height,
            unsigned int face_width, unsigned int face_height, double zoom)
{
        m_zoom = (float)zoom;
        resize.init(image_width, image_height, m_zoom);
        if (m_scales.size()) {
                fdetect.init(resize.gety()->width(), resize.gety()->height(), face_width, face_height,
                             &m_scales[0], (unsigned int)m_scales.size());
        } else
                fdetect.init(resize.gety()->width(), resize.gety()->height(), face_width, face_height);
}

int cvInitAI(const wchar_t* face_detector,
             const wchar_t* projection_matrix,
             const wchar_t* skin_filter,
             const wchar_t* preface_filter)               //0-OK, <0-err
{
        int res;

        res = fdetect.load_face_classifier(face_detector);
        if (res != 0)
                return res;

        res = fdetect.load_projection_matrix(projection_matrix);
        if (res != 0)
                return res;

        if (skin_filter != 0)
                fdetect.load_skin_filter(skin_filter) ;

        if (preface_filter != 0)
                fdetect.load_preface_filter(preface_filter);

        return fdetect.status_of_classifiers();
}

void cvSetScales(const double* scales, unsigned int size)
{
        m_scales.clear();
        for (unsigned int i = 0; i < size; i++)
                m_scales.push_back((float)scales[i]);
}
//


/*
    detect faces
*/
int cvDetect(const unsigned char *pBGR)
{
        if (resize.gety() == 0)
                return -1;

        resize.resize(pBGR);
        return fdetect.detect(resize.gety(), resize.getr(), resize.getg(), resize.getb());
}

float cvSkinAmount()
{
        return fdetect.get_skin_amount();
}

/*int cvDetect(unsigned char *r, unsigned char *g, unsigned char *b)
{
        if (resize.gety() == 0) return -1;

        resize.resize(r, g, b);
        fdetect.detectNNpca(resize.gety(), resize.getr(), resize.getg(), resize.getb());

        return fdetect.getfacenum();
}*/

void cvFaceRect(unsigned int i, RECT& rect)
{
        if (i < 0) i = 0;
        if (i >= fdetect.get_faces_number()) i = fdetect.get_faces_number() - 1;

        const RECT* r = fdetect.get_face_rect(i);

        rect.left = int((float)r->left * (1.0f / m_zoom));
        rect.top = int((float)r->top * (1.0f / m_zoom));
        rect.right = int((float)(r->right + 1) * (1.0f / m_zoom) - (float)r->left * (1.0f / m_zoom));
        rect.bottom = int((float)(r->bottom + 1) * (1.0f / m_zoom) - (float)r->top * (1.0f / m_zoom));
}

const vec2D* cvGetFace(unsigned int i)
{
        return fdetect.get_face(i);
}










/*
   setup params info
*/
const wchar_t *cvInfo()
{
        wchar_t tmp[256] = L"";

        swprintf(mess, L" classifiers: %d\r\n", fdetect.status_of_classifiers());
        if (resize.gety()) {
                swprintf(tmp, L" image: %dx%d\r\n", resize.gety()->width(), resize.gety()->height());
                wcscat(mess, tmp);
        }

        if (m_scales.size()) {
                swprintf(tmp, L" scales: ");
                wcscat(mess, tmp);
                for (unsigned int i = 0; i < m_scales.size(); i++) {
                        swprintf(tmp, L"%.2f ", m_scales[i]);
                        wcscat(mess, tmp);
                }
                wcscat(mess, L"\r\n");
        }

        return &mess[0];
}
//




LARGE_INTEGER m_nFreq;
LARGE_INTEGER m_nBeginTime;

void tic()
{
        QueryPerformanceFrequency(&m_nFreq);
        QueryPerformanceCounter(&m_nBeginTime);
}
__int64 toc()
{
        LARGE_INTEGER nEndTime;
        __int64 nCalcTime;

        QueryPerformanceCounter(&nEndTime);
        nCalcTime = (nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;

        return nCalcTime;
}
