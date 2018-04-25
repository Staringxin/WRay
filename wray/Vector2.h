#pragma once

class Vector2f
{
public:
	float x;
	float y;
	//���캯��	
	Vector2f();
	Vector2f(float i);
	Vector2f(const Vector2f&v);
	Vector2f(float ix,float iy);

	//��������
	Vector2f::~Vector2f(void){}
	//�ӷ�
	Vector2f operator+(const Vector2f&i2);
	Vector2f operator+=(const Vector2f&i2);

	//����
	Vector2f operator-(const Vector2f&i2);
	Vector2f operator-=(const Vector2f&i2);

	//����
	friend Vector2f operator*(const Vector2f&i1,float n);
	friend Vector2f operator*(float n,const Vector2f&i1);

	Vector2f operator*=(float f);

	//����
	friend Vector2f operator/(const Vector2f&i1,float n);
	friend Vector2f operator/(float n,const Vector2f&i1);

	Vector2f operator/=(float f);

	//����
	float length();
	float lengthSquared();

	//��λ��
	Vector2f normalize();

	//���
	float dot(const Vector2f&i);

	//���ֵ������ʱ��
	void showCoords();
};

class Vector2i
{
public:
	int m_x;
	int m_y;
	Vector2i() {}
	Vector2i(int x, int y) :m_x(x), m_y(y) {}
};