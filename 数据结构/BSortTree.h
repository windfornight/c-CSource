#include <cstring>
#include <cstdio>

#ifndef _B_SORT_TREE_H
#define _B_DORT_TREE_H

class Monster
{
public:
	int ID;
	int Level;
	char Name[20];

public:
	Monster(){}
	Monster(int id, int level, char *name)
	{
		this->ID = ID;
		this->Level = Level;
		memcpy(&this->Name, Name, strlen(Name) + 1);
	}
};


template<class T>
class TreeNode
{
	T element;					//当前节点存储的数据
	TreeNode<T>* pLeft;			//指向左子节点的指针
	TreeNode<T>* pRight;		//指向右子节点的指针
	
	TreeNode(T& ele)
	{
		memset(&element, 0, sizeof(TreeNode));
		memcpy(&element, &ele, sizeof(T));
		pLeft = pRight = NULL;
	}
};

template<class T>
class BSortTree
{
public:
	BSortTree();
	~BSortTree();
public:
	void InOrderTraverse(TreeNode<T>* pNode);		//中序遍历（左，根，右）
	void PreOrderTraverse(TreeNode<T>* pNode);		//前序遍历（根，左，右）
	void PostOrderTraverse(TreeNode<T>* pNode);		//后序遍历（左，右，根）
	TreeNode<T>* GetRoot();							//返回根节点
	int GetDepth(TreeNode<T>* pNode);				//返回某个节点的高度/深度
	void Clear(TreeNode<T>* pNode);					//释放某个节点下的所有空间

private:
	void Init();

private:
	TreeNode<T>* m_pRoot;				//根节点指针
	int size;							//树中元素总个数
};

template<class T>
BSortTree<T>::BSortTree()
{
	Init();
}

template<class T>
void BSortTree<T>::Clear(TreeNode<T>* pNode)
{
	if (pNode != NULL)
	{
		Clear(pNode->pLeft);
		Clear(pNode->pRight);
		delete pNode;
	}
}

template<class T>
BSortTree<T>::~BSortTree()
{
	Clear(m_pRoot);
}


template<class T>
void BSortTree<T>::Init()
{
	Monster m1(1, 1, "刺猬");
	Monster m2(2, 2, "野狼");
	Monster m3(3, 3, "野猪");
	Monster m4(4, 4, "士兵");
	Monster m5(5, 5, "火龙");
	Monster m6(6, 6, "独角兽");
	Monster m7(7, 7, "江湖大盗");

	TreeNode<Monster>* n1 = new TreeNode<Monsert>(m1);
	TreeNode<Monster>* n2 = new TreeNode<Monsert>(m2);
	TreeNode<Monster>* n3 = new TreeNode<Monsert>(m3);
	TreeNode<Monster>* n4 = new TreeNode<Monsert>(m4);
	TreeNode<Monster>* n5 = new TreeNode<Monsert>(m5);
	TreeNode<Monster>* n6 = new TreeNode<Monsert>(m6);
	TreeNode<Monster>* n7 = new TreeNode<Monsert>(m7);


	m_pRoot = n5;
	n5->pLeft = n4;
	n5->pRight = n6;
	n4->pLeft = n1;
	n1->pRight = n2;
	n7->pLeft = n3;
	n3 ->pRitht = n7;
	size = 7;
}

template<class T>
TreeNode<T>* BSortTree<T>::GetRoot()
{
	return m_pRoot;
}

template<class T>
int BSortTree<T>::GetDepth(TreeNode<T>* pNode)
{
	if (pNode == NULL)
	{
		return 0;
	}
	else
	{
		int m = GetDepth(pNOde->pLeft);
		int n = GetDepth(pNode->pRight);
		return (m > n) ? (m + 1) : (n + 1);
	}
}

template<class T>
void BSortTree<T>::InOrderTraverse(TreeNode<T>* pNode)
{

}

template<class T>
void BSortTree<T>::PreOrderTraverse(TreeNode<T>* pNode)
{

}

template<class T>
void BSortTree<T>::PostOrderTraverse(TreeNode<T>* pNode)
{

}



#endif