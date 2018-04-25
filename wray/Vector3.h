#pragma once
#include "Vector2.h"

class Vector3
{
public:
	//�����ȿ���ͨ�� x y z ���ʣ�Ҳ�������������
	union
	{
		struct
		{
			float x,y,z;
		};
		float v[4];
	};
//	float x,y,z;
	//���캯��
	Vector3();
	Vector3(float i);
	Vector3(const Vector3&v);
	Vector3(const Vector2f&v);
	Vector3(float ix,float iy,float iz);

	//��������
	Vector3::~Vector3(void);
	//�ӷ�
	Vector3 operator+(const Vector3&i)const;
	Vector3 operator+=(const Vector3&i);

	//����
	Vector3 operator-(const Vector3&i)const;
	Vector3 operator-=(const Vector3&i);

	//����
	friend Vector3 operator*(const Vector3&i,float n);
	friend Vector3 operator*(float n,const Vector3&i);
	friend Vector3 operator*(const Vector3&i1,const Vector3&i2);

	Vector3 operator*=(float f);
	Vector3 operator*=(const Vector3&i);

	//����
	friend Vector3 operator/(const Vector3&i,float n);
	friend Vector3 operator/(float n,const Vector3&i);
	friend Vector3 operator/(const Vector3&i,const Vector3&n);

	Vector3 operator/=(float f);
	Vector3 operator=(const Vector3&i);

	bool operator==(const Vector3& i)const;

	//����
	float length()const;
	float lengthSquared()const;
	//��λ��
	Vector3 normalize();
	//���㷴������
	Vector3 reflect(const Vector3 normal)const;
	Vector3 sqrtElement()const;

	bool isZero() { return x == 0 && y == 0 && z == 0; }

	//���
	float dot(const Vector3&i)const;
	//���
	Vector3 cross(const Vector3&i)const;

	//���ֵ������ʱ��
	void showCoords()const;
};
