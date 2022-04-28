#include "PickupLeftFactor.h"
#include <string.h>
#include <stdlib.h>

const char *VoidSymbol = "$"; // "ε"
const char *Postfix = "'";

char rule_table[20][256];
char ruleNameArr[20][64];

int main(int argc, char *argv[])
{
	//
	// 调用 InitRules 函数初始化文法
	//

	Rule *pHead = InitRules();

	//
	// 输出提取左因子之前的文法
	//
	printf("Before Pickup Left Factor:\n");
	PrintRule(pHead);

	//
	// 调用 PickupLeftFactor 函数对文法提取左因子
	//
	PickupLeftFactor(pHead);

	//
	// 输出提取左因子之后的文法
	//
	printf("\nAfter Pickup Left Factor:\n");
	PrintRule(pHead);

	return 0;
}

/*
功能：
	根据下标找到 Select 中的一个 Symbol。

参数：
	pSelect -- Select 指针。
	index -- 下标。

返回值：
	如果存在，返回找到的 Symbol 指针，否则返回 NULL。
*/
RuleSymbol *GetSymbol(RuleSymbol *pSelect, int index)
{
	int i = 0;
	RuleSymbol *pRuleSymbol;
	for (pRuleSymbol = pSelect, i = 0; pRuleSymbol != NULL;
		 pRuleSymbol = pRuleSymbol->pNextSymbol, i++)
	{
		if (i == index)
		{
			return pRuleSymbol;
		}
	}

	return NULL;
}

/*
功能：
	以 SelectTemplate 为模板，确定左因子的最大长度。

参数：
	pSelectTemplate -- 作为模板的 Select 指针。

返回值：
	左因子的最大长度，如果返回 0 说明不存在左因子。
*/
int LeftFactorMaxLength(RuleSymbol *pSelectTemplate)
{
	//
	// TODO: 在此添加代码
	//
	int maxcount = 0;
	if (pSelectTemplate->pOther == NULL)
		return 0;
	for (RuleSymbol *pSelectTmp = pSelectTemplate->pOther; pSelectTmp != NULL; pSelectTmp = pSelectTmp->pOther)
	{
		int tmp_count = 0;
		while (1)
		{
			if (SymbolCmp(GetSymbol(pSelectTemplate, tmp_count), GetSymbol(pSelectTmp, tmp_count)) && GetSymbol(pSelectTemplate, tmp_count) != NULL)
			{
				tmp_count++;
				if (tmp_count > maxcount)
					maxcount = tmp_count;
			}
			else
			{
				break;
			}
		}
	}
	return maxcount;
}

/*
功能：
	比较两个相同类型(同为终结符或同为非终结符)的 Symbol 是否具有相同的名字。

参数：
	pSymbol1 -- Symbol 指针。
	pSymbol2 -- Symbol 指针。

返回值：
	相同返回 1，不同返回 0。
*/
int SymbolCmp(RuleSymbol *pSymbol1, RuleSymbol *pSymbol2)
{

	//
	// TODO: 在此添加代码
	//
	if (pSymbol1->isToken == 1)
	{
		return !strcmp(pSymbol1->TokenName, pSymbol2->TokenName);
	}
	else
	{
		return !strcmp(pSymbol1->pRule->RuleName, pSymbol2->pRule->RuleName);
	}
}

/*
功能：
	取文法中的一个 Select 与 SelectTemplate 进行比较，判断该 Select 是否需要提取左因子。

参数：
	pSelectTemplate -- 作为模板的 Select 指针。
	Count -- SelectTemplate 中已确定的左因子的数量。
	pSelect -- Select 指针。

返回值：
	如果 Select 包含左因子返回 1，否则返回 0。
*/
int NeedPickup(RuleSymbol *pSelectTemplate, int Count, RuleSymbol *pSelect)
{

	//
	// TODO: 在此添加代码
	//
	int flag = 1;
	for (int i = 0; i < Count; i++)
	{
		if (!SymbolCmp(pSelectTemplate, pSelect) || pSelect == NULL)
		{
			flag = 0;

			break;
		}
		pSelectTemplate = pSelectTemplate->pNextSymbol;
		pSelect = pSelect->pNextSymbol;
	}
	return flag;
}

/*
功能：
	将一个 Select 加入到文法末尾，当 Select 为 NULL 时就将一个ε终结符加入到文法末尾。

参数：
	pRule -- 文法指针。
	pNewSelect -- Select 指针。
*/
void AddSelectToRule(Rule *pRule, RuleSymbol *pNewSelect)
{

	//
	// TODO: 在此添加代码
	//
	if (pNewSelect == NULL)
	{
		pNewSelect = CreateSymbol();
		pNewSelect->isToken = 1;
		pNewSelect->TokenName[0] = '$';
	}
	if (pRule->pFirstSymbol == NULL)
	{
		pRule->pFirstSymbol = pNewSelect;
	}
	else
	{
		RuleSymbol *pSelect = pRule->pFirstSymbol;
		while (pSelect->pOther != NULL)
		{
			pSelect = pSelect->pOther;
		}
		pSelect->pOther = pNewSelect;
	}
}

