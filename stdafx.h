// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include<fstream>
#include <tchar.h>
#include <queue>

// TODO: 在此处引用程序需要的其他头文件
#pragma comment (lib,"netsimlib.lib")

//#define GBN_RDT_RECEIVER_H 1
//#define GBN_RDT_SENDER_H 1
//#define NONE_GBN_RDT_SENDER_MODE 1
#define STOP_WAIT_RDT_RECEIVER_H 1
#define STOP_WAIT_RDT_SENDER_H 1
#define NONE_STOP_WAIT_RDT_SENDER_MODE 1
//将该条件编译行加入这个预编译头中，才能正常发挥作用，当然这也来自于VS的提示

#include <iostream>
using namespace std;

//#pragma warning(disable:4482)
