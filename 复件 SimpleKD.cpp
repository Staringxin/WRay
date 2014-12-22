#include "StdAfx.h"
#include "SimpleKD.h"

WSimpleKD::WSimpleKD(void)
{
	nodeMaxDepth = 0;
}

WSimpleKD::~WSimpleKD(void)
{
	clearTree();
}

void WSimpleKD::buildTree( WScene&scene )
{
	//��ó�������������Ͱ�Χ��
	cout<<"begin to build KD"<<endl;
	scene.getTriangleArray(triangles, totalTriangles);
	cout<<"total triangles: "<<totalTriangles<<endl;
	sceneBox = scene.getBBox();
//	sceneBox.pMin += WVector3(WBoundingBox::delta);
//	sceneBox.pMax -= WVector3(WBoundingBox::delta);
	WClock timer;
	timer.begin();


	//�½�bounding edge���飬���ڻ����������
	//����KD���Ļ���ƽ���� XYZ 3�������ϣ�
	//����bounding edge������3��
	vector<BoundingEdge> edgesX, edgesY, edgesZ;
	edgesX.reserve(totalTriangles * 2);
	edgesY.reserve(totalTriangles * 2);
	edgesZ.reserve(totalTriangles * 2);

	int* triangleMark = new int[totalTriangles];

	//��ʼ��bounding edge ������������
	for (unsigned int nthTriangle = 0;
		nthTriangle < totalTriangles;
		nthTriangle++)
	{
		WBoundingBox triangleBox = 
			WBoundingBox(triangles[nthTriangle], false);

		addEdge(edgesX, triangleBox.pMin.x, triangleBox.pMax.x, nthTriangle);
		addEdge(edgesY, triangleBox.pMin.y, triangleBox.pMax.y, nthTriangle);
		addEdge(edgesZ, triangleBox.pMin.z, triangleBox.pMax.z, nthTriangle);
	}

	//��bounding edge��������
	sort(edgesX.begin(), edgesX.end());
	sort(edgesY.begin(), edgesY.end());
	sort(edgesZ.begin(), edgesZ.end());

	//Ԥ���ڵ�ռ�
	nodes.reserve(totalTriangles * 6);
	nodeBoxes.reserve(totalTriangles * 6);

	//�ݹ鹹����
	buildTreeKernel(edgesX, edgesY, edgesZ, 
					sceneBox, triangleMark, totalTriangles, 0);

	//����ropes
	cout<<"nNodes: "<<nodes.size() << "depth" << nodeMaxDepth <<endl;
	buildBasicRopes(0, -1, -1, -1, -1, -1, -1);
//	buildExtendedRopes();
	timer.end();
	cout<<"build complete.Total time: "<<endl;
	timer.display();
	delete[] triangleMark;
}
void WSimpleKD::addEdge( vector<BoundingEdge>& edges, 
					   float minT, float maxT, 
					   int ithTriangle )
{
	if (minT == maxT)
	{
		BoundingEdge planarEdge;
		planarEdge.type = BoundingEdge::BE_PLANAR;
		planarEdge.t = minT;
		planarEdge.triangleID = ithTriangle;
		edges.push_back(planarEdge);
	} 
	else
	{
		BoundingEdge startEdge, endEdge;
		startEdge.type = BoundingEdge::BE_START;
		endEdge.type = BoundingEdge::BE_END;
		startEdge.triangleID = endEdge.triangleID = ithTriangle;
		startEdge.t = minT; endEdge.t = maxT;
		edges.push_back(startEdge);
		edges.push_back(endEdge);
	}
}
void WSimpleKD::clearTree()
{
	triangles = NULL;
	vector<WSKDNode*>::iterator pNode;
	for (pNode = nodes.begin(); pNode != nodes.end();
		pNode++)
	{
		delete *pNode;
	}
	nodes.clear();
	totalTriangles = 0;
	nodeMaxDepth = 0;
}

