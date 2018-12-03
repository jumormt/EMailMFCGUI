#include "stdafx.h"
#include "Smtpmail.h"
#include "MyMailDlg.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
#endif

CString strMsg;

//---------------------------------------------------------------------
int SmtpMail::SendMail()
{
	const int buflen = 256;
	char buf[buflen];
	int userlen, passlen;
	//(1)初始化网络环境
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		strMsg.Format("WSAStartup() error : %d\n", GetLastError());
		MessageBox(NULL,strMsg, "Error", MB_OK);
		return 1;
	}
	//(2)创建套接字
	SOCKET sockid;
	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		strMsg.Format("socket() error : %d\n", GetLastError());
		MessageBox(NULL, strMsg, "Error", MB_OK);
		WSACleanup();
		return 1;
	}
	//(3)得到SMTP服务器IP
	struct hostent *phostent = gethostbyname(SmtpSrvName);
	struct sockaddr_in addr;
	CopyMemory(&addr.sin_addr.S_un.S_addr,
		phostent->h_addr_list[0],
		sizeof(addr.sin_addr.S_un.S_addr));
	struct in_addr srvaddr;
	CopyMemory(&srvaddr, &addr.sin_addr.S_un.S_addr, sizeof(struct in_addr));
	strMsg.Format("Smtp server name is %s\n", SmtpSrvName);
	MessageBox(NULL, strMsg, "Tip", MB_OK);
	strMsg.Format("Smtp server ip is %s\n", inet_ntoa(srvaddr));
	MessageBox(NULL, strMsg, "Tip", MB_OK);
	//printf("Smtp server name is %s\n", SmtpSrvName);
	//printf("Smtp server ip is %s\n", inet_ntoa(srvaddr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(Port));
	ZeroMemory(&addr.sin_zero, 8);
	//(4)连接服务器
	if (connect(sockid, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		/*strMsg.Format("connect() error : %d\n", GetLastError());*/
		strMsg.Format("connect() error : please check your Internet\n");
		MessageBox(NULL, strMsg, "Error", MB_OK);
		//printf("connect() error : %d\n", GetLastError());
		goto STOP;
	}
	//(5)按照SMTP收发信息
	if (Talk(sockid, "220", "HELO asdf"))
	{
		goto STOP;
	}
	if (Talk(sockid, "250", "AUTH LOGIN"))
	{
		goto STOP;
	}
	ZeroMemory(buf, buflen);
	userlen = lstrlen(UserName);
	passlen = lstrlen(Password);
	/*for (i = 0; i < (userlen % 3 ? userlen / 3 + 1 : userlen / 3); i++)
	{
		Base64((unsigned char *)(UserName + i * 3), (unsigned char *)(buf + i * 4));
	}*/

	char* buffer0 = Base64(UserName, userlen);

	if (Talk(sockid, "334", buffer0))
	{
		goto STOP;
	}
	//ZeroMemory(buf, buflen);
	//for (i = 0; i < (passlen % 3 ? passlen / 3 + 1 : passlen / 3); i++)
	//{
	//	Base64((unsigned char *)(Password + i * 3), (unsigned char *)(buf + i * 4));
	//}

	char* buffer1 = Base64(Password, passlen);

	if (Talk(sockid, "334", buffer1))
	{
		goto STOP;
	}
	//ZeroMemory(buf, buflen);
	wsprintf(buf, "MAIL FROM:<%s>", From);
	if (Talk(sockid, "235", buf))
	{
		goto STOP;
	}
	ZeroMemory(buf, buflen);
	wsprintf(buf, "RCPT TO:<%s>", To);
	if (Talk(sockid, "250", buf))
	{
		goto STOP;
	}
	if (Talk(sockid, "250", "DATA"))
	{
		goto STOP;
	}
	ZeroMemory(buf, buflen);
	//发送正文
	wsprintf(buf, "TO: %s\r\nFROM: %s\r\nSUBJECT: %s\r\n%s\r\n\r\n.",
		To, From, Subject, Msg);
	send(sockid, buf, strlen(buf), 0);
	if (Talk(sockid, "354", buf))
	{
		goto STOP;
	}
	if (Talk(sockid, "250", "QUIT"))
	{
		goto STOP;
	}
	if (Talk(sockid, "221", ""))
	{
		goto STOP;
	}
	else
	{
		closesocket(sockid);
		WSACleanup();
		return 0;
	}
STOP://(6)关闭socket，释放网络资源
	closesocket(sockid);
	WSACleanup();
	return 1;
}
//---------------------------------------------------------------------
int SmtpMail::Talk(SOCKET sockid, const char *OkCode, char *pSend)
{
	const int buflen = 256;
	char buf[buflen];
	ZeroMemory(buf, buflen);
	//接收返回信息
	if (recv(sockid, buf, buflen, 0) == SOCKET_ERROR)
	{
		strMsg.Format("recv() error : %d\n", GetLastError());
		MessageBox(NULL, strMsg, "Error", MB_OK);
		//printf("recv() error : %d\n", GetLastError());
		return 1;
	}
	//else
	//{
	//	strMsg.Format("%s\n", buf);
	//	MessageBox(NULL, strMsg, "Tip", MB_OK);
	///*	printf("%s\n", buf);*/
	//}
	if (strstr(buf, OkCode) == NULL)
	{
	/*	strMsg.Format("Error: recv code != %s\n", OkCode);
		MessageBox(NULL, strMsg, "Error", MB_OK);*/
		if ("235" == OkCode)//用户名或密码错误
		{
			strMsg.Format("Error: username error or password error!");
			MessageBox(NULL, strMsg, "Error", MB_OK);
			return 1;
		}
		else if("250"== OkCode)//收发地址错误
		{
			strMsg.Format("Error: sendaddress error or receiveaddress error!");
			MessageBox(NULL, strMsg, "Error", MB_OK);
			return 1;
		}
		else if ("354" == OkCode)//正文发送错误
		{
			strMsg.Format("Error: sended unsuccessfully");
			MessageBox(NULL, strMsg, "Error", MB_OK);
			return 1;
		}

		//printf("Error: recv code != %s\n", OkCode);
	}
	//发送命令
	if (lstrlen(pSend))
	{
		ZeroMemory(buf, buflen);
		wsprintf(buf, "%s\r\n", pSend);
		if (send(sockid, buf, lstrlen(buf), 0) == SOCKET_ERROR)
		{
			strMsg.Format("send() error : %d\n", GetLastError());
			MessageBox(NULL, strMsg, "Error", MB_OK);
	/*		printf("send() error : %d\n", GetLastError());*/
			return 1;
		}
	}
	return 0;
}
//Base64编码，chasc：未编码的二进制代码，chuue：编码过的Base64代码

