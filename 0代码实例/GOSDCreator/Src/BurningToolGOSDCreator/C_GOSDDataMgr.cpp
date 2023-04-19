#include "StdAfx.h"
#include "C_GOSDDataMgr.h"

//opencv
#include "opencv2/opencv.hpp"


C_GOSDDataMgr::C_GOSDDataMgr(void)
	:m_dwColor0(RGB(0,0,0))
	,m_dwColor1(RGB(0,0,0))
	,m_dwColor2(RGB(0,0,0))
	,m_dwColor3(RGB(0,0,0))
	,m_dwMaxSize(64)
	,m_dwPicMaxWidth(1080)
	,m_dwPicMaxHeight(800)
	,m_dwEnable(0)
	,m_dwColor0_Y(0)
	,m_dwColor0_U(0)
	,m_dwColor0_V(0)
	,m_dwColor1_Y(0)
	,m_dwColor1_U(0)
	,m_dwColor1_V(0)
	,m_dwColor2_Y(0)
	,m_dwColor2_U(0)
	,m_dwColor2_V(0)
	,m_dwColor3_Y(0)
	,m_dwColor3_U(0)
	,m_dwColor3_V(0)
	,m_dwRes_X(0x500)
	,m_dwRes_Y(0x2D0)
	,m_dwStart_X(0x1E1)
	,m_dwStart_Y(0xF1)
	,m_dwSize_W(0x140)
	,m_dwSize_H(0xF0)
	,m_dwGPIO_Sel(0x60)
	,m_dwTrig_Mode(0)
	,m_dwBit_Mode(0)
	,m_dwScale_Value(0)
	,m_dwBk_Tie_YUV(1)
{
	InitDataMap();
}

C_GOSDDataMgr::~C_GOSDDataMgr(void)
{
	m_mapData.clear();
}

void C_GOSDDataMgr::InitDataMap()
{
	//Custom
	m_mapData[GOSD_COLOR0] = m_dwColor0;
	m_mapData[GOSD_COLOR1] = m_dwColor1;
	m_mapData[GOSD_COLOR2] = m_dwColor2;
	m_mapData[GOSD_COLOR3] = m_dwColor3;
	m_mapData[GOSD_MAXSIZE] = m_dwMaxSize;
	m_mapData[GOSD_PIC_MAX_W] = m_dwPicMaxWidth;
	m_mapData[GOSD_PIC_MAX_H] = m_dwPicMaxHeight;
	//Init
	m_mapData[GOSD_ENABLE] = m_dwEnable;
	m_mapData[GOSD_COLOR0_Y] = m_dwColor0_Y;
	m_mapData[GOSD_COLOR0_U] = m_dwColor0_U;
	m_mapData[GOSD_COLOR0_V] = m_dwColor0_V;
	m_mapData[GOSD_COLOR1_Y] = m_dwColor1_Y;
	m_mapData[GOSD_COLOR1_U] = m_dwColor1_U;
	m_mapData[GOSD_COLOR1_V] = m_dwColor1_V;
	m_mapData[GOSD_COLOR2_Y] = m_dwColor2_Y;
	m_mapData[GOSD_COLOR2_U] = m_dwColor2_U;
	m_mapData[GOSD_COLOR2_V] = m_dwColor2_V;
	m_mapData[GOSD_COLOR3_Y] = m_dwColor3_Y;
	m_mapData[GOSD_COLOR3_U] = m_dwColor3_U;
	m_mapData[GOSD_COLOR3_V] = m_dwColor3_V;

	m_mapData[GOSD_RES_X]	 = m_dwRes_X;
	m_mapData[GOSD_RES_Y]	 = m_dwRes_Y;
	m_mapData[GOSD_START_X]  = m_dwStart_X;
	m_mapData[GOSD_START_Y]  = m_dwStart_Y;
	m_mapData[GOSD_SIZE_W ]  = m_dwSize_W;
	m_mapData[GOSD_SIZE_H ]  = m_dwSize_H;
	m_mapData[GOSD_GPIO_SEL] = m_dwGPIO_Sel;
	m_mapData[GOSD_TRIG_MODE]= m_dwTrig_Mode;
	m_mapData[GOSD_BIT_MODE] = m_dwBit_Mode;
	m_mapData[GOSD_SCALE_VALUE] = m_dwScale_Value;
	m_mapData[GOSD_BK_TIE_YUV]  = m_dwBk_Tie_YUV;

	CString strInitCfgPath = _T(".\\Init\\GOSD_Setting.ini");
	LoadGOSDFile(strInitCfgPath);	
}

