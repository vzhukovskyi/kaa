package org.kaaproject.kaa.server.transport.http.config.gen;

import org.apache.avro.Schema;
import org.kaaproject.kaa.server.transport.KaaTransportConfig;
import org.kaaproject.kaa.server.transport.TransportConfig;

@KaaTransportConfig
public class HttpTransportConfig implements TransportConfig {

    private static final int HTTP_TRANSPORT_ID = 0xfb9a3cf0;
    private static final String HTTP_TRANSPORT_NAME = "org.kaaproject.kaa.server.transport.http";

    public HttpTransportConfig() {
        super();
    }

    @Override
    public int getId() {
        return HTTP_TRANSPORT_ID;
    }

    @Override
    public String getName() {
        return HTTP_TRANSPORT_NAME;
    }

    @Override
    public String getTransportClass() {
        return "org.kaaproject.kaa.server.transport.http.transport.HttpTransport";
    }

    @Override
    public Schema getConfigSchema() {
        return AvroHttpConfig.getClassSchema();
    }

}
