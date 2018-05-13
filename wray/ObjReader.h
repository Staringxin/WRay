#pragma once
#include "stdafx.h"
#include <atlstr.h>
#include "Primitive.h"
//#include "DisplaySystem.h"
#define OBJ_READ_CHECK_COUNTER 300000
#ifndef FILEREADINGPROCESSMSG
#define FILEREADINGPROCESSMSG WM_USER+1
#endif
struct float3
{
	float x,y,z;
};
struct float2
{
	float u,v;
};
struct ObjTriangle
{
	unsigned int vertIndex[3];			//vertex index--->��������Ƭ�������������Ӧ������ֵ
	unsigned int texcoordIndex[3];		//texture coordinate index--->��������Ƭ������������Ĳ��������Ӧ������ֵ
	unsigned int norIndex[3];			//normal index--->����ͬ�ϡ���
	unsigned int mtlIndex;				//material index
};
struct ObjPrimitive   //--->����һ������
{
	string name;				//primitive name---->���������
	vector<float3>vertices;	//vertex coordinate---->���嶥��ļ���
	vector<float2>texcoords;	//texture coordinate---->�����������ļ���
	vector<float3>normals;		//normal coordinate---->���嶥�㷨�����ļ���
	vector<ObjTriangle>faces;	//faces---->��������Ƭ��ļ���
};
struct ObjMaterial
{
	string name;				//name of material
	float3 diffuse;			//��������ɫ
	float3 specular;			// specular color
	float3 emission;			//�Է�����ɫ
	float3 transparency;		//͸����
	float   glossiness;			//�߹�ǿ��
	bool   isTransparent(){return transparency.x < 1 || transparency.y < 1 || transparency.z < 1;}
	bool   isShiny(){return glossiness > 0;}
};

class ObjReader
{
public:
	static enum coordType{VERTCOORDS,TEXCOORDS,NORCOORDS};
	static enum indexType{VERTINDICES,TEXINDICES,NORINDICES};
	ObjReader();
	~ObjReader();
	vector<ObjPrimitive>primitives;  //---->����������ļ���
	vector<ObjMaterial>Materials;   //---->�����Ӧ���ʵļ��ϣ������ƺ�ֻ�Ǽ򵥵����������
	//��ȡobj�Լ�������mtl�ļ�
	//obj��mtl�ļ�������ͬһĿ¼
	void readFile(const std::string& fileName);
	//ֻ��ȡobj�ļ�
	void readObjFile(const char*fileName);
	//ֻ��ȡmtl�ļ�
	void readMtlFile(const char*fileName);
	//return vertex buffer
	bool getWireArray(int primitive,WGLWireArray*vB);
	void showObj();
	void showMtl();
	void clear();
	void ObjReader::fillPrimitive(unsigned int nthPrimitive,MeshObject&pri);
	int getReadingProcess(){return readingProcess;}
private:
	//��vector���͵���������ת����float��������
	//pArrayΪ����ָ�룬nFloatsΪ����Ԫ�ظ���
	//type��3��ȡֵ��OBJ_VERTICES,OBJ_TEXCOORDS,OBJ_NORMALS
	//��ʾ���ض�Ӧ���͵�����
	//nthPrimitiveָ��������һ�����������
	void getCoordArray(float*&pArray,unsigned int&nFloats,
		unsigned nthPrimitive,coordType type);
	//��vector���͵���������ת����unsigned int��������
	//������getCoordArray���ƣ�nInt����������Ϊ����*3
	//ע������ֵ��3�����Ƕ�Ӧ���㣬������������±�
	//ע������ֵ��2�����Ƕ�Ӧ��ͼ����������±�
	void getIndexArray(unsigned int*&pArray,unsigned int &nInts,
		unsigned nthPrimitive,indexType type);
	ifstream file;
	float readingProcess;
};
