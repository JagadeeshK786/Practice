package com.game.http.ChannelWrapper;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.security.KeyStore;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import javax.net.ssl.KeyManager;
import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLEngine;
import javax.net.ssl.SSLEngineResult;
import javax.net.ssl.SSLEngineResult.HandshakeStatus;
import javax.net.ssl.SSLEngineResult.Status;
import javax.net.ssl.SSLException;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;

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

public class AsyncChannelSslWrapper extends AsyncChannelWrapper {

    public AsyncChannelSslWrapper(AsynchronousSocketChannel src) {
	super(src);

	SSLContext context = null;
	try {
	    context = SSLContext.getInstance("TLSv1.2");
	} catch (NoSuchAlgorithmException e) {
	    // should never happen
	    e.printStackTrace();
	}
	// context.init(createKeyManagers("./src/main/resources/client.jks",
	// "storepass", "keypass"),
	// createTrustManagers("./src/main/resources/trustedCerts.jks", "storepass"),
	// new SecureRandom());
	engine = context.createSSLEngine();
	engine.setUseClientMode(true);

	SSLSession session = engine.getSession();
	myAppData = ByteBuffer.allocate(1024);
	myNetData = ByteBuffer.allocate(session.getPacketBufferSize());
	peerAppData = ByteBuffer.allocate(1024);
	peerNetData = ByteBuffer.allocate(session.getPacketBufferSize());
    }

    public <AttachmentType> void connect(SocketAddress remote, AttachmentType attachment,
	    CompletionHandler<Void, ? super AttachmentType> handler) {
	AsyncChannelSslWrapper sock = this;
	super.connect(remote, attachment, new CompletionHandler<Void, AttachmentType>() {
	    @Override
	    public void completed(Void result, AttachmentType attachment) {
		try {
		    engine.beginHandshake();
		    if (!doHandshake(socketChannel, engine)) {
			sock.close();
			handler.failed(new Throwable("SSL handshake failed"), attachment);
		    } else {
			handler.completed(null, attachment);
		    }
		} catch (Exception e) {
		    handler.failed(e, attachment);
		}
	    }

	    @Override
	    public void failed(Throwable exc, AttachmentType attachment) {
		handler.failed(exc, attachment);
	    }
	});
    }

    public void close() {
	try {
	    shutdownSsl(socketChannel, engine);
	} catch (IOException | InterruptedException | ExecutionException e) {
	    e.printStackTrace();
	} finally {
	    executor.shutdown();
	    super.close();
	}
    }

    public <AttachmentType> void read(ByteBuffer dst, AttachmentType attachment,
	    CompletionHandler<Integer, ? super AttachmentType> handler) {
	super.read(peerNetData, attachment, new CompletionHandler<Integer, AttachmentType>() {
	    @Override
	    public void completed(Integer bytesRead, AttachmentType attachment) {
		try {
		    peerNetData.clear();
		    boolean exitReadLoop = false;
		    while (!exitReadLoop) {
			if (bytesRead > 0) {
			    peerNetData.flip();
			    while (peerNetData.hasRemaining()) {
				peerAppData.clear();
				SSLEngineResult result = engine.unwrap(peerNetData, peerAppData);
				switch (result.getStatus()) {
				case OK:
				    peerAppData.flip();
				    exitReadLoop = true;
				    break;
				case BUFFER_OVERFLOW:
				    peerAppData = enlargeApplicationBuffer(engine, peerAppData);
				    break;
				case BUFFER_UNDERFLOW:
				    peerNetData = handleBufferUnderflow(engine, peerNetData);
				    break;
				case CLOSED:
				    shutdownSsl(socketChannel, engine);
				    return;
				default:
				    throw new IllegalStateException("Invalid SSL status: " + result.getStatus());
				}
			    }
			} else if (bytesRead < 0) {
			    handleEndOfStream(socketChannel, engine);
			    return;
			}
		    }
		} catch (IOException | InterruptedException | ExecutionException e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		}
	    }

	    @Override
	    public void failed(Throwable exc, AttachmentType attachment) {
	    }

	});
    }