void WSimpleKD::buildTreeKernel( vector<BoundingEdge>& edgesX, 
							   vector<BoundingEdge>& edgesY, 
							   vector<BoundingEdge>& edgesZ, 
							   WBoundingBox bBox, 
							   int* triangleMark, int nTriangles, int depth )
{
/*	printf("\n\n##################################################################\nnthNode: %d\nnTriangles: %d\ndepth: %d\n", 
			nodes.size(), nTriangles, depth);*/
	//��¼����������
	if (depth > nodeMaxDepth)
	{
		nodeMaxDepth = depth;
	}
	//���������������ڹ涨ֵ���½�Ҷ�ڵ�
	if (nTriangles <= WSKDNode::maxTrianglesPerLeaf)
	{
		buildLeaf(bBox, edgesX);
		return;
	}
	//�����½��ڲ��ڵ�
	//�ҳ����귶Χ��������ָ�
	WVector3 deltaBox = bBox.pMax - bBox.pMin;
	WSKDNode::NodeType splitType = 
		(deltaBox.x > deltaBox.y)?
		(deltaBox.x > deltaBox.z? WSKDNode::KDN_XSPLIT:WSKDNode::KDN_ZSPLIT):
		(deltaBox.y > deltaBox.z? WSKDNode::KDN_YSPLIT:WSKDNode::KDN_ZSPLIT);

	int bestPosition, nTriangles_L, nTriangles_R;
	float bestT;
	bool isLeft;
	WBoundingBox box_L = bBox, box_R = bBox;
	vector<BoundingEdge> edgesX_L, edgesY_L, edgesZ_L, 
						 edgesX_R, edgesY_R, edgesZ_R;

	WSKDInterior* interior = new WSKDInterior;
	float tmin, tmax;

//	printf("---------------------------begin to compute split plane\n");
	//����ָ�ƽ������λ��
	int ithTest;
	for(ithTest = 0; ithTest < 3; ithTest++)
	{
		tmin = bBox.pMin.v[splitType]; 
		tmax = bBox.pMax.v[splitType];
		switch (splitType)
		{
		case WSKDNode::KDN_XSPLIT:
			computeBestSplit(edgesX, splitType, bBox, 
							nTriangles, nTriangles_L, nTriangles_R,
							bestPosition, isLeft, bestT);
			break;
		case WSKDNode::KDN_YSPLIT:
			computeBestSplit(edgesY, splitType, bBox, 
							nTriangles, nTriangles_L, nTriangles_R,
							bestPosition, isLeft, bestT);
			break;
		case WSKDNode::KDN_ZSPLIT:
			computeBestSplit(edgesZ, splitType, bBox, 
							nTriangles, nTriangles_L, nTriangles_R,
							bestPosition, isLeft, bestT);
		}
		//  ����Ϊ��Ч����,��һ������
		//	1. �ָ�ƽ����bbox���棬�ҷָ�����һ����������Ŀ����
		//	2. �ָ���������������Ŀ��������
		if((nTriangles_L == nTriangles && nTriangles_R ==nTriangles) ||
				((bestT == tmin || bestT == tmax)&&
				(nTriangles_L == nTriangles || nTriangles_R == nTriangles)))
		{
//			printf("----------next axis\n");
			splitType = WSKDNode::NodeType((splitType + 1) % 3);
		}
		else 
			break;
	}
	//�����⻮�֣��½�Ҷ�ڵ�
	if (ithTest == 3)
	{
		printf("no proper division.\n");
		buildLeaf(bBox, edgesX);
		return;
	}
/*
	printf("nTriangles_L: %d, nTriangles_R: %d\n",nTriangles_L,nTriangles_R);
	printf("bestT: %1.3f\tbestBE:%d\tisLeft:%d\n",bestT,bestPosition,(int)isLeft);
	printf("---------------------------begin to mark triangle\n");
*/
	vector<int> middleTriangleID;
	vector<BoundingEdge> middleEdgeX_L, middleEdgeY_L, middleEdgeZ_L,
						 middleEdgeX_R, middleEdgeY_R, middleEdgeZ_R;

	//�������еķָ�ƽ��λ�ã����������
	//ֻ����ڵ�������ζ�Ӧ���Ϊ 2
	//ֻ����ڵ�������ζ�Ӧ���Ϊ-2
	//���ָ�ƽ��������ζ�Ӧ���Ϊ 0
	interior->type = splitType;
	switch (splitType)
	{
	case WSKDNode::KDN_XSPLIT:
//		printf("Xsplit\n");
		markTriangles(edgesX, middleTriangleID, bestPosition, 
			nTriangles, triangleMark, isLeft);
		break;
	case WSKDNode::KDN_YSPLIT:
//		printf("Ysplit\n");
		markTriangles(edgesY, middleTriangleID, bestPosition, 
			nTriangles, triangleMark, isLeft);
		break;
	case WSKDNode::KDN_ZSPLIT:
//		printf("Zsplit\n");
		markTriangles(edgesZ, middleTriangleID, bestPosition, 
			nTriangles, triangleMark, isLeft);
	}
	box_L.pMax.v[splitType] = bestT;
	box_R.pMin.v[splitType] = bestT;
/*
	printf("overlapping triangles: %d\n",middleTriangleID.size());
	printf("---------------------------begin to compute new BE\n");
*/
	//�����µ�BE���������ҽڵ㣬�µ�BE��������һ����
	getNewSortedBE(middleTriangleID, box_L, middleEdgeX_L, middleEdgeY_L, middleEdgeZ_L);
	getNewSortedBE(middleTriangleID, box_R, middleEdgeX_R, middleEdgeY_R, middleEdgeZ_R);

	
/*
	printf("middleEdgeX_L: %d\tmiddleEdgeY_L: %d\tmiddleEdgeZ_L: %d\n",
		middleEdgeX_L.size(),middleEdgeY_L.size(),middleEdgeZ_L.size());
	printf("middleEdgeX_R: %d\tmiddleEdgeY_R: %d\tmiddleEdgeZ_R: %d\n",
		middleEdgeX_R.size(),middleEdgeY_R.size(),middleEdgeZ_R.size());
	printf("---------------------------begin to merge BE\n");
*/
	//�����������BE���кϲ�
	//�ϲ������ÿ���ӽڵ����������BE��������һ���ģ�
	//ʵ���Ͼ��ǽڵ��ڲ��������εİ�Χ������ֵ
	mergeBE(edgesX, middleEdgeX_L, middleEdgeX_R, edgesX_L, edgesX_R, 
		nTriangles_L, nTriangles_R, triangleMark);
	mergeBE(edgesY, middleEdgeY_L, middleEdgeY_R, edgesY_L, edgesY_R, 
		nTriangles_L, nTriangles_R, triangleMark);
	mergeBE(edgesZ, middleEdgeZ_L, middleEdgeZ_R, edgesZ_L, edgesZ_R,  
		nTriangles_L, nTriangles_R, triangleMark);

/*
	edgesX.clear();
	edgesY.clear();
	edgesZ.clear();
	middleEdgeX_L.clear();
	middleEdgeX_R.clear();
	middleEdgeY_L.clear();
	middleEdgeY_R.clear();
	middleEdgeZ_L.clear();
	middleEdgeZ_R.clear();
	middleTriangleID.clear();*/

/*
	printf("bestPos: %d\nsplitT: %1.0f\n", bestPosition, bestT);
	printf("edgesX_L: %d\tedgesY_L: %d\tedgesZ_L: %d\n",
		edgesX_L.size(),edgesY_L.size(),edgesZ_L.size());
	printf("edgesX_R: %d\tedgesY_R: %d\tedgesZ_R: %d\n",
		edgesX_R.size(),edgesY_R.size(),edgesZ_R.size());
*/
	interior->splitPlane = bestT;
	nodes.push_back(interior);
	nodeBoxes.push_back(bBox);
	interior->leftChild = nodes.size();
	buildTreeKernel(edgesX_L, edgesY_L, edgesZ_L,
				box_L, triangleMark, nTriangles_L, depth+1);
	interior->rightChild = nodes.size();
	buildTreeKernel(edgesX_R, edgesY_R, edgesZ_R,
				box_R, triangleMark, nTriangles_R, depth+1);
}

