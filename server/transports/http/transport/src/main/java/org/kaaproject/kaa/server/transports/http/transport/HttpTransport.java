/*
 * Copyright 2014 CyberVision, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.kaaproject.kaa.server.transports.http.transport;

import io.netty.channel.ChannelInitializer;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.channel.socket.SocketChannel;
import io.netty.util.concurrent.DefaultEventExecutorGroup;
import io.netty.util.concurrent.EventExecutorGroup;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.UUID;

import org.kaaproject.kaa.server.common.server.AbstractNettyServer;
import org.kaaproject.kaa.server.common.server.http.AbstractCommand;
import org.kaaproject.kaa.server.common.server.http.DefaultHttpServerInitializer;
import org.kaaproject.kaa.server.transport.AbstractKaaTransport;
import org.kaaproject.kaa.server.transport.TransportLifecycleException;
import org.kaaproject.kaa.server.transport.TransportMetaData;
import org.kaaproject.kaa.server.transport.TransportProperties;
import org.kaaproject.kaa.server.transport.http.config.gen.AvroHttpConfig;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Implementation of Kaa http transport
 * 
 * @author Andrew Shvayka
 *
 */
public class HttpTransport extends AbstractKaaTransport<AvroHttpConfig> {

    private static final String BIND_INTERFACE_PROP_NAME = "transport.bindInterface";
    private static final String LOCALHOST = "localhost";
    private static final Logger LOG = LoggerFactory.getLogger(HttpTransport.class);
    private static final Charset UTF8 = Charset.forName("UTF-8");
    private static final int SIZE_OF_INT = 4;
    private static final int SUPPORTED_VERSION = 1;
    private AvroHttpConfig configuration;
    private AbstractNettyServer netty;
    //TODO: move to avro config
    private static final int EXECUTOR_GROUP_SIZE = 1;

    @Override
    public void init(TransportProperties commonProperties, AvroHttpConfig configuration) throws TransportLifecycleException {
        this.configuration = configuration;
        this.configuration.setBindInterface(replaceProperty(this.configuration.getBindInterface(), BIND_INTERFACE_PROP_NAME,
                commonProperties.getProperty(BIND_INTERFACE_PROP_NAME, LOCALHOST)));

        final EventExecutorGroup executorGroup = new DefaultEventExecutorGroup(EXECUTOR_GROUP_SIZE);

        this.netty = new AbstractNettyServer(configuration.getBindInterface(), configuration.getBindPort(),
                configuration.getThreadPoolSize()) {

            @Override
            protected ChannelInitializer<SocketChannel> configureInitializer() throws Exception {
                return new DefaultHttpServerInitializer() {
                    @Override
                    protected SimpleChannelInboundHandler<AbstractCommand> getMainHandler(UUID uuid) {
                        return new HttpHandler(uuid, HttpTransport.this.handler, executorGroup);
                    }
                };
            }
        };
    }

    @Override
    public void start() {
        netty.init();
        netty.start();
    }

    @Override
    public void stop() {
        netty.shutdown();
    }

    @Override
    public Class<AvroHttpConfig> getConfigurationClass() {
        return AvroHttpConfig.class;
    }

    @Override
    public TransportMetaData getConnectionInfo() {
        LOG.info("Serializing configuration info {}", configuration);
        byte[] interfaceData = toUTF8Bytes(configuration.getBindInterface());
        ByteBuffer buf = ByteBuffer.wrap(new byte[SIZE_OF_INT + interfaceData.length]);
        buf.putInt(configuration.getBindPort());
        buf.put(interfaceData);
        LOG.trace("Serialized configuration info {}", Arrays.toString(buf.array()));
        return new TransportMetaData(SUPPORTED_VERSION, SUPPORTED_VERSION, buf.array());
    }

    private String replaceProperty(String source, String propertyName, String propertyValue) {
        return source.replace("${" + propertyName + "}", propertyValue);
    }

    private byte[] toUTF8Bytes(String str) {
        return str.getBytes(UTF8);
    }
}
