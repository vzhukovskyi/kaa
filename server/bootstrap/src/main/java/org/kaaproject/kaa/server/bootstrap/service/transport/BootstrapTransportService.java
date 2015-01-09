package org.kaaproject.kaa.server.bootstrap.service.transport;

import java.util.Properties;

import org.kaaproject.kaa.server.transport.AbstractTransportService;
import org.kaaproject.kaa.server.transport.TransportService;
import org.kaaproject.kaa.server.transport.message.MessageHandler;
import org.springframework.beans.factory.annotation.Autowired;

public class BootstrapTransportService extends AbstractTransportService implements TransportService {
    
    @Autowired
    private Properties properties;

    public BootstrapTransportService() {
        super();
    }

    @Override
    protected Properties getServiceProperties() {
        return properties;
    }

    @Override
    protected MessageHandler getMessageHandler() {
        // TODO Auto-generated method stub
        return null;
    }

}
