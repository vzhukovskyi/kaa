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

package org.kaaproject.kaa.sandbox.web.client.mvp.view;

import java.util.List;

import org.kaaproject.kaa.sandbox.demo.projects.Feature;
import org.kaaproject.kaa.sandbox.demo.projects.Platform;

import com.google.gwt.event.dom.client.HasClickHandlers;
import com.google.gwt.user.client.ui.HasText;
import com.google.gwt.user.client.ui.Image;

public interface ProjectView extends BaseView {

    void setTargetPlatform(Platform platform);
    
    void setFeatures(List<Feature> features);
    
    Image getApplicationImage();
    
    HasText getDescription();
    
    HasClickHandlers getSourceButton();
    
    HasClickHandlers getBinaryButton();
    
}
