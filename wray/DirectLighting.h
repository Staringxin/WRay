#pragma once
#include "surfaceintegrator.h"

class WDirectLighting :
	public WSurfaceIntegrator
{
public:
	WDirectLighting(Scene *iscene,WAccelerator*itree):
	  WSurfaceIntegrator(iscene,itree){}
	virtual ~WDirectLighting(void);

	//���㳡�����еƹ�Ĺ���
	Vector3 sampleAllLights(
		BSDF*bsdf,Sample3D&lightSample,
		Sample2D&bsdfSample,const Vector3&ro, int* nodeInfo = NULL);

	Vector3 sampleOneLight(
		BSDF*bsdf, Sample3D&lightSample,
		Sample2D&bsdfSample, const Vector3&ro, int* nodeInfo = NULL);
private:
	//����ѡ���ĵƹ���������ֱ�ӹ���
	//roΪ���߳��䷽��
	Vector3 computeDirectLight(
		Light *light,BSDF*bsdf,Sample3D&lightSample,
		Sample2D&bsdfSample,const Vector3&ro, int* nodeInfo = NULL);

	bool isVisible(Vector3 pos1,Vector3 pos2, int* beginNode = NULL);

};
