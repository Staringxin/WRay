#pragma once
#include "Vector3.h"

class Vector4
{
public:
	float x,y,z,h;
	//���캯��
	Vector4();//xyz����ʼ��Ϊ0.0f��hΪ1.0f
	Vector4(float i);
	Vector4(const Vector4&v);
	Vector4(const Vector3&v);//h����ʼ��Ϊ1.0f
	Vector4(const Vector2&v);//h����ʼ��Ϊ1.0f
	Vector4(float ix,float iy,float iz,float ih);

	//��������
	Vector4::~Vector4(void);
	//�ӷ�
	Vector4 operator+(const Vector4&i);
	Vector4 operator+=(const Vector4&i);

	//����
	Vector4 operator-(const Vector4&i);
	Vector4 operator-=(const Vector4&i);

	//����
	friend Vector4 operator*(const Vector4&i,float n);
	friend Vector4 operator*(float n,const Vector4&i);

	Vector4 operator*=(float f);

	//����
	friend Vector4 operator/(const Vector4&i,float n);
	friend Vector4 operator/(float n,const Vector4&i);

	Vector4 operator/=(float f);

	//����
	float length();
	float lengthSquared();
	//��λ��
	Vector4 normalize();

	//���
	float dot(const Vector4&i);
};
