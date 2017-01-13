#include "stdafx.h"
#include <iostream>
#include "list.h"
#include "subwayGlobalDef.h"
#include "subwayMacro.h"
#include "subwayCard.h"
#include <assert.h>
using namespace std;

typedef struct STR_CARD {
	EN_CARD_TYPE enCard;
	unsigned int cardNo;
	bool useFlag;
	unsigned int balance;
	TListHead list;
}ST_SUBWAY_CARD;

ST_SUBWAY_CARD g_cardList[MAX_CARD_NUM];
TListHead g_idleCardList;
TListHead g_usedCardList;


/*
@ 初始化所有卡信息
@ 返回值: 无
*/
void InitCardManagerInfo()
{
    memset(g_cardList, 0, sizeof(g_cardList));
  /*  if(!ListEmpty(&g_idleCardList)){
	    ListDel(&g_idleCardList);
    }
    if(!ListEmpty(&g_usedCardList)){
	    ListDel(&g_usedCardList);
    }*/

    g_idleCardList.next = &g_idleCardList;
    //g_idleCardList.prev = &g_idleCardList;
    g_usedCardList.next = &g_usedCardList;
    //g_usedCardList.prev = &g_usedCardList;

    for (int i = 0; i < MAX_CARD_NUM; i++) {
		ListAddTail(&(g_cardList[i].list), &(g_idleCardList));
	}
}

/*
@ 开卡，分配卡号和卡信息
@ 入参：enCard,卡类型; charge: 充值
@ 出参: cardNo, 分配的卡号
@ 返回值: EN_RETURN_SUCC，分配成功; EN_RETURN_CARD_OVERLOW, 分配失败;
*/
EN_RETURN_CODE AssignCard(unsigned int &cardNo, EN_CARD_TYPE enCard, unsigned int charge)
{
	if (ListEmpty(&g_idleCardList)) {
		return EN_RETURN_CARD_OVERLOW;
	}
	if (charge > MAX_BALANCE) {
		return EN_RETURN_RECHARGE_OVERFLOW;
	}
	ST_SUBWAY_CARD* tmpCard = NULL;
	assert(tmpCard = CONTAINER_OF(&g_idleCardList, ST_SUBWAY_CARD, list));
	TListHead tempListHead = g_idleCardList;
	g_idleCardList = *(g_idleCardList.next);
	_ListDel(&tempListHead, &g_idleCardList);

	tmpCard -> cardNo = cardNo;
	tmpCard -> enCard = enCard;
	tmpCard -> balance = charge;
	tmpCard -> useFlag = true;

	ListAddTail(&(tmpCard -> list), &(g_usedCardList));

    return EN_RETURN_SUCC;
}

/*
@ 充值
@ 入参：cardNo,卡号; recharge: 充值
@ 出参: 无
@ 返回值: EN_RETURN_SUCC，成功; 其他, 失败;
*/
EN_RETURN_CODE RechargeCard(unsigned int cardNo, unsigned int recharge)
{
	if (cardNo > MAX_CARD_NUM || cardNo < 0) {
		return EN_RETURN_INVALID_CARD;
	}
	TListHead* pos = NULL;
	ST_SUBWAY_CARD* tempSubwayCard = NULL;
	bool findit = false;
	ListForEach(pos, &(g_usedCardList)) {
		tempSubwayCard = CONTAINER_OF(pos, ST_SUBWAY_CARD, list);
		if (tempSubwayCard -> cardNo == cardNo) {
			if (tempSubwayCard -> enCard == EN_CARD_TYPE_SINGLE) {
				return EN_RETURN_SINGLE_CARD_CANNOT_RECHARGE;
			}
			tempSubwayCard -> balance += recharge;
			if (tempSubwayCard -> balance > MAX_BALANCE) {
				return EN_RETURN_RECHARGE_OVERFLOW;
			}
			findit = true;
		}
	}

	if (findit == false) {
		return EN_RETURN_INVALID_CARD;
	}

    return EN_RETURN_SUCC;
}

