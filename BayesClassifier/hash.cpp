#include "Header.h"


class Node
{
	friend class HashTable;
private:
	unsigned int verifyCode; //验证码,用于验证字符串身份
	int cnt; //出现次数
	int next;

	Node() {} //防止生成
};


class HashTable
{
public:
	static const int N = 165749;
	
	int head[N] = {0}; //大小为N
	
	Node hashTable[5 * N]; //所有节点都存下来，可能一条链有多个节点
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

	if (hashTable[i].verifyCode == verifyCode) //已有相同串
	{
		hashTable[i].cnt++;
		return 0;
	}
	else //未出现过
	{
		hashTable[nodeCnt].cnt = 0;
		hashTable[nodeCnt].verifyCode = verifyCode;
		hashTable[nodeCnt].next = head[key];
		head[key] = nodeCnt++;
		return 1;
	}
}

int HashTable::find(const char * s) //查找字符串s所在节点编号,如果没有则返回0
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
		if ((x = h & 0xf0000000L) != 0) //超出七个字符
		{
			h ^= (x >> 24); //将多余位与末四位异或
			h &= ~x; //删除多余位
		}
	}
	return h;
}