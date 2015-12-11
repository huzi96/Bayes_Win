#include "Header.h"

extern int indicator;
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

unsigned int HashTable::getHashVal(const char * str, int flag) //BKDRHash
{
    unsigned int seed = 1313131;
    unsigned int hash = 0;
    
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return hash;
}
unsigned int HashTable::getHashVal(const char * str) //BKDRHash
{
    unsigned int seed = 1313131;
    unsigned int hash = 0;
    for (int i=0; i<16; i++)
    {
        hash = hash * seed + (*str++);
    }
    
    return hash;
}
unsigned int HashTable::getVerifyCode(const char * str, int flag) //EFLHash
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
unsigned int HashTable::getVerifyCode(const char * str) //EFLHash
{
    unsigned int h = 0;
    unsigned int x = 0;
    
    for (int i=0; i<16; i++)
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