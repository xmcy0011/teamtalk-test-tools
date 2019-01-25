#include "stdafx.h"
#include "yalog/yaolog.h"
#include "tcp_client.h"
#include "leonetwork/im_core.h"
#include "leoprotocol/IM.Login.pb.h"
#include "leoprotocol/IM.Other.pb.h"

UInt16 g_seqNum = 0;

TcpClient::TcpClient()
	:m_tcpClientState(kTcpClientStateOK)
	, m_pImLoginResp(0)
	, m_pServerPingTimer(0)
	, keepalive_thread_(nullptr)
	, m_bDoReloginServerNow(FALSE)
	, m_socketHandle(NETLIB_INVALID_HANDLE)
{
	m_eventReceived = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_eventConnected = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pImLoginResp = new IM::Login::IMLoginRes();
}

TcpClient::~TcpClient()
{
	CloseHandle(m_eventConnected);
	CloseHandle(m_eventReceived);
	delete m_pImLoginResp;
	m_pImLoginResp = 0;
}

void TcpClient::onClose()
{
	LOG__(ERR, _T("on socket onClose,handle:%d"), m_socketHandle);
	m_tcpClientState = TcpClient_State::kTcpClientStateDisconnect;
	_stopHearbeat();

	imcore::IMLibCoreClose(m_socketHandle);
	imcore::IMLibCoreUnRegisterCallback(m_socketHandle);

	CAutoLock lock(&m_lockForSocketHandle);
	m_socketHandle = NETLIB_INVALID_HANDLE;                 /* 2017/04/24 10:13 :  */
}

void TcpClient::onReceiveData(const char* data, int32_t size)
{
	if (m_pServerPingTimer)
		m_pServerPingTimer->m_bHasReceivedPing = TRUE;

	imcore::TTPBHeader header;
	header.unSerialize((byte*)data, imcore::HEADER_LENGTH);
	if (IM::BaseDefine::CID_OTHER_HEARTBEAT == header.getCommandId() && IM::BaseDefine::SID_OTHER == header.getModuleId())
	{
		//模块器端过来的心跳包，不跳到业务层派发
		return;
	}

	LOG__(NET, _T("receiveData message moduleId:0x%x,commandId:0x%x"), header.getModuleId(), header.getCommandId());

	if (g_seqNum == header.getSeqNumber())
	{
		m_pImLoginResp->ParseFromArray(data + imcore::HEADER_LENGTH, size - imcore::HEADER_LENGTH);
		::SetEvent(m_eventReceived);
		return;
	}

	//将网络包包装成任务放到逻辑任务队列里面去
	_handlePacketOperation(data, size);
}

void TcpClient::onReceiveError()
{
}

void TcpClient::onConnectDone()
{
	m_tcpClientState = TcpClient_State::kTcpClientStateOK;
	::SetEvent(m_eventConnected);

	m_bDoReloginServerNow = FALSE;
	if (!m_pServerPingTimer)
	{
		_startServerPingTimer();
	}
}

IM::Login::IMLoginRes* TcpClient::doLogin(std::string &linkaddr, UInt16 port, std::string& uName, std::string& pass, eErrorCode *peErrCode/* = nullptr*/)
{
	/* mod_S 需要在连接之前就进行清理，否则会返回上次的残留值，造成“登录成功”的假象 */
	m_pImLoginResp->Clear();
	m_pImLoginResp->set_result_code(IM::BaseDefine::REFUSE_REASON_DB_VALIDATE_FAILED);
	/* 2017/04/24 10:11 : TcpClient 只允许有一个连接实例，
	* 上一个连接没有关闭之前不许再创建一个新连接
	*/
	{
		CAutoLock lock(&m_lockForSocketHandle);
		if (m_socketHandle != NETLIB_INVALID_HANDLE)
		{
			LOG__(APP, _T("尝试建立一个新的 tcp 客户端连接，被驳回，现有 tcp 客户端连接 handle = %d"), m_socketHandle);
			peErrCode ? (*peErrCode = eErrorCode::eAlreadyConnected) : eErrorCode::eOk;
			return NULL;
		}

		m_socketHandle = imcore::IMLibCoreConnect(linkaddr, port);
		imcore::IMLibCoreRegisterCallback(m_socketHandle, this);
	}

	if (waitSingleObject(m_eventConnected, 5000))
	{
		IM::Login::IMLoginReq imLoginReq;
		string& name = uName;
		imLoginReq.set_user_name(name);
		imLoginReq.set_password(pass);
		imLoginReq.set_online_status(IM::BaseDefine::USER_STATUS_ONLINE);
		imLoginReq.set_client_type(IM::BaseDefine::CLIENT_TYPE_WINDOWS);
		imLoginReq.set_client_version("win_10086");

		if (TcpClient_State::kTcpClientStateOK != m_tcpClientState)
			return 0;

		sendPacket(IM::BaseDefine::SID_LOGIN, IM::BaseDefine::CID_LOGIN_REQ_USERLOGIN, ++g_seqNum, &imLoginReq);

		waitSingleObject(m_eventReceived, 10000);
	}
	return m_pImLoginResp;
}

