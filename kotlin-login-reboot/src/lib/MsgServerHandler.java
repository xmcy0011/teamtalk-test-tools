package lib;

import org.jboss.netty.buffer.ChannelBuffer;
import org.jboss.netty.channel.*;

public class MsgServerHandler extends SimpleChannelHandler {

    MsgServerHandler(ISocketCallback callback){
        callback = callback;
    }
    private ISocketCallback callback;

    @Override
    public void channelConnected(ChannelHandlerContext ctx, ChannelStateEvent e)
            throws Exception {
        super.channelConnected(ctx, e);
        callback.onConnected();
    }

    @Override
    public void channelDisconnected(ChannelHandlerContext ctx, ChannelStateEvent e) throws Exception {
        /**
         * 1. 已经与远程主机建立的连接，远程主机主动关闭连接，或者网络异常连接被断开的情况
         2. 已经与远程主机建立的连接，本地客户机主动关闭连接的情况
         3. 本地客户机在试图与远程主机建立连接时，遇到类似与connection refused这样的异常，未能连接成功时
         而只有当本地客户机已经成功的与远程主机建立连接（connected）时，连接断开的时候才会触发channelDisconnected事件，即对应上述的1和2两种情况。
         *
         **/
        //logger.d("channel#channelDisconnected");
        super.channelDisconnected(ctx, e);
        callback.onDisConnected(0);
    }

    @Override
    public void messageReceived(ChannelHandlerContext ctx, MessageEvent e)
            throws Exception {
        super.messageReceived(ctx, e);
        ChannelBuffer channelBuffer = (ChannelBuffer) e.getMessage();
        if (null != channelBuffer){
            callback.onReceive(channelBuffer);
        }
    }

    /**
     * bug问题点:
     * exceptionCaught会调用断开链接， channelDisconnected 也会调用断开链接，事件通知冗余不合理。
     * a.另外exceptionCaught 之后channelDisconnected 依旧会被调用。 [切花网络方式]
     * b.关闭channel 也可能触发exceptionCaught
     * recvfrom failed: ETIMEDOUT (Connection timed out) 没有关闭长连接
     */
    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, ExceptionEvent e) throws Exception {
        super.exceptionCaught(ctx, e);
        if (e.getChannel() == null || !e.getChannel().isConnected()) {
            callback.onDisConnected(1);
        }
        //logger.e("channel#[网络异常了]exceptionCaught:%s", e.getCause().toString());
    }
}