/*
功能：
	将 pRuleName 与文法中的其他 RuleName 比较, 如果相同就增加一个后缀。

参数：
	pHead -- Rule 链表的头指针。
	pRuleName -- Rule 的名字。
*/
void GetUniqueRuleName(Rule *pHead, char *pRuleName)
{
	Rule *pRuleCursor = pHead;
	for (; pRuleCursor != NULL;)
	{
		if (0 == strcmp(pRuleCursor->RuleName, pRuleName))
		{
			strcat(pRuleName, Postfix);
			pRuleCursor = pHead;
			continue;
		}
		pRuleCursor = pRuleCursor->pNextRule;
	}
}

/*
功能：
	释放一个 Select 的内存。

参数：
	pSelect -- 需要释放的 Select 的指针。
*/
void FreeSelect(RuleSymbol *pSelect)
{

	//
	// TODO: 在此添加代码
	//
	// pSelect = NULL;
}

/*
功能：
	提取左因子。

参数：
	pHead -- 文法的头指针。
*/
void PickupLeftFactor(Rule *pHead)
{
	Rule *pRule;				 // Rule 游标
	int isChange;				 // Rule 是否被提取左因子的标志
	RuleSymbol *pSelectTemplate; // Select 游标
	Rule *pNewRule;				 // Rule 指针
	RuleSymbol *pSelect;		 // Select 游标

	do
	{
		isChange = 0;

		for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
		{
			int Count = 0;
			for (pSelectTemplate = pRule->pFirstSymbol; pSelectTemplate != NULL; pSelectTemplate = pSelectTemplate->pOther)
			{
				if ((Count = LeftFactorMaxLength(pSelectTemplate)) > 0)
					break;
			}

			// 忽略没用左因子的 Rule
			if (Count == 0)
				continue;
			pNewRule = CreateRule(pRule->RuleName); // 创建新 Rule
			GetUniqueRuleName(pRule, pNewRule->RuleName);
			isChange = 1; // 设置标志

			// 调用 AddSelectToRule 函数把模板左因子之后的部分加到新 Rule 的末尾
			// 将模板左因子之后的部分替换为指向新 Rule 的非终结符

			//
			// TODO: 在此添加代码
			//
			//新建一个指向newRule的Symbol
			AddSelectToRule(pNewRule, GetSymbol(pSelectTemplate, Count));
			RuleSymbol *pSymbol2newRule = CreateSymbol();
			pSymbol2newRule->isToken = 0;
			pSymbol2newRule->pRule = pNewRule;
			GetSymbol(pSelectTemplate, Count - 1)->pNextSymbol = pSymbol2newRule;

			// 从模板之后的位置循环查找包含左因子的 Select，并提取左因子
			pSelect = pSelectTemplate->pOther;
			RuleSymbol **pSelectPtr = &pSelectTemplate->pOther;
			while (pSelect != NULL)
			{
				if (NeedPickup(pSelectTemplate, Count, pSelect)) // Select 包含左因子
				{
					// 调用 AddSelectToRule 函数把左因子之后的部分加到新 Rule 的末尾
					// 将该 Select 从 Rule 中移除，释放内存，并移动游标

					//
					// TODO: 在此添加代码
					//
					AddSelectToRule(pNewRule, GetSymbol(pSelect, Count));
					//("%s\n", pNewRule->pFirstSymbol->pOther->TokenName);
					if (pSelect == pSelectTemplate->pOther)
					{
						pSelectTemplate->pOther = pSelect->pOther;
					}
					else
					{
						RuleSymbol *pHead_new_select = pSelectTemplate->pOther;
						while (pHead_new_select->pOther != pSelect)
						{
							pHead_new_select = pHead_new_select->pOther;
						}
						pHead_new_select->pOther = pSelect->pOther;
					}
					pSelect = pSelect->pOther;
				}
				else // Select 不包含左因子
				{
					// 移动游标

					//
					// TODO: 在此添加代码
					//
					pSelect = pSelect->pOther;
				}
			}

			// 将新 Rule 加入到文法链表

			//
			// TODO: 在此添加代码
			//
			pRule->pNextRule = pNewRule;
		}

	} while (isChange == 1);
}

/*
功能：
	使用给定的数据初始化文法链表

返回值：
	文法的头指针
*/
typedef struct _SYMBOL
{
	int isToken;
	char Name[MAX_STR_LENGTH];
} SYMBOL;

typedef struct _RULE_ENTRY
{
	char RuleName[MAX_STR_LENGTH];
	SYMBOL Selects[64][64];
} RULE_ENTRY;

