#pragma once
#include "Vector3.h"
class Light
{
public:
	enum LightType{
		LIGHT_POINT=0,
		LIGHT_RECTANGLE=1,
		LIGHT_OBJECT=2
	};
	LightType type;

	Light(LightType itype,bool iisPoint):
	type(itype),isPoint(iisPoint){};
	virtual ~Light(void);
	//�Ƿ���Դ
	bool isPoint;

	virtual void sampleLight(
		float u1,float u2,float u3,BSDF&bsdf,
		Vector3&iposition,Vector3&iintensity,float&PDF)=0;
	virtual void draw()=0;
	//�����������,
	//�˺������ڰ����еƹ�����Դ����һ��float4������
	//����properties�Ĵ�Сһ��Ϊ4�ı���
	virtual void getProperties(vector<float>& properties){};
	//�����������Ĵ�С
	virtual void getPropertySize(int& nFloat4s){}
	virtual bool isDeltaLight() = 0;
};

class PointLight:public Light
{
public:
	PointLight(Vector3 iintensity,Vector3 iposition):
	  Light(LIGHT_POINT,true),intensity(iintensity),
	  position(iposition){}
	~PointLight(){}

	void setIntensity(Vector3 iintensity)
	{intensity=iintensity;}
	void setPosition(Vector3 iposition)
	{position=iposition;}
	void sampleLight(float u1,float u2,float u3, BSDF&bsdf,Vector3&iposition,Vector3&iintensity,float&PDF);
	void draw();
	//����������飬�����ƹ��ǿ�ȣ��ƹ��λ��
	//���Ե�˳���ǣ�ǿ�� r g b a �� λ�� x y z w
	//a wΪ 1.0f
	virtual void getProperties(vector<float>& properties);
	virtual bool isDeltaLight() { return true; }
private :
	Vector3 intensity;
	Vector3 position;
};

class RectangleLight:public Light
{
public:
	RectangleLight(Vector3 iposition,Vector3 idirection,
		Vector3 up,float width,float height,
		Vector3 iintensity,bool iisDoubleSide=true);
	void setIntensity(Vector3 iintensity)
	{intensity=iintensity;}
	void setPosition(Vector3 iposition)
	{position=iposition;}
	void sampleLight(float u1,float u2,float u3,BSDF&bsdf, Vector3&iposition,Vector3&iintensity,float&PDF);
	void draw();
	//����������飬�����ƹ��ǿ�ȣ��ƹ��λ��
	//���Ե�˳���ǣ�ǿ�� r g b a �� λ�� x y z w
	//a wΪ 1.0f
	virtual void getProperties(vector<float>& properties);
	virtual bool isDeltaLight() { return false; }
private:
	Vector3 position;
	Vector3 direction;
	Vector3 x,y;
	Vector3 intensity;
	float area;
	bool isDoubleSide;
};
//����ƹ�
//�������ƹ�Ĳ����ǣ�
//1������һ������ƹ����
//2����Ҫ�������ӵ��ƹ��������
//3�����õƹ����ĺ������޸�Ҫ�������Ĳ���

class ObjectLight:public Light
{
public:
	ObjectLight(Vector3 iintensity, int materialID, Scene*iscene,bool iisDoubleSide):
		Light(LIGHT_OBJECT,false), m_materialID(materialID), m_intensity(iintensity),m_scene(iscene),m_isDoubleSide(iisDoubleSide)
	{}

	void addTriangle(int objectID, int triangleID);

	void sampleLight(
		float u1, float u2, float u3, BSDF&bsdf,
		Vector3&iposition, Vector3&iintensity, float&PDF);
	void draw(){};
	void clear() { m_faceIDList.clear(); }
	virtual bool isDeltaLight() { return false; }
private:
	Scene*m_scene;
	struct FaceID
	{
		int m_objectID;
		int m_triangleID;
	};
    std::vector<FaceID> m_faceIDList; // object id -> face id

	Vector3 m_intensity;

	bool m_isDoubleSide;
	int m_materialID;
};