void WSimpleKD::computeBestSplit( 
			vector<BoundingEdge>& edges, 
			WSKDNode::NodeType splitType, 
			WBoundingBox& bBox, 
			int nTriangles,
			int& nTriangles_L,
			int& nTriangles_R,
			int& bestPosition,
			bool& isLeft,
			float& bestT)
{
	//�ָ�ƽ��ķ�Χ
	float tmin, tmax;

	//����splitTypeѡ����
	tmin = bBox.pMin.v[splitType];
	tmax = bBox.pMax.v[splitType];

	//ѡ����ѵķָ�ƽ��λ��
	int nthBE = 0;
	int bestBE = 0;
	float emptyFactor, cost;
	float bestcost = FLT_MAX;
	//nBelow �� nAbove�������ߵ�����������
	float nTempBelow = 0.0f, nAbove = (float)nTriangles, nBelow = 0.0f;
	float lengthL, lengthR;
	vector<BoundingEdge>::iterator pBE;
//	printf("minT: %1.3f \t maxT:%1.3f\n",tmin,tmax);
	for (pBE = edges.begin(); pBE != edges.end();
		pBE++, nthBE++)
	{
		//����ָ�ƽ����������ĳ���
		lengthL = (pBE->t) - tmin;
		lengthR = tmax - (pBE->t);
		if(pBE->type != BoundingEdge::BE_PLANAR)
		{
			//����ָ�ƽ�����������������
			//ʵ�����㷢�֣�ʵ�ʵ�����������Ӧ����nTempBelow����һ��ֵ
			nBelow = nTempBelow;
			if (pBE->type == BoundingEdge::BE_START)
				nTempBelow++;
			else
				nAbove--;
			//���������������������
			emptyFactor = computeEmptyFtr(nBelow, nAbove, nTriangles, pBE->t, tmin, tmax);
			cost = computeCost(emptyFactor, lengthL, lengthR, nBelow, nAbove);
			//����ҵ���С�Ŀ�����������ز���
			if (cost < bestcost)
			{
	/*   		printf("BE-t : %1.3f\t TriID: %d\t cost: %1.0f\t nthBE:%d\n",
						pBE->t,pBE->triangleID, cost, nthBE);*/
				updateBest(bestBE, nthBE, bestcost, cost, 
					nTriangles_L, nTriangles_R, (int)nBelow, (int)nAbove);
			}
		}
		else
		{
			//���� BE_PLANAR ���͵� BE�� �ֱ���������������ڷָ�ƽ����ߺ��ұߵ����
			//���������ұߵ�������൱������һ����ʼBE
			nBelow = nTempBelow; nTempBelow++;
			//���������������������
			emptyFactor = computeEmptyFtr(nBelow, nAbove, nTriangles, pBE->t, tmin, tmax);
			cost = computeCost(emptyFactor, lengthL, lengthR, nBelow, nAbove);

			//����ҵ���С�Ŀ�����������ز���
			if (cost < bestcost)
			{
/*   		printf("BE-t : %1.3f\t TriID: %d\t cost: %1.0f\t nthBE:%d\n",
					pBE->t,pBE->triangleID, cost, nthBE);*/
				updateBest(bestBE, nthBE, bestcost, cost, 
					nTriangles_L, nTriangles_R, (int)nBelow, (int)nAbove);
				isLeft = false;
			}

			//����������ߵ�������൱������һ������BE
			nBelow = nTempBelow; nAbove--;
			//���������������������
			emptyFactor = computeEmptyFtr(nBelow, nAbove, nTriangles, pBE->t, tmin, tmax);
			cost = computeCost(emptyFactor, lengthL, lengthR, nBelow, nAbove);

			//����ҵ���С�Ŀ�����������ز���
			if (cost < bestcost)
			{
/*   		printf("BE-t : %1.3f\t TriID: %d\t cost: %1.0f\t nthBE:%d\n",
				pBE->t,pBE->triangleID, cost, nthBE);*/
				updateBest(bestBE, nthBE, bestcost, cost, 
					nTriangles_L, nTriangles_R, (int)nBelow, (int)nAbove);
				isLeft = true;
			}
		}

	}
//	printf("bestBE: %d\n",bestBE);
	bestPosition = bestBE;
	bestT = edges[bestBE].t;
}

void WSimpleKD::markTriangles( 
			vector<BoundingEdge>& refEdges, 
			vector<int>& middleTriangleID,
			int bestBE,
			int nTriangles,
			int* triangleMark,
			bool isLeft)
{
	//��¼��������ӽڵ������������
	middleTriangleID.reserve(nTriangles);

	vector<BoundingEdge>::iterator pRefEdges,pTarEdges1,pTarEdges2;
	BoundingEdge splitEdge = refEdges[bestBE];

	//ȷ�������λ�ֵ��ĸ��ڵ�����
	//ͬһ�������ε�BE��Ȼ�ȳ���start���ٳ���end
	//ѭ����
	//ֻ����ߵ������ζ�Ӧ���Ϊ 2
	//ֻ���ұߵ������ζ�Ӧ���Ϊ-2
	//���ڵ�������ζ�Ӧ���Ϊ 0
	int ithEdge = 0;
	for (pRefEdges = refEdges.begin();
		pRefEdges != refEdges.end(); pRefEdges++, ithEdge++)
	{
/*		printf("triangleID: %d\tedgeType:%d\tT:%3.23f",pRefEdges->triangleID,pRefEdges->type,pRefEdges->t);*/
		//���ڿ�ʼ��
		if (pRefEdges->type == BoundingEdge::BE_START)
			if (ithEdge < bestBE)
			{
				//�����
//				printf(" left\tbegin\n");
				triangleMark[pRefEdges->triangleID] = 1;
			}
			else
			{
//				printf(" right\tbegin\n");
				//���ұߣ�˵�������������ڷָ�ƽ����ұ�
				triangleMark[pRefEdges->triangleID] = -1;
			}
		//���ڽ�����
		else if (pRefEdges->type == BoundingEdge::BE_END)
			if (ithEdge <= bestBE)
			{
//				printf(" left\tend\n");
				//����ߣ� ˵�������������ڷָ�ƽ������
				triangleMark[pRefEdges->triangleID] += 1;
			}
			else
			{
				//���ұ�
//				printf(" right\tend\n");
				triangleMark[pRefEdges->triangleID] -= 1;
				//�Ѻ��ڵ�������μ�¼����
				if (triangleMark[pRefEdges->triangleID] == 0)
					middleTriangleID.push_back(pRefEdges->triangleID);
			}
		//����ƽ��߸�������t�����Լ�isLeft�Ϳ���ȷ���ڷָ�ƽ�����һ��
		else
		{	//printf("\n");
			if (ithEdge < bestBE)
				triangleMark[pRefEdges->triangleID] = 2;
			else if (ithEdge > bestBE)
				triangleMark[pRefEdges->triangleID] = -2;
			else
				triangleMark[pRefEdges->triangleID] = isLeft ? 2 : -2;
		}
	}

	for (pRefEdges = refEdges.begin();
		pRefEdges != refEdges.end(); pRefEdges++)
	{
		//��ӡ������ı��
		if (triangleMark[pRefEdges->triangleID] != -2 &&
			triangleMark[pRefEdges->triangleID] != 0 &&
			triangleMark[pRefEdges->triangleID] != 2)
			printf("***************wrong triangleMark: %d, triangleID: %d\n",
				triangleMark[pRefEdges->triangleID],pRefEdges->triangleID);
	}
/*	for (int i = 0; i< middleTriangleID.size(); i++)
		printf("overlapping ID: %d\n",middleTriangleID[i]);*/
}

