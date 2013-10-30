#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

 
//“екстовый файл состоит из двух строк. 
//ќпределить, есть ли во второй строке слова, 
//которые не получаютс€ путЄм конкатенации 
//нескольких слов из первой строки.


typedef struct
{
	char **data;
	size_t typeSize;
	size_t size;
	size_t reserved;
} DynArray;

DynArray *createArray(size_t typeSize, size_t elemCnt);

void resizeArray(DynArray *dArray, size_t newSize)
{
	size_t resSize = (size_t)(newSize*1.6 + 0.5);
	size_t i;
	// free element memory if decrease array size
	for(i = dArray->size; i > newSize; --i)
	{
		if(dArray->data[i - 1])
			free(dArray->data[i - 1]);
	}

	dArray->data = (char **)realloc(dArray->data, dArray->typeSize*resSize);
	//assert(dArray->data != NULL &&

	dArray->size = newSize;
	dArray->reserved = resSize;
}

void resetArray(DynArray *dArray)
{
	resizeArray(dArray, 0);
}


void pushBackArray(DynArray *dArray, char *elem)
{
	size_t size = dArray->size;
	if(size <= dArray->reserved)
		resizeArray(dArray, size + 1);
	else
		dArray->size += 1;

	dArray->data[size]  = elem;
}

DynArray *createArray(size_t typeSize, size_t elemCnt)
{
	DynArray *res;
	res = (DynArray *)malloc(sizeof(DynArray));

	assert(res != NULL &&
		"Dynamic array memory allocation error");//?
	
	res->typeSize = typeSize;

	if(elemCnt != 0)
	{
		resizeArray(res, elemCnt);
	}
	else
	{
		res->data = NULL;
		res->size = 0;
		res->reserved = 0;
	}

	return res;
}

DynArray *splitWords(char *str, size_t length, char delimeter)
{
	DynArray *res = createArray(sizeof(char *), 0);
	char *pos = str;
	char *prevPos = str;
	char *endPos = str + length;
	char *tmpStr;
	size_t len = 0;

	do
	{
		pos = strchr(prevPos, delimeter);
		if(pos == NULL)
			pos = str + length - 1;

		len = pos - prevPos;
		tmpStr = malloc((len + 1)*sizeof(char));
		strncpy(tmpStr, prevPos, len);
		tmpStr[len] = 0;
		pushBackArray(res, tmpStr);
		prevPos = pos + 1;
	}
	while(prevPos < endPos);

	return res;
}

void printArray(DynArray *strArr)
{
	size_t i;
	for(i = 0; i < strArr->size; ++i)
		printf("%s\n", strArr->data[i]);
}

int checkWord(char *word, DynArray *strArr, size_t deep, size_t *perm)
{
	size_t size = strArr->size;
	size_t wordLen = strlen(word), len;
	size_t i, j;
	size_t flag = 1;
	char *candidate;

	for(i = 0; i < size; ++i)
	{
		flag = 1;
		// check for repetition
		for(j = 0; j < deep; ++j)
		{
			if(perm[j] == i)
			{
				flag = 0;
				break;
			}
		}

		if(!flag)
			continue;

		candidate = strArr->data[i];
		len = strlen(candidate);
		if(len <= wordLen && strncmp(word, candidate, len) == 0)
		{
			perm[deep] = i;
			if(len == wordLen)
				flag =  1;
			else
				flag = checkWord(word + len, strArr, deep + 1, perm);
			if(flag)
				return 1;
		}
	}
	return 0;
}

void main()
{
	FILE *f;
	DynArray *inWords;
	DynArray *outWords;
	char buf[4096];
	size_t r, i, size;
	size_t *perm;

	f = fopen("in.txt", "rb");
	assert(f != NULL && 
		"Input file open error");
	
	r = fscanf(f, "%[^\n]s", buf);
	inWords = splitWords(buf, strlen(buf), ' ');
	
	printf("\nIn words:\n");
	printArray(inWords);

	//r = fscanf(f, "%[A-za-z]s", buf);
	fseek(f, 1, SEEK_CUR);
	r = fgets(buf, 4096, f);
	outWords = splitWords(buf, strlen(buf), ' ');
	
	printf("\nOut words:\n");
	printArray(outWords);
	
	fclose(f);
	
	size = outWords->size;
	perm = (size_t *)malloc(sizeof(size_t)*inWords->size);
	printf("\n");
	for(i = 0; i < size; ++i)
	{
		if(!checkWord(outWords->data[i], inWords, 0, perm))
		{
			printf("Word: \"%s\" is bad\n", outWords->data[i]);
		}
	}

	// free memory
	free(perm);
	resetArray(inWords);
	resetArray(outWords);
	free(inWords);
	free(outWords);
}