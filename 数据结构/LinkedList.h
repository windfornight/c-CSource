#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#define SUCCESS 1
#define ERROR -1
#define INDEX_IS_ERROR -2
#define BUFFER_IS_EMPTY -3

template<class T_ELE>
class LinkedList
{
public:
	LinkedList();
	~LinkedList();
public:
	bool isEmpty();								//�ж������Ƿ�Ϊ��
	void clear();								//�������
	int getElement(int index, T_ELE& element);	//����������ȡԪ��
	int getElementIndex(T_ELE& Element);		//����Ԫ�ػ�ȡ�����е�����
	int insert(T_ELE element);					//����Ԫ��
	int insert(int index, T_ELE element);		//������������Ԫ��
	int deleteElement(int index);				//��������ɾ��Ԫ��
	int getSize();								//��ȡ������Ԫ�ص�����

private:
	typedef struct _NODE
	{
		T_ELE Data;
		_NODE *pNext;
	}NODE, *PNODE;

	PNODE getIndexCurrentNode(int index);		//��ȡ����Ϊindex��ָ��
	PNODE getIndexPreviousNode(int index);		//��ȡǰһ���ڵ�ָ��
	PNODE getIndexNextNode(int Index);			//��ȡ��һ���ڵ�ָ��

private:
	PNODE mPtrList;								//����ָ�룬ֻ���һ���ڵ�
	int mLen;									//Ԫ��ɫ����
};

//�޲ι��캯������ʼ����Ա
template<class T_ELE>
LinkedList<T_ELE>::LinkedList():mPtrList(NULL), mLen(0)
{

}

//�������������Ԫ��
template<class T_ELE>
LinkedList<T_ELE>::~LinkedList()
{
	clear();
}

//�ж������Ƿ�Ϊ��
template<class T_ELE>
bool LinkedList<T_ELE>::isEmpty()
{

}

//�������
template<class T_ELE> 
void LinkedList<T_ELE>::clear()
{
	//1.�ж������Ƿ�Ϊ��

	//2.ѭ��ɾ�������еĽڵ�

	//3.ɾ�����һ���ڵ㲢���ı�����Ϊ0
}


//����������ȡԪ��
template<class T_ELE>
int LinkedList<T_ELE>::getElement(int index, T_ELE& element)
{
	//1.�ж������Ƿ���Ч

	//2.ȥ������ָ��Ľڵ�

	//3.����Ӱָ����޵�ֵ���Ƶ�out����
}


//����Ԫ�����ݻ�ȡ����
template<class T_ELE>
int LinkedList<T_ELE>::getElementIndex(T_ELE& element)
{
	//1.�ж��ı��Ƿ�Ϊ��

	//2.ѭ�����������ҵ���element��ͬ��Ԫ��
}

//������β�������ڵ�
template<class T_ELE>
int LinkedList<T_ELE>::insert(T_ELE element)
{
	//1.�ж������Ƿ�Ϊ��

	//2.����������Ѿ���Ԫ��
}

//���ڵ�������ָ��������λ��
template<class T_ELE>
int LinkedList<T_ELE>::insert(int index, T_ELE element)
{
	//1.�ж������Ƿ�Ϊ��

	//2.�ж�����ֵ�Ƿ���Ч

	//3.�������Ϊ0

	//4.�������Ϊ����β

	//5.�������Ϊ������
}

//��������ɾ���ڵ�
template<class T_ELE>
int LinkedList<T_ELE>::deleteElement(int index)
{
	//1.�ж������Ƿ�Ϊ��

	//2.�ж�����ֵ�Ƿ���Ч

	//3.���������ֻ��ͷ��㣬��Ҫɾ��ͷ�ڵ�

	//4.���Ҫɾ��ͷ�ڵ�

	//5.������������
}

//��ȡ�����нڵ������
template<class T_ELE>
int LinkedList<T_ELE>::getSize()
{

}

//��ȡindexǰ��ڵ�ĵ�ַ
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexPreviousNode(int index)
{
	//����һ��ѭ��
}

//��ȡindex�ڵ�ĵ�ַ
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexCurrentNode(int index)
{

}


//��ȡindex����ڵ�ĵ�ַ
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexNextNode(int index)
{

}



#endif