BOOL C_GOSDDataMgr::LoadGOSDFile(LPCTSTR lpszFilePath)
{
	CString strKeyArray[] = 
	{
		//Custom
		GOSD_COLOR0,
		GOSD_COLOR1,
		GOSD_COLOR2,
		GOSD_COLOR3,

		//Init
		GOSD_MAXSIZE,
		GOSD_PIC_MAX_W,
		GOSD_PIC_MAX_H,

		GOSD_ENABLE,
		GOSD_COLOR0_Y,
		GOSD_COLOR0_U,	
		GOSD_COLOR0_V,	
		GOSD_COLOR1_Y,	
		GOSD_COLOR1_U,	
		GOSD_COLOR1_V,	
		GOSD_COLOR2_Y,	
		GOSD_COLOR2_U,	
		GOSD_COLOR2_V,	
		GOSD_COLOR3_Y,	
		GOSD_COLOR3_U,	
		GOSD_COLOR3_V,

		GOSD_RES_X,	
		GOSD_RES_Y,		
		GOSD_START_X,	
		GOSD_START_Y,	
		GOSD_SIZE_W,
		GOSD_SIZE_H,	
		GOSD_GPIO_SEL,	
		GOSD_TRIG_MODE,	
		GOSD_BIT_MODE,
		GOSD_SCALE_VALUE,
		GOSD_BK_TIE_YUV
	};

	TCHAR   szText[MAX_PATH + 1] = {0};
	CString strSection = _T("General");
	CString strKey     = _T("");
	DWORD   dwValue = 0;
	CString strText = _T("");

	for (int i = 0; i < sizeof(strKeyArray)/sizeof(strKeyArray[0]); i++)
	{
		strKey = strKeyArray[i];

		if(::GetPrivateProfileString(strSection, strKey, _T(""), szText, MAX_PATH, lpszFilePath) <= 0)
			continue;//return FALSE;
		else
		{
			strText = szText;
			if (strText.Find(_T("0x")) != -1 || strText.Find(_T("0X")) != -1)
			{
				strText.Replace(_T("0x"),_T(""));
				strText.Replace(_T("0X"),_T(""));
				char pBuf[255] = {0};
				USES_CONVERSION;
				strcpy_s(pBuf,T2A(strText));
				dwValue = strtol(pBuf,NULL,16);
			}
			else
				dwValue = _ttoi(strText);

			m_mapData[strKey] = dwValue;
		}
	}

	return TRUE;
}

void C_GOSDDataMgr::GetYUV(COLORREF clr,int* nY,int* nU,int* nV)
{
	int R = GetRValue(clr);
	int G = GetGValue(clr);
	int B = GetBValue(clr);

	*nY = R *  .299000 + G *  .587000 + B *  .114000;
	*nU = R * -.168736 + G * -.331264 + B *  .500000 + 128;
	*nV = R *  .500000 + G * -.418688 + B * -.081312 + 128;
}

COLORREF C_GOSDDataMgr::GetClrByYUV(int nY,int nU,int nV)
{
	int R = 0,G = 0,B = 0;
	R = nY + 1.4075 * (nV - 128);
	G = nY - 0.3455 * (nU - 128) - (0.7169 * (nV - 128));
	B = nY + 1.7790 * (nU - 128);

	return RGB(R,G,B);

}

DWORD C_GOSDDataMgr::GetItemValue(CString strItemName)
{
	DWORD dwValue = 0;
	std::map<CString,DWORD>::iterator it = m_mapData.find(strItemName);
	if (it != m_mapData.end())
	{
		dwValue = it->second;
	}
	return dwValue;
}
void  C_GOSDDataMgr::SetItemValue(CString strItemName,DWORD dwValue)
{
	std::map<CString,DWORD>::iterator it = m_mapData.find(strItemName);
	if (it != m_mapData.end())
	{
		it->second = dwValue;
	}
}



///////////////////K-meansEx////////////////////////////////
#define  RANDOM_INIT_CENTER 1
using namespace cv;
static void generateRandomCenter(const vector<Vec2f>& box, float* center, RNG& rng)
{
	size_t j, dims = box.size();
	float margin = 1.f/dims;
	for( j = 0; j < dims; j++ )
		center[j] = ((float)rng*(1.f+margin*2.f)-margin)*(box[j][1] - box[j][0]) + box[j][0];
}

