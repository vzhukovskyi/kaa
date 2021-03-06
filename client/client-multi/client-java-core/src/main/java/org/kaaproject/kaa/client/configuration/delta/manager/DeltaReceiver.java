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

package org.kaaproject.kaa.client.configuration.delta.manager;

import org.kaaproject.kaa.client.configuration.delta.ConfigurationDelta;

/**
 * Interface for listener to receive configuration deltas.
 *
 * @author Yaroslav Zeygerman
 *
 */
public interface DeltaReceiver {

    /**
     * This callback will be called on each received delta (either for a root
     * record or some configuration subtree).
     *
     * @param delta Configuration delta
     * @see ConfigurationDelta
     *
     */
    void loadDelta(ConfigurationDelta delta);

}
