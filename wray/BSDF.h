#pragma once
#include "DifferentialGeometry.h"
#include "MC.h"

class BSDF
{
public:
	enum BSDFType {
		BSDF_LAMBERT = 0,
		BSDF_PHONG = 1,
		BSDF_PERFECTREFLECTION = 2,
		BSDF_PERFECTREFRACTION = 3,
		BSDF_METAL = 4,
		BSDF_DIELECTRIC = 5,
		BSDF_GGX_METAL = 6,
	};

	BSDF(DifferentialGeometry& iDG, BSDFType itype,
		bool iisEmissive = false, const Vector3& iemission = Vector3(0)) :
		DG(iDG), type(itype), isEmissive(iisEmissive), emission(0) {};
	virtual ~BSDF(void);

	//��BSDF��ֵ,˳���������������
	//ri ro�����ȵ�λ��
	virtual Vector3 evaluateFCos(Vector3&ri, const Vector3&ro) = 0;
	//�������������u vֵ��������Ĺ���
	//pdfΪ��Ӧλ�õĸ����ܶ�
	//sampleWi��Ҫ����Ĳ�������
	virtual void sampleRay(float u, float v, Vector3&sampleWi, const Vector3&wo, float&pdf) = 0;
	virtual Vector3 rho(Vector3&wo) { return Vector3(0); }
	virtual Vector3 getEmission() { return emission; }
	virtual void     setEmission(const Vector3& emi) { emission = emi; }
	virtual bool isDeltaBSDF() = 0;
	//����get����
	BSDFType getType() { return type; }
	Vector3 getPosition() { return DG.position; }
	Vector3 getNormal() { return DG.normal; }
	Vector3 getBitangent() { return DG.bitangent; }
	Vector3 getTangent() { return DG.tangent; }
	Vector2f getTexCoord() { return DG.texCoord; }

	//����set����
	//����DG
	void setDifferentialGeometry(
		DifferentialGeometry&iDG);
	unsigned int getMtlID() { return DG.mtlId; }
	BSDFType type;
	DifferentialGeometry DG;
	bool isEmissive;
private:
	Vector3 emission;

};

class LambertBSDF :public BSDF
{
public:
	LambertBSDF(DifferentialGeometry& iDG, const Vector3 &icolor) :
		BSDF(iDG, BSDF_LAMBERT), color(icolor) {}
	~LambertBSDF() {}

	//���ѡ��һ�����߷���
	//���ߵķ���������ҷֲ�
	void sampleRay(
		float u, float v,
		Vector3&sampleWi, const Vector3&wo, float&pdf);
	//��ΪBSDF��ֵ����
	//������ɫֵ/PI
	//��Ϊ���������غ㣬Ӧ��Integrate[F*cos(thetaI)*dwi]<=1
	//֤��������BSDF�Ǳ�ʾ����һ�����һ����������ʱ��
	//���������ķ����ķֲ������
	//����˵�������м��������������dwi������ڣ����������
	//��һ���ض�����BSDF����
	//�Ǽ��������������������Ϲ��׵�����ռ���������ı�ֵ
	//F=dLo/dEi=dLo/(Li*cos(thetaI)*dwi)
	//���룬����ʽ�ӻ�ΪIntegrate[dLo/Li]
	//��һ����������Ĺ��߱������յؾ��ȷ��䵽��������ʱ��
	//��������Ϊ1
	//����Ϊ����Lambert���ʣ�������߱����ȵ�ɢ�䵽��������
	//��FΪһ��������������Ϊ1,
	//������Ĺ��߱������յؾ��ȷ��䵽�������򣬼�Ϊ����ɫʱ
	//FΪ1/PI
	//���������գ�����ɫ��Ϊ����ʱ��ֻ���ڴ˻����ϳ���
	//��ɫ�Ĺ����������ɫΪ��1,1,1����
	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	Vector3 rho(Vector3&wo) { return color; }
	virtual bool isDeltaBSDF() { return false; }
protected:
	Vector3 color;
};

class PhongBSDF :public LambertBSDF
{
public:
	PhongBSDF(DifferentialGeometry& iDG, const Vector3& icolor,
		const Vector3& ispecular, float iglossiness) :
		LambertBSDF(iDG, icolor), specular(ispecular), glossiness(iglossiness)
	{
		type = BSDF_PHONG;
	}
	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	virtual bool isDeltaBSDF() { return false; }
private:
	Vector3 specular;
	float glossiness;
};


//���BSDFģ����ȫ�⻬����ķ���
class PerfectReflectionBSDF :public BSDF
{
public:
	PerfectReflectionBSDF(DifferentialGeometry& iDG,
		Vector3& icolor) :BSDF(iDG, BSDF::BSDF_PERFECTREFLECTION), color(icolor) {}
	~PerfectReflectionBSDF() {};
	void sampleRay(
		float u, float v,
		Vector3&sampleWi, const Vector3&wo,
		float&pdf);

