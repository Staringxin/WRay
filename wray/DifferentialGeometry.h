#pragma once
#include "stdafx.h"
#include "Vector2.h"
#include "Vector3.h"
class DifferentialGeometry
{
public:
	Vector3 position;
	Vector3 normal,tangent,bitangent;
	Vector2f texCoord;
	Vector3 rayDir;
	Vector3 dpdu,dpdv;
	Vector3 dndu,dndv;
	unsigned int mtlId;

	DifferentialGeometry(void);//ע��˺���û�н��г�ʼ��
	DifferentialGeometry(const DifferentialGeometry&DG);

	//�˺���ͨ����������Զ��������������
	DifferentialGeometry(
		const Vector3&iposition,const Vector3&inormal,
		const Vector3&irayDir,const Vector2f&itexCoord,
		const Vector3&idpdu,const Vector3&idpdv,
		const Vector3&idndu,const Vector2f&idndv,
		unsigned int imtlId);
	
	DifferentialGeometry(
		const Vector3&iposition,const Vector3&inormal,
		const Vector3&itangent,const Vector3&ibitangent,
		const Vector3&irayDir,const Vector2f&itexCoord,
		const Vector3&idpdu,const Vector3&idpdv,
		const Vector3&idndu,const Vector2f&idndv,
		unsigned int imtlId);
	virtual ~DifferentialGeometry(void);

	void draw();		//��openGL�������㣬����ʱ��
};
