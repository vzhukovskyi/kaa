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
package org.kaaproject.kaa.server.operations.service.transport;

import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.MessageFormat;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.avro.generic.GenericRecord;
import org.kaaproject.kaa.common.avro.GenericAvroConverter;
import org.kaaproject.kaa.server.common.zk.gen.OperationsNodeInfo;
import org.kaaproject.kaa.server.common.zk.operations.OperationsNode;
import org.kaaproject.kaa.server.operations.service.akka.AkkaService;
import org.kaaproject.kaa.server.operations.service.delta.DefaultDeltaService;
import org.kaaproject.kaa.server.transport.KaaTransportConfig;
import org.kaaproject.kaa.server.transport.Transport;
import org.kaaproject.kaa.server.transport.TransportConfig;
import org.kaaproject.kaa.server.transport.TransportLifecycleException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.config.BeanDefinition;
import org.springframework.context.annotation.ClassPathScanningCandidateComponentProvider;
import org.springframework.core.type.filter.AnnotationTypeFilter;
import org.springframework.stereotype.Service;

@Service
public class DefaultTransportService implements TransportService {

    private static final String TRANSPORT_CONFIGURATION_SCAN_PACKAGE = "org.kaaproject.kaa.server.transport";

    /** The Constant logger. */
    private static final Logger LOG = LoggerFactory.getLogger(DefaultDeltaService.class);

    /** The cache service. */
    @Autowired
    private AkkaService akkaService;

    private OperationsNode operationsNode;

    private final Map<Integer, TransportConfig> configs;
    private final Map<Integer, Transport> transports;

    public DefaultTransportService() {
        super();
        this.configs = new HashMap<Integer, TransportConfig>();
        this.transports = new HashMap<Integer, Transport>();
    }

    @Override
    public void setOperationsNode(OperationsNode operationsNode) {
        this.operationsNode = operationsNode;
    }

    @Override
    public void lookupAndInit() {
        LOG.info("Lookup of available transport configurations started in package {}.", TRANSPORT_CONFIGURATION_SCAN_PACKAGE);
        configs.clear();
        transports.clear();
        ClassPathScanningCandidateComponentProvider scanner = new ClassPathScanningCandidateComponentProvider(false);
        scanner.addIncludeFilter(new AnnotationTypeFilter(KaaTransportConfig.class));
        Set<BeanDefinition> beans = scanner.findCandidateComponents(TRANSPORT_CONFIGURATION_SCAN_PACKAGE);
        for (BeanDefinition bean : beans) {
            LOG.info("Found transport configuration {}", bean.getBeanClassName());
            try {
                Class<?> clazz = Class.forName(bean.getBeanClassName());
                TransportConfig transportConfig = (TransportConfig) clazz.newInstance();
                configs.put(transportConfig.getId(), transportConfig);
            } catch (ReflectiveOperationException e) {
                LOG.error(MessageFormat.format("Failed to init transport configuration for {0}", bean.getBeanClassName()), e);
            }
        }
        LOG.info("Lookup of available transport configurations found {} configurations.", configs.size());

        for (TransportConfig config : configs.values()) {
            LOG.info("Initializing transport with name {} and class {}", config.getName(), config.getTransportClass());
            try {
                Class<?> clazz = Class.forName(config.getTransportClass());
                Transport transport = (Transport) clazz.newInstance();
                LOG.info("Lookup of transport configuration file {}", config.getConfigFileName());
                URL configFileURL = this.getClass().getClassLoader().getResource(config.getConfigFileName());
                GenericAvroConverter<GenericRecord> configConverter = new GenericAvroConverter<GenericRecord>(config.getConfigSchema());
                GenericRecord configRecord = configConverter.decodeJson(Files.readAllBytes(Paths.get(configFileURL.toURI())));
                LOG.info("Lookup of transport configuration file {}", config.getConfigFileName());
                // TODO: init common properties;
                transport.init(null, configConverter.encode(configRecord));
                transports.put(config.getId(), transport);
            } catch (ReflectiveOperationException | IOException | URISyntaxException | TransportLifecycleException e) {
                LOG.error(MessageFormat.format("Failed to init transport for {0}", config.getTransportClass()), e);
            }
        }
    }

    @Override
    public void start() {
        LOG.info("Starting {} available transports.", transports.size());
        for (Entry<Integer, Transport> entry : transports.entrySet()) {
            LOG.info("Starting transport {}.", configs.get(entry).getName());
            entry.getValue().start();
        }
        updateZkData();
    }

    @Override
    public void stop() {
        LOG.info("Stoping {} available transports.", transports.size());
        for (Entry<Integer, Transport> entry : transports.entrySet()) {
            LOG.info("Stoping transport {}.", configs.get(entry).getName());
            entry.getValue().stop();
        }
        updateZkData();
    }

    private void updateZkData() {
        LOG.info("Started {} available transports.", transports.size());
        OperationsNodeInfo nodeInfo = operationsNode.getSelfNodeInfo();
        // TODO: populate data from transport meta data;
        try {
            operationsNode.updateNodeData(nodeInfo);
        } catch (IOException e) {
            LOG.error("Failed to update zk data.", e);
        }
    }
}
