#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define TRUE 1
#define FALSE 0
#define BOOL int
#define LEFT_CODE '0'
#define RIGHT_CODE '1'

typedef struct huffmannode
{
	BOOL IsSrcChar;
	char CodingNum;
	int Weight;
	char CodedChar;
	struct huffmannode *pLeftChlid;
	struct huffmannode *pRightChlid;
	struct huffmannode *pFather;
	struct huffmannode *pNext;
}HuffmanNode;

typedef struct codertablenode
{
	char CodedChar;
	char* CoderNum;
	int CoderPlacesNum;
}CoderTableNode;

typedef struct codelist
{
	char CodedNum;
	struct codelist *pNext;
}CodeList;
typedef struct stack
{
	HuffmanNode *tree;
	struct stack *pNext;
}Stack;

typedef struct MyStack
{
	Stack *stack;
	int count;
}Stack_Top;


CoderTableNode *pCoderTable = NULL;//编码表
int CoderTableLen = -1;

//当前字符是否在之前出现过 return 出现过返回1 没有出现过：0
int IsAppearBefore(char src_string[],char src_char,int char_appear_index)
{
	while(char_appear_index > 0)
	{
		if(src_char == src_string[--char_appear_index]) return 1;
	}
	return 0;
}

//统计字符概率
HuffmanNode* StatCodedChar(char src_string[],int len_src)
{
	int count;
	HuffmanNode *pHead = NULL;
	HuffmanNode *pTemp = NULL;
	//进行统计
	for(count = 0;count<len_src;count++)
	{
		if( !IsAppearBefore(src_string,src_string[count],count) )
		{
			pTemp = (HuffmanNode *)malloc(sizeof(HuffmanNode));
			
			pTemp->IsSrcChar = TRUE;
			pTemp->CodingNum = LEFT_CODE;
			pTemp->Weight = 1;
			pTemp->CodedChar = src_string[count];
			pTemp->pRightChlid = NULL;
			pTemp->pLeftChlid = NULL;
			pTemp->pFather = NULL;

			//用pNext指针来贯穿链表 头添加
			if(pHead != NULL)
			{
				pTemp->pNext = pHead;
				pHead->pFather = pTemp;
			}
			else
			{
				pTemp->pNext = NULL;
				pTemp->pFather = NULL;
			}
			pHead = pTemp;
		}
		else
		{
			pTemp = pHead;
			while(pTemp)
			{
				if(pTemp->CodedChar == src_string[count])
				{
					pTemp->Weight++;
					break;
				}
				pTemp = pTemp->pNext;
			}
		}
	}
	return pHead;
}

//冒泡排序：小->大
void BubbleSort(HuffmanNode **pHead)
{
	char temp_char;
	int temp_weigtht;
	HuffmanNode *pTemp = NULL;
	HuffmanNode *pLoopPoint = NULL;
	HuffmanNode *pLoopPoint_in = NULL;
	if(pHead == NULL)return ;

	pTemp = pLoopPoint = *pHead;
	while(pLoopPoint->pNext)
	{
		pLoopPoint_in = pLoopPoint;
		while(pLoopPoint_in->pNext)
		{
			if(pLoopPoint_in->Weight > pLoopPoint_in->pNext->Weight)
			{
				temp_char = pLoopPoint_in->CodedChar;
				temp_weigtht = pLoopPoint_in->Weight;
				pLoopPoint_in->Weight = pLoopPoint_in->pNext->Weight;
				pLoopPoint_in->CodedChar = pLoopPoint_in->pNext->CodedChar;
				pLoopPoint_in->pNext->Weight = temp_weigtht;
				pLoopPoint_in->pNext->CodedChar = temp_char;
			}
			pLoopPoint_in = pLoopPoint_in->pNext;
		}

		pLoopPoint = pLoopPoint->pNext;
	}
}