	//ע�⣬����Ϊ��ȫ�⻬���棬������߷����ϸ���ѭ���䶨��
	//����������BSDF�Ķ��壬BSDFΪ
	//��������ļ����������
	//��һ���ض����䷽���Ϲ��׵�����ռ���������ı�ֵ
	//�ʶ�����ȫ�⻬���棬bsdf��Ϊһ������������һ��delta����
	//���Բ�����MC�������ƣ�������ͨ��ֱ�Ӽ����������÷����
	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	virtual bool isDeltaBSDF() { return true; }
private:
	Vector3 color;
};
//���BSDFģ����ȫ�⻬���������
class PerfectRefractionBSDF :public BSDF
{
public:
	PerfectRefractionBSDF(DifferentialGeometry& iDG,
		Vector3& icolor, float iIOR) :BSDF(iDG, BSDF::BSDF_PERFECTREFRACTION), color(icolor), IOR(iIOR) {}
	~PerfectRefractionBSDF() {}
	void sampleRay(
		float u, float v,
		Vector3&sampleWi, const Vector3&wo,
		float&pdf);

	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	virtual bool isDeltaBSDF() { return true; }
private:
	Vector3 color;
	float IOR;

	//����������߷����Ѱ���ȫ����Ч��
	Vector3 refractionDirection(
		const Vector3&i, const Vector3&normal);
};

//�����࣬���ڼ��������ϵ��
//�������Ծ�Ե��
class FresnelDielectric
{
public:
	FresnelDielectric(float iIOR, Vector3 inormal);

	//����������߷����Ѱ���ȫ����Ч��
	Vector3 refractionDirection(
		const Vector3&i, const Vector3&normal);
	float evaluateF(Vector3&wi);
	void setNormal(Vector3&inormal) { normal = inormal; }
	virtual bool isDeltaBSDF() { return false; }
private:
	//����rֵ ��etaTOΪ������
	float computeR(float cosWi, float cosWt, float etaTO);
	//IORΪ�����ʣ�IOR=����������/����������
	float IOR;
	Vector3 normal;
};
//�������Ե���
class FresnelConductor
{
public:
	FresnelConductor(Vector3 ieta, Vector3 ik, Vector3 inormal);
	Vector3 evaluateF(Vector3&wi);
	void setNormal(Vector3&inormal) { normal = inormal; }
private:
	Vector3 eta, k, normal;

};

//ģ���������
class MetalBSDF :public BSDF
{
public:
	MetalBSDF(DifferentialGeometry iDG,
		Vector3 ieta, Vector3 ik, float iexp) :
		BSDF(iDG, BSDF_METAL),
		fresnel(ieta, ik, iDG.normal),
		exp(iexp) {}
	~MetalBSDF() {}
	void sampleRay(float u, float v, Vector3&sampleWi, const Vector3&wo, float&pdf);
	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	virtual bool isDeltaBSDF() { return false; }
private:
	float computeD(const Vector3&wh);
	float computeG(const Vector3&wi, const Vector3&wo, const Vector3&wh);
	float computePDF(const Vector3&wi, const Vector3&wo);
	FresnelConductor fresnel;
	float exp;
};

class GGXMetalBSDF :public BSDF
{
public:
	GGXMetalBSDF(DifferentialGeometry iDG, Vector3 ieta, Vector3 ik, float ag) :
		BSDF(iDG, BSDF_GGX_METAL), fresnel(ieta, ik, iDG.normal), m_ag(ag) {}
	~GGXMetalBSDF() {}
	void sampleRay(float u, float v, Vector3&sampleWi, const Vector3&wo, float&pdf);
	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	virtual bool isDeltaBSDF() { return false; }
private:
	float computeD(const Vector3&wh);
	float computeG(const Vector3&wi, const Vector3&wo, const Vector3&wh);
	float computeG1(const Vector3&v, const Vector3&wh);
	float computePDF(const Vector3&wi, const Vector3&wo);
	FresnelConductor fresnel;
	float m_ag;
};

//��BSDF��ʾ��͸����Ե���BSDF
class DielectricBSDF :public BSDF
{
public:
	DielectricBSDF(DifferentialGeometry iDG, float iior,
		Vector3 icolor, float iexp) :
		BSDF(iDG, BSDF_DIELECTRIC),
		fresnel(iior, iDG.normal),
		color(icolor), exp(iexp) {}
	~DielectricBSDF() {}
	void sampleRay(
		float u, float v,
		Vector3&sampleWi, const Vector3&wo,
		float&pdf);

	Vector3 evaluateFCos(Vector3&ri, const Vector3&ro);
	virtual bool isDeltaBSDF() { return false; }
private:
	float computeD(const Vector3&wh);
	float computeG(const Vector3&wi, const Vector3&wo, const Vector3&wh);
	float computePDF(const Vector3&wi, const Vector3&wo);
	FresnelDielectric fresnel;
	float IOR;
	float exp;
	Vector3 color;
};
