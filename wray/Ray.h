#pragma once
#include "Vector3.h"
#include "MathValues.h"
#define RAY_EPSILON 1e-4f
class Ray
{
public:
	Vector3 point;
	Vector3 direction;
	float tMin,tMax;			//tΪ��Ч������Χ
	Ray(void);
	//ע��iDirection������ֱ�Ӵ�������������
	Ray(const Vector3&iDirection);
	Ray(const Vector3&iPoint,const Vector3&iDirection);
	Ray(const Vector3&iPoint,const Vector3&iDirection,float iTMin,float iTMax);
	virtual ~Ray(void);
	void normalizeDir();	//�ѷ���������λ��
	void inverseDir();		//��ת���߷���
	void reflect(Vector3 normal);//���㾵�淴��Ĺ��߷���
	void draw();			//��openGL�������ߣ�����ʱ��
	void drawSegment();		//��openGL�������߲�����Χ�ڵ�һ�Σ�����ʱ��
};
