#pragma once
#include "Vector3.h"
class WLight
{
public:
	enum LightType{
		LIGHT_POINT=0,
		LIGHT_RECTANGLE=1,
		LIGHT_OBJECT=2
	};
	LightType type;

	WLight(LightType itype,bool iisPoint):
	type(itype),isPoint(iisPoint){};
	virtual ~WLight(void);
	//是否点光源
	bool isPoint;

	virtual void sampleLight(
		float u1,float u2,float u3,WBSDF&bsdf,
		WVector3&iposition,WVector3&iintensity,float&PDF)=0;
	virtual void draw()=0;
	//获得属性数组,
	//此函数用于把所有灯光的属性打包到一个float4纹理中
	//所以properties的大小一定为4的倍数
	virtual void getProperties(vector<float>& properties){};
	//获得属性数组的大小
	virtual void getPropertySize(int& nFloat4s){}
	virtual bool isDeltaLight() = 0;
};

class WPointLight:public WLight
{
public:
	WPointLight(WVector3 iintensity,WVector3 iposition):
	  WLight(LIGHT_POINT,true),intensity(iintensity),
	  position(iposition){}
	~WPointLight(){}

	void setIntensity(WVector3 iintensity)
	{intensity=iintensity;}
	void setPosition(WVector3 iposition)
	{position=iposition;}
	void sampleLight(float u1,float u2,float u3, WBSDF&bsdf,WVector3&iposition,WVector3&iintensity,float&PDF);
	void draw();
	//获得属性数组，包括灯光的强度，灯光的位置
	//属性的顺序是：强度 r g b a ， 位置 x y z w
	//a w为 1.0f
	virtual void getProperties(vector<float>& properties);
	virtual bool isDeltaLight() { return true; }
private :
	WVector3 intensity;
	WVector3 position;
};

class WRectangleLight:public WLight
{
public:
	WRectangleLight(WVector3 iposition,WVector3 idirection,
		WVector3 up,float width,float height,
		WVector3 iintensity,bool iisDoubleSide=true);
	void setIntensity(WVector3 iintensity)
	{intensity=iintensity;}
	void setPosition(WVector3 iposition)
	{position=iposition;}
	void sampleLight(float u1,float u2,float u3,WBSDF&bsdf, WVector3&iposition,WVector3&iintensity,float&PDF);
	void draw();
	//获得属性数组，包括灯光的强度，灯光的位置
	//属性的顺序是：强度 r g b a ， 位置 x y z w
	//a w为 1.0f
	virtual void getProperties(vector<float>& properties);
	virtual bool isDeltaLight() { return false; }
private:
	WVector3 position;
	WVector3 direction;
	WVector3 x,y;
	WVector3 intensity;
	float area;
	bool isDoubleSide;
};
//物体灯光
//添加物体灯光的步骤是：
//1）创建一个物体灯光对象
//2）把要发光的面加到灯光对象里面
//3）调用灯光对象的函数，修改要发光的面的材质

class ObjectLight:public WLight
{
public:
	ObjectLight(WVector3 iintensity, int materialID, WScene*iscene,bool iisDoubleSide):
		WLight(LIGHT_OBJECT,false), m_materialID(materialID), m_intensity(iintensity),m_scene(iscene),m_isDoubleSide(iisDoubleSide)
	{}

	void addTriangle(int objectID, int triangleID);

	void sampleLight(
		float u1, float u2, float u3, WBSDF&bsdf,
		WVector3&iposition, WVector3&iintensity, float&PDF);
	void draw(){};
	void clear() { m_faceIDList.clear(); }
	virtual bool isDeltaLight() { return false; }
private:
	WScene*m_scene;
	struct FaceID
	{
		int m_objectID;
		int m_triangleID;
	};
    std::vector<FaceID> m_faceIDList; // object id -> face id

	WVector3 m_intensity;

	bool m_isDoubleSide;
	int m_materialID;
};