//void SmtpMail::Base64(unsigned char *chasc, unsigned char *chuue)
//{
//	int i, k = 2;
//	unsigned char t = 0;
//	for (i = 0;i<3;i++)
//	{
//		*(chuue + i) = *(chasc + i) >> k;
//		*(chuue + i) |= t;
//		t = *(chasc + i) << (8 - k);
//		t >>= 2;
//		k += 2;
//	}
//	*(chuue + 3) = *(chasc + 2) & 63;
//	for (i = 0;i<4;i++)
//		if ((*(chuue + i) >= 0) && (*(chuue + i) <= 25)) *(chuue + i) += 65;
//		else if ((*(chuue + i) >= 26) && (*(chuue + i) <= 51)) *(chuue + i) += 71;
//		else if ((*(chuue + i) >= 52) && (*(chuue + i) <= 61)) *(chuue + i) -= 4;
//		else if (*(chuue + i) == 62) *(chuue + i) = 43;
//		else if (*(chuue + i) == 63) *(chuue + i) = 47;
//}

char* SmtpMail::Base64(char* const origSigned, unsigned origLength)
{
	unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set  
	if (orig == NULL) return NULL;

	unsigned const numOrig24BitValues = origLength / 3;
	bool havePadding = origLength > numOrig24BitValues * 3;
	bool havePadding2 = origLength == numOrig24BitValues * 3 + 2;
	unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding);
	char* result = new char[numResultBytes + 3]; // allow for trailing '/0'  

												 // Map each full group of 3 input bytes into 4 output base-64 characters:  
	unsigned i;
	for (i = 0; i < numOrig24BitValues; ++i)
	{
		result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
		result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
		result[4 * i + 2] = base64Char[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F];
		result[4 * i + 3] = base64Char[orig[3 * i + 2] & 0x3F];
	}

	// Now, take padding into account.  (Note: i == numOrig24BitValues)  
	if (havePadding)
	{
		result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
		if (havePadding2)
		{
			result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
			result[4 * i + 2] = base64Char[(orig[3 * i + 1] << 2) & 0x3F];
		}
		else
		{
			result[4 * i + 1] = base64Char[((orig[3 * i] & 0x3) << 4) & 0x3F];
			result[4 * i + 2] = '=';
		}
		result[4 * i + 3] = '=';
	}

	result[numResultBytes] = '\0';
	return result;
}