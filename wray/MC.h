#pragma once

struct PCGRandomObj
{
public:
	PCGRandomObj(uint64_t state, uint64_t inc) : m_state(state), m_inc(inc) {}
	inline uint32_t pcgRandom()
	{
		uint64_t oldstate = m_state;
		// Advance internal state
		m_state = oldstate * 6364136223846793005ULL + (m_inc | 1);
		// Calculate output function (XSH RR), uses old state for max ILP
		uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
		uint32_t rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}
	uint64_t m_state;
	uint64_t m_inc;
};



//����������ֲ�������
class RandomNumber
{
public:
	RandomNumber(void);
	virtual ~RandomNumber(void);
	static float randomFloat()
	{
		//return rand() / 32767.0;
		//return s_uniformFloatObj(s_randObj);
		uint32_t s = s_PCGRandObj.pcgRandom();
		return min(0.99999994f, float(s * 2.3283064365386963e-10f));
		//return s / float(INT32_MAX);
	}
	static int randomInt(int count)
	{
		//return rand() % count;
		//return s_uniformIntObj(s_randObj) % count;
		return s_PCGRandObj.pcgRandom() % count;
	}
	static void randomSeed(unsigned int seed)
	{
		//s_randObj.seed(seed);
		s_PCGRandObj.m_inc = seed;
		s_PCGRandObj.m_state = 12345;
	}

	//�����������������������0-1������������ز���������ֵ
	//��Բ�β��������ص�λԲ�ڲ�������ֵ
	static void uniformSampleDisk(const float u1, const float u2, float &x, float &y);
	//�԰��������ʹ�ò���ֵ�������ҷֲ���PDFΪ��Ӧλ�õĸ����ܶȺ���
	static void cosineSampleHemisphere(const float u1, const float u2, Vector3 &sample,float&PDF);
	static void uniformSampleTriangle(float u1,float u2,
		float &u,float &v);
	static float powerHeuristic(int nf, float fPdf, int ng, float gPdf);

private:
	static std::mt19937 s_randObj;
	static std::uniform_real_distribution<float> s_uniformFloatObj;
	static std::uniform_int_distribution<> s_uniformIntObj;
	static PCGRandomObj s_PCGRandObj;
};
