#include "StdAfx.h"
#include "IrradianceCacheIntegrator.h"

WIrradianceCacheIntegrator::WIrradianceCacheIntegrator(
	Scene*iscene,WAccelerator*itree,
	unsigned int ioctTreeMaxDepth,
	unsigned int inRaysForLambert,
	unsigned int inRaysForSpecular,
	unsigned int iminSamples,
	unsigned int inDirectLightSamples,
	unsigned int imaxTracingDepth,
	Sampler::SamplerType samplerType,
	float imaxNormalError,
	float imaxPlanarError,
	float idistanceErrorFactor,
	float ismoothFactor):

	SurfaceIntegrator(iscene,itree),
	interpolator(
	imaxNormalError,imaxPlanarError,ismoothFactor,iminSamples),
	octTree(iscene,ioctTreeMaxDepth),
	pathIntegrator(iscene,itree,1,samplerType,1.0),
	Dlighting(iscene,itree),
	nRaysForLambert(inRaysForLambert),
	nRaysForSpecular(inRaysForSpecular),
	nDirectLightSamples(inDirectLightSamples),
	maxTracingDepth(imaxTracingDepth),
	LambertBSDFSamples(1),SpecularBSDFSamples(1),lightSamples(1),
	distanceErrorFactor(idistanceErrorFactor)
{
	if(samplerType==Sampler::SAMPLER_RANDOM)
		sampler=new RandomSampler;
	else if(samplerType==Sampler::SAMPLER_STRATIFIED)
		sampler=new StratifiedSampler;

	LambertBSDFSamples.setSize(sqrt(float(nRaysForLambert))+1);
	LambertBSDFSamples.allocateSpace();
	SpecularBSDFSamples.setSize(sqrt(float(nRaysForSpecular))+1);
	SpecularBSDFSamples.allocateSpace();

	unsigned int nLightSamples=
		scene->getLightNum()*nDirectLightSamples;
	lightSamples.setSize(sqrt(float(nLightSamples))+1);
	lightSamples.allocateSpace();
}

WIrradianceCacheIntegrator::~WIrradianceCacheIntegrator(void)
{
	octTree.clear();
	LambertBSDFSamples.clear();
	SpecularBSDFSamples.clear();
	lightSamples.clear();
}
void WIrradianceCacheIntegrator::setInterpolatorParams(
	float imaxNormalError,
	float imaxPlanarError ,
	float ismoothFactor,
	unsigned int iminSamples)
{
	interpolator.setInterpolateParams(
		imaxNormalError,
		imaxPlanarError,
		iminSamples,
		ismoothFactor);
}
void WIrradianceCacheIntegrator::setSampleParams(
	unsigned int inRaysForLambert,
	unsigned int inRaysForSpecular,
	unsigned int inDirectLightSamples,
	float idistanceErrorFactor)
{
	nRaysForLambert=inRaysForLambert;
	nRaysForSpecular=inRaysForSpecular;
	nDirectLightSamples=inDirectLightSamples;

	LambertBSDFSamples.setSize(sqrt(float(nRaysForLambert))+1);
	LambertBSDFSamples.allocateSpace();
	SpecularBSDFSamples.setSize(sqrt(float(nRaysForSpecular))+1);
	SpecularBSDFSamples.allocateSpace();
	distanceErrorFactor=idistanceErrorFactor;

	unsigned int nLightSamples=
		scene->getLightNum()*nDirectLightSamples;
	lightSamples.setSize(sqrt(float(nLightSamples))+1);
	lightSamples.allocateSpace();
}
void WIrradianceCacheIntegrator::displaySamplePoints(bool isDisplayBox)
{
	octTree.displaySamples(isDisplayBox);
}
void WIrradianceCacheIntegrator::clearTree()
{
	octTree.clear();
}