static inline float distance(const float* a, const float* b, int n)
{
	int j = 0; float d = 0.f;
#if CV_SSE
	if( USE_SSE2 )
	{
		float CV_DECL_ALIGNED(16) buf[4];
		__m128 d0 = _mm_setzero_ps(), d1 = _mm_setzero_ps();

		for( ; j <= n - 8; j += 8 )
		{
			__m128 t0 = _mm_sub_ps(_mm_loadu_ps(a + j), _mm_loadu_ps(b + j));
			__m128 t1 = _mm_sub_ps(_mm_loadu_ps(a + j + 4), _mm_loadu_ps(b + j + 4));
			d0 = _mm_add_ps(d0, _mm_mul_ps(t0, t0));
			d1 = _mm_add_ps(d1, _mm_mul_ps(t1, t1));
		}
		_mm_store_ps(buf, _mm_add_ps(d0, d1));
		d = buf[0] + buf[1] + buf[2] + buf[3];
	}
	else
#endif
	{
		for( ; j <= n - 4; j += 4 )
		{
			float t0 = a[j] - b[j], t1 = a[j+1] - b[j+1], t2 = a[j+2] - b[j+2], t3 = a[j+3] - b[j+3];
			d += t0*t0 + t1*t1 + t2*t2 + t3*t3;
		}
	}

	for( ; j < n; j++ )
	{
		float t = a[j] - b[j];
		d += t*t;
	}
	return d;
}

static void generateCentersPP(const Mat& _data, Mat& _out_centers,
							  int K, RNG& rng, int trials)
{
	int i, j, k, dims = _data.cols, N = _data.rows;
	const float* data = _data.ptr<float>(0);
	size_t step = _data.step/sizeof(data[0]);
	vector<int> _centers(K);
	int* centers = &_centers[0];
	vector<float> _dist(N*3);
	float* dist = &_dist[0], *tdist = dist + N, *tdist2 = tdist + N;
	double sum0 = 0;

	centers[0] = (unsigned)rng % N;

	for( i = 0; i < N; i++ )
	{
		dist[i] = distance(data + step*i, data + step*centers[0], dims);
		sum0 += dist[i];
	}

	for( k = 1; k < K; k++ )
	{
		double bestSum = DBL_MAX;
		int bestCenter = -1;

		for( j = 0; j < trials; j++ )
		{
			double p = (double)rng*sum0, s = 0;
			for( i = 0; i < N-1; i++ )
				if( (p -= dist[i]) <= 0 )
					break;
			int ci = i;
			for( i = 0; i < N; i++ )
			{
				tdist2[i] = std::min(distance(data + step*i, data + step*ci, dims), dist[i]);
				s += tdist2[i];
			}

			if( s < bestSum )
			{
				bestSum = s;
				bestCenter = ci;
				std::swap(tdist, tdist2);
			}
		}
		centers[k] = bestCenter;
		sum0 = bestSum;
		std::swap(dist, tdist);
	}

	for( k = 0; k < K; k++ )
	{
		const float* src = data + step*centers[k];
		float* dst = _out_centers.ptr<float>(k);
		for( j = 0; j < dims; j++ )
			dst[j] = src[j];
	}
}

