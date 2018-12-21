#ifndef _VECTOR_H_
#define _VECTOR_H_

/*ģ�������ͷ�ļ�ʵ��*/

/*��ģ����������࣬����Ĺ��캯����new��ʱ����ô���*/

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
	int at(int index, T_ELE* pEle);			//����Ԫ�ص������õ�Ԫ��
	int push_back(T_ELE Element);			//��Ԫ�ش洢���������һ��λ��
	void pop_back();						//ɾ�����һ��Ԫ��
	int insert(int index, T_ELE Element);	//��ָ��λ������һ��Ԫ��
	int capacity();							//�����ڲ����ݵ�����£����ܴ洢���ٸ�Ԫ��
	void clear();							//�������Ԫ��
	bool empty();							//�ж�Vector�Ƿ�Ϊ�գ�����trueʱΪ��
	void erase(int index);					//ɾ��ָ��Ԫ��
	int size();								//����VectorԪ�������Ĵ�С

private:
	bool expand();

private:
	int mIndex;  //����
	int mIncrement;  //���ݵ�λ
	int mLen;  //������ǰ����
	int mInitSize;  //��ʼ��Ĭ�ϳ���
	T_ELE* mPVector;  //ͷָ��
};



template <class T_ELE>
Vector<T_ELE>::Vector():mInitSize(100), mIncrement(5)
{
	//1.��������ΪmInitSIze��T_ELE������
	T_ELE* pEle = new T_ELE[mInitSize];

	//2.���½��Ŀռ��ʼ��
	memset(pEle, 0, sizeof(T_ELE)*mInitSize);

	//3.��������ֵ
	mIndex = 0;
	mLen = mInitSize;
	mPVector = pEle;
	pEle = NULL;

}

template<class T_ELE>
Vector<T_ELE>::Vector(int size):mIncrement(5)
{
	//1.��������Ϊsize��T_ELE����
	T_ELE* pEle = new T_ELE[size];

	//2.���´����Ŀռ��ʼ��
	memset(pEle, 0, size);

	//3.��������ֵ
	mInitSize = size;
	mIndex = 0;
	mLen = mInitSize;
	mPVector = pEle;
	pEle = NULL;
}

template<class T_ELE>
Vector<T_ELE>::~Vector()
{
	//�ͷſռ� delete[]
	delete[] mPVector;
	mPVector = NULL;
}

template <class T_ELE>
bool Vector<T_ELE>::expand()
{
	//1.�������Ӻ�ĳ���
	int size = mLen + mIncrement;

	//2.����ռ�
	T_ELE* pEle = new T_ELE[size];
	if(!pEle)
	{
		return false;
	}
	memset(pEle, 0, sizeof(T_ELE)*size);

	//3.�����ݸ��Ƶ��µĿռ�
	memcpy(pEle, mPVector, sizeof(T_ELE)*mLen);

	//4.�ͷ�ԭ���ռ�
	delete[] mPVector;
	mPVector = pEle;
	pEle = NULL;

	//5.Ϊ�������Ը�ֵ
	mLen = size;
	return true;
}


template<class T_ELE>
int Vector<T_ELE>::push_back(T_ELE element)
{
	//1.�ж�ʦ����Ҫ���ݣ������Ҫ�͵������ݵĺ���
	if(mIndex >= mLen){
		if(!expand())
		{
			return MALLOC_ERROR;
		}
	}

	//2.���µ�Ԫ�ظ��Ƶ����������һ��λ��(�������ʱ���Ƿ��е㲻�ף���ֵ���ܸ�����һЩ�����캯����new��ʱ��)
	memcpy(mPVector+mIndex, &element, sizeof(T_ELE));

	//3.�޸�����ֵ
	++mIndex;

	return SUCCESS;
}

template<class T_ELE>
int Vector<T_ELE>::insert(int index, T_ELE element)
{
	//1.�ж������Ƿ��ں�������
	if(index < 0 || index > mIndex)
	{
		return INDEX_ERR;
	}

	//2.�ж��Ƿ���Ҫ���ݣ������Ҫ�͵������ݵĺ���
	if(mIndex >= mLen)
	{
		if(!expand())
		{
			return MALLOC_ERROR;
		}
	}

	//3.��index֮���Ԫ�غ���
	for(int i = mIndex; i > index; --i)
	{
		mPVector[i] = mPVector[i-1];
	}

	//4.��elementԪ�ظ��Ƶ�indexλ��
	mPVector[index] = element;

	//5.�޸�����ֵ
	++mIndex;

	return SUCCESS;
}

template <class T_ELE>
int Vector<T_ELE>::at(int index, T_ELE* pEle)
{
	//�ж������Ƿ��ں�������
	if(index < 0 || index >= mIndex)
	{
		return INDEX_ERR;
	}

	//��index��ֵ���Ƶ�pEleָ�����ڴ�
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