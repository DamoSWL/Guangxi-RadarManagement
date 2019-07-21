#include "stdafx.h"
#include "Websocket.h"

CWebSokcet::CWebSokcet()
{

}

CWebSokcet::~CWebSokcet()
{

}

std::string CWebSokcet::generate_websocket_response(const string &ws_req)
{
	string resp;
	{
		resp += "HTTP/1.1 101 Switching Protocols\r\n";
		resp += "Connection: Upgrade\r\n";
		resp += "Sec-WebSocket-Accept: " + generate_key(ws_req) + "\r\n";
		resp += "Upgrade: websocket\r\n";
		resp += "Accept-unmasked-frame: true\r\n";
		resp += "\r\n";
	}
	return resp;
}

std::string CWebSokcet::generate_key(const string &key)
{
	string server_key = key;
	server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	SHA1        sha;
	unsigned int    message_digest[5];

	sha.Reset();
	sha << server_key.c_str();

	sha.Result(message_digest);
	// convert sha1 hash bytes to network byte order because this sha1
	//  library works on ints rather than bytes
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}

	server_key = base64_encode(
		reinterpret_cast<const unsigned char*>
		(message_digest),20
		);

	return server_key;	
}

bool CWebSokcet::tryUnMarkIncomingPacket(IN OUT char* InputBuf,IN const unsigned int &InBufSize,OUT unsigned int &OutWebSockeHeadSize)
{
	if (InputBuf == NULL )
	{
		return false;
	}
	int nMinExpectedSize = 6;
	if (InBufSize < nMinExpectedSize)
		return false;
	BYTE payloadFlags = InputBuf[0];
	// 	 if (payloadFlags != 129)
	// 		 return false;
	BYTE basicSize = InputBuf[1] & 0x7F;
	unsigned __int64 payloadSize;
	int masksOffset;
	if (basicSize <= 125)
	{
		payloadSize = basicSize;
		masksOffset = 2;
	}
	else if (basicSize == 126)
	{
		nMinExpectedSize += 2;
		if (InBufSize < nMinExpectedSize)
			return false;
		payloadSize = ntohs( *(u_short*) (InputBuf + 2) );
		masksOffset = 4;
	}
	else if (basicSize == 127)
	{
		nMinExpectedSize += 8;
		if (InBufSize < nMinExpectedSize)
			return false;
		payloadSize = ntohl( *(u_long*) (InputBuf + 2) );
		masksOffset = 10;
	}
	else
		return false;

	nMinExpectedSize += payloadSize;
	if (InBufSize < nMinExpectedSize)
		return false;

	BYTE masks[4];
	memcpy(masks, InputBuf + masksOffset, 4);
	//转化后，把头直接覆盖
	OutWebSockeHeadSize  = masksOffset + 4;
	for (unsigned __int64 i = 0; i < payloadSize; i++) {
		InputBuf[i] = (InputBuf[i+OutWebSockeHeadSize] ^ masks[i%4]);
	}
	return true;
}

bool CWebSokcet::ChangeDataToWebSocket(IN const char *InPutBuf,IN const size_t InSize, OUT char** OutBaseDatabuf,OUT size_t &iBaseDataSize, OUT char* OutWebSocketDatabuf,OUT size_t &iWebSockeSize, IN const size_t NetMaxSendSize)
{
	unsigned __int64 payloadSize = InSize;

	size_t expectedSize = payloadSize + 1; //flags byte.
	if(payloadSize <= 125  && payloadSize <= 65535 )
		expectedSize += 1;
	else if(payloadSize > 125  && payloadSize <= 65535)
		expectedSize += 3;
	else
		expectedSize += 9;
	//判断缓冲区是否越界
	if (expectedSize > NetMaxSendSize)
		return false;
	//移动数据，保留出前面的头标示
	unsigned int iWebSocketHeadSize = expectedSize-payloadSize;
	unsigned int iWebSocketHeadIdx = 0;
	memcpy_s(OutWebSocketDatabuf + iWebSocketHeadSize,NetMaxSendSize-iWebSocketHeadSize,InPutBuf,payloadSize);
	//create the flags byte
	//char payloadFlags = 0x82;
	OutWebSocketDatabuf[iWebSocketHeadIdx++] = 0x82;
	//create the length byte
	if (payloadSize <= 125)
	{
		char basicSize = payloadSize;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = basicSize;
	}
	else if (payloadSize > 125 && payloadSize <= 65535)
	{
		char basicSize = 126;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = basicSize;

		char len[2];
		len[0] = ( payloadSize >> 8 ) & 255;
		len[1] = ( payloadSize ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 8 ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize ) & 255;
	}
	else
	{
		char basicSize = 127;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = basicSize;

		char len[8];
		len[0] = ( payloadSize >> 56 ) & 255;
		len[1] = ( payloadSize >> 48  ) & 255;
		len[2] = ( payloadSize >> 40 ) & 255;
		len[3] = ( payloadSize >> 32  ) & 255;
		len[4] = ( payloadSize >> 24 ) & 255;
		len[5] = ( payloadSize >> 16  ) & 255;
		len[6] = ( payloadSize >> 8 ) & 255;
		len[7] = ( payloadSize ) & 255;

		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 56 ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 48  ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 40 ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 32  ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 24 ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 16  ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize >> 8 ) & 255;
		OutWebSocketDatabuf[iWebSocketHeadIdx++] = ( payloadSize ) & 255;
	}
	if (iWebSocketHeadIdx != iWebSocketHeadSize)
	{
		return false;
	}

	iWebSockeSize = expectedSize;

	iBaseDataSize = InSize;

	*OutBaseDatabuf = OutWebSocketDatabuf + expectedSize-InSize;

	return true;
}

