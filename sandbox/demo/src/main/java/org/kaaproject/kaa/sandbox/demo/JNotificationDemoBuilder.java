/*
 * Copyright 2014-2015 CyberVision, Inc.
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

package org.kaaproject.kaa.sandbox.demo;

import org.kaaproject.kaa.common.dto.*;
import org.kaaproject.kaa.common.dto.admin.SdkPlatform;
import org.kaaproject.kaa.common.dto.user.UserVerifierDto;
import org.kaaproject.kaa.server.common.admin.AdminClient;
import org.kaaproject.kaa.server.common.core.algorithms.generation.DefaultRecordGenerationAlgorithm;
import org.kaaproject.kaa.server.common.core.algorithms.generation.DefaultRecordGenerationAlgorithmImpl;
import org.kaaproject.kaa.server.common.core.configuration.RawData;
import org.kaaproject.kaa.server.common.core.configuration.RawDataFactory;
import org.kaaproject.kaa.server.common.core.schema.RawSchema;
import org.kaaproject.kaa.server.verifiers.trustful.config.TrustfulVerifierConfig;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Date;
import java.util.List;

public class JNotificationDemoBuilder extends AbstractDemoBuilder {
    private static final Logger logger = LoggerFactory.getLogger(JConfigurationDemoBuilder.class);
    private static final int NOTIFICATION_VERSION = 1;
    private static final Date NOTIFICATION_EXPIRE_DATE = new Date(1900000000000L);

    public JNotificationDemoBuilder() {
        super("demo/jnotification");
    }

    @Override
    protected void buildDemoApplicationImpl(AdminClient client) throws Exception {

        logger.info("Loading 'Java notification demo application' data...");

        loginTenantAdmin(client);

        ApplicationDto notificationApplication = new ApplicationDto();
        notificationApplication.setName("Java notification");
        notificationApplication = client.editApplication(notificationApplication);

        sdkKey.setApplicationId(notificationApplication.getId());
        sdkKey.setProfileSchemaVersion(1);
        sdkKey.setConfigurationSchemaVersion(1);
        sdkKey.setLogSchemaVersion(1);
        sdkKey.setTargetPlatform(SdkPlatform.JAVA);

        loginTenantDeveloper(client);

        logger.info("Creating notification schema...");
        NotificationSchemaDto notificationSchemaDto = new NotificationSchemaDto();
        notificationSchemaDto.setApplicationId(notificationApplication.getId());
        notificationSchemaDto.setName("Notification schema");
        notificationSchemaDto.setDescription("Notification schema of a sample notification");
        notificationSchemaDto = client.createNotificationSchema(notificationSchemaDto, getResourcePath("notification_schema.avsc"));
        sdkKey.setNotificationSchemaVersion(notificationSchemaDto.getMajorVersion());
        logger.info("Notification schema was created.");

        TopicDto optionalTopic = new TopicDto();
        optionalTopic.setApplicationId(notificationApplication.getId());
        optionalTopic.setName("Sample optional topic");
        optionalTopic.setType(TopicTypeDto.OPTIONAL);
        optionalTopic.setDescription("Sample optional topic to demonstrate notifications API");
        logger.info("Creating optional topic: {}", optionalTopic);
        optionalTopic = client.createTopic(optionalTopic);
        logger.info("Optional topic {} was created", optionalTopic);

        NotificationDto optionalTopicNotification = new NotificationDto();
        optionalTopicNotification.setApplicationId(notificationApplication.getId());
        optionalTopicNotification.setSchemaId(notificationSchemaDto.getId());
        optionalTopicNotification.setVersion(NOTIFICATION_VERSION);
        optionalTopicNotification.setType(NotificationTypeDto.USER);
        optionalTopicNotification.setExpiredAt(NOTIFICATION_EXPIRE_DATE);
        optionalTopicNotification.setTopicId(optionalTopic.getId());
        logger.info("Creating notification for optional topic: {}", optionalTopicNotification.toString());
        client.sendNotification(optionalTopicNotification, getResourcePath("optional_notification.json"));
        logger.info("Notification for optional topic was created");

        logger.info("Getting base endpoint group");
        EndpointGroupDto baseEndpointGroup = null;
        List<EndpointGroupDto> endpointGroups = client.getEndpointGroups(notificationApplication.getId());
        if (endpointGroups.size() == 1 && endpointGroups.get(0).getWeight() == 0) {
            baseEndpointGroup = endpointGroups.get(0);
        }

        if (baseEndpointGroup == null) {
            throw new RuntimeException("Can't get default endpoint group for Java configuration demo application!");
        }

        logger.info("Base endpoint group was successfully gotten");

        TopicDto mandatoryTopic = new TopicDto();
        mandatoryTopic.setApplicationId(notificationApplication.getId());
        mandatoryTopic.setName("Sample mandatory topic");
        mandatoryTopic.setType(TopicTypeDto.MANDATORY);
        mandatoryTopic.setDescription("Sample mandatory topic to demonstrate notifications API");
        mandatoryTopic = client.createTopic(mandatoryTopic);
        client.addTopicToEndpointGroup(baseEndpointGroup, mandatoryTopic);

        NotificationDto mandatoryNotification = new NotificationDto();
        mandatoryNotification.setApplicationId(notificationApplication.getId());
        mandatoryNotification.setSchemaId(notificationSchemaDto.getId());
        mandatoryNotification.setVersion(NOTIFICATION_VERSION);
        mandatoryNotification.setType(NotificationTypeDto.USER);
        mandatoryNotification.setExpiredAt(NOTIFICATION_EXPIRE_DATE);
        mandatoryNotification.setTopicId(mandatoryTopic.getId());
        client.sendNotification(mandatoryNotification, getResourcePath("mandatory_notification.json"));

        TrustfulVerifierConfig trustfulVerifierConfig = new TrustfulVerifierConfig();
        UserVerifierDto trustfulUserVerifier = new UserVerifierDto();
        trustfulUserVerifier.setApplicationId(notificationApplication.getId());
        trustfulUserVerifier.setName("Trustful verifier");
        trustfulUserVerifier.setPluginClassName(trustfulVerifierConfig.getPluginClassName());
        trustfulUserVerifier.setPluginTypeName(trustfulVerifierConfig.getPluginTypeName());
        RawSchema rawSchema = new RawSchema(trustfulVerifierConfig.getPluginConfigSchema().toString());
        DefaultRecordGenerationAlgorithm<RawData> algotithm =
                new DefaultRecordGenerationAlgorithmImpl<>(rawSchema, new RawDataFactory());
        RawData rawData = algotithm.getRootData();
        trustfulUserVerifier.setJsonConfiguration(rawData.getRawData());
        trustfulUserVerifier = client.editUserVerifierDto(trustfulUserVerifier);
        sdkKey.setDefaultVerifierToken(trustfulUserVerifier.getVerifierToken());

        logger.info("Finished loading 'Java notification demo application' data...");
    }
}
