#include "StdAfx.h"
#include "DirectLighting.h"



WDirectLighting::~WDirectLighting(void)
{
}

bool WDirectLighting::isVisible(WVector3 pos1, WVector3 pos2, int* beginNode)
{
	WVector3 delta=pos2-pos1;
	float length=delta.length();
	delta.normalize();
	//ע�⣺����tMin�����������1e-1����Ӱ����Ӱ������
	//ֵ̫С��ʱ�����ڼ�����
	//��ģ�ͱ�������һЩ�ߵ㣬
	//̫���ʱ���ʹ����Ӱ��׼ȷ
	WRay r(pos1,delta,1e-2f,length - 1e-2f);
	int begNode = beginNode ? *beginNode : -1;
	return !tree->isIntersect(r, begNode);
}
WVector3 WDirectLighting::computeDirectLight(WLight *light, WBSDF *bsdf, WSample3D &lightSample, WSample2D &bsdfSample,const WVector3&ro, int* nodeInfo)
{
	float LSu,LSv,LSw;
	lightSample.get3D(LSu,LSv,LSw);
	float LPDF;//�Եƹ�������ܶȺ���
	WVector3 lightPosition,intensity;
	light->sampleLight(LSu,LSv,LSw,*bsdf,lightPosition,intensity,LPDF);
	bool visibilityTest=isVisible(bsdf->DG.position,lightPosition, nodeInfo);

	float BSu, BSv;
	bsdfSample.get2D(BSu, BSv);
	WVector3 sampleWi, sampleWo;
	float BPDF;
	bsdf->sampleRay(BSu, BSv, sampleWi, sampleWo, BPDF);
	
	WVector3 Ld(0);
	if (visibilityTest)
	{
		WVector3 ri = lightPosition - bsdf->DG.position;
		ri.normalize();
		WVector3 fCos = bsdf->evaluateFCos(ri, ro);
		float weight = 1.0;
		if (!light->isDeltaLight())
		{
			weight = WMonteCarlo::powerHeuristic(1, LPDF, 1, BPDF);
		}
		Ld = fCos*intensity*weight / LPDF;
	}

//	Ld.showCoords();
	//���ﻹ�ж�bsdf�����Ĳ���δʵ��
	WRay ray(bsdf->DG.position, sampleWi);
	WDifferentialGeometry DG;
	int beginNode = 0, endNode;
	if (tree->intersect(ray, DG, &endNode, beginNode))
	{
		WMaterial*mtl;
		scene->getNthMaterial(mtl, DG.mtlId);
		WBSDF* sourceBSDF;
		mtl->buildBSDF(DG, sourceBSDF);
		WVector3 emission = sourceBSDF->getEmission();
		if (!emission.isZero())
		{
			WVector3 fCos = bsdf->evaluateFCos(sampleWi, ro);
			float weight = WMonteCarlo::powerHeuristic(1, BPDF, 1, LPDF);
			Ld += fCos*emission* weight / BPDF;
		}
		delete sourceBSDF;
	}
	return Ld;
}
WVector3 WDirectLighting::sampleAllLights(WBSDF *bsdf, WSample3D &lightSample, WSample2D &bsdfSample, const WVector3 &ro, int* nodeInfo)
{	
	WLight*pLight;
	WVector3 color(0);
	unsigned int lightNum=scene->getLightNum();
//	cout<<lightNum<<endl;
	for(unsigned int nthLight=0;nthLight<lightNum;nthLight++)
	{
		pLight=scene->getLightPointer(nthLight);
//		cout<<nthLight<<endl;
		color+=computeDirectLight(pLight,bsdf,lightSample,bsdfSample,ro, nodeInfo);
//		color.showCoords();
	}
//	color.showCoords();
	return color +bsdf->getEmission();
}