void WSimpleKD::drawTree( unsigned int nthBox/*=0*/, float R /*= 0.7*/, float G /*= 0.7*/, float B /*= 0.7*/ )
{
//	cout<<"\nnode size:"<<nodes.size()<<endl;
	glColor3f(R, G, B);
	drawTreeRecursive(0, sceneBox);
}

void WSimpleKD::drawTreeRecursive( int nthNode, const WBoundingBox& box )
{
	box.draw();
	if (!nodes.size())
		return;
	WBoundingBox leftBox, rightBox;
	leftBox = rightBox = box;
	WSKDNode* node = nodes[nthNode];
	switch(node->type)
	{
	case WSKDNode::KDN_LEAF:
		return;
	case WSKDNode::KDN_XSPLIT:
		leftBox.pMax.x = rightBox.pMin.x = ((WSKDInterior*)node)->splitPlane;
		break;
	case WSKDNode::KDN_YSPLIT:
		leftBox.pMax.y = rightBox.pMin.y = ((WSKDInterior*)node)->splitPlane;
		break;
	case WSKDNode::KDN_ZSPLIT:
		leftBox.pMax.z = rightBox.pMin.z = ((WSKDInterior*)node)->splitPlane;
		break;
	default:
		return;
	}
//	cout<<"left child: "<<((WSKDInterior*)node)->leftChild<<'\n'
//		<<"right child:"<<((WSKDInterior*)node)->rightChild<<endl;
	drawTreeRecursive(((WSKDInterior*)node)->leftChild, leftBox);
	drawTreeRecursive(((WSKDInterior*)node)->rightChild, rightBox);
}


void WSimpleKD::buildLeaf( WBoundingBox bBox, vector<BoundingEdge>& edges )
{
	WSKDLeaf* leaf = new WSKDLeaf;
	leaf->box[0] = bBox.pMin.x;
	leaf->box[1] = bBox.pMin.y;
	leaf->box[2] = bBox.pMin.z;
	leaf->box[3] = bBox.pMax.x;
	leaf->box[4] = bBox.pMax.y;
	leaf->box[5] = bBox.pMax.z;
	leaf->type = WSKDNode::KDN_LEAF;
	int nthTriangle = 0;
	for (unsigned int ithEdge = 0; 
		  ithEdge<edges.size(); 
		  ithEdge++)
	{
		if (edges[ithEdge].type == BoundingEdge::BE_START ||
			edges[ithEdge].type == BoundingEdge::BE_PLANAR)
		{
//			printf("leaf triangle ID: %d\n",edges[ithEdge].triangleID);
			leaf->triangleIDs[nthTriangle] = 
				edges[ithEdge].triangleID;
			nthTriangle = min(nthTriangle+1, WSKDNode::maxTrianglesPerLeaf-1);
		}
	}
	leaf->nTriangles = nthTriangle;
	nodes.push_back(leaf);
	nodeBoxes.push_back(bBox);
}

void WSimpleKD::getNewSortedBE( vector<int>& middleTriangleID,
						const WBoundingBox& clipBox, 
						vector<BoundingEdge>& middleBEX, 
						vector<BoundingEdge>& middleBEY, 
						vector<BoundingEdge>& middleBEZ )
{
	//Ԥ���ռ�
	int nNewEdges = middleTriangleID.size() * 2;
	middleBEX.reserve(nNewEdges);
	middleBEY.reserve(nNewEdges);
	middleBEZ.reserve(nNewEdges);

	//�������ڼ��еĳ�����
//	clipBox.displayCoords();
	WBoxClipper clipper(clipBox);
	BoundingEdge startEdge, endEdge;
	startEdge.type = BoundingEdge::BE_START;
	endEdge.type = BoundingEdge::BE_END;

	vector<int>::iterator pTriID;
	for(pTriID = middleTriangleID.begin();
		pTriID != middleTriangleID.end(); pTriID++)
	{
		WTriangle tri;
		tri.point1 = triangles[*pTriID].point1;
		tri.point2 = triangles[*pTriID].point2;
		tri.point3 = triangles[*pTriID].point3;

		//��ü��к��Ӧ�İ�Χ��
		WBoundingBox box;
		if (/*clipper.getClipTriangleBox(tri, box)*/1)
		{
			//�����л���ʣ�࣬�½�һ��BE
			addEdge(middleBEX, box.pMin.x, box.pMax.x, *pTriID);
			addEdge(middleBEY, box.pMin.y, box.pMax.y, *pTriID);
			addEdge(middleBEZ, box.pMin.z, box.pMax.z, *pTriID);
		}
		else
		{
			//��ʾû���ཻ�����
			//��������������ζ�����Χ���ཻ���������������
			cout<<"#####WTriangle#####"<<endl;
			printf("TriangleID: %d\n",*pTriID);
			tri.showVertexCoords();
			clipBox.displayCoords();
		}
	}
	//�Ա�����
	sort(middleBEX.begin(), middleBEX.end());
	sort(middleBEY.begin(), middleBEY.end());
	sort(middleBEZ.begin(), middleBEZ.end());
}

