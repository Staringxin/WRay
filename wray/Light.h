#pragma once
#include "Vector3.h"
class Light
{
public:
	enum LightType{
		LIGHT_POINT=0,
		LIGHT_RECTANGLE=1,
		LIGHT_OBJECT=2,
        LIGHT_SIMPLE_SKY=3,
	};
	LightType type;

	Light(LightType itype,bool iisPoint):
	type(itype),isPoint(iisPoint){};
	virtual ~Light(void);
	//�Ƿ���Դ
	bool isPoint;

    virtual bool isEnvironmentLight() const { return false; }

	virtual void sampleLight(
		float u1,float u2,float u3,BSDF&bsdf,
		Vector3&iposition,Vector3&iintensity,float&PDF, MemoryPool& memoryPool)=0;
    virtual Vector3 getEnvironmentEmission() const { return Vector3(0); }
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
	void sampleLight(float u1,float u2,float u3, BSDF&bsdf,Vector3&iposition,Vector3&iintensity,float&PDF, MemoryPool& memoryPool);
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
	void sampleLight(float u1,float u2,float u3,BSDF&bsdf, Vector3&iposition,Vector3&iintensity,float&PDF, MemoryPool& memoryPool);
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
		Vector3&iposition, Vector3&iintensity, float&PDF, MemoryPool& memoryPool);
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

class SimpleSkyLight :public Light
{
public:
    SimpleSkyLight(const Vector3& iintensity, const WBoundingBox& sceneBox) :
        Light(LIGHT_SIMPLE_SKY, true), intensity(iintensity)
    {
        m_sceneCenter = sceneBox.center();
        m_sceneRadius = sceneBox.extent().length();
    }
    ~SimpleSkyLight() {}

    void setIntensity(Vector3 iintensity)
    {
        intensity = iintensity;
    }

    void sampleLight(float u1, float u2, float u3, BSDF&bsdf, Vector3&iposition, Vector3&iintensity, float&PDF, MemoryPool& memoryPool)
    {
        iintensity = intensity;
        Vector3 sample;
        RandomNumber::uniformSampleSphere(u1, u2, sample, PDF);
        iposition = bsdf.DG.position + m_sceneRadius * 2.f * sample;
    }

    virtual Vector3 getEnvironmentEmission() const { return intensity; }

    bool isEnvironmentLight() const { return true; }

    void draw() {}
    virtual void getProperties(vector<float>& properties)
    {
        properties.push_back(intensity.x);
        properties.push_back(intensity.y);
        properties.push_back(intensity.z);
    }
    virtual bool isDeltaLight() { return true; }
private:
    Vector3 intensity;
    Vector3 m_sceneCenter;
    float   m_sceneRadius;
};
