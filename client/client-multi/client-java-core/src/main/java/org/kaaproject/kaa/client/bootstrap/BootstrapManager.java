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

package org.kaaproject.kaa.client.bootstrap;

import java.util.List;

import org.kaaproject.kaa.client.channel.BootstrapTransport;
import org.kaaproject.kaa.client.channel.KaaChannelManager;
import org.kaaproject.kaa.client.channel.KaaInternalChannelManager;
import org.kaaproject.kaa.client.channel.TransportProtocolId;
import org.kaaproject.kaa.client.transport.TransportException;
import org.kaaproject.kaa.common.endpoint.gen.ProtocolMetaData;

/**
 * Bootstrap manager manages the list of available operation servers.
 *
 * @author Yaroslav Zeygerman
 *
 */
public interface BootstrapManager {

    /**
     * Receives the latest list of servers from the bootstrap server.
     *
     */
    void receiveOperationsServerList() throws TransportException;

    /**
     * Force switch to the next operations server that support given {@link TransportProtocolId}
     *
     * @param transportId of the transport protocol.
     * @see TransportProtocolId
     */
    void useNextOperationsServer(TransportProtocolId transportId);

    /**
     * Update the Channel Manager with endpoint's properties retrieved by its DNS.
     *
     * @param name endpoint's DNS.
     *
     */
    void useNextOperationsServerByAccessPointId(int accessPointId);

    /**
     * Sets bootstrap transport object.
     *
     * @param transport object to be set.
     * @see BootstrapTransport
     */
    void setTransport(BootstrapTransport transport);

    /**
     * Sets Channel manager.
     *
     * @param manager the channel manager to be set.
     * @see KaaChannelManager
     */
    void setChannelManager(KaaInternalChannelManager manager);

    /**
     * Updates the operation server list.
     *
     * @param list the operation server list.
     * @see OperationsServerList
     */
    void onProtocolListUpdated(List<ProtocolMetaData> list);
}
