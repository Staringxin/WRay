#pragma once
#include "stdafx.h"
#include "Vector2.h"
#include "Vector3.h"
class WDifferentialGeometry
{
public:
	Vector3 position;
	Vector3 normal,tangent,bitangent;
	Vector2 texCoord;
	Vector3 rayDir;
	Vector3 dpdu,dpdv;
	Vector3 dndu,dndv;
	unsigned int mtlId;

	WDifferentialGeometry(void);//ע��˺���û�н��г�ʼ��
	WDifferentialGeometry(const WDifferentialGeometry&DG);

	//�˺���ͨ����������Զ��������������
	WDifferentialGeometry(
		const Vector3&iposition,const Vector3&inormal,
		const Vector3&irayDir,const Vector2&itexCoord,
		const Vector3&idpdu,const Vector3&idpdv,
		const Vector3&idndu,const Vector2&idndv,
		unsigned int imtlId);
	
	WDifferentialGeometry(
		const Vector3&iposition,const Vector3&inormal,
		const Vector3&itangent,const Vector3&ibitangent,
		const Vector3&irayDir,const Vector2&itexCoord,
		const Vector3&idpdu,const Vector3&idpdv,
		const Vector3&idndu,const Vector2&idndv,
		unsigned int imtlId);
	virtual ~WDifferentialGeometry(void);

	void draw();		//��openGL�������㣬����ʱ��
};
