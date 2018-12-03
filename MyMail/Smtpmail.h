#pragma once
#ifndef __AFXWIN_H__
  #error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class SmtpMail
{
private:
	char SmtpSrvName[32];
	char Port[7];
	char UserName[32];
	char Password[16];
	char From[32];
	char To[32];
	char Subject[32];
	char Msg[64];
	char* Base64(char * const origSigned, unsigned origLength);
	int Talk(SOCKET sockid, const char *OkCode, char *pSend);
public:
	SmtpMail(const char* s, const char* p, const char* u, const char* w,
		const char* f, const char* t, const char* j, const char* m)
	{
		strcpy(SmtpSrvName, s);
		strcpy(Port, p);
		strcpy(UserName, u);
		strcpy(Password, w);
		strcpy(From, f);
		strcpy(To, t);
		strcpy(Subject, j);
		strcpy(Msg, m);
	}
	int SendMail();
};