    public Future<Integer> read(ByteBuffer dst) {
	return super.read(dst);
    }

    public <AttachmentType> void write(ByteBuffer src, AttachmentType attachment,
	    CompletionHandler<Integer, ? super AttachmentType> handler) {
	super.write(src, attachment, handler);
    }

    private boolean doHandshake(AsynchronousSocketChannel socketChannel, SSLEngine engine)
	    throws IOException, InterruptedException, ExecutionException {
	SSLEngineResult result;
	HandshakeStatus handshakeStatus;

	int appBufferSize = engine.getSession().getApplicationBufferSize();
	ByteBuffer myAppData = ByteBuffer.allocate(appBufferSize);
	ByteBuffer peerAppData = ByteBuffer.allocate(appBufferSize);
	myNetData.clear();
	peerNetData.clear();

	handshakeStatus = engine.getHandshakeStatus();
	while (handshakeStatus != SSLEngineResult.HandshakeStatus.FINISHED
		&& handshakeStatus != SSLEngineResult.HandshakeStatus.NOT_HANDSHAKING) {
	    switch (handshakeStatus) {
	    case NEED_UNWRAP:
		if (socketChannel.read(peerNetData).get() < 0) {
		    if (engine.isInboundDone() && engine.isOutboundDone()) {
			return false;
		    }
		    try {
			engine.closeInbound();
		    } catch (SSLException e) {
		    }
		    engine.closeOutbound();
		    // After closeOutbound the engine will be set to WRAP state, in order to try to
		    // send a close message to the client.
		    handshakeStatus = engine.getHandshakeStatus();
		    break;
		}
		peerNetData.flip();
		try {
		    result = engine.unwrap(peerNetData, peerAppData);
		    peerNetData.compact();
		    handshakeStatus = result.getHandshakeStatus();
		} catch (SSLException sslException) {
		    engine.closeOutbound();
		    handshakeStatus = engine.getHandshakeStatus();
		    break;
		}
		switch (result.getStatus()) {
		case OK:
		    break;
		case BUFFER_OVERFLOW:
		    // Will occur when peerAppData's capacity is smaller than the data derived from
		    // peerNetData's unwrap.
		    peerAppData = enlargeApplicationBuffer(engine, peerAppData);
		    break;
		case BUFFER_UNDERFLOW:
		    // Will occur either when no data was read from the peer or when the peerNetData
		    // buffer was too small to hold all peer's data.
		    peerNetData = handleBufferUnderflow(engine, peerNetData);
		    break;
		case CLOSED:
		    if (engine.isOutboundDone()) {
			return false;
		    } else {
			engine.closeOutbound();
			handshakeStatus = engine.getHandshakeStatus();
			break;
		    }
		default:
		    throw new IllegalStateException("Invalid SSL status: " + result.getStatus());
		}
		break;
	    case NEED_WRAP:
		myNetData.clear();
		try {
		    result = engine.wrap(myAppData, myNetData);
		    handshakeStatus = result.getHandshakeStatus();
		} catch (SSLException sslException) {
		    engine.closeOutbound();
		    handshakeStatus = engine.getHandshakeStatus();
		    break;
		}
		switch (result.getStatus()) {
		case OK:
		    myNetData.flip();
		    while (myNetData.hasRemaining()) {
			socketChannel.write(myNetData);
		    }
		    break;
		case BUFFER_OVERFLOW:
		    // Will occur if there is not enough space in myNetData buffer to write all the
		    // data that would be generated by the method wrap.
		    // Since myNetData is set to session's packet size we should not get to this
		    // point because SSLEngine is supposed
		    // to produce messages smaller or equal to that, but a general handling would be
		    // the following:
		    myNetData = enlargePacketBuffer(engine, myNetData);
		    break;
		case BUFFER_UNDERFLOW:
		    throw new SSLException(
			    "Buffer underflow occured after a wrap. I don't think we should ever get here.");
		case CLOSED:
		    try {
			myNetData.flip();
			while (myNetData.hasRemaining()) {
			    socketChannel.write(myNetData);
			}
			// At this point the handshake status will probably be NEED_UNWRAP so we make
			// sure that peerNetData is clear to read.
			peerNetData.clear();
		    } catch (Exception e) {
			handshakeStatus = engine.getHandshakeStatus();
		    }
		    break;
		default:
		    throw new IllegalStateException("Invalid SSL status: " + result.getStatus());
		}
		break;
	    case NEED_TASK:
		Runnable task;
		while ((task = engine.getDelegatedTask()) != null) {
		    executor.execute(task);
		}
		handshakeStatus = engine.getHandshakeStatus();
		break;
	    case FINISHED:
		break;
	    case NOT_HANDSHAKING:
		break;
	    default:
		throw new IllegalStateException("Invalid SSL status: " + handshakeStatus);
	    }
	}

	return true;

    }

