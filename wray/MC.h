#pragma once

#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL

struct PCGRandomObj
{
public:
	PCGRandomObj(uint64_t state = PCG32_DEFAULT_STATE, uint64_t inc = PCG32_DEFAULT_STREAM) : m_state(state), m_inc(inc) {}
	inline uint32_t pcgRandom()
	{
		uint64_t oldstate = m_state;
		// Advance internal state
		m_state = oldstate * PCG32_MULT + (m_inc | 1);
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
    float randomFloat();
    int randomInt(int count);
    void randomSeed(unsigned int seed);

    static RandomNumber* getGlobalObj();

	//�����������������������0-1������������ز���������ֵ
	//��Բ�β��������ص�λԲ�ڲ�������ֵ
	static void uniformSampleDisk(const float u1, const float u2, float &x, float &y);
	//�԰��������ʹ�ò���ֵ�������ҷֲ���PDFΪ��Ӧλ�õĸ����ܶȺ���
    static void cosineSampleHemisphere(const float u1, const float u2, Vector3 &sample,float&PDF);
    static void uniformSampleSphere(float u1, float u2, Vector3& sample, float& PDF);
    static void uniformSampleTriangle(float u1,float u2, float &u,float &v);
    static float powerHeuristic(int nf, float fPdf, int ng, float gPdf);
private:
//	std::mt19937 s_randObj;
//	std::uniform_real_distribution<float> s_uniformFloatObj;
//	std::uniform_int_distribution<> s_uniformIntObj;
	PCGRandomObj m_PCGRandObj;
    static RandomNumber* s_globalObj;
    static std::unordered_map<std::thread::id, RandomNumber*> s_threadRNG;
    static std::mutex s_lock;
};
