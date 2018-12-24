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
	bool isEmpty();								//判断链表是否为空
	void clear();								//清空链表
	int getElement(int index, T_ELE& element);	//根据索引获取元素
	int getElementIndex(T_ELE& Element);		//根据元素获取链表中的索引
	int insert(T_ELE element);					//新增元素
	int insert(int index, T_ELE element);		//根据索引新增元素
	int deleteElement(int index);				//根据索引删除元素
	int getSize();								//获取链表中元素的数量

private:
	typedef struct _NODE
	{
		T_ELE Data;
		_NODE *pNext;
	}NODE, *PNODE;

	PNODE getIndexCurrentNode(int index);		//获取索引为index的指针
	PNODE getIndexPreviousNode(int index);		//获取前一个节点指针
	PNODE getIndexNextNode(int Index);			//获取后一个节点指针

private:
	PNODE mPtrList;								//链表指针，指向第一个节点
	int mLen;									//元素的数量
};

//无参构造函数，初始化成员
template<class T_ELE>
LinkedList<T_ELE>::LinkedList():mPtrList(NULL), mLen(0)
{

}

//析构函数，清空元素
template<class T_ELE>
LinkedList<T_ELE>::~LinkedList()
{
	clear();
}

//判断链表是否为空
template<class T_ELE>
bool LinkedList<T_ELE>::isEmpty()
{
	return (mLen == 0);
}

//清空链表
template<class T_ELE> 
void LinkedList<T_ELE>::clear()
{
	//1.判断链表是否为空
	if (isEmpty())
	{
		return ;
	}

	//2.循环删除链表中的节点
	while (mPtrList)
	{
		LinkedList<T_ELE>::PNODE tmp = mPtrList;
		mPtrList = mPtrList->pNext;
		delete tmp;
	}

	//3.删除最后一个节点并将聊表长度置为0
	mPtrList = NULL;
	mLen = 0;

}


//根据索引获取元素
template<class T_ELE>
int LinkedList<T_ELE>::getElement(int index, T_ELE& element)
{
	//1.判断索引是否有效
	if (index < 0 || index >= mLen)
	{
		return INDEX_IS_ERROR;
	}

	//2.去的索引指向的节点
	LinkedList<T_ELE>::PNODE ptr = mPtrList;
	int idx = 0;
	while (idx < index)
	{
		ptr = ptr->pNext;
		idx++;
	}

	//3.将缩影指向界限的值复制到out参数
	memcpy(&element, &ptr->Data, sizeof(T_ELE));

	return SUCCESS;
}


//根据元素内容获取索引
template<class T_ELE>
int LinkedList<T_ELE>::getElementIndex(T_ELE& element)
{
	//1.判断聊表是否为空
	if (isEmpty())
	{
		return -1;
	}

	//2.循环遍历链表，找到与element相同的元素
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

//在链表尾部新增节点
template<class T_ELE>
int LinkedList<T_ELE>::insert(T_ELE element)
{
	//1.判断链表是否为空
	if (isEmpty())
	{
		LinkedList<T_ELE>::PNODE tmp = new NODE;
		tmp->pNext = NULL;
		memcpy(tmp->Data, &element, sizeof(T_ELE));
		mPtrList = tmp;
		mLen++;
		return 1;
	}

	//2.如果链表中已经有元素
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

//将节点新增到指定索引的位置
template<class T_ELE>
int LinkedList<T_ELE>::insert(int index, T_ELE element)
{
	//1.判断索引值是否有效
	if (index < 0 || index > mLen)
	{
		return ERROR;
	}

	//2.判断链表是否为空
	
	LinkedList<T_ELE>::PNODE tmp = new NODE;
	tmp->pNext = NULL;
	memcpy(&tmp->Data; &element, sizeof(T_ELE));
	
	//3.如果索引为0
	if (index == 0)
	{
		tmp->pNext = mPtrList;
		mPtrList = tmp;
	}
	else if (index == mLen)  //4.如果索引为链表尾
	{
		LinkedList<T_ELE>::PNODE ptrLast = getIndexCurrentNode(index - 1);
		ptrLast->pNext = tmp;
	}
	else //5.如果索引为链表中
	{
		LinkedList<T_ELE>::PNODE preNode = getIndexPreviousNode(index);
		LinkedList<T_ELE>::PNODE nextNode = getIndexCurrentNdoe(index);
		preNode->pNext = tmp;
		tmp->pNext = nextNode;
	}

	mLen++;
	return SUCCESS;
}

//根据索引删除节点
template<class T_ELE>
int LinkedList<T_ELE>::deleteElement(int index)
{
	//1.判断链表是否为空
	if (isEmpty())
	{
		return INDEX_IS_ERROR;
	}

	//2.判断索引值是否有效
	if (index < 0 || index >= mLen)
	{
		return INDEX_IS_ERROR;
	}

	LinkedList<T_ELE>::PNODE delNode = getIndexCurrentNode(index);
	LinkedList<T_ELE>::PNODE preNode = getIndexPreviousNode(index);
	LinkedList<T_ELE>::PNODE nextNode = getIndexNextNode(index);

	//3.如果链表中只有头结点，且要删除头节点
	if (index == 0)
	{
		mPtrList = NULL;
	}
	else if (index == mLen - 1)//4.如果要删除头节点
	{
		preNode->pNext = NULL;
	}
	else//5.如果是其他情况
	{
		preNode->pNext = nextNode;
	}

	delete delNode;
	mLen--;
	return SUCCESS;
}

//获取链表中节点的数量
template<class T_ELE>
int LinkedList<T_ELE>::getSize()
{
	return mLen;
}

//获取index前面节点的地址
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexPreviousNode(int index)
{
	//就是一个循环
	return getIndexCurrentNode(index - 1);
}

//获取index节点的地址
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


//获取index后面节点的地址
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::getIndexNextNode(int index)
{
	return getIndexCurrentNode(index + 1);
}



#endif