void WSimpleKD::mergeBE( vector<BoundingEdge>& oldEdge, 
					   vector<BoundingEdge>& middleEdge_L,
					   vector<BoundingEdge>& middleEdge_R,
					   vector<BoundingEdge>& newEdge_L,
					   vector<BoundingEdge>& newEdge_R,
					   int nTriangles_L, int nTriangles_R, 
					   int* triangleMark )
{
	newEdge_L.clear();newEdge_R.clear();
	newEdge_L.reserve(nTriangles_L * 2);
	newEdge_R.reserve(nTriangles_R * 2);

	vector<BoundingEdge>::iterator pOldEdge, pMiddleEdge_L, pMiddleEdge_R,
								   pNewEdge_L, pNewEdge_R;
	
	pOldEdge = oldEdge.begin();
	pMiddleEdge_L = middleEdge_L.begin();
	pMiddleEdge_R = middleEdge_R.begin();

	bool isOldTerminated ,isLeftTerminated, isRightTerminated;
	isOldTerminated = isLeftTerminated = isRightTerminated = false;
	if (middleEdge_L.size() == 0)
		isLeftTerminated = true;
	if (middleEdge_R.size() == 0)
		isRightTerminated = true;
	if (oldEdge.size() == 0)
		isOldTerminated = true;

	//�ϲ��´����ıߣ�middleEdge����ԭ�еıߣ�oldEdge��
	//���ڷָ������ڰ�˳�����ԭ����BEʱ��
	//��Ȼ���ȳ��ֶ�Ӧ�����α��Ϊ2��BE���ٳ��ֱ��Ϊ-2��BE
	//���Ƕ����������ߣ���û��������ɣ�����ڵ��BE��������tֵ�ϴ��������ҽڵ��BE����
	//��ˣ��������ҽڵ㶼��Ҫ��ԭ����BE��ͷ��βɨ��

	//������ڵ��BE
	for (int nthLeftEdge = 0; nthLeftEdge < nTriangles_L * 2; nthLeftEdge++)
	{
		//�������õĽڵ�
		if(!isOldTerminated)
		{	
			while (triangleMark[pOldEdge->triangleID] != 2 )
			{
//				printf("triangleMark: %d\n",triangleMark[pOldEdge->triangleID]);
				if (pOldEdge + 1 != oldEdge.end())
					++pOldEdge;
				else
				{
					isOldTerminated = true;
					break;
				}
			}
		}
		//���oldEdge��middleEdge��û����ֹ�������С��һ��
		if (!isOldTerminated && !isLeftTerminated)
			if(*pOldEdge < *pMiddleEdge_L)
			{
				newEdge_L.push_back(*pOldEdge);
				if (pOldEdge + 1 != oldEdge.end())
					++pOldEdge;
				else
					isOldTerminated = true;
			}
			else
			{
				newEdge_L.push_back(*pMiddleEdge_L);
				if (pMiddleEdge_L + 1 != middleEdge_L.end())
					++pMiddleEdge_L;
				else
					isLeftTerminated = true;
			}
		//���middleEdge��ֹ�ˣ�����oldEdgeû��ֹ������oldEdge
		else if (!isOldTerminated)
		{
			newEdge_L.push_back(*pOldEdge);
			if (pOldEdge + 1 != oldEdge.end())
				++pOldEdge;
			else
				isOldTerminated = true;
		}
		//���oldEdge��ֹ�ˣ�����middleEdgeû��ֹ������middleEdge
		else if (!isLeftTerminated)
		{
			newEdge_L.push_back(*pMiddleEdge_L);
			if (pMiddleEdge_L + 1 != middleEdge_L.end())
				++pMiddleEdge_L;
			else
				isLeftTerminated = true;
		}
	}
	//�����ҽڵ��BE
	pOldEdge = oldEdge.begin();
	isOldTerminated = false;
	if (oldEdge.size() == 0)
		isOldTerminated = true;
	for (int nthRightEdge = 0; nthRightEdge < nTriangles_R * 2; nthRightEdge++)
	{
		//�������õĽڵ�
		if (!isOldTerminated)
		{
			while (triangleMark[pOldEdge->triangleID] != -2 )
			{
				if (pOldEdge + 1 != oldEdge.end())
					++pOldEdge;
				else
				{
					isOldTerminated = true;
					break;
				}
			}
		}
		//���oldEdge��middleEdge��û����ֹ�������С��һ��
		if (!isOldTerminated && !isRightTerminated)
			if(*pOldEdge < *pMiddleEdge_R)
			{
				newEdge_R.push_back(*pOldEdge);
				if (pOldEdge + 1 != oldEdge.end())
					++pOldEdge;
				else
					isOldTerminated = true;
			}
			else
			{
				newEdge_R.push_back(*pMiddleEdge_R);
				if (pMiddleEdge_R + 1 != middleEdge_R.end())
					++pMiddleEdge_R;
				else
					isRightTerminated = true;
			}
		//���middleEdge��ֹ�ˣ�����oldEdgeû��ֹ������oldEdge
		else if (!isOldTerminated)
		{
			newEdge_R.push_back(*pOldEdge);
			if (pOldEdge + 1 != oldEdge.end())
				++pOldEdge;
			else
				isOldTerminated = true;
		}
		//���oldEdge��ֹ�ˣ�����middleEdgeû��ֹ������middleEdge
		else if (!isRightTerminated)
		{
			newEdge_R.push_back(*pMiddleEdge_R);
			if (pMiddleEdge_R + 1 != middleEdge_R.end())
				++pMiddleEdge_R;
			else
				isRightTerminated = true;
		}
	}
	
/*	printf("###########################\n");
	for (pOldEdge = oldEdge.begin();
		pOldEdge != oldEdge.end(); pOldEdge++)
	{
		printf(" triangleOldID: %d\t triangleMark:%d\n",pOldEdge->triangleID,triangleMark[pOldEdge->triangleID]);
	}*/
/*
	for (pNewEdge_L = newEdge_L.begin();
		pNewEdge_L != newEdge_L.end(); pNewEdge_L++)
	{
		printf(" triangleID_L: %d\t triangleMark:%d\t edgeType:%d\n",
			pNewEdge_L->triangleID,triangleMark[pNewEdge_L->triangleID],
			(int)pNewEdge_L->type);
	}
	for (pNewEdge_R = newEdge_R.begin();
		pNewEdge_R != newEdge_R.end(); pNewEdge_R++)
	{
		printf(" triangleID_R: %d\t triangleMark:%d\t edgeType:%d\n",
			pNewEdge_R->triangleID,triangleMark[pNewEdge_R->triangleID],
			(int)pNewEdge_R->type);
	}*/
}

