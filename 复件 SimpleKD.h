#pragma once

//Ҷ�ӽڵ����������������

struct WSKDNode
{
	static const unsigned int maxTrianglesPerLeaf =	5;
	enum NodeType {KDN_XSPLIT=0,
		KDN_YSPLIT=1,KDN_ZSPLIT=2,KDN_LEAF=3};
	inline bool isInterior(){return type != KDN_LEAF;}
	NodeType type;
};
//�ڲ��ڵ�
struct WSKDInterior : public WSKDNode
{
	float splitPlane;
	int leftChild;
	int rightChild;
};
//Ҷ�ڵ�
struct WSKDLeaf : public WSKDNode
{
//	WBoundingBox box;
	//�ֱ�Ϊ pMin.x pMin.y pMin.z pMax.x pMax.y pMax.z
	float box[6];
	//�����ڽڵ������,�ֱ�Ϊ 
	//x ���棬 x���棬 y ���棬 y���棬 z ���棬 z���� 
	int ropes[6];		
	int triangleIDs[maxTrianglesPerLeaf];
	int nTriangles;
};

class WSimpleKD:public WAccelerator
{
	//Bounding Edge ���ڰ������ΰ��հ�Χ�зֳ�����
	struct BoundingEdge
	{
		//BE�����ͣ�
		//�ֱ�Ϊ�����ε���ʼ�㣬��ֹ��
		enum EdgeType{
			  BE_END = 0,BE_PLANAR = 1, BE_START = 2};

		EdgeType type;
		float t;
		int   triangleID;

		//������������
		//����ȷ��BE����tֵ����
		//��ͬt��BE����BE�����ͺ�������ID�Ĺ�ϵ����
		bool operator<(const BoundingEdge&e)const;
	};

public:
	WSimpleKD(void);
	~WSimpleKD(void);
	void buildTree(WScene&scene);
	bool intersect(WRay& r,WDifferentialGeometry& DG, 
		int* endNode = NULL, int beginNode = 0);
	void clearTree();
	bool isIntersect(WRay& r, int beginNode = 0);
	void drawTree(unsigned int nthBox=0,
					float R = 0.7, 
					float G = 0.7, 
					float B = 0.7);
	void displayNodeInfo() {};
	void drawTriangles();
private:
	WTriangle* triangles;				//��������������
	unsigned int totalTriangles;			//�������ܸ���
	WBoundingBox sceneBox;				//������Χ��

	vector<WSKDNode*> nodes;				//�ڵ�����
	vector<WBoundingBox> nodeBoxes;		//�ڵ��Χ������
	int nodeMaxDepth;					//��¼KD���ﵽ��������

	//���� ropes, ��6�������ֱ����ڵ��Χ��6��������ڽڵ�����
	void buildBasicRopes(int ithNode, 
						int ropeX_P, int ropeX_N,
						int ropeY_P, int ropeY_N,
						int ropeZ_P, int ropeZ_N);

	//������ǿ�� ropes
	void buildExtendedRopes();
	//�ж� refBox ��ָ�����Ƿ���ȫ�� testBox ��һ����
	bool isOverlap(WBoundingBox& refBox,
				   WBoundingBox& testBox,
				   int refFace);
	//��鲢����Ҷ�ڵ��һ����
	int checkFace(WBoundingBox& refBox,
				  int& rope, int refFace);
	//�ҵ�һ������ǰ��Χ�е�
	void buildTreeKernel(vector<BoundingEdge>& edgesX, 
						vector<BoundingEdge>& edgesY, 
						vector<BoundingEdge>& edgesZ,
						WBoundingBox bBox, 
						int* triangleMark, 
						int nTriangles, int depth);