void TcpClient::shutdown()
{
	_stopHearbeat();
	_stopServerPingTimer();

	imcore::IMLibCoreShutdown(m_socketHandle);
}

void TcpClient::sendPacket(UInt16 moduleId, google::protobuf::MessageLite* pbBody)
{
	m_TTPBHeader.clear();
	m_TTPBHeader.setModuleId(moduleId);

	_sendPacket(pbBody);
}

void TcpClient::sendPacket(UInt16 moduleId, UInt16 cmdId, google::protobuf::MessageLite* pbBody)
{
	m_TTPBHeader.clear();
	m_TTPBHeader.setModuleId(moduleId);
	m_TTPBHeader.setCommandId(cmdId);

	_sendPacket(pbBody);
}

void TcpClient::sendPacket(UInt16 moduleId, UInt16 cmdId, UInt16 seq, google::protobuf::MessageLite* pbBody)
{
	m_TTPBHeader.clear();
	m_TTPBHeader.setModuleId(moduleId);
	m_TTPBHeader.setCommandId(cmdId);
	m_TTPBHeader.setSeqNumber(seq);

	_sendPacket(pbBody);
}

void TcpClient::_sendPacket(google::protobuf::MessageLite* pbBody)
{
	UInt32 length = imcore::HEADER_LENGTH + pbBody->ByteSize();
	m_TTPBHeader.setLength(length);
	std::unique_ptr<byte> data(new byte[length]);
	memset(data.get(), 0, length);
	memcpy(data.get(), m_TTPBHeader.getSerializeBuffer(), imcore::HEADER_LENGTH);
	if (!pbBody->SerializeToArray(data.get() + imcore::HEADER_LENGTH, pbBody->ByteSize()))
	{
		LOG__(ERR, _T("pbBody SerializeToArray failed"));
		return;
	}
	imcore::IMLibCoreWrite(m_socketHandle, data.get(), length);
}

void TcpClient::startHeartbeat()
{
	std::thread t([=]() {
		while (true)
		{
			imcore::IMLibCoreStartOperationWithLambda(
				[=]()
			{
				IM::Other::IMHeartBeat imHearBeat;
				sendPacket(IM::BaseDefine::SID_OTHER, IM::BaseDefine::CID_OTHER_HEARTBEAT, &imHearBeat);
			}
			);

			Sleep(5 * 1000); // 每5秒一个包
		}
	});
	keepalive_thread_ = &t;
}

void TcpClient::_stopHearbeat()
{
	if (keepalive_thread_ != nullptr)
	{
		keepalive_thread_->joinable();
		keepalive_thread_->join();
	}
	keepalive_thread_ = nullptr;
}

void TcpClient::_handlePacketOperation(const char* data, UInt32 size)
{
	std::string copyInBuffer(data, size);
	imcore::IMLibCoreStartOperationWithLambda(
		[=]()
	{
		imcore::TTPBHeader header;
		header.unSerialize((byte*)copyInBuffer.data(), imcore::HEADER_LENGTH);

		IPduPacketParse* pModule = nullptr;// (IPduPacketParse*)__getModule(header.getModuleId());
		if (!pModule)
		{
			assert(FALSE);
			LOG__(ERR, _T("module is null, moduleId:%d,commandId:%d")
				, header.getModuleId(), header.getCommandId());
			return;
		}
		std::string pbBody(copyInBuffer.data() + imcore::HEADER_LENGTH, size - imcore::HEADER_LENGTH);
		pModule->onPacket(header, pbBody);
	});
}

void TcpClient::_startServerPingTimer()
{
	/*if (!m_pServerPingTimer)
		m_pServerPingTimer = new ServerPingTimer(this);
		module::getEventManager()->scheduleTimer(m_pServerPingTimer, 60, TRUE);*/
}

void TcpClient::_stopServerPingTimer()
{
	/*if (m_pServerPingTimer)
		module::getEventManager()->killTimer(m_pServerPingTimer);
		m_pServerPingTimer = 0;*/
}

void TcpClient::_doReloginServer()
{
	LOG__(APP, _T("doReloginServer now!!!"));
	m_bDoReloginServerNow = TRUE;
	onClose();
}

UInt8 TcpClient::getTcpClientNetState() const
{
	return m_tcpClientState;
}

ServerPingTimer::ServerPingTimer(TcpClient* pTcpClient)
	:m_pTcpClient(pTcpClient)
	, m_bHasReceivedPing(FALSE)
{

}

void ServerPingTimer::process()
{
	if (!m_bHasReceivedPing && !m_pTcpClient->m_bDoReloginServerNow)
	{
		m_pTcpClient->_doReloginServer();
	}
	m_bHasReceivedPing = FALSE;
}

void ServerPingTimer::release()
{
	delete this;
}



//////////////////////////////////////////////////////////////////////////

BOOL waitSingleObject(HANDLE handle, Int32 timeout)
{
	int t = 0;
	DWORD waitResult = WAIT_FAILED;
	do
	{
		int timeWaiter = 500;
		t += timeWaiter;
		waitResult = WaitForSingleObject(handle, timeWaiter);
	} while ((WAIT_TIMEOUT == waitResult) && (t < timeout));

	return (WAIT_OBJECT_0 == waitResult);
}