void WSimpleKD::buildBasicRopes(int ithNode, 
							   int ropeX_P, int ropeX_N,
							   int ropeY_P, int ropeY_N,
							   int ropeZ_P, int ropeZ_N)
{
	WSKDNode* pNode = nodes[ithNode];
	if (pNode->type == WSKDNode::KDN_LEAF)
	{
		WSKDLeaf* pLeaf = (WSKDLeaf*)pNode;
		pLeaf->ropes[0] = ropeX_P;
		pLeaf->ropes[1] = ropeX_N;
		pLeaf->ropes[2] = ropeY_P;
		pLeaf->ropes[3] = ropeY_N;
		pLeaf->ropes[4] = ropeZ_P;
		pLeaf->ropes[5] = ropeZ_N;
	} 
	else if(pNode->type == WSKDLeaf::KDN_XSPLIT)
	{
		WSKDInterior* pInterior = (WSKDInterior*)pNode;
		buildBasicRopes(pInterior->leftChild,
			pInterior->rightChild, ropeX_N, ropeY_P, ropeY_N, ropeZ_P, ropeZ_N);
		buildBasicRopes(pInterior->rightChild,
			ropeX_P, pInterior->leftChild, ropeY_P, ropeY_N, ropeZ_P, ropeZ_N);
	}
	else if(pNode->type == WSKDLeaf::KDN_YSPLIT)
	{
		WSKDInterior* pInterior = (WSKDInterior*)pNode;
		buildBasicRopes(pInterior->leftChild,
			ropeX_P, ropeX_N, pInterior->rightChild, ropeY_N, ropeZ_P, ropeZ_N);
		buildBasicRopes(pInterior->rightChild,
			ropeX_P, ropeX_N, ropeY_P, pInterior->leftChild, ropeZ_P, ropeZ_N);
	}
	else if(pNode->type == WSKDLeaf::KDN_ZSPLIT)
	{
		WSKDInterior* pInterior = (WSKDInterior*)pNode;
		buildBasicRopes(pInterior->leftChild,
			ropeX_P, ropeX_N, ropeY_P, ropeY_N, pInterior->rightChild, ropeZ_N);
		buildBasicRopes(pInterior->rightChild,
			ropeX_P, ropeX_N, ropeY_P, ropeY_N, ropeZ_P, pInterior->leftChild);
	}
}


bool WSimpleKD::BoundingEdge::operator<( const BoundingEdge&e ) const
{
	//��ͬλ��, tֵ��С�Ľ�ǰ
	if (t < e.t)
		return true;
	if (t > e.t)
		return false;
	//����Ϊ t ���ʱ������
	//���� t ���ʱ��ȷ�� planar���͵���ǰ��start���͵Ľ�ǰ�� end ���͵ĽϺ�
	//������ȷ���ڱ�������ε�ʱ��������
	//һ�������ε�start�ߣ�������end��
	if (type < e.type)
		return true;
	if (type > e.type)
		return false;
	//���� t ���, ������ȵ�BE
	//����triangleID����
	return triangleID < e.triangleID;
}

bool WSimpleKD::isIntersect( WRay& r, int beginNode)
{
	//	printf("************************\n");
	WVector3 entryPoint;
//	beginNode = 0;
	for (int i = 0; i < 3; i++)
	{
		//ȥ��-0.0f�Ŀ���
		r.direction.v[i] = r.direction.v[i] == 0.0f ? 0.0f : r.direction.v[i];
	}
	if (sceneBox.isInBoxInclusive(r.point))
		entryPoint = r.point;
	else
	{
		float tMin, tMax;
		if (sceneBox.isIntersect(r, tMin, tMax))
		{
			//�ҵ��볡����Χ�еĽ���
			entryPoint = r.point + tMin* r.direction;
		} 
		//�볡����Χ�в��ཻ��ֱ�ӷ���
		else
			return false;
	}
	int currNodeID = beginNode, bestTriID = -1;
	while (currNodeID != -1)
	{
//		cout<<"currNodeID:" <<currNodeID<<endl;
		//�ڲ��ڵ�
/*		glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex3f(entryPoint.x, entryPoint.y, entryPoint.z);
		glEnd();*/
		if (nodes[currNodeID]->type != WSKDNode::KDN_LEAF)
		{
			WSKDInterior interior = *((WSKDInterior*)nodes[currNodeID]);
			currNodeID = 
				entryPoint.v[interior.type] < interior.splitPlane ? 
				interior.leftChild : interior.rightChild;
		} 
		//Ҷ�ڵ�
		else
		{
			float farT;
			WSKDLeaf& leaf = *((WSKDLeaf*)nodes[currNodeID]);
			int nextNodeID = computeExitFace(leaf, r, entryPoint, farT);
			//�ҳ��ڵ��������������
			for (int ithTri = 0; ithTri < leaf.nTriangles; ithTri++)
			{
//				cout<<"leaf triangleID "<<leaf.triangleIDs[ithTri]<<endl;
//				triangles[pNode->triangleIDs[ithTri]].showVertexCoords();
				float currT = triangles[leaf.triangleIDs[ithTri]].intersectTest(r);/*
				glColor3f(rand()/32767.0f, 1.0f,1.0f);
				triangles[leaf.triangleIDs[ithTri]].draw(false,true);*/
				if (currT <= r.tMax && currT > r.tMin)
				{
					//					triangles[leaf.triangleIDs[ithTri]].draw(false,true);
					return true;
					//					cout<<"update triangleID:"<<bestTriID<<endl;
				}
			}
			//���û�н��㣬ͨ��rope������һ���ڵ�
			currNodeID = nextNodeID;
		}
	}
	return false;

}