    private ByteBuffer enlargePacketBuffer(SSLEngine engine, ByteBuffer buffer) {
	return enlargeBuffer(buffer, engine.getSession().getPacketBufferSize());
    }

    private ByteBuffer enlargeApplicationBuffer(SSLEngine engine, ByteBuffer buffer) {
	return enlargeBuffer(buffer, engine.getSession().getApplicationBufferSize());
    }

    private ByteBuffer enlargeBuffer(ByteBuffer buffer, int sessionProposedCapacity) {
	if (sessionProposedCapacity > buffer.capacity()) {
	    buffer = ByteBuffer.allocate(sessionProposedCapacity);
	} else {
	    buffer = ByteBuffer.allocate(buffer.capacity() * 2);
	}
	return buffer;
    }

    private ByteBuffer handleBufferUnderflow(SSLEngine engine, ByteBuffer buffer) {
	if (engine.getSession().getPacketBufferSize() < buffer.limit()) {
	    return buffer;
	} else {
	    ByteBuffer replaceBuffer = enlargePacketBuffer(engine, buffer);
	    buffer.flip();
	    replaceBuffer.put(buffer);
	    return replaceBuffer;
	}
    }

    private void shutdownSsl(AsynchronousSocketChannel socketChannel, SSLEngine engine)
	    throws IOException, InterruptedException, ExecutionException {
	engine.closeOutbound();
	doHandshake(socketChannel, engine);
    }

    private void handleEndOfStream(AsynchronousSocketChannel socketChannel, SSLEngine engine)
	    throws IOException, InterruptedException, ExecutionException {
	try {
	    engine.closeInbound();
	} catch (Exception e) {
	}
	shutdownSsl(socketChannel, engine);
    }

    private KeyManager[] createKeyManagers(String filepath, String keystorePassword, String keyPassword)
	    throws Exception {
	KeyStore keyStore = KeyStore.getInstance("JKS");
	InputStream keyStoreIS = new FileInputStream(filepath);
	try {
	    keyStore.load(keyStoreIS, keystorePassword.toCharArray());
	} finally {
	    if (keyStoreIS != null) {
		keyStoreIS.close();
	    }
	}
	KeyManagerFactory kmf = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
	kmf.init(keyStore, keyPassword.toCharArray());
	return kmf.getKeyManagers();
    }

    private TrustManager[] createTrustManagers(String filepath, String keystorePassword) throws Exception {
	KeyStore trustStore = KeyStore.getInstance("JKS");
	InputStream trustStoreIS = new FileInputStream(filepath);
	try {
	    trustStore.load(trustStoreIS, keystorePassword.toCharArray());
	} finally {
	    if (trustStoreIS != null) {
		trustStoreIS.close();
	    }
	}
	TrustManagerFactory trustFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
	trustFactory.init(trustStore);
	return trustFactory.getTrustManagers();
    }

    private ByteBuffer myAppData;
    private ByteBuffer myNetData;
    private ByteBuffer peerAppData;
    private ByteBuffer peerNetData;

    private SSLEngine engine;
    private ExecutorService executor = Executors.newSingleThreadExecutor();

}