Vector3 WIrradianceCacheIntegrator::integrate(Ray&ray)
{
	DifferentialGeometry DG;
	BSDF*bsdf=NULL;
	Vector3 ro,directLight=Vector3(0),indirectLight=Vector3(0);
	int beginNode = 0, endNode;
	if(tree->intersect(ray,DG,&endNode,beginNode))
	{
		//�ɲ�������BSDF
		Material*mtl;
		scene->getNthMaterial(mtl,DG.mtlId);
		mtl->buildBSDF(DG,bsdf, m_rng, m_memoryPool);
		sampler->computeSamples(LambertBSDFSamples);
		sampler->computeSamples(lightSamples);

		//����ֱ�ӹ���
		ro=-1*ray.direction;
		for(unsigned int i=0;i<nDirectLightSamples;i++)
		{
			directLight=Dlighting.sampleAllLights(
				bsdf,lightSamples,LambertBSDFSamples,ro);
		}
		directLight/=nDirectLightSamples;

//		directLight.showCoords();
		//���ھ��淴������䣬ֱ����·�����ٰ취
		if(!(bsdf->type==BSDF::BSDF_LAMBERT||
			bsdf->type==BSDF::BSDF_PHONG))
		{
//				return Vector3(0);
			float bsdfU,bsdfV;
			unsigned int nIntersectedRays=0;
			Ray ray;				//��Ҫ�ٴη���Ĺ���
			Vector3 ri,ro,L;
			float PDF;
			sampler->computeSamples(SpecularBSDFSamples);

			//������ȫ���淴�䣬����·�����ٰ취
			for(unsigned int i=0;i<nRaysForSpecular;i++)
			{
				SpecularBSDFSamples.get2D(bsdfU,bsdfV);
				ro=bsdf->DG.rayDir;

				//����һ������
				bsdf->sampleRay(bsdfU,bsdfV,ri,ro,PDF);
				ray.point=bsdf->DG.position;
				ray.direction=ri;
				ray.tMax=M_INF_BIG;
				ray.tMin=1e-5f;
				//���䷽���뵱ǰ���߼н����Ҿ���ֵ
//				cosThetaAbs=abs(bsdf->DG.normal.dot(ri));

				//��ÿһ�����߽���·�����٣�ֱ������
				//���������Ϊֹ
				Vector3 pathDirectLight;
				Vector3 pathThroughPut=
					bsdf->evaluateFCos(ri,ro)/PDF;
//				WBSDF*pathBSDF;
				DifferentialGeometry pathDG;
//				Material*pathMtl;
//				float pathBSDFU,pathBSDFV;
				bool isGetLambert=false;
				unsigned int depth=maxTracingDepth;

				
// 				while(depth--)
// 				{
// 					//��֮ǰ����Ĺ����볡���󽻣�
// 					//�������Ϊ���������
// 					//ֹͣ·������
// 					if(tree->intersect(ray,pathDG))
// 					{
// 						//�ҵ�����Ĳ��ʺ�BSDF
// 						scene->getNthMaterial(pathMtl,pathDG.mtlId);
// 						pathMtl->buildBSDF(pathDG,pathBSDF);
// 						ro=-1*ray.direction;
// 
// 						//�������Ĺ����������������
// 						//ֹͣ·�����٣���irradianceCach
// 						//��������ֱ�ӹ���
// 						if(pathBSDF->type==WBSDF::BSDF_LAMBERT||
// 							pathBSDF->type==WBSDF::BSDF_PHONG)
// 						{
// 							isGetLambert=true;
// 							break;
// 						}
// 
// 						pathDirectLight=Dlighting.sampleAllLights(
// 							pathBSDF,lightSamples,LambertBSDFSamples,ro);
// 						indirectLight+=pathThroughPut*pathDirectLight;
// 						
// 						pathBSDFU=WMonteCarlo::randomFloat();
// 						pathBSDFV=WMonteCarlo::randomFloat();
// 						pathBSDF->sampleRay(pathBSDFU,pathBSDFV,ri,ro,PDF);
// 						ray.point=pathDG.position;
// 						ray.direction=ri;
// 						ray.tMax=M_INF_BIG;
// 
// 						pathThroughPut*=pathBSDF->evaluateFCos(ri,ro)/PDF;
// 						delete pathBSDF;
// 					}
// 					else 
// 					{
// 						break;
// 					}
// 				}
// 
// 
// 				
// 
// 				if(isGetLambert)
// 				{
// 					//�������Ϊ������������������ֹͣ���ٵ�
// 					//��irradianceCach����������������淢���Ĺ���
// 					pathDirectLight=computeIndirectLight(pathBSDF);
// 					indirectLight+=pathThroughPut*pathDirectLight;
// 				}
 				pathTracing(ray,indirectLight,pathThroughPut);

			}
//			directLight.showCoords();
			indirectLight/=nRaysForSpecular;
//			indirectLight.showCoords();
//			cout<<"a"<<endl;
			return directLight+indirectLight;
		}
		//���������䣬��irradianceCache�취���
		else
		{
//			return Vector3(0);
			indirectLight+=computeIndirectLight(bsdf);
			//delete bsdf;
			mtl->freeBSDF(bsdf, m_memoryPool);
			return directLight+indirectLight;
		}
	}
	//delete bsdf;
	Material::freeBSDF(bsdf, m_memoryPool);
	return Vector3(0);
}


