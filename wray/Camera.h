#pragma once
#include "Vector3.h"
#include "Film.h"
class WCamera
{
public:
	WCamera(void);
	virtual ~WCamera(void);
	void setParameter(
		Vector3 iori=Vector3(0,0,1),
		Vector3 itar=Vector3(0,0,0),
		Vector3 iup=Vector3(0,0,1),
		float ifov= 3.14159265/3.0f,
		float iratio=1.33f
		);
	void setDirectionParams(
		Vector3 iori=Vector3(0,0,1),
		Vector3 itar=Vector3(0,0,0),
		Vector3 iup=Vector3(0,0,1));

	//���ý�Ƭ�ֱ��ʣ���ֱ����ֱ��ʸ��ݿ�߱����
	virtual void setFilmResolutionX(unsigned int resX);
	virtual void setFilmResolutionXY
		(unsigned int resX,unsigned int resY);
	virtual void getFilmResolution(int&resX, int&resY);
	//������������߿�
	virtual void drawCamera(float R=0,float G=0,float B=0.8);
	//������Ƭ���ݣ�ʵ��������Ⱦ��ϵ�ͼ��
	//offsetX=0.0,offsetY=0.0fʱ��ͼ������������½�
	virtual void drawFilmInScreen(float offsetX=0.0f,float offsetY=0.0f);
	//Ҳ�ǻ�����Ƭ���ݣ�����ͬ������Ҫ������������ֵ
	virtual void drawFilmInWorld(float offsetX=0.0f,
		float offsetY=0.0f,float offsetZ=0.0f);
	//������������
	void getNextRay(WRay&ray);
	void getNextRay( WRay&ray, float xi, float yi );
	//���ù���׷��֮�󷵻ص���ɫ
	void setColor(float R,float G,float B);
	void setColor(float R, float G, float B, int x, int y);
	void accumulateColor( float R, float G, float B, int x, int y );
	Vector3 getColor(unsigned int x,unsigned int y);
	//����ǲ����������ض���������
	virtual bool isFilmFull();
	//��ʾ���¿�ʼ��Ⱦ,���������õ�Ƭ��ɫ
	virtual void clearFilm(float R=0.0,float G=0.0,float B=0.0);
	//�����ӵ�һ�����ؿ�ʼ�ع�,֮ǰ����ɫ�ᱻ���ӽ�ȥ
	virtual void nextExposurePass();
	virtual void changeSampleSize(unsigned int size);
	virtual void changeSampler(WSampler::WSamplerType type);
	//���ص�ǰ��Ⱦ����
	virtual float currProgress();
	virtual Vector3 getDirection(){return dir;}
	virtual Vector3 getOrigin(){return origin;}
	virtual void getXY(Vector3&ix,Vector3&iy)
	{ix=x;iy=y;	}
	virtual int*getFilmBitPointer();
	virtual unsigned int getSampleSize(){return filmSampleSize;};
	virtual float getRatio(){return ratio;}

protected:
	Vector3 origin;		//���
	Vector3 target;		//Ŀ���	
	Vector3 dir;		//�����ָ��Ŀ���ĵ�λ����������Ϊ1
	Vector3 up;			//��������
	Vector3 x,y;		//��Ļx y����Ŀռ䷽��,���ǵ�λ����
						//��Ļ�����ľ���Ϊ1
	float fov;
	float ratio;		//��߱ȣ���/�ߣ�
	WFilm film;
	unsigned int filmSampleSize;

	//�ѽ�Ƭ�����ó�ͬһ����ɫ
	void cleanFilmColors(float R=1.0,float G=1.0,float B=1.0);
	//ͨ����Ļ��xy�������һ������
	//xRatio��yRatio��ֵ����-1��1֮��	
	//����xy����
	void computeXY();
	void setFilmResolutionX(unsigned int resX,unsigned int resY);

private:
	void generateRay(float xRatio,float yRatio,WRay&ray);
};
