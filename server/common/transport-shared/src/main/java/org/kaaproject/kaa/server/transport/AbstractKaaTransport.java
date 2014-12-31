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
package org.kaaproject.kaa.server.transport;

import java.io.IOException;
import java.text.MessageFormat;

import org.apache.avro.specific.SpecificRecordBase;
import org.kaaproject.kaa.common.avro.AvroByteArrayConverter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Abstract implementation of {@link Transport} that handles deserialization of
 * binary configuration.
 * 
 * @author Andrew Shvayka
 *
 * @param <T>
 *            specific configuration record type
 */
public abstract class AbstractKaaTransport<T extends SpecificRecordBase> implements Transport {
    private static final Logger LOG = LoggerFactory.getLogger(AbstractKaaTransport.class);

    /*
     * (non-Javadoc)
     * 
     * @see org.kaaproject.kaa.server.transport.Transport#init(byte[])
     */
    @Override
    public void init(TransportProperties commonProperties, byte[] configuration) throws TransportLifecycleException {
        AvroByteArrayConverter<T> converter = new AvroByteArrayConverter<>(getConfigurationClass());
        try {
            T config = converter.fromByteArray(configuration);
            LOG.info("Initializing transport {} with {}", getClassName(), config);
            init(commonProperties, config);
        } catch (IOException e) {
            LOG.error(MessageFormat.format("Failed to initialize transport {0}", getClassName()), e);
            throw new TransportLifecycleException(e);
        }
    }

    /**
     * Initializes transport with specific configuration object.
     *
     * @param configuration
     *            the configuration
     * @throws TransportLifecycleException
     */
    public abstract void init(TransportProperties commonProperties, T configuration) throws TransportLifecycleException;

    /**
     * Gets the configuration class.
     *
     * @return the configuration class
     */
    public abstract Class<T> getConfigurationClass();

    private String getClassName() {
        return this.getClass().getName();
    }
}