//对已排序的数据建造哈夫曼树
HuffmanNode* CreateHuffmanTree(HuffmanNode **pHead)
{
	int count;
	HuffmanNode *pTemp = NULL;
	HuffmanNode *HeadLastPoint = NULL;
	if(pHead == NULL)return NULL;

	while(*pHead != NULL)
	{
		pTemp =(HuffmanNode *)malloc(sizeof(HuffmanNode));
		pTemp->pLeftChlid = *pHead;
		pTemp->pLeftChlid->CodingNum = LEFT_CODE;//左孩子编码值为0
		pTemp->pLeftChlid->pFather = pTemp;//更新父亲
		pTemp->pRightChlid = (*pHead)->pNext;
		pTemp->pRightChlid->CodingNum = RIGHT_CODE;//右孩子编码值为1
		pTemp->pRightChlid->pFather = pTemp;//更新父亲
		pTemp->IsSrcChar = FALSE;
		pTemp->CodingNum = LEFT_CODE;
		pTemp->Weight = (*pHead)->Weight + (*pHead)->pNext->Weight;
		pTemp->CodedChar = -1;
		pTemp->pFather = NULL;
		pTemp->pNext = NULL;

		for(count = 0;count<2;count++)//断开原有结构 头指针后移
		{
			HeadLastPoint = (*pHead);
			(*pHead) = (*pHead)->pNext;
			if(HeadLastPoint->IsSrcChar == TRUE)
			{
				HeadLastPoint->pNext = NULL;
			}
		}

		if(*pHead == NULL)
			break;

		HeadLastPoint = *pHead;
		while(pTemp->Weight > HeadLastPoint->Weight && HeadLastPoint->pNext != NULL)
		{
			HeadLastPoint = HeadLastPoint->pNext;
		}

		if(HeadLastPoint == *pHead && HeadLastPoint->Weight >= pTemp->Weight)
		{
			pTemp->pNext = *pHead;
			(*pHead)->pFather = pTemp;
			*pHead = pTemp;
		}
		else
		{
			if(HeadLastPoint->pNext == NULL && HeadLastPoint->Weight < pTemp->Weight)
			{
				HeadLastPoint->pNext = pTemp;
				pTemp->pFather = HeadLastPoint;
				pTemp->pNext = NULL;
			}
			else
			{
				HeadLastPoint->pFather->pNext = pTemp;
				pTemp->pNext = HeadLastPoint;
				pTemp->pFather = HeadLastPoint->pFather;
				HeadLastPoint->pFather = pTemp;
			}
		}
	}
	return pTemp;
}

int GetCharCount(HuffmanNode *pHead)
{
	int CharCount = 0;
	HuffmanNode *ptemp = NULL;
	if(pHead == NULL)return -1;
	ptemp = pHead;
	while(ptemp)
	{
		CharCount++;
		ptemp = ptemp->pNext;
	}
	return CharCount;
}

//-------------------------------辅助栈-------------------------------------
void InitStack(Stack_Top** stacktop)
{
	*stacktop = (Stack_Top *)malloc(sizeof(Stack_Top));
	(*stacktop)->stack = NULL;
	(*stacktop)->count = 0;
}

void StackPush(Stack_Top *stacktop,HuffmanNode* val)
{
	Stack *pTemp = NULL;

	pTemp = (Stack *)malloc(sizeof(Stack));
	pTemp->tree = val;
	pTemp->pNext = NULL;
	if(stacktop->stack != NULL)
	{
		pTemp->pNext = stacktop->stack;
	}
	stacktop->stack = pTemp;
	stacktop->count++;
	return ;
}
HuffmanNode* StackPop(Stack_Top *stacktop)
{
	HuffmanNode *val = NULL;
	if(stacktop->stack == NULL)
		return NULL;
	else
	{
		Stack *pTemp = stacktop->stack;
		val = (stacktop->stack)->tree;
		stacktop->stack = stacktop->stack->pNext;
		free(pTemp);
		stacktop->count--;
		return val;
	}
}
//-----------------------------辅助栈---------------------------------------