/*
功能：
	初始化文法链表(在执行流水线时调用)。

返回值：
	文法的头指针。
*/
Rule *InitRules()
{
	int nRuleCount = 0;
	for (int i = 0; i < 20; i++)
	{
		gets(rule_table[i]);
		int length = strlen(rule_table[i]);
		if (length == 0)
		{
			break;
		}

		for (int j = 0; j < length; j++)
		{
			if (rule_table[i][j] == ' ')
			{
				ruleNameArr[i][j] = '\0';
				break;
			}
			ruleNameArr[i][j] = rule_table[i][j];
		}

		nRuleCount++;
	}

	Rule *pHead, *pRule;
	RuleSymbol **pSymbolPtr1, **pSymbolPtr2;

	int i, j, k;

	Rule **pRulePtr = &pHead;
	for (i = 0; i < nRuleCount; i++)
	{
		*pRulePtr = CreateRule(ruleNameArr[i]);
		pRulePtr = &(*pRulePtr)->pNextRule;
	}

	pRule = pHead;
	for (i = 0; i < nRuleCount; i++)
	{
		pSymbolPtr1 = &pRule->pFirstSymbol;

		int start = 0;
		for (int j = 0; rule_table[i][j] != '\0'; j++)
		{
			if (rule_table[i][j] == ' ' && rule_table[i][j + 1] == '-' && rule_table[i][j + 2] == '>' && rule_table[i][j + 3] == ' ')
			{
				start = j + 4;
				break;
			}
		}

		for (k = start; rule_table[i][k] != '\0'; k++)
		{
			if (rule_table[i][k] == '|')
			{
				pSymbolPtr1 = &(*pSymbolPtr1)->pOther;
				pSymbolPtr2 = pSymbolPtr1;
				continue;
			}
			if (rule_table[i][k] == ' ')
			{
				continue;
			}
			if (k == start)
			{
				pSymbolPtr2 = pSymbolPtr1;
			}

			*pSymbolPtr2 = CreateSymbol();

			char tokenName[MAX_STR_LENGTH] = {};
			tokenName[0] = rule_table[i][k];
			tokenName[1] = '\0';
			(*pSymbolPtr2)->isToken = 1;
			for (int m = 0; m < nRuleCount; m++)
			{
				if (strcmp(tokenName, ruleNameArr[m]) == 0)
				{
					(*pSymbolPtr2)->isToken = 0;
					(*pSymbolPtr2)->pRule = FindRule(pHead, tokenName);
					if (NULL == (*pSymbolPtr2)->pRule)
					{
						printf("Init rules error, miss rule \"%s\"\n", tokenName);
						exit(1);
					}
				}
			}
			if ((*pSymbolPtr2)->isToken == 1)
			{
				strcpy((*pSymbolPtr2)->TokenName, tokenName);
			}

			pSymbolPtr2 = &(*pSymbolPtr2)->pNextSymbol;
		}

		pRule = pRule->pNextRule;
	}

	return pHead;
}

/*
功能：
	创建一个新的 Rule。

参数：
	pRuleName -- 文法的名字。

返回值：
	Rule 指针
*/
Rule *CreateRule(const char *pRuleName)
{
	Rule *pRule = (Rule *)malloc(sizeof(Rule));

	strcpy(pRule->RuleName, pRuleName);
	pRule->pFirstSymbol = NULL;
	pRule->pNextRule = NULL;

	return pRule;
}

/*
功能：
	创建一个新的 Symbol。

返回值：
	RuleSymbol 指针
*/
RuleSymbol *CreateSymbol()
{
	RuleSymbol *pSymbol = (RuleSymbol *)malloc(sizeof(RuleSymbol));

	pSymbol->pNextSymbol = NULL;
	pSymbol->pOther = NULL;
	pSymbol->isToken = -1;
	pSymbol->TokenName[0] = '\0';
	pSymbol->pRule = NULL;

	return pSymbol;
}

/*
功能：
	根据 RuleName 在文法链表中查找名字相同的文法。

参数：
	pHead -- 文法的头指针。
	RuleName -- 文法的名字。

返回值：
	Rule 指针
*/
Rule *FindRule(Rule *pHead, const char *RuleName)
{
	Rule *pRule;
	for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		if (0 == strcmp(pRule->RuleName, RuleName))
		{
			break;
		}
	}

	return pRule;
}

/*
功能：
	输出文法。

参数：
	pHead -- 文法的头指针。
*/
void PrintRule(Rule *pHead)
{

	//
	// TODO: 在此添加代码
	//
	Rule *pRule;
	//遍历每条产生式
	for (pRule = pHead; pRule != NULL; pRule = pRule->pNextRule)
	{
		printf("%s->", pRule->RuleName);
		RuleSymbol *pSelect;
		//遍历每个select
		for (pSelect = pRule->pFirstSymbol; pSelect != NULL; pSelect = pSelect->pOther)
		{
			RuleSymbol *pSymobl;
			for (pSymobl = pSelect; pSymobl != NULL; pSymobl = pSymobl->pNextSymbol)
			{
				if (pSymobl->isToken == 1)
				{
					printf("%s", pSymobl->TokenName);
				}
				else
				{
					printf("%s", pSymobl->pRule->RuleName);
				}
			}
			if (pSelect->pOther != NULL)
				printf("|");
		}
		printf("\n");
	}
}
