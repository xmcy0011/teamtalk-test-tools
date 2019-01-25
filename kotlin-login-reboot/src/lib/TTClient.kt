package lib

import lib.base.DefaultHeader
import lib.base.Header
import lib.protocol.IMBaseDefine
import lib.protocol.IMLogin
/**
 * 与服务器的连接
 */
class TTClient {
    /**
     * 连接
     */
    fun Connect(name: String, pwd: String, ip: String, port: Int) {
        val header = DefaultHeader(IMBaseDefine.ServiceID.SID_LOGIN_VALUE, IMBaseDefine.LoginCmdID.CID_LOGIN_REQ_USERLOGIN_VALUE)
        var req = IMLogin.IMLoginReq.newBuilder()
        req.userName = name
        req.password = pwd
        req.clientType = IMBaseDefine.ClientType.CLIENT_TYPE_ANDROID
        req.onlineStatus = IMBaseDefine.UserStatType.USER_STATUS_ONLINE
        req.clientVersion = "1.0.0"


    }



}