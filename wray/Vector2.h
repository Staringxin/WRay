#pragma once

class Vector2
{
public:
	float x;
	float y;
	//���캯��	
	Vector2();
	Vector2(float i);
	Vector2(const Vector2&v);
	Vector2(float ix,float iy);

	//��������
	Vector2::~Vector2(void){}
	//�ӷ�
	Vector2 operator+(const Vector2&i2);
	Vector2 operator+=(const Vector2&i2);

	//����
	Vector2 operator-(const Vector2&i2);
	Vector2 operator-=(const Vector2&i2);

	//����
	friend Vector2 operator*(const Vector2&i1,float n);
	friend Vector2 operator*(float n,const Vector2&i1);

	Vector2 operator*=(float f);

	//����
	friend Vector2 operator/(const Vector2&i1,float n);
	friend Vector2 operator/(float n,const Vector2&i1);

	Vector2 operator/=(float f);

	//����
	float length();
	float lengthSquared();

	//��λ��
	Vector2 normalize();

	//���
	float dot(const Vector2&i);

	//���ֵ������ʱ��
	void showCoords();
};