/*
@ 获取卡余额
@ 入参：cardNo,卡号; 
@ 出参: balance: 余额
        enCard:  卡类型
@ 返回值: EN_RETURN_SUCC，成功; EN_RETURN_INVALID_CARD, 失败;
*/
EN_RETURN_CODE GetCardInfo(unsigned int cardNo, unsigned int &balance, EN_CARD_TYPE &enCard)
{
    TListHead* pos = NULL;
	ST_SUBWAY_CARD* tempSubwayCard = NULL;
	ListForEach(pos, &(g_usedCardList)) {
		tempSubwayCard = CONTAINER_OF(pos, ST_SUBWAY_CARD, list);
		if (tempSubwayCard -> cardNo == cardNo) {
			balance = tempSubwayCard -> balance;
			enCard = tempSubwayCard -> enCard;
			return EN_RETURN_SUCC;
		}
	}
    return EN_RETURN_INVALID_CARD;
}

/*
@ 卡扣费
@ 入参：cardNo,卡号; enCard, 卡类型; deductPrice, 扣费票价
@ 出参: balance, 扣费后的余额
@ 返回值: EN_RETURN_SUCC，成功; EN_RETURN_INVALID_CARD, EN_RETURN_BALANCE_NOT_ENOUGH, 失败;
*/
EN_RETURN_CODE DeductCard(unsigned int cardNo, EN_CARD_TYPE enCard, unsigned int deductPrice, unsigned int &balance)
{
    TListHead* pos = NULL;
	ST_SUBWAY_CARD* tempSubwayCard = NULL;
	ListForEach(pos, &(g_usedCardList)) {
		tempSubwayCard = CONTAINER_OF(pos, ST_SUBWAY_CARD, list);
		if (tempSubwayCard -> cardNo == cardNo) {
			if (tempSubwayCard -> balance < deductPrice) {
				return EN_RETURN_BALANCE_NOT_ENOUGH;
			}
			tempSubwayCard -> balance -= deductPrice;
			balance = tempSubwayCard -> balance;
			if (balance < 20) {
				return EN_RETURN_BALANCE_TOO_LOW;
			}
		}
	}

	return EN_RETURN_INVALID_CARD;
}

/*
@ 删除卡信息
@ 入参：cardNo,卡号; 
@ 出参: 无
@ 返回值: 0，成功; -1, 失败;
*/
int DeleteCard(unsigned int cardNo)
{
    TListHead* pos = NULL;
	ST_SUBWAY_CARD* tempSubwayCard = NULL;
	ListForEach(pos, &(g_usedCardList)) {
		assert(tempSubwayCard = CONTAINER_OF(pos, ST_SUBWAY_CARD, list));
		if (tempSubwayCard -> cardNo == cardNo) {
			tempSubwayCard -> balance = 0;
			tempSubwayCard -> useFlag = false;
			tempSubwayCard -> cardNo = 0;
			tempSubwayCard -> enCard = EN_CARD_TYPE_SINGLE;
			tempSubwayCard -> list.next = NULL;
			tempSubwayCard -> list.prev = NULL;

			TListHead tmppos = tempSubwayCard -> list;
			_ListDel(&tmppos, tmppos.next);
			ListAddTail(&(tempSubwayCard -> list), &g_idleCardList);
			return 0;
			
		}
	}

    return -1;
}

/*
@ 获取卡类型字符串
@ 入参：enCard,卡类型; 
@ 出参: 无
@ 返回值: 卡类型字符串;
*/
char* GetCardTypeStr(EN_CARD_TYPE enCard)
{
	if (enCard == EN_CARD_TYPE_BUTT) {

    }
    return NULL;
}

/*
@ 根据卡类型字符串, 识别卡类型
@ 入参：cardType,      卡类型字符串; 
@ 出参: enCard,        卡类型
@ 返回值: EN_RETURN_SUCC，成功; EN_RETURN_INPUT_INVALID_CARDTYPE, 失败;
*/
EN_RETURN_CODE GetCardType(char cardType[], EN_CARD_TYPE &enCard)
{
    

    return EN_RETURN_SUCC;
}