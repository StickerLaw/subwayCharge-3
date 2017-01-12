#include "stdafx.h"
#include <iostream>
#include "subwayGlobalDef.h"
#include "subwayMacro.h"
#include "subwayCard.h"
#include "subwayCmdParse.h"
#include "subwayline.h"
#include "subwayPrice.h"
#include "subwayCommon.h"
#include "subwayOutput.h"
#include "subwayError.h"
using namespace std;

/*
@ 查询地铁线
@ 入参：unCmd, 命令内容
@ 出参: returnStr
@ 返回值: 无
*/
void ProcQueryLineCmd(UN_CMD &unCmd, char returnStr[MAX_SEND_BUFFER_LENGTH])
{
    //查询所有地铁线 GetLineInfo
    string outStr;
    GetLineInfo(outStr);
    //memcpy_s(returnStr, outStr.c_str(), outStr.size());
    memcpy_s(returnStr, MAX_SEND_BUFFER_LENGTH, outStr.c_str(), outStr.size());
    returnStr[outStr.size()] = 0;
    return;
}