double kmeansEx( InputArray _data, int K,
				InputOutputArray _bestLabels,
				TermCriteria criteria, int attempts,
				int flags, OutputArray _centers )
{
	int nAdjustTimes = 0;//user add
	const int SPP_TRIALS = 3;
	Mat data = _data.getMat();
	int N = data.rows > 1 ? data.rows : data.cols;
	int dims = (data.rows > 1 ? data.cols : 1)*data.channels();
	int type = data.depth();

	attempts = std::max(attempts, 1);
	CV_Assert( data.dims <= 2 && type == CV_32F && K > 0 );

	_bestLabels.create(N, 1, CV_32S, -1, true);

	Mat _labels, best_labels = _bestLabels.getMat();
	if( flags & CV_KMEANS_USE_INITIAL_LABELS )
	{
		CV_Assert( (best_labels.cols == 1 || best_labels.rows == 1) &&
			best_labels.cols*best_labels.rows == N &&
			best_labels.type() == CV_32S &&
			best_labels.isContinuous());
		best_labels.copyTo(_labels);
	}
	else
	{
		if( !((best_labels.cols == 1 || best_labels.rows == 1) &&
			best_labels.cols*best_labels.rows == N &&
			best_labels.type() == CV_32S &&
			best_labels.isContinuous()))
			best_labels.create(N, 1, CV_32S);
		_labels.create(best_labels.size(), best_labels.type());
	}
	int* labels = _labels.ptr<int>();

	Mat centers(K, dims, type), old_centers(K, dims, type);
	vector<int> counters(K);
	vector<Vec2f> _box(dims);
	Vec2f* box = &_box[0];

	double best_compactness = DBL_MAX, compactness = 0;//64bit float Max
	RNG& rng = theRNG();//Returns the default random number generator.
	int a, iter, i, j, k;

	if( criteria.type & TermCriteria::EPS )//stop the algorithm after the achieved algorithm-dependent accuracy becomes lower than epsilon
		criteria.epsilon = std::max(criteria.epsilon, 0.);
	else
		criteria.epsilon = FLT_EPSILON;
	criteria.epsilon *= criteria.epsilon;

	if( criteria.type & TermCriteria::COUNT )
		criteria.maxCount = std::min(std::max(criteria.maxCount, 2), 100);
	else
		criteria.maxCount = 100;

	if( K == 1 )
	{
		attempts = 1;
		criteria.maxCount = 2;
	}

	const float* sample = data.ptr<float>(0);

	for( j = 0; j < dims; j++ )
		box[j] = Vec2f(sample[j], sample[j]);

	for( i = 1; i < N; i++ )
	{
		sample = data.ptr<float>(i);
		for( j = 0; j < dims; j++ )
		{
			float v = sample[j];
			box[j][0] = std::min(box[j][0], v);
			box[j][1] = std::max(box[j][1], v);
		}
	}

	for( a = 0; a < attempts; a++ )
	{
		double max_center_shift = DBL_MAX;
		for( iter = 0; iter < criteria.maxCount && max_center_shift > criteria.epsilon; iter++)
		{
			swap(centers, old_centers);
			if( iter == 0 && (a > 0 || !(flags & KMEANS_USE_INITIAL_LABELS)) )
			{
				if( flags & KMEANS_PP_CENTERS )
				{
#if RANDOM_INIT_CENTER

					//PP Random
					generateCentersPP(data, centers, K, rng, SPP_TRIALS);

#else
					switch(a)
					{
					case 0:
						for( k = 0; k < K; k++ )
						{
							//User set Init point center
							float* pcenters = centers.ptr<float>(k);
							for (int d = 0; d < dims; d++)
							{
//								pcenters[d] = InitCenter0[k][d];
							}
						}
						break;
					case 1:
						for( k = 0; k < K; k++ )
						{
							//User set Init point center
							float* pcenters = centers.ptr<float>(k);
							for (int d = 0; d < dims; d++)
							{
//								pcenters[d] = InitCenter1[k][d];
							}
						}
						break;
					case 2:
						for( k = 0; k < K; k++ )
						{
							//User set Init point center
							float* pcenters = centers.ptr<float>(k);
							for (int d = 0; d < dims; d++)
							{
//								pcenters[d] = InitCenter2[k][d];
							}
						}
						break;
					case 3:
						for( k = 0; k < K; k++ )
						{
							//User set Init point center
							float* pcenters = centers.ptr<float>(k);
							for (int d = 0; d < dims; d++)
							{
//								pcenters[d] = InitCenter3[k][d];
							}
						}
						break;
					case 4:
						for( k = 0; k < K; k++ )
						{
							//User set Init point center
							float* pcenters = centers.ptr<float>(k);
							for (int d = 0; d < dims; d++)
							{
								pcenters[d] = InitCenter4[k][d];
							}
						}
						break;
					case 5:
					default:
						for( k = 0; k < K; k++ )
						{
							//User set Init point center
							float* pcenters = centers.ptr<float>(k);
							for (int d = 0; d < dims; d++)
							{
								pcenters[d] = InitCenter5[k][d];
							}
						}
						break;
					}
#endif

					//print out 
					TRACE("1--------------------------1\n");
					for( k = 0; k < K; k++ )
					{
						float* pCenter = centers.ptr<float>(k);
						for (int d = 0; d < dims; d++)
						{
							TRACE("%f",pCenter[d]);
							TRACE(",");
						}
						TRACE("\n");
					}
				}
				else
				{
					//complete random
					for( k = 0; k < K; k++ )
					{
						generateRandomCenter(_box, centers.ptr<float>(k), rng);
					}

					//print out 					
					TRACE("2--------------------------2\n");
					for( k = 0; k < K; k++ )
					{
						float* pCenter = centers.ptr<float>(k);
						for (int d = 0; d < dims; d++)
						{
							TRACE("%f",pCenter[d]);
							TRACE(",");
						}
						TRACE("\n");
					}
				}
			}
			else
			{
				if( iter == 0 && a == 0 && (flags & KMEANS_USE_INITIAL_LABELS) )
				{
					for( i = 0; i < N; i++ )
						CV_Assert( (unsigned)labels[i] < (unsigned)K );
				}

				// compute centers
				centers = Scalar(0);
				for( k = 0; k < K; k++ )
					counters[k] = 0;

				for( i = 0; i < N; i++ )
				{
					sample = data.ptr<float>(i);
					k = labels[i];
					float* center = centers.ptr<float>(k);
					for( j = 0; j <= dims - 4; j += 4 )
					{
						float t0 = center[j] + sample[j];
						float t1 = center[j+1] + sample[j+1];

						center[j] = t0;
						center[j+1] = t1;

						t0 = center[j+2] + sample[j+2];
						t1 = center[j+3] + sample[j+3];

						center[j+2] = t0;
						center[j+3] = t1;
					}

					for( ; j < dims; j++ )
						center[j] += sample[j];

					counters[k]++;
				}

				if( iter > 0 )
					max_center_shift = 0;


				for( k = 0; k < K; k++ )
				{
					float* center = centers.ptr<float>(k);
					if( counters[k] != 0 )
					{
						float scale = 1.f/counters[k];
						for( j = 0; j < dims; j++ )
							center[j] *= scale;
					}
					else
					{
#if RANDOM_INIT_CENTER
						//complete random
						generateRandomCenter(_box, center, rng);

#else

						int n = nAdjustTimes%25;//max = 25
						for( j = 0; j < dims; j++ )
							center[j] = AdjustCenter[n][j] ;

						nAdjustTimes++;//simulation of random 
#endif

						//print out 
						TRACE("==========================\n");
						for (int d = 0; d < dims; d++)
						{
							TRACE("%f",center[d]);
							TRACE(",");
						}
						TRACE("\n");
					}

					if( iter > 0 )
					{
						double dist = 0;
						const float* old_center = old_centers.ptr<float>(k);
						for( j = 0; j < dims; j++ )
						{
							double t = center[j] - old_center[j];
							dist += t*t;
						}
						max_center_shift = std::max(max_center_shift, dist);
					}
				}
			}

			compactness = 0;
			for( i = 0; i < N; i++ )
			{
				sample = data.ptr<float>(i);
				int k_best = 0;
				double min_dist = DBL_MAX;

				for( k = 0; k < K; k++ )
				{
					const float* center = centers.ptr<float>(k);
					double dist = distance(sample, center, dims);

					if( min_dist > dist )
					{
						min_dist = dist;
						k_best = k;
					}
				}

				compactness += min_dist;
				labels[i] = k_best;
			}
		}

		if( compactness < best_compactness )
		{
			best_compactness = compactness;
			if( _centers.needed() )
				centers.copyTo(_centers);
			_labels.copyTo(best_labels);
		}
	}

	return best_compactness;
}