//编码并加入表中
void CallBackCode(HuffmanNode *Node,CoderTableNode *CoderTable)
{
	int count_Places;
	int count;
	int count_Add;
	HuffmanNode *pTemp = NULL;
	CodeList *pHead = NULL;
	CodeList *pDel = NULL;
	CodeList *pTemp_CodeList = NULL;
	if(Node == NULL || CoderTable == NULL)return ;

	count_Places = 0;
	pTemp = Node;
	while(pTemp->pFather)
	{
		pTemp_CodeList = (CodeList*)malloc(sizeof(CodeList));
		pTemp_CodeList->CodedNum = pTemp->CodingNum;

		//头添加
		if(pHead == NULL)
		{
			pTemp_CodeList->pNext = NULL;
		}
		else
		{
			pTemp_CodeList->pNext = pHead;
		}
		pHead = pTemp_CodeList;
		count_Places++;
		pTemp = pTemp->pFather;
	}

	//放入编码表
	for(count = 0;count<CoderTableLen;count++)
	{
		if(pCoderTable[count].CodedChar == -1)
			break;
	}
	pCoderTable[count].CodedChar = Node->CodedChar;
	pCoderTable[count].CoderPlacesNum = count_Places;
	pCoderTable[count].CoderNum = (char *)malloc(sizeof(char)*count_Places);
	
	for(count_Add = 0;count_Add<count_Places;count_Add++)
	{
		pCoderTable[count].CoderNum[count_Add] = pHead->CodedNum;
		pDel = pHead;
		pHead = pHead->pNext;
		free(pDel);
		pDel = NULL;
	}
}

//非递归前序遍历
void LoopFOrderTree(HuffmanNode *tree)
{
	Stack_Top *stack = NULL;
	if(tree == NULL) return ;

	InitStack(&stack);
	while(1)
	{
		while(tree != NULL)
		{
			StackPush(stack,tree);
			tree = tree->pLeftChlid;
		}
		tree = StackPop(stack);

		if(tree == NULL) return ;

		if(tree->IsSrcChar == TRUE)
		{
			CallBackCode(tree,pCoderTable);//编码并加入表中
		}
		tree = tree->pRightChlid;
	}
}

//编码
void CreateHuffmancode(char src_string[],int len_src)
{
	int count;
	int count_in;
	int count_print;
	CoderTableNode *pTemp = NULL;
	FILE *pFile = NULL;
	if(src_string == NULL || len_src <= 0) return ;
	pTemp = pCoderTable;

	if(fopen_s(&pFile,"./Huffmancode.txt","w") != 0)return ;
	printf(">>>编码结果输出文件在此程序源文件同目录下 名为：Huffmancode.txt\n\n");//编码结果输出文件
	for(count = 0;count<len_src;count++)
	{	
		for(count_in = 0;count_in<CoderTableLen;count_in++)
		{	
			if(pTemp[count_in].CodedChar == src_string[count])
			{
				for(count_print = 0;count_print<pTemp[count_in].CoderPlacesNum;count_print++)
				{
					//printf("%c",pTemp[count_in].CoderNum[count_print]);
					fputc(pTemp[count_in].CoderNum[count_print],pFile);
				}
				break;
			}
		}
	}
	printf(">>>编码完成\n\n");
	fclose(pFile);
}

//输出编码表到文件
void PrintToFileOfCoderTable()
{
	int count;
	int count_in;
	FILE *pFile = NULL;
	if(pCoderTable == NULL || CoderTableLen <= 0)return ;

	if(fopen_s(&pFile,"./CoderTable.txt","w") != 0)return ;
	printf(">>>编码表结果输出文件在此程序源文件同目录下 名为：CoderTable.txt\n\n");//编码表结果输出文件
	for(count = 0;count<CoderTableLen;count++)
	{
		fputc(pCoderTable[count].CodedChar,pFile);
		fputc(':',pFile);
		for(count_in = 0;count_in<pCoderTable[count].CoderPlacesNum;count_in++)
		{
			fputc(pCoderTable[count].CoderNum[count_in],pFile);
		}
		fseek(pFile,4,SEEK_CUR);
	}
}

//递归释放哈夫曼树
void FreeHuffmanTree(HuffmanNode *pHunffmanTree)
{
	if(pHunffmanTree == NULL)return ;
	FreeHuffmanTree(pHunffmanTree->pLeftChlid);
	FreeHuffmanTree(pHunffmanTree->pRightChlid);
	free(pHunffmanTree);
	pHunffmanTree = NULL;
}

