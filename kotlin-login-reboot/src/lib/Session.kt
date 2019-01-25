package lib

import lib.base.DefaultHeader
import lib.protocol.IMBaseDefine
import lib.protocol.IMLogin
import org.jboss.netty.buffer.ChannelBuffer

/**
 * 与服务器的会话
 */
class Session constructor(ip: String, port: Int) : ISocketCallback {
    val serverIp = ip
    val serverPort = port
    val socket: SocketThread = SocketThread(ip, port, this)

    fun login(name: String, pwd: String) {
        val header = DefaultHeader(IMBaseDefine.ServiceID.SID_LOGIN_VALUE, IMBaseDefine.LoginCmdID.CID_LOGIN_REQ_USERLOGIN_VALUE)
        val req = IMLogin.IMLoginReq.newBuilder()
        req.userName = name
        req.password = pwd
        req.clientType = IMBaseDefine.ClientType.CLIENT_TYPE_ANDROID
        req.onlineStatus = IMBaseDefine.UserStatType.USER_STATUS_ONLINE
        req.clientVersion = "1.0.0"


    }

    fun connect(ip: String, port: Int) {

    }

    override fun onConnected() {
        super.onConnected()
    }

    override fun onDisConnected(code: Int) {
        super.onDisConnected(code)
    }

    override fun onReceive(buf: ChannelBuffer) {
        super.onReceive(buf)
    }
}