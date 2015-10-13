#include "Header.h"


class Node
{
	friend class HashTable;
private:
	unsigned int verifyCode; //��֤��,������֤�ַ������
	int cnt; //���ִ���
	int next;

	Node() {} //��ֹ����
};


class HashTable
{
public:
	static const int N = 165749;
	
	int head[N] = {0}; //��СΪN
	
	Node hashTable[5 * N]; //���нڵ㶼������������һ�����ж���ڵ�
	int nodeCnt = 1;


	bool insert(const char * s);
	int find(const char * s);

	Node & operator[] (const int num);

private:
	unsigned int getHashVal(const char * str);
	unsigned int getVerifyCode(const char * str);
};



bool HashTable::insert(const char * s)
{
	unsigned int key = getHashVal(s) % N;
	unsigned int verifyCode = getVerifyCode(s);

	int i = head[key];
	while (i && hashTable[i].verifyCode != verifyCode)
		i = hashTable[i].next;

	if (hashTable[i].verifyCode == verifyCode) //������ͬ��
	{
		hashTable[i].cnt++;
		return 0;
	}
	else //δ���ֹ�
	{
		hashTable[nodeCnt].cnt = 0;
		hashTable[nodeCnt].verifyCode = verifyCode;
		hashTable[nodeCnt].next = head[key];
		head[key] = nodeCnt++;
		return 1;
	}
}

int HashTable::find(const char * s) //�����ַ���s���ڽڵ���,���û���򷵻�0
{
	unsigned int key = getHashVal(s) % N;
	unsigned int verifyCode = getVerifyCode(s);

	int i = head[key];
	while (i && hashTable[i].verifyCode != verifyCode)
		i = hashTable[i].next;

	return i;
}

Node & HashTable::operator[] (const int num)
{
	return hashTable[num];
}

unsigned int HashTable::getHashVal(const char * str) //BKDRHash
{
	unsigned int seed = 1313131;
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}
	return hash;
}

unsigned int HashTable::getVerifyCode(const char * str) //EFLHash
{
	unsigned int h = 0;
	unsigned int x = 0;

	while (*str)
	{
		h = (h << 4) + (*str++);
		if ((x = h & 0xf0000000L) != 0) //�����߸��ַ�
		{
			h ^= (x >> 24); //������λ��ĩ��λ���
			h &= ~x; //ɾ������λ
		}
	}
	return h;
}