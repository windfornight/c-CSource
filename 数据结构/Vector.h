#ifndef _VECTOR_H_
#define _VECTOR_H_

/*模板必须在头文件实现*/

/*当模板的类型是类，而类的构造函数又new的时候怎么解决*/

#define SUCCESS 1
#define ERROR -1
#define MALLOC_ERROR -2
#define INDEX_ERR -3

template <class T_ELE>
class Vector
{
public:
	Vector();
	Vector(int size);
	~Vector();

public:
	int at(int index, T_ELE* pEle);			//根据元素的索引得到元素
	int push_back(T_ELE Element);			//将元素存储到容器最后一个位置
	void pop_back();						//删除最后一个元素
	int insert(int index, T_ELE Element);	//向指定位置新增一个元素
	int capacity();							//返回在不增容的情况下，还能存储多少个元素
	void clear();							//清空所有元素
	bool empty();							//判断Vector是否为空，返回true时为空
	void erase(int index);					//删除指定元素
	int size();								//返回Vector元素数量的大小

private:
	bool expand();

private:
	int mIndex;  //索引
	int mIncrement;  //扩容单位
	int mLen;  //容器当前长度
	int mInitSize;  //初始化默认长度
	T_ELE* mPVector;  //头指针
};



template <class T_ELE>
Vector<T_ELE>::Vector():mInitSize(100), mIncrement(5)
{
	//1.创建长度为mInitSIze个T_ELE个对象
	T_ELE* pEle = new T_ELE[mInitSize];

	//2.将新建的空间初始化
	memset(pEle, 0, sizeof(T_ELE)*mInitSize);

	//3.设置其他值
	mIndex = 0;
	mLen = mInitSize;
	mPVector = pEle;
	pEle = NULL;

}

template<class T_ELE>
Vector<T_ELE>::Vector(int size):mIncrement(5)
{
	//1.创建长度为size个T_ELE对象
	T_ELE* pEle = new T_ELE[size];

	//2.将新创建的空间初始化
	memset(pEle, 0, size);

	//3.设置其他值
	mInitSize = size;
	mIndex = 0;
	mLen = mInitSize;
	mPVector = pEle;
	pEle = NULL;
}

template<class T_ELE>
Vector<T_ELE>::~Vector()
{
	//释放空间 delete[]
	delete[] mPVector;
	mPVector = NULL;
}

template <class T_ELE>
bool Vector<T_ELE>::expand()
{
	//1.计算增加后的长度
	int size = mLen + mIncrement;

	//2.申请空间
	T_ELE* pEle = new T_ELE[size];
	if(!pEle)
	{
		return false;
	}
	memset(pEle, 0, sizeof(T_ELE)*size);

	//3.将数据复制到新的空间
	memcpy(pEle, mPVector, sizeof(T_ELE)*mLen);

	//4.释放原来空间
	delete[] mPVector;
	mPVector = pEle;
	pEle = NULL;

	//5.为各种属性赋值
	mLen = size;
	return true;
}


template<class T_ELE>
int Vector<T_ELE>::push_back(T_ELE element)
{
	//1.判断师傅需要增容，如果需要就调用增容的函数
	if(mIndex >= mLen){
		if(!expand())
		{
			return MALLOC_ERROR;
		}
	}

	//2.将新的元素复制到容器的最后一个位置(当是类的时候是否有点不妥，赋值可能更合适一些，构造函数有new的时候)
	memcpy(mPVector+mIndex, &element, sizeof(T_ELE));

	//3.修改属性值
	++mIndex;

	return SUCCESS;
}

template<class T_ELE>
int Vector<T_ELE>::insert(int index, T_ELE element)
{
	//1.判断索引是否在合理区间
	if(index < 0 || index > mIndex)
	{
		return INDEX_ERR;
	}

	//2.判断是否需要增容，如果需要就调用增容的函数
	if(mIndex >= mLen)
	{
		if(!expand())
		{
			return MALLOC_ERROR;
		}
	}

	//3.将index之后的元素后移
	for(int i = mIndex; i > index; --i)
	{
		mPVector[i] = mPVector[i-1];
	}

	//4.将element元素复制到index位置
	mPVector[index] = element;

	//5.修改属性值
	++mIndex;

	return SUCCESS;
}

template <class T_ELE>
int Vector<T_ELE>::at(int index, T_ELE* pEle)
{
	//判断索引是否在合理区间
	if(index < 0 || index >= mIndex)
	{
		return INDEX_ERR;
	}

	//将index的值复制到pEle指定的内存
	*pEle = mPVector[index];

	return SUCCESS;
}


template<class T_ELE>
void Vector<T_ELE>::pop_back()
{
	if(mIndex > 0)
	{
		mIndex--;
	}
}


template<class T_ELE>
int Vector<T_ELE>::capacity()
{
	return (mLen - mIndex);
}

template<class T_ELE>
void Vector<T_ELE>::clear()
{
	memset(mPVector, 0, mIndex*sizeof(T_ELE));
	mIndex = 0;
}

template<class T_ELE>
bool Vector<T_ELE>::empty()
{
	return (mIndex == 0);
}

template<class T_ELE>
void Vector<T_ELE>::erase(int index)
{
	if(index < 0 || index >= mIndex)
	{
		return;
	}

	for(int i = index; i < mIndex-1; ++i)
	{
		memcpy(mPVector[i], mPVector[i+1], sizeof(T_ELE));
	}

	mIndex--;
	memset(mPVector[mIndex], 0, sizeof(T_ELE));
}

template<class T_ELE>
int Vector<T_ELE>::size()
{
	return mIndex;
}


#endif