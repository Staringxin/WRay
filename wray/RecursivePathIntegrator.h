#pragma once

class WRecursivePathIntegrator:public WSurfaceIntegrator
{
public:
	WRecursivePathIntegrator(Scene*scene,WAccelerator*tree,
		unsigned int ipathDepth=1,unsigned int inBranch=2,
		Sampler::SamplerType samplerType=Sampler::SAMPLER_RANDOM,float imultiplier=1);
	virtual ~WRecursivePathIntegrator(void);

	//����׷�ٺ���
	Vector3 integrate(Ray&ray);
	void setPathMaxDepth(unsigned int idepth);
	void displayTime();
private:
	WClock timer;
	Sampler*sampler;
	Sample2D BSDFSamples;
	Sample3D lightSamples;
	//����ֱ�ӹ��յĶ���
	WDirectLighting Dlighting;
	//�������
	unsigned int maxDepth;
	//��֧��
	unsigned int nBranchs;
	float multiplier;
	void clearSamples();
	void allocateSamples();
	void computeSamples();
	//����׷�ٵݹ麯��
	Vector3 integrateCore(Ray ray,unsigned int depth,int beginNode = 0);
};
