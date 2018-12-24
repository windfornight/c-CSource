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
	PNODE mPtrList;								//����ָ�룬ָ���һ���ڵ�
	int mLen;									//Ԫ�ص�����
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
	return (mLen == 0);
}

//�������
template<class T_ELE> 
void LinkedList<T_ELE>::clear()
{
	//1.�ж������Ƿ�Ϊ��
	if (isEmpty())
	{
		return ;
	}

	//2.ѭ��ɾ�������еĽڵ�
	while (mPtrList)
	{
		LinkedList<T_ELE>::PNODE tmp = mPtrList;
		mPtrList = mPtrList->pNext;
		delete tmp;
	}

	//3.ɾ�����һ���ڵ㲢���ı�����Ϊ0
	mPtrList = NULL;
	mLen = 0;

}


//����������ȡԪ��
template<class T_ELE>
int LinkedList<T_ELE>::getElement(int index, T_ELE& element)
{
	//1.�ж������Ƿ���Ч
	if (index < 0 || index >= mLen)
	{
		return INDEX_IS_ERROR;
	}

	//2.ȥ������ָ��Ľڵ�
	LinkedList<T_ELE>::PNODE ptr = mPtrList;
	int idx = 0;
	while (idx < index)
	{
		ptr = ptr->pNext;
		idx++;
	}

	//3.����Ӱָ����޵�ֵ���Ƶ�out����
	memcpy(&element, &ptr->Data, sizeof(T_ELE));

	return SUCCESS;
}


//����Ԫ�����ݻ�ȡ����
template<class T_ELE>
int LinkedList<T_ELE>::getElementIndex(T_ELE& element)
{
	//1.�ж��ı��Ƿ�Ϊ��
	if (isEmpty())
	{
		return -1;
	}

	//2.ѭ�����������ҵ���element��ͬ��Ԫ��
	int index = 0;
	LinkedList<T_ELE>::PNODE ptr = mPtrList;
	while (ptr)
	{
		if (memcmp(&(ptr->Data), &element, sizeof(T_ELE)) == 0)
		{
			return index;
		}
		++index;
		ptr = ptr->pNext;
	}
	return -1;
}

//������β�������ڵ�
template<class T_ELE>
int LinkedList<T_ELE>::insert(T_ELE element)
{
	//1.�ж������Ƿ�Ϊ��
	if (isEmpty())
	{
		LinkedList<T_ELE>::PNODE tmp = new NODE;
		tmp->pNext = NULL;
		memcpy(tmp->Data, &element, sizeof(T_ELE));
		mPtrList = tmp;
		mLen++;
		return 1;
	}

	//2.����������Ѿ���Ԫ��
	if (getElementIndex(element) >= 0)
	{
		return 0;
	}

	LinkedList<T_ELE>::PNODE ptrLast = mPtrList;
	while (ptrLast->pNext)
	{
		ptrLast = ptrLast->pNext;
	}

	LinkedList<T_ELE>::PNODE tmp = new NODE;
	tmp->pNext = NULL;
	memcpy(tmp->Data, &element, sizeof(T_ELE));
	ptrLast->pNext = tmp;
	mLen++;
	return 1;
}

//���ڵ�������ָ��������λ��
template<class T_ELE>
int LinkedList<T_ELE>::insert(int index, T_ELE element)
{
	//1.�ж�����ֵ�Ƿ���Ч
	if (index < 0 || index > mLen)
	{
		return ERROR;
	}

	//2.�ж������Ƿ�Ϊ��
	
	LinkedList<T_ELE>::PNODE tmp = new NODE;
	tmp->pNext = NULL;
	memcpy(&tmp->Data; &element, sizeof(T_ELE));
	
	//3.�������Ϊ0
	if (index == 0)
	{
		tmp->pNext = mPtrList;
		mPtrList = tmp;
	}
	else if (index == mLen)  //4.�������Ϊ����β
	{
		LinkedList<T_ELE>::PNODE ptrLast = getIndexCurrentNode(index - 1);
		ptrLast->pNext = tmp;
	}
	else //5.�������Ϊ������
	{
		LinkedList<T_ELE>::PNODE preNode = getIndexPreviousNode(index);
		LinkedList<T_ELE>::PNODE nextNode = getIndexCurrentNdoe(index);
		preNode->pNext = tmp;
		tmp->pNext = nextNode;
	}

	mLen++;
	return SUCCESS;
}

//��������ɾ���ڵ�
template<class T_ELE>
int LinkedList<T_ELE>::deleteElement(int index)
{
	//1.�ж������Ƿ�Ϊ��
	if (isEmpty())
	{
		return INDEX_IS_ERROR;
	}

	//2.�ж�����ֵ�Ƿ���Ч
	if (index < 0 || index >= mLen)
	{
		return INDEX_IS_ERROR;
	}

	LinkedList<T_ELE>::PNODE delNode = getIndexCurrentNode(index);
	LinkedList<T_ELE>::PNODE preNode = getIndexPreviousNode(index);
	LinkedList<T_ELE>::PNODE nextNode = getIndexNextNode(index);

	//3.���������ֻ��ͷ��㣬��Ҫɾ��ͷ�ڵ�
	if (index == 0)
	{
		mPtrList = NULL;
	}
	else if (index == mLen - 1)//4.���Ҫɾ��ͷ�ڵ�
	{
		preNode->pNext = NULL;
	}
	else//5.������������
	{
		preNode->pNext = nextNode;
	}

	delete delNode;
	mLen--;
	return SUCCESS;
}

//��ȡ�����нڵ������
template<class T_ELE>
int LinkedList<T_ELE>::getSize()
{
	return mLen;
}

//��ȡindexǰ��ڵ�ĵ�ַ
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexPreviousNode(int index)
{
	//����һ��ѭ��
	return getIndexCurrentNode(index - 1);
}

//��ȡindex�ڵ�ĵ�ַ
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexCurrentNode(int index)
{
	if (index < 0 || index >= mLen)
	{
		return NULL;
	}

	LinkedList<T_ELE>::PNODE res = mPtrList;
	while (index)
	{
		res = res->pNext;
		index--;
	}

	return res;
}


//��ȡindex����ڵ�ĵ�ַ
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexNextNode(int index)
{
	return getIndexCurrentNode(index + 1);
}



#endif