//释放编码表
void FreeCoderTable()
{
	int count;
	if(pCoderTable == NULL)return ;
	for(count = 0;count<CoderTableLen;count++)
	{
		free(pCoderTable[count].CoderNum);
		pCoderTable[count].CoderNum = NULL;
	}
	free(pCoderTable);
	pCoderTable = NULL;
}

//前期准备过程   编码
void HuffmanEncoder(char src_string[],int len_src)
{
	int CharCount;
	HuffmanNode *pHead = NULL;
	HuffmanNode *pHunffmanTree = NULL;

	if(src_string == NULL || len_src <= 0) return ;

	//准备过程
	pHead = StatCodedChar(src_string,len_src);//统计字符概率
	BubbleSort(&pHead);//按照概率排序
	CharCount = GetCharCount(pHead);//统计编码元素个数
	pHunffmanTree = CreateHuffmanTree(&pHead);//创建哈夫曼树

	pCoderTable = (CoderTableNode *)malloc(sizeof(CoderTableNode)*CharCount);//申请编码表
	CoderTableLen = CharCount;
	memset(pCoderTable,-1,sizeof(CoderTableNode)*CharCount);
	LoopFOrderTree(pHunffmanTree);//遍历树并放入编码表
	PrintToFileOfCoderTable();//编码表结果输出文件
	CreateHuffmancode(src_string,len_src);//编码过程
	FreeHuffmanTree(pHunffmanTree);//回收空间
}

//按照编码表匹配并打印 返回编码占位数
int SearchIsRightAndPrint(char huffmancodechar[],int LoopCount)
{
	int count;
	int count_in;
	int LoopCountTemp;
	if(huffmancodechar == NULL || LoopCount < 0)return -1;

	//查找匹配
	for(count = 0;count<CoderTableLen;count++)
	{
		LoopCountTemp = LoopCount;
		for(count_in = 0;count_in<pCoderTable[count].CoderPlacesNum;count_in++)
		{
			if(pCoderTable[count].CoderNum[count_in] == huffmancodechar[LoopCountTemp])
			{
				LoopCountTemp++;
			}
			else
			{
				break;
			}
		}
		//匹配成功 打印
		if(count_in == pCoderTable[count].CoderPlacesNum)
		{
			printf("%c",pCoderTable[count].CodedChar);
			return pCoderTable[count].CoderPlacesNum;
		}
	}
	return -1;
}

//解码
void HuffmanDecoder(char *filename)
{
	int filesize;
	int SearchResult;
	int LoopCount;
	FILE *pFile = NULL;
	char *huffmancodechar = NULL;
	if(fopen_s(&pFile,filename,"r") != 0)return ;

	//得到编码
	fseek(pFile,0,SEEK_END);//偏移至文件尾
	filesize = ftell(pFile);//得到文件大小
	rewind(pFile);//重新指向文件开头
	huffmancodechar = (char *)malloc(sizeof(char)*filesize);
	memset(huffmancodechar,0,sizeof(char)*filesize);

	if( fgets(huffmancodechar,filesize+1,pFile) == NULL)
	{
		return ;
		//printf("%s",huffmancodechar);
	}
	fclose(pFile);

	//解码
	LoopCount = 0;
	while(LoopCount <= filesize)
	{
		SearchResult = SearchIsRightAndPrint(huffmancodechar,LoopCount);//按照编码表匹配并打印
		if(SearchResult == -1)
			break ;
		else
			LoopCount+=SearchResult;
	}
	printf("\n");
	FreeCoderTable();//释放编码表
}
int main()
{
	char arr[] = "fdg;d;gfherwrewrwqersddfshghdsfgghjdsfgfhasfashhsdtgsdghhhfghdhdfyuuyuoipasopuyiuhhfyuareryewgrelgeryuthreunrblerbytbyltjbysbhbewtbhetbhbtwebrltberwhtberlktbertbhewrlbtlhewrbtlethgsdfhgkdsfhglghsdfhgjldsfhgjhdsfglhdsflghdfhgjldsfhgjfdhgldshgdsflghdsfhgdsghjdhgdfhglkdffsgdsgsdgfgsgfhfdhdf";
	HuffmanEncoder(arr,strlen(arr));
	HuffmanDecoder("./Huffmancode.txt");
	system("pause");
	return 0;
}