	//����ָ�ƽ������λ�ã�
	//splitTypeָ�� bounding edge �ķ���
	//ע�⣬�˺���Ҫ��ͬʱ�ṩһ�������ε�
	//��ʼ BE �ͽ��� BE ,�����ṩһ��ƽ�� BE
	//Ҳ���� bounding edge �ĸ���Ϊż��
	//ͬʱ���˺���Ҫ�� bounding edge ��Ԫ�ظ�����Ϊ 0
	//isLeft������ʾ������ŵķָ�ƽ��Ϊһ��ƽ������BE��
	//���Ӧ���������Ƿ������
	void computeBestSplit(vector<BoundingEdge>& edges,
						  WSKDNode::NodeType splitType,
						  WBoundingBox& bBox,
						  int nTriangles,
						  int& nTriangles_L,
						  int& nTriangles_R,
						  int& bestPosition,
						  bool& isLeft,
						  float& bestT
						  );

	//���ݷָ�ƽ���λ��
	//ȷ�������ε�����
	//ֻ����ߵ������ζ�Ӧ���Ϊ 2
	//ֻ���ұߵ������ζ�Ӧ���Ϊ-2
	//���ڵ�������ζ�Ӧ���Ϊ 0 ,
	//��Ӧ�������� middleTriangleID
	//isLeft������ʾ�������Ƿ���ƽ������BE�����
	void markTriangles(vector<BoundingEdge>& refEdges,
						vector<int>& middleTriangleID,
						int bestBE,
						int nTriangles,
						int* triangleMark,
						bool isLeft);

	//���ݺ��ָ�ƽ��������μ��������ӽڵ��Χ���ڲ���BE
	//����ƽ��BE�Ĵ��ڣ��������������middleBE�������ܲ����
	void getNewSortedBE(vector<int>& middleTriangleID,
				  const WBoundingBox& clipBox,
				  vector<BoundingEdge>& middleBEX,
				  vector<BoundingEdge>& middleBEY,
				  vector<BoundingEdge>& middleBEZ);

	//���ݱ���˵�����������
	//�Լ��´�����BE�õ����ҽڵ��BE
	//oldEdge Ϊԭ�е�BE
	//middleEdgeΪ�´�����BE
	//newEdgeΪ�ϲ��Ժ��BE����Ϊ�ӽڵ����BE
	void mergeBE(vector<BoundingEdge>& oldEdge, 
				vector<BoundingEdge>& middleEdge_L, 
				vector<BoundingEdge>& middleEdge_R, 
				vector<BoundingEdge>& newEdge_L, 
				vector<BoundingEdge>& newEdge_R, 
				int nTriangles_L, 
				int nTriangles_R, 
				int* triangleMark);

	void buildLeaf(WBoundingBox bBox,
					vector<BoundingEdge>& edges);
	void drawTreeRecursive(int nthNode, const WBoundingBox& box);
	//���ݲ���ֵ���Bounding Edge,
	//��������εİ�Χ�������غϣ�ֻ���һ�� BE_PLANAR ���͵�BE
	//������� BE_START �� BE_END ���͵�BE
	void addEdge( vector<BoundingEdge>& edges, 
				float minT, float maxT, 
				int ithTriangle );

	//��������������ȷ����ѵķָ�
	inline float computeCost(float& emptyFactor,
							  float& lengthL, float& lengthR,
							  float& nBelow, float& nAbove)
	{	return emptyFactor * 
			(lengthL * nBelow + lengthR * nAbove);}

	//���������������һ��Ϊ��ʱ��
	//����ָ�ƽ�治�ڰ�Χ�б�����
	//�ʵ���С����������
	//���������������
	inline float computeEmptyFtr(float nBelow, float nAbove,
								 float nTriangles,
								 float& t, 
								 float& tMin, float& tMax)
	{
		return (((nBelow == 0.0f) || (nAbove == 0.0f))?
			((t > tMin  && t < tMax)? 0.8f: 5.0f):
			1.0f);
	}
	//��¼���Ż��������ߵ�����������
	inline void updateBest(int& bestBE, int& nthBE, 
					float& bestCost, float& cost,
					int& nTriangles_L, int& nTriangles_R,
					int nBelow, int nAbove)
	{
		bestBE = nthBE;		bestCost = cost;
		nTriangles_L = nBelow;		nTriangles_R = nAbove;
	}


	//����һ�����Χ���ཻ�Ĺ��ߣ���������ߴӰ�Χ���ĸ������뿪
	int computeExitFace( WSKDLeaf& node, const WRay& r, WVector3& exitPoint, float& farT);

};