bool WSimpleKD::intersect( WRay& r,WDifferentialGeometry& DG, 
						 int* endNode, int beginNode)
{
	//	printf("************************\n");
//	beginNode = 0;
	WVector3 entryPoint;
	//ȥ��-0.0f�Ŀ���
	r.direction.v[0] = r.direction.v[0] == 0.0f ? 0.0f : r.direction.v[0];
	r.direction.v[1] = r.direction.v[1] == 0.0f ? 0.0f : r.direction.v[1];
	r.direction.v[2] = r.direction.v[2] == 0.0f ? 0.0f : r.direction.v[2];
	
	if (sceneBox.isInBoxInclusive(r.point))
		entryPoint = r.point;
	else
	{
		float tMin, tMax;
		if (sceneBox.isIntersect(r, tMin, tMax))
		{
			//�ҵ��볡����Χ�еĽ���
//			tMin = max(tMin, 0.0f);
			entryPoint = r.point + tMin* r.direction;
		} 
		//�볡����Χ�в��ཻ��ֱ�ӷ���
		else
			return false;
	}
	int currNodeID = beginNode, bestTriID = -1;
	float bestT = M_INF_BIG;
	while (currNodeID != -1)
	{
//		cout<<"currNodeID:" <<currNodeID<<endl;
		//�ڲ��ڵ�
		/*
		glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex3f(entryPoint.x, entryPoint.y, entryPoint.z);
		glEnd();*/
		if (nodes[currNodeID]->type != WSKDNode::KDN_LEAF)
		{
			WSKDInterior interior = *((WSKDInterior*)nodes[currNodeID]);
			currNodeID = 
				entryPoint.v[interior.type] < interior.splitPlane ? 
				interior.leftChild : interior.rightChild;
		} 
		//Ҷ�ڵ�
		else
		{
			WSKDLeaf& leaf = *((WSKDLeaf*)nodes[currNodeID]);
			float farT;
			int nextNodeID = computeExitFace(leaf, r, entryPoint, farT);
			//�ҳ��ڵ��������������
			for (int ithTri = 0; ithTri < leaf.nTriangles; ithTri++)
			{
//				cout<<"leaf triangleID "<<leaf.triangleIDs[ithTri]<<endl;
//				triangles[pNode->triangleIDs[ithTri]].showVertexCoords();
				float currT = triangles[leaf.triangleIDs[ithTri]].intersectTest(r);
/*				glColor3f(rand()/32767.0f, 1.0f,1.0f);
				triangles[leaf.triangleIDs[ithTri]].draw(false,true);*/
				if (currT < bestT)
				{
//					triangles[leaf.triangleIDs[ithTri]].draw(false,true);
					bestT = currT;
					bestTriID = leaf.triangleIDs[ithTri];
//					cout<<"update triangleID:"<<bestTriID<<endl;
				}
			}
			//��������ڽڵ��ڲ����󽻳ɹ�
			if (bestT <= farT)
			{
				if (endNode)
					*endNode = currNodeID;
				goto INTERSECT_END;
			}
			//��������ڽڵ��⣬����û�н��㣬ͨ��rope������һ���ڵ�
			else
			{
				currNodeID = nextNodeID;
			}
		}
	}
INTERSECT_END:
	if (bestTriID == -1)
	{
		return false;
	} 
	else
	{
//		cout<<"bestTriID:"<<bestTriID<<endl;
		triangles[bestTriID].intersect(r,DG);
		return true;
	}
}

int WSimpleKD::computeExitFace( WSKDLeaf& node, const WRay& r, WVector3& exitPoint, float& farT)
{
	float tFar[3];
	//�����߷������Ϊ0.0f(֮ǰ�Ѿ���-0.0fת����)ʱ��ȷ����Ӧ�����tֵΪ������
	tFar[0] = (node.box[(r.direction.x >= 0.0f) * 3] - r.point.x) / 
		r.direction.x;
	tFar[1] = (node.box[(r.direction.y >= 0.0f) * 3 + 1] - r.point.y) / 
		r.direction.y;
	tFar[2] = (node.box[(r.direction.z >= 0.0f) * 3 + 2] - r.point.z) / 
		r.direction.z;

	//�ҳ����������
	int bestAxis = tFar[0] < tFar[1] ? 
		(tFar[0] < tFar[2] ? 0 : 2) :
		(tFar[1] < tFar[2] ? 1 : 2);
	farT = tFar[bestAxis];
	exitPoint = r.point + farT * r.direction;
	exitPoint.v[bestAxis] = node.box[(r.direction.v[bestAxis] >= 0.0f) * 3 + bestAxis];

	return node.ropes[bestAxis * 2 + (r.direction.v[bestAxis] < 0.0f)];

/*
	//��box������������
	WVector3 invDir = 1.0f / r.direction;
	float bestT;
	if (box.pMin.x < box.pMax.x && box.pMin.y < box.pMax.y && 
		box.pMin.z < box.pMax.z)
	{		
		float tmax = (box.pMax.x - r.point.x) * invDir.x;
		float tmin = (box.pMin.x - r.point.x) * invDir.x;
		bestT = max(tmin, tmax);
		exitFace = (tmax > tmin) ? 0 : 1;
		tmax = (box.pMax.y - r.point.y) * invDir.y;
		tmin = (box.pMin.y - r.point.y) * invDir.y;
		if (max(tmax, tmin) < bestT)
		{
			exitFace = (tmax > tmin) ? 2 : 3;
			bestT = max(tmax, tmin);
		}
		tmax = (box.pMax.z - r.point.z) * invDir.z;
		tmin = (box.pMin.z - r.point.z) * invDir.z;
		if (max(tmax, tmin) < bestT)
		{
			exitFace = (tmax > tmin) ? 4 : 5;
			bestT = max(tmax, tmin);
		}
		exitPoint = r.point + r.direction * bestT;
		return;
	}
	else if(box.pMin.x >= box.pMax.x)
	{
		exitFace = r.direction.x > 0 ? 0 : 1;
		float bestT = (box.pMax.x - r.point.x) * invDir.x;
		exitPoint = r.point + r.direction * bestT;
	}
	else if(box.pMin.y >= box.pMax.y)
	{
		exitFace = r.direction.y > 0 ? 2 : 3;
		float bestT = (box.pMax.y - r.point.y) * invDir.y;
		exitPoint = r.point + r.direction * bestT;
	}
	else if(box.pMin.z >= box.pMax.z)
	{
		exitFace = r.direction.z > 0 ? 4 : 5;
		float bestT = (box.pMax.z - r.point.z) * invDir.z;
		exitPoint = r.point + r.direction * bestT;
	}*/
}

