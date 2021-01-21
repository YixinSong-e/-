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
extern  Tool *pUtils;						//指向唯一的工具类实例，只在main函数结束前delete
extern  NetworkService *pns;				//指向唯一的模拟网络环境类实例，只在main函数结束前delete

#endif