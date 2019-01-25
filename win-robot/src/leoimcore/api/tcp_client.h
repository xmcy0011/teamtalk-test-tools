/** @file tcp_client.h
  * @brief tcp封装
  * @author Captain China
  * @date 2019/01/25
  */

#ifndef _TCP_CLIENT_0F818C1F_6023_485B_8D52_9C12BD59C8B8_H_
#define _TCP_CLIENT_0F818C1F_6023_485B_8D52_9C12BD59C8B8_H_

#include "leonetwork/im_conn.h"
#include "leonetwork/tt_pb_header.h"
#include "leoprotocol/IM.Login.pb.h"
#include "i_event.h"
#include "im_sdk_dll.h"
#include <thread>

using namespace module;

struct IPduPacketParse
{
public:
	/**
	* 收到TcpClient网络包后的包解析回调接口
	*
	* @param   std::auto_ptr<CImPdu> pdu
	* @return  void
	* @exception there is no any exception to throw.
	*/

	virtual void onPacket(imcore::TTPBHeader& header, std::string& pbBody) = 0;
};

struct ITcpClientSocketCallback
{
	virtual void onClose(int error) = 0;
	virtual void onReceiveData(const char* data, UInt32 size) = 0;
	virtual void onParseError() = 0;
	virtual void onConnectDone() = 0;
};

enum TcpClient_State
{
	kTcpClientStateOK = 0,
	kTcpClientStateDisconnect = 1,
};

class TcpClient;
/** @class ServerPingTimer
  * @brief 模块器端的一个心跳的维护
  */
class ServerPingTimer : public module::ITimerEvent
{
public:
	ServerPingTimer(TcpClient* pTcpClient);
	virtual void process();
	virtual void release();

public:
	BOOL						m_bHasReceivedPing;
	TcpClient*					m_pTcpClient;
};

/**
* The class <code>客户端TCP网络长连接模块实现，对TcpClientScoket做一个适配</code>
*
*/
class IM_SDK_DLL TcpClient final : public ITcpSocketCallback
{
	friend class ServerPingTimer;
public:
	TcpClient();
	virtual ~TcpClient();
public:
	enum class eErrorCode : int
	{
		eOk,
		eAlreadyConnected,
	};

	virtual IM::Login::IMLoginRes* doLogin(std::string &linkaddr, UInt16 port, std::string& uName, std::string& pass, eErrorCode *peErrCode = nullptr);
	virtual void shutdown();
	virtual void sendPacket(UInt16 moduleId, google::protobuf::MessageLite* pbBody);
	virtual void sendPacket(UInt16 moduleId, UInt16 cmdId, google::protobuf::MessageLite* pbBody);
	virtual void sendPacket(UInt16 moduleId, UInt16 cmdId, UInt16 seq, google::protobuf::MessageLite* pbBody);

	virtual void startHeartbeat();
	virtual UInt8 getTcpClientNetState()const;

public:
	virtual void onReceiveData(const char* data, int32_t size);
	virtual void onReceiveError();
	virtual void onConnectDone();
	virtual void onClose();

private:
	void _stopHearbeat();
	void _handlePacketOperation(const char* data, UInt32 size);
	void _startServerPingTimer();
	void _stopServerPingTimer();
	void _doReloginServer();
	void _sendPacket(google::protobuf::MessageLite* pbBody);

private:
	std::thread*					keepalive_thread_;
	IM::Login::IMLoginRes*			m_pImLoginResp;
	ServerPingTimer*				m_pServerPingTimer;
	BOOL							m_bDoReloginServerNow;
	UInt8							m_tcpClientState;
	HANDLE							m_eventConnected;
	HANDLE							m_eventReceived;
	int								m_socketHandle;
	imcore::TTPBHeader				m_TTPBHeader;
	CLock                           m_lockForSocketHandle;
};

BOOL waitSingleObject(HANDLE handle, Int32 timeout);

#endif// _TCP_CLIENT_0F818C1F_6023_485B_8D52_9C12BD59C8B8_H_