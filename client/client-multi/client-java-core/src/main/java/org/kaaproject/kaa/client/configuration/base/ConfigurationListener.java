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
package org.kaaproject.kaa.client.configuration.base;

import javax.annotation.Generated;

import org.kaaproject.kaa.schema.base.Configuration;

/**
 * The listener to configuration updates.
 *
 * @author Andrew Shvayka
 *
 */
@Generated("ConfigurationListener.java.template")
public interface ConfigurationListener {
    /**
     * Called on each configuration update.
     *
     * @param configuration the configuration object.
     *
     */
    void onConfigurationUpdate(Configuration configuration);
}