BOOL C_GOSDDataMgr::ConvertImage(int nClusterNum,CString strSourcePath,CString strTargetPath)
{
	std::string  strSource = (CT2A)strSourcePath;
	std::string  strTarget = (CT2A)strTargetPath;

	using namespace cv;
//	Mat src = imread("C:/1.png"); 
//	imshow("input", src);

	Mat src = imread(strSource);
	int width = src.cols; 
	int height = src.rows; 
	int dims = src.channels(); 

	//Init Value
	int sampleCount = width*height; 
	int clusterCount = nClusterNum; 
	Mat points(sampleCount, dims, CV_32F, Scalar(10)); 
	Mat labels; 
	Mat centers(clusterCount, 1, points.type()); 

	//RGB Data Convert
	int index = 0; 
	for (int row = 0; row < height; row++) { 
		for (int col = 0; col < width; col++) { 
			index = row*width + col; 
			Vec3b rgb = src.at<Vec3b>(row, col); 
			points.at<float>(index, 0) = static_cast<int>(rgb[0]); 
			points.at<float>(index, 1) = static_cast<int>(rgb[1]); 
			points.at<float>(index, 2) = static_cast<int>(rgb[2]); 
		} 
	} 

	//K-Means
//	TermCriteria criteria = TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0); 
//	kmeans(points, clusterCount, labels, criteria, 3, KMEANS_RANDOM_CENTERS ,centers);

	TermCriteria criteria = TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0); 
	kmeansEx(points, clusterCount, labels, criteria, 3, KMEANS_RANDOM_CENTERS ,centers);
	

	//Color Cluster Merge
	Mat result = Mat::zeros(src.size(), CV_8UC3); 
	for (int row = 0; row < height; row++) { 
		for (int col = 0; col < width; col++) { 
			index = row*width + col; 
			int label = labels.at<int>(index, 0); 
			result.at<Vec3b>(row, col)[0] = centers.at<float>(label,0);
			result.at<Vec3b>(row, col)[1] = centers.at<float>(label,1); 
			result.at<Vec3b>(row, col)[2] = centers.at<float>(label,2);	
		} 
	} 

	//Write Image
//	imshow("kmeans-demo", result); 
 	imwrite(strTarget, result);

	return TRUE;
}

