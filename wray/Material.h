#pragma once
#include "BSDF.h"
class WMaterial
{
public:
	enum MaterialType{
		MATERIAL_LAMBERT=0,
		MATERIAL_PHONG=1,
		MATERIAL_PERFECTREFLECTION=2,
		MATERIAL_PERFECTREFRACTION=3,
		MATERIAL_METAL=4,
		MATERIAL_DIELECTRIC=5
	};


	WMaterial(
		MaterialType itype,string iName,unsigned int iID,
		Vector3 icolor=Vector3(1),Vector3 iLight=Vector3(0)):
	type(itype),name(iName),ID(iID),
	color(icolor),light(iLight){}

	virtual ~WMaterial(void);
	//由材质创建BSDF,bsdf指针不需要预先新建对象
	//由此函数新建对象
	virtual void buildBSDF(
		DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool)=0;
	static void freeBSDF(BSDF* bsdf, MemoryPool& customPool)
	{
		customPool.free<BSDF>(bsdf);
	}

	virtual void setColor(Vector3 icolor){color=icolor;}
	Vector3 getColor(){return color;}
	Vector3 getLight(){return light;}
	virtual void getProperties(vector<float>& properties){};
	MaterialType getType(){return type;}
	bool     isEmissive(){return light.lengthSquared() > 1e-3f;}
protected:
	Vector3 light;
	Vector3 color;
	MaterialType type;
	string name;
	unsigned int ID;
};
class WLambertMaterial:public WMaterial
{
public:
	WLambertMaterial(string iName,unsigned int iID,
		Vector3 icolor, Vector3 ilight=Vector3(0)):
	WMaterial(MATERIAL_LAMBERT,iName,iID,icolor,ilight){}
	~WLambertMaterial(){}

	//设置颜色
//	void setColor(Vector3 icolor){color=icolor;}
	//创建一个LambertBSDF
	void buildBSDF(DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool);
	void getProperties(vector<float>& properties);

};
class WPhongMaterial:public WLambertMaterial
{
public:
	WPhongMaterial(string iName,unsigned int iID,
		const Vector3& icolor,const Vector3& ispecular,float iglossiness, Vector3& ilight=Vector3(0)):
	WLambertMaterial(iName,iID,icolor,ilight),
		specular(ispecular),
		glossiness(iglossiness){type=MATERIAL_PHONG;}
	void setParams(float ispecular,
		float iglossiness)
	{specular=ispecular;
	glossiness=iglossiness;}
	void setSpecular(float ispecular)
	{specular=ispecular;}
	void setGlossiness(float iglossiness)
	{glossiness=iglossiness;}
	void buildBSDF(DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool);
protected:
	Vector3 specular;
	float glossiness;
};
//完全光滑表面的材质
class WPerfectReflectionMaterial:public WMaterial
{
public:
	WPerfectReflectionMaterial(string iName,unsigned int iID,
		Vector3 icolor=Vector3(1), Vector3 ilight=Vector3(0)):
	  WMaterial(
		  MATERIAL_PERFECTREFLECTION,iName,iID,icolor,ilight)
	  {}
	  ~WPerfectReflectionMaterial(){}


//	  void setColor(Vector3 icolor){color=icolor;}
	  void buildBSDF(DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool);
	  void getProperties(vector<float>& properties);
private:

};
//完全透明且光滑物体的材质
class WPerfectRefractionMaterial:public WMaterial
{
public:
	WPerfectRefractionMaterial(string iName,unsigned int iID,
		Vector3 icolor=Vector3(1),float iIOR=1.33, Vector3 ilight=Vector3(0)):
	WMaterial(
		  MATERIAL_PERFECTREFRACTION,iName,iID,
		  icolor,ilight),IOR(iIOR){}
	~WPerfectRefractionMaterial(){}

	void buildBSDF(DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool);
	void setIOR(float ior){IOR=ior;}

	void getProperties(vector<float>& properties);
private:

	float IOR;
};
class WMetalMaterial:public WMaterial
{
public:
	WMetalMaterial(string iName,unsigned int iID,Vector3 Fr,float iexp, Vector3 ilight=Vector3(0));
	~WMetalMaterial(){}
	void buildBSDF(DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool);
	void refreshColor();
	void setGlossiness(float iglossiness)
	{exp=iglossiness;}
private:
	Vector3 k,eta;
	float exp;
};
class WDielectricMaterial:public WMaterial
{
public:
	WDielectricMaterial(string iName,unsigned int iID,
		Vector3 icolor,float iexp,float iior, Vector3 ilight=Vector3(0)):
	WMaterial(WMaterial::MATERIAL_DIELECTRIC,iName,iID,icolor,ilight),
	exp(iexp),ior(iior){}
	~WDielectricMaterial(){}
	void buildBSDF(DifferentialGeometry DG,BSDF*&bsdf, MemoryPool& customPool);
	void setParams(float iglossiness,float iior)
	{exp=iglossiness;iior=ior;}
	void setGlossiness(float iglossiness)
	{exp=iglossiness;}
	void setIOR(float iior)
	{ior=iior;}
private:
	float exp,ior;
};