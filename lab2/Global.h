#ifndef GLOBAL_H
#define GLOBAL_H

#include "Tool.h"
#include "NetworkService.h"
#include <iostream>
#include <vector>
#define GBN_WINDOW_SIZE 4
#define SEQ_BIT 3
#define MAX_SEQ (1 << SEQ_BIT)
#define SR_WINDOW_SIZE 4
extern  Tool *pUtils;						//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
extern  NetworkService *pns;				//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

#endif