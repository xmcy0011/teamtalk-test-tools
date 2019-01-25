package lib

import org.jboss.netty.buffer.ChannelBuffer

/**
 * 网络收到数据回调
 */
interface ISocketCallback {
    fun onConnected() {}
    fun onDisConnected(code: Int) {}
    fun onReceive(buf: ChannelBuffer) {}
}