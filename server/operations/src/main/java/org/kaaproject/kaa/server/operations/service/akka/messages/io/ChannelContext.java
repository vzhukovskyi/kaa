package org.kaaproject.kaa.server.operations.service.akka.messages.io;

public interface ChannelContext {

    void writeAndFlush(Object response);

    void fireExceptionCaught(Exception e);

    void write(Object object);

    void flush();

}