void WIrradianceCacheIntegrator::pathTracing(
	Ray&ray, 
	Vector3&indirectLight,Vector3&pathThroughPut)
{
	Vector3 pathDirectLight;
	Vector3 ri,ro;
	BSDF*pathBSDF;
	DifferentialGeometry pathDG;
	Material*pathMtl;
	float pathBSDFU,pathBSDFV;
	unsigned int depth=maxTracingDepth;
	float PDF;
	bool isGetLambert=false;
	int beginNode = 0, endNode = 0;
	while(depth--)
	{
		//��֮ǰ����Ĺ����볡���󽻣�
		//�������Ϊ���������
		//ֹͣ·������
		if(tree->intersect(ray,pathDG, &endNode, beginNode))
		{
			//�ҵ�����Ĳ��ʺ�BSDF
			scene->getNthMaterial(pathMtl,pathDG.mtlId);
			pathMtl->buildBSDF(pathDG,pathBSDF, m_rng, m_memoryPool);
			ro=-1*ray.direction;

			//�������Ĺ����������������
			//ֹͣ·�����٣���irradianceCach
			//��������ֱ�ӹ���
			if(pathBSDF->type==BSDF::BSDF_LAMBERT||
				pathBSDF->type==BSDF::BSDF_PHONG)
			{
				isGetLambert=true;
//				delete pathBSDF;
				break;
			}

			pathDirectLight=Dlighting.sampleAllLights(
				pathBSDF,lightSamples,LambertBSDFSamples,ro);
			indirectLight+=pathThroughPut*pathDirectLight;

			pathBSDFU= m_rng.randomFloat();
			pathBSDFV= m_rng.randomFloat();
			pathBSDF->sampleRay(pathBSDFU,pathBSDFV,ri,ro,PDF);
			ray.point=pathDG.position;
			ray.direction=ri;
			ray.tMax=M_INF_BIG;

			pathThroughPut*=pathBSDF->evaluateFCos(ri,ro)/PDF;
			//delete pathBSDF;
			pathMtl->freeBSDF(pathBSDF, m_memoryPool);
			beginNode = endNode;
		}
		else 
		{

			break;
		}
	}
	if(isGetLambert)
	{
		//�������Ϊ������������������ֹͣ���ٵ�
		//��irradianceCach����������������淢���Ĺ���
		pathDirectLight=computeIndirectLight(pathBSDF);
		indirectLight+=pathThroughPut*pathDirectLight;
		delete pathBSDF;
	}
}

bool WIrradianceCacheIntegrator::interpolate(
	Vector3&point,
	Vector3&normal,
	Vector3&E)
{
	//��ʼ���ṹ
	interpolator.refresh(point,normal);
	//�ҵ����ڵĲ����㣬�����Խ��в�ֵ��ʵ�����������Ȩ��
	octTree.process(interpolator);
	//����ֵ�Ƿ�ɹ�����ɹ�������true���������Ȩƽ��
	//���򷵻�false
	return interpolator.finalInterpolate(E);
}
Vector3 WIrradianceCacheIntegrator::computeNewSamples(BSDF*bsdf)
{
	float bsdfU,bsdfV;
	float rayTotalLength=0;
//	float rayMinLength=M_INF_BIG;
	float cosThetaAbs;
	unsigned int nIntersectedRays=0;
	sampler->computeSamples(LambertBSDFSamples);
	Vector3 L(0);
	Vector3 Li;
	DifferentialGeometry DG;
	Ray ray;
	Vector3 ri,ro;
	ro=bsdf->DG.rayDir;
	Vector3 directLight;
	float PDF;

	//����nSecondaryRays������
	for(unsigned int i=0;i<nRaysForLambert;i++)
	{
		LambertBSDFSamples.get2D(bsdfU,bsdfV);


		//�Թ��߲���
		bsdf->sampleRay(bsdfU,bsdfV,ri,ro,PDF);
		ray.point=bsdf->DG.position;
		ray.direction=ri;
		ray.tMax=M_INF_BIG;
		ray.tMin=1e-5f;
		//���䷽���뵱ǰ���߼н����Ҿ���ֵ
		cosThetaAbs=max(bsdf->DG.normal.dot(ri),1e-3f);
//		cout<<PDF<<endl;

		//ֻ����ֱ�ӹ���
		pathIntegrator.setPathMaxDepth(3);
		L+=pathIntegrator.integrate(ray)*cosThetaAbs/max(1e-2f, PDF);
		if(ray.tMax!=M_INF_BIG)
		{
			nIntersectedRays++;
			rayTotalLength+=ray.tMax;
// 			if(ray.tMax<rayMinLength)
// 				rayMinLength=ray.tMax;
		}
	}
//	L.showCoords();
	Vector3 E;
	if(nIntersectedRays>0)
	{
		E=L/(float)nIntersectedRays;
	}
	else
		E=Vector3(0);
//	cout<<nIntersectedRays<<endl;
//	E.showCoords();
	WIrradianceSample irSample;
	irSample.E=E;
	irSample.maxDist=
		rayTotalLength/nIntersectedRays*
		distanceErrorFactor;//*E.lengthSquared();
//	cout<<10000000000000000<<irSample.maxDist<<endl;
//	cout<<nIntersectedRays;
	irSample.normal=bsdf->DG.normal;
	irSample.point=bsdf->DG.position;
	octTree.addSample(irSample);
//	E.showCoords();
	return E;
}

Vector3 WIrradianceCacheIntegrator::computeIndirectLight(BSDF *bsdf)
{
	Vector3 E;
	if(!interpolate(bsdf->DG.position,bsdf->DG.normal,E))
	{
//		cout<<"directcompute"<<endl;
		//�ڼ�������л�ʹ��pathIntegrator
		//������������Ϊ1��ֻ������������������ֱ�ӹ���
		pathIntegrator.setPathMaxDepth(5);
		E=computeNewSamples(bsdf);
	}
	else
	{
//		cout<<"interpolate"<<endl;
	}
	return E*bsdf->rho(bsdf->DG.rayDir)/**M_PI*/;
}
void WIrradianceCacheIntegrator::displayTreeNodes()
{
	octTree.displayNodes();
}