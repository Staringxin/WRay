#pragma once

#include "ObjReader.h"
#include "Primitive.h"
#include "Material.h"
#include "Light.h"
class Scene
{
public:
	Scene(void);
	~Scene(void);

	//�˺�����ObjReader�õ������ݵ���Scene��֮��
	void buildScene(ObjReader&reader);
	//�������
	void clearScene();
	//�˺���������������
	void drawScene(bool showNormal=false,bool fillMode=false);
	//�˺���������������������TriangleArray��Ϊ����
	//���Դ˼��TriangleArray�Ƿ��д�
	void drawByTriangleArray(bool showNormal=false,bool fillMode=false,Vector3 color=Vector3(0,0,0));
	//�˺����ؽ����������subPrimitive,������ʾÿ��SubPrimitive�������������,���ô˺���������ؽ�KD��
	void rebuildAllSubPs(unsigned int inFacesPerSubP);
	//��û������ָ�룬�Լ�������ĸ���
	void getObjects(MeshObject*&iprimitives,unsigned int&nPrims);
	void getObject(MeshObject*&iprimitives,unsigned int nthPrim);
	//��ò���ָ�������ָ�룬�Լ����ʵĸ���
	void getMaterials(Material**&imaterials,unsigned int&nMtl);
	//��õ�nthMtl������
	void getNthMaterial(Material*&imaterial,unsigned int nthMtl);
	//���õ�nthMtl������
	void setNthMaterial(Material*imaterial,unsigned int nthMtl);
	//���ػ����������
	unsigned int getPrimNum(){return m_objects.size();}
	//�����ӻ����������
	unsigned int getSubPrimNum(){return nSubPrimitives;}
	//���س����İ�Χ��
	WBoundingBox getBBox(){return sceneBox;}
	//�������������İ�Χ��,��ɫΪ����ɫ
	void drawSceneBBox();
	//��������ӵƹ�
	void addLight(Light *light);
	Light* getLightPointer(unsigned int nthLight);
	unsigned int getLightNum();
	//��ʾ��ѡ����������
	void clearSelect();
	void getTriangleArray(WTriangle*&itriangles,unsigned int&nTris);
	void getTriAccelArray(float*& array, int& nElements);

	//���º����Ѳ��ʺ͵ƹ����ݷ��ص���Ӧ����������
	//�������������GPUʹ��
	//1�����ʷ��棬����һ�����������һ����������
	//   ��������洢���ʵ����ͺ���ʼλ�ã���������������ĵڼ������أ�
	//   ��������洢���ֲ��ʵ����ԣ����������ɫ
	//2���ƹⷽ�棬����һ�����������һ����������
	//   ��������洢�ƹ�����ͺ���ʼλ�ã���������������ĵڼ������أ�
	//   ��������洢���ֵƹ�����ԣ�����ǿ�ȣ�λ��
	void getMaterialArrayFloat4Uint2(
		unsigned int*& mtlIDs,  unsigned int& nIDPixels,
		float*& mtlData,		unsigned int& nDataPixels);
	//   ��������洢���ֵƹ�����ԣ�����ǿ�ȣ�λ��
	void getLightArrayFloat4Uint2(
		unsigned int*& lightIDs,  unsigned int& nIDPixels,
		float*& lightData,		unsigned int& nDataPixels);

	//for testing
	void showTriangles(){
		for(int i = 0; i < this->nTriangles; i ++){
			printf("tri%d p1: %f, %f, %f;\n", i, this->triangles[i].point1.x, this->triangles[i].point1.y, this->triangles[i].point1.z);
			printf("tri%d p2: %f, %f, %f;\n", i, this->triangles[i].point2.x, this->triangles[i].point2.y, this->triangles[i].point2.z);
			printf("tri%d p3: %f, %f, %f;\n", i, this->triangles[i].point3.x, this->triangles[i].point3.y, this->triangles[i].point3.z);
		}
	}

	void showMaterials(){
		for(int i = 0; i < this->nMaterials; i ++){
			Vector3 tmp = this->materials[i]->getColor();
			printf("mtl%d: %f, %f, %f;\n", i, tmp.x, tmp.y, tmp.z);
		}
	}

    void setEnvironmentLight(Light* light);
    Light* getEnvironmentLight() { return m_envLight; }

private:
	vector<Light*>m_lights;					//�洢�ƹ�ָ��
    Light* m_envLight;                      // Environment Light
	Material**materials;					//Materialָ������
	unsigned int nMaterials;				//Material����
	std::vector<MeshObject>m_objects;		//Primitive����
	unsigned int nSubPrimitives;			//SubPrimitive����
	WBoundingBox sceneBox;					//���������İ�Χ��

	WTriangle*triangles;					//����������
	unsigned int nTriangles;				//�����θ���
	void buildTriangleArray();				//��������������
	void clearTriangleArray();				//�������������

	void buildSceneBBox();					//����������Χ��
	void buildLightData();					// Build light data
};
