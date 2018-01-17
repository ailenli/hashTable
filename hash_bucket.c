/***********************
*author: ailen
*data: 2018/1/16
*************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKET_COUNT 1024

typedef struct tagHashEntry{
	char * key;
	char * value;
	struct tagHashEntry * next;
}hashEntry;

typedef struct tagHashTable{
	hashEntry bucket[BUCKET_COUNT];
}hashTable;

int initHashTable(hashTable *t)
{
	if(NULL == t) return -1;
	for(int i=0;i<BUCKET_COUNT;i++)
	{
		t->bucket[i].key = NULL;
		t->bucket[i].value = NULL;
		t->bucket[i].next = NULL;
	}
	return 0;
}

int freeHashTable(hashTable * t)
{
	if(NULL == t) return -1;
	hashEntry * entry,* entry_next;
	for(int i=0;i<BUCKET_COUNT;i++)
	{
		entry = &(t->bucket[i]);
		while(entry->next != NULL)
		{
			entry_next = entry->next;
			
			free(entry_next->key);
			entry_next->key = NULL;
			free(entry_next->value);
			entry_next->value = NULL;

			entry->next = entry_next->next;
			free(entry_next);
			entry_next = NULL;
		}
	}
}

//返回hash桶的index,key to index的计算过程
int hashFunc(const char * key) //key 指向的内容不能改变
{
	if(NULL == key) return -1;
	int len = strlen(key);
	int bucket_index = (int)key[0];
	for(int i = 0;i<len;i++)
	{
		bucket_index *= 1103515245 + (int)key[i];
	}
	bucket_index >>=27;
	bucket_index &= (BUCKET_COUNT - 1);
	return bucket_index;
}

//insert hashTable,还可以替换数据

int insertEntry(hashTable * t,const char * key,const char * value)
{
	if(NULL == t || NULL == key || NULL == value)
		return -1;

	hashEntry * entry,* entry_next;
	int bucket_index = hashFunc(key);
	if(t->bucket[bucket_index].key == NULL)
	{
		t->bucket[bucket_index].key = strdup(key);
		t->bucket[bucket_index].value = strdup(value);
	}
	else{
		entry = &(t->bucket[bucket_index]);
		while(entry != NULL)
		{
			if(strcmp(entry->key,key) == 0) //找到了这个key,替换掉原值
			{
				int new_len = strlen(value);
				int old_len = strlen(entry->value);
				if(new_len > old_len)
				{
					free(entry->value);
					entry->value = (char*)malloc(new_len);
				}
				memcpy(entry->value,value,new_len);
				return bucket_index;
			}
			entry_next = entry;
			entry = entry->next;

		}//end of while
		//走到这里，就说明在桶里面，没有找到
		//没有找到，那就创建一个条目
		entry = (hashEntry*)malloc(sizeof(hashEntry));
		entry->key = strdup(key);
		entry->value = strdup(value);
		entry->next = NULL;
		entry_next->next = entry;
	}
	return bucket_index;
}

const char * findValueByKey(const hashTable * t,const char * key)
{
	if(NULL == t||NULL == key)
		return NULL;
	int bucket_index = hashFunc(key);
	const hashEntry * entry = &(t->bucket[bucket_index]);
	while(entry != NULL)
	{
		if(strcmp(entry->key,key) == 0)
		{//找到了
			return entry->value;
		}
		entry = entry->next;
	}
	return NULL;
}

void printHashTable(hashTable * t)
{
	hashEntry * entry;
	for(int i =0;i<BUCKET_COUNT;i++)
	{
		printf("\nbucket [%d]:\n",i);
		entry = &(t->bucket[i]);
		while(entry != NULL)
		{
			if(entry->key != NULL)
			{
				printf("\t%s\t=\t%s\n",entry->key,entry->value);
			}
			entry = entry->next;
		}
	}
}

int main(int argc, char const *argv[])
{
	hashTable t;
	initHashTable(&t);
	insertEntry(&t,"ailen","27");
	insertEntry(&t,"lamber","1");
	insertEntry(&t,"trista","25");
	printHashTable(&t);

	return 0;
}
