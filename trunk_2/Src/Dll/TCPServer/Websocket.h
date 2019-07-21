#ifndef _WEBSOCKET_20150312_H_
#define _WEBSOCKET_20150312_H_

#include <winsock2.h>
#include "sha1.h"
#include "base64.h"

using namespace std;

class CWebSokcet
{
public:
	CWebSokcet();
	~CWebSokcet();
	static string generate_websocket_response(const string &ws_req);		//计算sha1返回给WEB
	static bool tryUnMarkIncomingPacket(IN OUT char* InputBuf,IN const unsigned int &InBufSize,OUT unsigned int &OutWebSockeHeadSize);//解密传入的信息

	static bool ChangeDataToWebSocket(IN const char *InPutBuf,IN const size_t InSize,
		OUT char** OutBaseDatabuf,OUT size_t &iBaseDataSize,
		OUT char* OutWebSocketDatabuf,OUT  size_t &iWebSockeSize,
		IN const size_t NetMaxSendSize);	//转化为WEBSocket格式
private:

	static string generate_key(const string &key);

};



#endif
