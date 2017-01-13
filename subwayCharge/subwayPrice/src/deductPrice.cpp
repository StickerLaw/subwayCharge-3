#include "stdafx.h"
#include "subwayGlobalDef.h"
#include "subwayMacro.h"
#include "include/basePrice.h"
#include "include/deductPrice.h"
#include "subwayPrice.h"
#include <iostream>
using namespace std;

/*
@ 获取扣费票价，几种情况;
@   1. 里程数为0，按时间收费;
@   2. 里程数不为0;
@ 返回值: 
*/
EN_RETURN_CODE GetDeductPrice(EN_CARD_TYPE enCardType, unsigned int balance, unsigned int meters, ST_SUBWAY_TIME &enterTime, ST_SUBWAY_TIME &exitTime, unsigned int &deductPrice)
{
	//里程数为0（同站进出）
	if(meters == 0){
		//小于30分钟
		if((exitTime.hour - enterTime.hour) * 60 + (exitTime.minutes - enterTime.minutes) <= 30){
			if(EN_CARD_TYPE_SINGLE == enCardType){
				deductPrice = balance;
			}else{
				deductPrice = 0;
			}
		}else{
			if(EN_CARD_TYPE_SINGLE == enCardType){
				deductPrice = balance > 3 ? balance : 3;
			}else{
				deductPrice = 3;
			}
		}
	}else{
		deductPrice = GetBasePrice(meters);
		if(EN_CARD_TYPE_NOMAL == enCardType){
			;
		}else if(EN_CARD_TYPE_ELDER == enCardType){
			if(enterTime.hour >= 10 && enterTime.hour < 15){
				deductPrice = (int)(deductPrice * 0.8);
			}
		}else if(EN_CARD_TYPE_SINGLE == enCardType){
			deductPrice = deductPrice > balance ? deductPrice : balance;
		}
	}
    return EN_RETURN_SUCC;
}

