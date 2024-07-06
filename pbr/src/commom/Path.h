#include "Log.h"

#define PATH "../resource/"

//#define PATH "C:\\work\\vs 2022\\pbr02\\data\\"

const int gDisplaySizeX = 1920;         // ��ʾ���ڵĿ�ȣ���λΪ����
const int gDisplaySizeY = 1080;         // ��ʾ���ڵĸ߶ȣ���λΪ����
const int gDisplaySamples = 16;			// ��Ⱦʱ�Ĳ���������Խ������Խ�ã���Ҳ�����Ӽ�����
const float gViewDistance = 150.0f;     // �ӵ���Ŀ��֮��ľ���
const float gViewFOV = 45.0f;           // ��Ұ�ĽǶȴ�С����λΪ�ȣ�degree��
const float gOrbitSpeed = 1.0f;         // �����ת�ٶȣ�Ӱ���ӵ�Χ��Ŀ����ת���ٶ�
const float gZoomSpeed = 4.0f;          // �����ٶȣ�Ӱ���ӵ�������Զ��Ŀ����ٶ�

// Parameters
static constexpr int gEnvMapSize = 1024;		// ������ͼ�Ĵ�С�����ڷ���͹��ռ��㣩
static constexpr int gIrradianceMapSize = 32;	// ���ն���ͼ�Ĵ�С��������������ռ��㣩
static constexpr int gBRDF_LUT_Size = 256;		// BRDF ���ұ�Ĵ�С�����ھ��淴����㣩

// �������Թ��ˣ�Anisotropic Filtering�����AF����һ�����������������ļ������ر�����������۲�
// ���ӽǳ����ʱ��maxAnisotropyͨ����ʾͼ��Ӳ��֧�ֵ����������Թ��˼������磬ֵΪ16��ʾӲ��
// ֧�ֵ����������Թ��˼�����16����

extern float gMaxAnisotropy;					// ����������

void InitPath();