bool WSimpleKD::isOverlap( WBoundingBox& refBox, 
						 WBoundingBox& testBox, 
						 int refFace )
{
	switch (refFace)
	{
	case 0:
		return testBox.pMin.z <= refBox.pMin.z &&
			   testBox.pMin.y <= refBox.pMin.y &&
			   testBox.pMax.z >= refBox.pMax.z &&
			   testBox.pMax.y >= refBox.pMax.y &&
			   refBox.pMax.x == testBox.pMin.x;
		break;
	case 1:
		return testBox.pMin.z <= refBox.pMin.z &&
			testBox.pMin.y <= refBox.pMin.y &&
			testBox.pMax.z >= refBox.pMax.z &&
			testBox.pMax.y >= refBox.pMax.y &&
			refBox.pMin.x == testBox.pMax.x;
		break;
	case 2:
		return testBox.pMin.z <= refBox.pMin.z &&
			testBox.pMin.x <= refBox.pMin.x &&
			testBox.pMax.z >= refBox.pMax.z &&
			testBox.pMax.x >= refBox.pMax.x &&
			refBox.pMax.y == testBox.pMin.y;
		break;
	case 3:
		return testBox.pMin.z <= refBox.pMin.z &&
			testBox.pMin.x <= refBox.pMin.x &&
			testBox.pMax.z >= refBox.pMax.z &&
			testBox.pMax.x >= refBox.pMax.x &&
			refBox.pMin.y == testBox.pMax.y;
		break;
	case 4:
		return testBox.pMin.y <= refBox.pMin.y &&
			testBox.pMin.x <= refBox.pMin.x &&
			testBox.pMax.y >= refBox.pMax.y &&
			testBox.pMax.x >= refBox.pMax.x &&
			refBox.pMax.z == testBox.pMin.z;
		break;
	case 5:
		return testBox.pMin.y <= refBox.pMin.y &&
			testBox.pMin.x <= refBox.pMin.x &&
			testBox.pMax.y >= refBox.pMax.y &&
			testBox.pMax.x >= refBox.pMax.x &&
			refBox.pMin.z == testBox.pMax.z;
		break;
	}
	return false;
}

int WSimpleKD::checkFace( WBoundingBox& refBox, int& rope , int refFace)
{
	//����ָ��յ�rope��ֱ�ӷ���
	if (rope == -1)
		return rope;
	int currNode = rope;
//	printf("currRope: %d\n", rope);
	while (1)
	{
		//����Ҷ�ڵ㣬��������
		if (nodes[currNode]->type == WSKDNode::KDN_LEAF)
		{
			return rope;
		} 
		else
		{
			WSKDInterior* pNode = (WSKDInterior*)nodes[currNode];
			int nOverlap = 0;
			//������ڵ���û���غϵ���
			if (isOverlap(refBox, nodeBoxes[pNode->leftChild], refFace))
			{
				nOverlap++;		currNode = pNode->leftChild;
			} 
			//�����ҽڵ���û���غϵ���
			if (isOverlap(refBox, nodeBoxes[pNode->rightChild], refFace))
			{
				nOverlap++;		currNode = pNode->rightChild;
			}
			//����������ڵ㶼�غϣ�˵����һ���ڵ�����Ϊ0
			if (nOverlap == 2)
			{
				if ((refFace & 0x01) == 1)
				{
					currNode = pNode->rightChild;
				} 
				else
				{
					currNode = pNode->rightChild;
				}
			}
			//��������ӽڵ㶼û����ȫ�غϵ��棬ֱ�ӷ��أ����޸�rope
			if (nOverlap == 0)
				return rope;
			//�����rope�޸�Ϊ�ӽڵ������
			rope = currNode;
		}

//		printf("newRope: %d\n", rope);
	}
}

void WSimpleKD::drawTriangles()
{
	vector<WSKDNode*>::iterator pNode;
	for (pNode = nodes.begin(); pNode != nodes.end(); ++pNode)
	{
		if ((*pNode)->type == WSKDNode::KDN_LEAF)
		{
			WSKDLeaf* pLeaf = (WSKDLeaf*)(*pNode);
			for (int ithTriangle = 0; ithTriangle < pLeaf->nTriangles; 
				ithTriangle++)
			{
				triangles[pLeaf->triangleIDs[ithTriangle]].draw();
			}
		}
	}
}

void WSimpleKD::buildExtendedRopes()
{
	vector<WSKDNode*>::iterator pNode;
	int ithNode = 0;
	printf("nodeBoxes: %d\n", nodeBoxes.size());
	for (pNode = nodes.begin(); pNode != nodes.end(); pNode++, ithNode++)
	{
		if ((*pNode)->type == WSKDNode::KDN_LEAF)
		{
			WSKDLeaf* pLeaf = (WSKDLeaf*)(*pNode);
			checkFace(nodeBoxes[ithNode], pLeaf->ropes[0], 0);
			checkFace(nodeBoxes[ithNode], pLeaf->ropes[1], 1);
			checkFace(nodeBoxes[ithNode], pLeaf->ropes[2], 2);
			checkFace(nodeBoxes[ithNode], pLeaf->ropes[3], 3);
			checkFace(nodeBoxes[ithNode], pLeaf->ropes[4], 4);
			checkFace(nodeBoxes[ithNode], pLeaf->ropes[5], 5);
		}
	}
}