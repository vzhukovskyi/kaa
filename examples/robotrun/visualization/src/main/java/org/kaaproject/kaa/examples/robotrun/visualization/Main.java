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

package org.kaaproject.kaa.examples.robotrun.visualization;

import org.kaaproject.kaa.examples.robotrun.visualization.Context.ContextInitCallback;

public class Main implements ContextInitCallback {
    
    public static void main(String[] args) {
        Main main = new Main();
        main.init();
    }
    
    private Context context;
    private VisualizationFrame frame;
    
    public Main() {
    }

    private void init() {
        context = new Context();
        context.init(this);
    }

    @Override
    public void onContextInitCompleted() {
        frame = new VisualizationFrame(context);
    }
    
}
