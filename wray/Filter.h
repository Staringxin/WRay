#pragma once
#include "Sampler.h"
#include "Sample.h"
#include "Vector3.h"
class WFilter
{
public:
	enum WFilterType{FILTER_BOX=0};
	WFilterType type;
	WFilter(unsigned int size=1,
		Sampler::SamplerType sampType=Sampler::SAMPLER_RANDOM,
		unsigned int iseed=65535,
		WFilterType filterType=WFilter::FILTER_BOX);
	virtual ~WFilter(void);
	//�ı�������ĸ���
	void changeSampleSize(unsigned int isize);
	void changeSampler(Sampler::SamplerType type=Sampler::SAMPLER_RANDOM);
	//�˺����������������ɫ�Ƿ��ȡ��ϣ�
	//����true��ʾ��ȡ���
	bool isFull();
	//�������������������ʹ��
	//��ÿ����һ��setSampleClr���������һ��getFinalClr

	//�˺���ͨ��������ɫֵ����һ�����ص�������ɫ
	virtual void getFinalClr(Vector3&final)=0;
	//�˺�������һ�����������ɫ
	void setSampleClr(Vector3 sampClr);
	//��sampler���¼���������,ͬʱ��������nthPoint��Ա��0
	//�˺���ֱ�ӵ���Sampler��computeSamples�ӿڣ�
	//computeSamples���ȸ���Sampler������ֵ��
	//�ټ������������
	void refresh();
	//��õ�ǰ�������λ��
	void getCurrSample(float &sampX,float&sampY);
protected:
	Sample2D samples;
	Sampler*sampler;
	Vector3*sampleColors;
	unsigned int seed;
};
//��򵥵Ĺ����������������ȡƽ��
class WBoxFilter:public WFilter
{
public:
	WBoxFilter(unsigned int size=1,
		Sampler::SamplerType sampType=Sampler::SAMPLER_RANDOM,
		unsigned int iseed=65535):
	WFilter(size,sampType,iseed,WFilter::FILTER_BOX){}
	void getFinalClr(Vector3&final);
};