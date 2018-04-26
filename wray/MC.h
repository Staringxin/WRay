#pragma once
//����������ֲ�������
class RandomNumber
{
public:
	RandomNumber(void);
	virtual ~RandomNumber(void);
	static float randomFloat();
	static int randomInt(int count) 
	{
		return s_uniformIntObj(s_randObj) % count;
	}
	static void randomSeed(unsigned int seed) { s_randObj.seed(seed); }

	//�����������������������0-1������������ز���������ֵ
	//��Բ�β��������ص�λԲ�ڲ�������ֵ
	static void uniformSampleDisk(const float u1, const float u2, float &x, float &y);
	//�԰��������ʹ�ò���ֵ�������ҷֲ���PDFΪ��Ӧλ�õĸ����ܶȺ���
	static void cosineSampleHemisphere(const float u1, const float u2, Vector3 &sample,float&PDF);
	static void uniformSampleTriangle(float u1,float u2,
		float &u,float &v);
	static float powerHeuristic(int nf, float fPdf, int ng, float gPdf);

private:
	static std::minstd_rand s_randObj;
	static std::uniform_real_distribution<float> s_uniformFloatObj;
	static std::uniform_int_distribution<> s_uniformIntObj;
};
