//
//  Header.h
//  Classifier0
//
//  Created by Michael on 15/10/4.
//  Copyright © 2015年 Michael. All rights reserved.
//
//  Modify to test git
#pragma pack(8)
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define FileScale 10000000
#define FULL_SCALE 662928831
#include <unordered_map>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <algorithm>
#include <exception>
#include <string>
#include <sstream>
#define HASHLEN 17
//Attention
//Hashtables have diffrent length
//os 165749 id 1657493 ip 7657493
using namespace std;
const int line = 120;
const long num = 10000;
const long cache_size = line * num;
struct HASH_VALUE
{
    //char bytes[HASHLEN];
    long long seg1,seg2;
    HASH_VALUE (){}
    HASH_VALUE (string &str)//不安全
    {
        char *dist = (char *)this;
        for(int i=0; i<16; i++)
        {
            dist[i] = str[i];
        }
    }
    HASH_VALUE (const char str[])//不安全
    {
        char *dist = (char *)this;
        for(int i=0; i<16; i++)
        {
            dist[i] = str[i];
        }
    }
	char * c_str()//奇怪的写法
	{
		char * p = (char *)this;
		return p;
	}
};
bool operator == (const HASH_VALUE &op1, const HASH_VALUE &op2);
ostream &operator << (ostream &out, const HASH_VALUE &op);

struct Info
{
    HASH_VALUE id, ads_id, pos_id, ip_id;//16bytes HASH value
    int lang;//en OR zh , 0->zh, 1->en
    char OS_info[HASHLEN];//whose length is unknown
    long long timeStamp;//Unix Time stamp
    int stable, click;//is stable / is clicked
};


static string directory = "/Volumes/Hyakuya/orderedData/d";
//大型文件选择器
struct Selector
{
private:
	fstream file;
	int openedFileCode;
	int whichHalf;
	char *cache;
	//选择第i条记录
public:
	Selector() :cache(NULL), openedFileCode(0)
	{
		whichHalf = -1;
		cout << "Requesting for cache" << endl;
		cache = new char[FileScale * 104 / 2 + 1000];
		cout << "Succeed" << endl;
	}

	//为顺序访问优化的选择器
	Info sequence_read(int i)
	{
		//计算应该选用哪个文件
		unsigned file_code = i / FileScale + 1;
		if (openedFileCode == file_code)
		{
			//已经打开了正确地文件
			//计算在缓冲区中的地址
			unsigned bias = i % FileScale;
			if (bias < 5000000)
			{
				if (whichHalf == 0)
				{
					return *(Info *)(cache + 104 * bias);
				}
				else
				{
					whichHalf = 0;
					file.seekg(0, file.beg);
					file.read(cache, 52 * FileScale);
					return *(Info *)(cache + 104 * bias);
				}
			}
			else
			{
				if (whichHalf == 1)
				{
					return *(Info *)(cache + 104 * (bias - 5000000));
				}
				else
				{
					whichHalf = 1;
					file.seekg(104 * 5000000, file.beg);
					file.read(cache, 52 * FileScale);
					return *(Info *)(cache + 104 * (bias - 5000000));
				}
			}
		}
		else if (openedFileCode == 0)
		{
			openedFileCode = file_code;
			stringstream ss;
			ss << file_code;
			file.open(directory + ss.str(), ios::in | ios::binary);
		}
		else
		{
			file.close();
			openedFileCode = file_code;
			stringstream ss;
			ss << file_code;
			file.open(directory + ss.str(), ios::in | ios::binary);
		}


		unsigned bias = i % FileScale;

		if (bias < 5000000)
		{
			whichHalf = 0;
			file.seekg(0, file.beg);
			file.read(cache, 52 * FileScale);
			return *(Info *)(cache + 104 * bias);
		}
		else
		{
			whichHalf = 1;
			file.seekg(104 * 5000000, file.beg);
			file.read(cache, 52 * FileScale);
			return *(Info *)(cache + 104 * (bias - 5000000));
		}
	}

	~Selector()
	{
		if (openedFileCode != 0)
		{
			file.close();
		}
		delete[]cache;
	}
};

class Node
{
	friend class HashTable;
public:
	unsigned int verifyCode; //验证码,用于验证字符串身份
	int cnt; //出现次数
	int next;
    char str[20];
	Node() 
	{
		cnt = 0;
		next = 0;
		verifyCode = 0;
	}
};

const int bigN = 5657493;
const int smallN = 165749;


class HashTable
{
public:
	static const int N = bigN;

	int head[N]; //大小为N

	Node hashTable[5 * N]; //所有节点都存下来，可能一条链有多个节点
	int nodeCnt = 1;


	bool insert(const char * s);
	int find(const char * s);

	Node & operator[] (const int num);

	HashTable()
	{
		memset(hashTable, 0, sizeof(hashTable));
		memset(head, 0, sizeof(head));
	}

private:
	unsigned int getHashVal(const char * str);
	unsigned int getVerifyCode(const char * str);
};

bool operator == (const Info & op1, const Info & op2);
void searchForFirstUser();
void searchForClicks();
