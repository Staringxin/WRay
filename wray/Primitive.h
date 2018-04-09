#pragma once
//#include "stdafx.h"
#include <gl/glut.h>
#include "Triangle.h"
#include "BoundingBox.h"
class MeshObject;
struct WSubPrimitive
{
	unsigned int beginIndex;
	unsigned int nFaces;
	WBoundingBox box;
	MeshObject*pPrimitive;//������Primitive��ָ��
	void draw(bool isFill);//����������������
};
class MeshObject
{
	//ObjReader��Primitive����Ԫ�࣬����ֱ�ӷ�������˽�г�Ա
	friend class WObjReader;
	friend class WScene;
public:
	MeshObject(void);
	virtual ~MeshObject(void);

	bool isSelected;			//��ʾ���Ƿ�ѡ��

	//���������
	bool getTriangle(unsigned int nthFace,WTriangle&tri);

	void drawBBox();			//��������Primitive�İ�Χ��
	bool isIntersectWBBox(WRay&ray)//�������Χ���Ƿ��ཻ
	{float tmin,tmax;return box.isIntersect(ray,tmin,tmax);}

	//��������Primitive,����ʱ��
	void drawPrimitive(bool showNormal=false,bool fillMode=false);
	void clear();				//����洢�ռ�
	void drawSubPBBox();		//����SubPrimitive�İ�Χ��
	
	//�ؽ�SubPrimitive������ÿ��SubPrimitive������
	void rebuildSubP(int inFacesPerSubP);
	//���ÿ��SubPrimitive��ָ�룬�Լ�Ԫ�ظ���
	void getSubPrimitives(WSubPrimitive*&isubPs,unsigned int&nSubP);
	//����ܵ�subPrimitive�ĸ���
	void getSubPrimNum(unsigned int&insubP)
	{insubP=nSubPs;}
	void getFaceNum(unsigned int &inFace)
	{inFace=nFaces;}
	//��ð�Χ��
	WBoundingBox getBBox(){return this->box;}
private:
	unsigned int nVertices;		//�ܵĶ�����=���������С/3
	unsigned int nTexcoords;	//�ܵ���ͼ������=��ͼ���������С/2
	unsigned int nNormals;		//�ܵķ�������=�����������С/3
	unsigned int nFaces;		//�ܵ�����=������/3
	unsigned int nSubPs;		//����ͼԪ(SubPrimitive)��
	unsigned int nFacesPerSubP;	//ÿ����ͼԪ������

	float*vertices;				//��������
	float*normals;				//����������
	float*texcoords;			//��ͼ��������

	unsigned int*vertexIndices;	//������������
	unsigned int*normalIndices;	//��������������
	unsigned int*texCoordIndices;//��ͼ������������
	unsigned int*mtlIndices;	//������������

	WSubPrimitive *subPs;
	WBoundingBox box;

	void buildBBox();			//��������Primitive�İ�Χ��
	void buildSubP();			//������ͼԪ���˺�����ObjReader�౻����
};
