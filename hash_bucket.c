/***********************
*author: ailen
*data: 2018/1/16
*************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKET_COUNT 4

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
	hashEntry * entry,* tmp_entry;
	for(int i=0;i<BUCKET_COUNT;i++)
	{
		entry = &(t->bucket[i]);
		//释放数组这里的节点
		free(entry->key);
		entry->key = NULL;
		free(entry->value);
		entry->value = NULL;
		entry = entry->next;

		while(entry != NULL)
		{
			//释放每个节点的key,value
			free(entry->key);
			entry->key = NULL;
			free(entry->value);
			entry->value = NULL;
			//释放整个entry节点
			tmp_entry = entry;
			entry = entry->next;
			free(tmp_entry);
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

//insert hashTable,如果原来的key有value存在，还可以替换原来的value数据
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
//如果key存在就在hashTable中删除,return -1表示没有这个key
int removeHashEntry(hashTable * t,const char *key)
{
	if(NULL == t || NULL == key)
		return -1;
	int bucket_index = hashFunc(key);
	hashEntry * entry = &(t->bucket[bucket_index]);
	//判断是否是桶里面的第一个
	if(strcmp(entry->key,key) == 0)
	{
		//桶里面只有一个entry
		if(entry->next == NULL)
		{
			free(entry->key);
			entry->key = NULL;
			free(entry->value);
			entry->value = NULL;
		}
		else//桶里面都至少两个entry
		{
			free(entry->key);
			free(entry->value);
			entry->key = entry->next->key;
			entry->value = entry->next->value;
			entry->next = entry->next->next;
		}
	}
	else//链表里面的entry
	{
		hashEntry * pre_entry = entry;
		entry = entry->next;
		if(entry == NULL)
			return -1;//链表里面没有entry
		while(entry != NULL)
		{
			if(strcmp(entry->key,key) == 0)
			{
				free(entry->key);
				free(entry->value);
				pre_entry->next = entry->next;
				free(entry);
				entry = NULL;
				return 0;
			}
			entry = entry->next;
		}
	}
	return -1;
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
	insertEntry(&t,"sugar","2");
	insertEntry(&t,"li","49");
	printHashTable(&t);
	printf("----------------------------------\n");
	removeHashEntry(&t,"ailen");
	printHashTable(&t);
	printf("----------------------------------\n");
	removeHashEntry(&t,"li");	
	printHashTable(&t);
	printf("----------------------------------\n");
	freeHashTable(&t);
	
	printHashTable(&t);

	return 0;
}
