package com.game.http.ChannelWrapper;

import java.io.IOException;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.concurrent.Future;

/** 
 * @author  liuziang
 * @contact liuziang@liuziangexit.com
 * @date    11/22/2018
 * 
 * HTTP请求实用工具
 * 
 * 这是一个AsynchronousSocketChannel的包装
 * 暂时只包装了回调接口
 * 
 */

public class AsyncChannelWrapper {

    public AsyncChannelWrapper(AsynchronousSocketChannel src) {
	this.socketChannel = src;
    }

    public <AttachmentType> void connect(SocketAddress remote, AttachmentType attachment,
	    CompletionHandler<Void, ? super AttachmentType> handler) {
	this.socketChannel.connect(remote, attachment, handler);
    }

    public void close() {
	if (this.socketChannel.isOpen()) {
	    try {
		this.socketChannel.close();
	    } catch (IOException e) {
	    }
	}
    }

    public <AttachmentType> void read(ByteBuffer dst, AttachmentType attachment,
	    CompletionHandler<Integer, ? super AttachmentType> handler) {
	this.socketChannel.read(dst, attachment, handler);
    }

    public Future<Integer> read(ByteBuffer dst) {
	return this.socketChannel.read(dst);
    }

    public <AttachmentType> void write(ByteBuffer src, AttachmentType attachment,
	    CompletionHandler<Integer, ? super AttachmentType> handler) {
	this.socketChannel.write(src, attachment, handler);
    }

    protected final AsynchronousSocketChannel socketChannel;

}
