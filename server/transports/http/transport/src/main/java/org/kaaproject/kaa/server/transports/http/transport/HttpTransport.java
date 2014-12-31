package org.kaaproject.kaa.server.transports.http.transport;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;

import org.kaaproject.kaa.server.transport.AbstractKaaTransport;
import org.kaaproject.kaa.server.transport.TransportLifecycleException;
import org.kaaproject.kaa.server.transport.TransportProperties;
import org.kaaproject.kaa.server.transport.http.config.gen.AvroHttpConfig;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class HttpTransport extends AbstractKaaTransport<AvroHttpConfig> {
    
    private static final Logger LOG = LoggerFactory.getLogger(HttpTransport.class);
    private static final Charset UTF8 = Charset.forName("UTF-8");
    private static final int SIZE_OF_INT = 4;
    private AvroHttpConfig configuration;

    @Override
    public byte[] getConnectionInfo() {
        LOG.info("Serializing configuration info {}", configuration);
        byte[] interfaceData = toUTF8Bytes(configuration.getBindInterface());
        ByteBuffer buf = ByteBuffer.wrap(new byte[SIZE_OF_INT + interfaceData.length]);
        buf.putInt(configuration.getBindPort());
        buf.put(interfaceData);
        LOG.trace("Serialized configuration info {}", Arrays.toString(buf.array()));
        return buf.array();
    }

    private byte[] toUTF8Bytes(String str) {
        return str.getBytes(UTF8);
    }

    @Override
    public void start() {
        // TODO Auto-generated method stub
    }

    @Override
    public void stop() {
        // TODO Auto-generated method stub
    }

    @Override
    public void init(TransportProperties commonProperties, AvroHttpConfig configuration) throws TransportLifecycleException {
        this.configuration = configuration;
    }

    @Override
    public Class<AvroHttpConfig> getConfigurationClass() {
        return AvroHttpConfig.class;
    }

}
