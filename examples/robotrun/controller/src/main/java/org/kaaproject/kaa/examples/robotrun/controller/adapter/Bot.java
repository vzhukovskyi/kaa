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

/**
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
package org.kaaproject.kaa.examples.robotrun.controller.adapter;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.kaaproject.kaa.examples.robotrun.controller.adapter.bluetooth.BTDriver;
import org.kaaproject.kaa.examples.robotrun.controller.adapter.bluetooth.BTManageable;
import org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable;
import org.kaaproject.kaa.examples.robotrun.controller.robot.PingDirection;
import org.kaaproject.kaa.examples.robotrun.controller.robot.TurnDirection;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.ErrorCallback;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.MoveForwardCallback;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.OperationCallback;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.OperationStatus;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.PingCallback;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.PongStatus;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.StateCallback;
import org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.TurnCallback;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Robot bluetooth drivable adapter class.
 * Implement Drivable interface and handle BTDriver instance.
 * 
 * @author Andriy Panasenko <apanasenko@cybervisiontech.com>
 *
 */
public class Bot implements Drivable,BTManageable {

    /** Robotrun Bluetooth commands and responses */ 
    private final static String COMMAND_BUSY = ".*u.*";
    private final static String COMMAND_INVALID = ".*i.*";
    private final static String SEND_COMMAND_TURN_LEFT = "l";
    private final static String COMMAND_TURN_LEFT = ".*l.*\\s+";
    private final static String SEND_COMMAND_TURN_RIGHT = "r";
    private final static String COMMAND_TURN_RIGHT = ".*r.*\\s+";
    private final static String SEND_COMMAND_MOVE_FORWARD = "f";
    private final static String SEND_COMMAND_MOVE_FORWARD_CLBR = "F";
    private final static String COMMAND_MOVE_FORWARD = ".*f.*\\s+";
    private final static String SEND_COMMAND_PING_LEFT = "b";
    private final static String SEND_COMMAND_PING_FRONT = "n";
    private final static String SEND_COMMAND_PING_RIGHT = "m";
    private final static String COMMAND_PING = ".*p([0-9]+).*";
    private final static String COMMAND_PING_COMPLETE = "\\s*.*p([0-9]+).*\\s+";
    private final static String SEND_COMMAND_KEEP_ALIVE = "k";
    private final static String COMMAND_KEEP_ALIVE = "\\s*k\\s+";
    
    /** Default distance, if less - mean WALL if more - mean EMPTY */
    private final static int DISTANCE_TO_WALL = 30;
    
    /** Constant logger */
    private static final Logger LOG = LoggerFactory.getLogger(Bot.class);
    
    /** BT driver class */
    private final BTDriver driver;
    
    /** Drivable callbacks */
    private TurnCallback turnCallback;
    private MoveForwardCallback mfCallback;
    private PingCallback pingCallback;
    private ErrorCallback errorCallback;
    private StateCallback stateCallback;
    
    private boolean operated;
    
    /** current wait response */
    private String waitResponse;
    
    /** actually received response string */
    private String recivedResponse;
    
    private String recivedResponseBuffer = "";

    /** status of response */
    private OperationStatus waitStatus;
    
    /** synchronized object */
    private final Object waitSync = new Object();
    
    /** Thread executor, used Fixed size thread with size 1 */
    private ExecutorService executor;
    
    /** BT connected sync */
    private final Object btSync = new Object();
    /** BT connected flag */
    private boolean BtConnected = false;
    
    /**
     * Constructor.
     * @param driverClass - String class name of BTDriver instance.
     * @exception  Exception - throws if initialization fails.
     */
    public Bot(BTDriver driver) throws Exception {
        executor = Executors.newFixedThreadPool(1);
        if (driver == null) {
            throw new BotException("driver is null");
        }
        this.driver = driver;
        this.driver.registerResponseCallback(this);
        this.driver.init();
        LOG.info("Bot: initialization complete.");
    }
    
    @Override
    public void start() throws Exception {
        driver.connect();
        final Object sync = new Object();
        setOperated(false);
        Runner run = new Runner(SEND_COMMAND_KEEP_ALIVE, COMMAND_KEEP_ALIVE, new OperationCallback() {
            @Override
            public void complete(OperationStatus status) {
                if (status == OperationStatus.SUCESSFULL) {
                    setOperated(true);
                } else {
                    setOperated(false);
                }
                synchronized (sync) {
                    sync.notify();
                }
            }
        });
        executor.execute(run);
        
        synchronized (sync) {
            sync.wait(60000);
        }
        if (isOperated()) {
            LOG.info("Bot: start complete.");
        } else {
            LOG.error("Bot: start failed.");
            throw new BotException("Error initialize robot keep alive command failed");
        }
    }
    
    /**
     * Shutdown Bot class.
     */
    @Override
    public void shutdown() {
        LOG.info("Bot: shutdown.");
        driver.shutdown();
        executor.shutdown();
        synchronized (waitSync) {
            waitSync.notifyAll();
        }
        try {
            executor.awaitTermination(1, TimeUnit.MINUTES);
        } catch (InterruptedException e) {
            LOG.error("Error FixedThread Executor shutdown", e);
        }
    }

    /**
     * Ping runner Class.
     * 
     */
    protected class PingRunner implements Runnable {
        
        private String pingCommand;
        
        protected PingRunner(PingDirection direction) {
            switch (direction) {
            case LEFT:
                pingCommand = SEND_COMMAND_PING_LEFT;
                break;
            case FRONT:
                pingCommand = SEND_COMMAND_PING_FRONT;
                break;
            case RIGHT:
                pingCommand = SEND_COMMAND_PING_RIGHT;
                break;
            }
        }

        /* (non-Javadoc)
         * @see java.lang.Runnable#run()
         */
        @Override
        public void run() {
            OperationStatus status = OperationStatus.FAILED;
            waitConnected();
            try {
                getDriver().sendCommand(pingCommand);
                status = waitResponse(COMMAND_PING_COMPLETE);
                if (status == OperationStatus.SUCESSFULL) {
                    LOG.trace("Ping response sucessfull");
                    if (pingCallback != null) {
                        if (recivedResponse != null) {

                            LOG.info("Ping response: "+recivedResponse);
                            Pattern p = Pattern.compile(COMMAND_PING);
                            Matcher m = p.matcher(recivedResponse.trim());
                            if (m.matches()) {
                                String ls = m.group(1);
                                int l = Integer.parseInt(ls);
                                if (l >= DISTANCE_TO_WALL) {
                                    LOG.info("Ping response: "+recivedResponse+" status EMPTY");
                                    pingCallback.pong(PongStatus.EMPTY);
                                } else {
                                    LOG.info("Ping response: "+recivedResponse+" status WALL");
                                    pingCallback.pong(PongStatus.WALL);
                                }
                            } else {
                                LOG.error("Send command Ping: recivedResponse don't matches to "+COMMAND_PING);
                                if (errorCallback != null) {
                                    errorCallback.error(new BotException("Ping: recivedResponse don't matches to "+COMMAND_PING));
                                }
                            }

                        } else {
                            LOG.error("Send command Ping: recived response not initialized");
                            if (errorCallback != null) {
                                errorCallback.error(new BotException("Ping: recived response not initialized"));
                            }
                        }
                    }
                } else {
                    LOG.error("Send command Ping: status failed");
                    if (errorCallback != null) {
                        errorCallback.error(new BotException("Ping status: failed."));
                    }   
                }
            } catch (Exception e) {
                LOG.error("Send command() failed", e);
                if (errorCallback != null) {
                    errorCallback.error(e);
                }
            }
            
        }
        
    }
    
    /**
     * Movement and Turn runner class. 
     */
    protected class Runner implements Runnable {
        
        private final String sendCommand;
        private final String waitResponse;
        private final OperationCallback callback;

        
        protected Runner(String sendCommand, String waitResponse, OperationCallback callback) {
            this.sendCommand = sendCommand;
            this.waitResponse = waitResponse;
            this.callback = callback;
            LOG.debug("Runner() for {}:{}",sendCommand,waitResponse);
        }
        
        /* (non-Javadoc)
         * @see java.lang.Runnable#run()
         */
        @Override
        public void run() {
            LOG.trace("Runner run() for {}:{}",sendCommand,waitResponse);
            OperationStatus status = OperationStatus.FAILED;
            waitConnected();
            try {
                
                getDriver().sendCommand(sendCommand);
                status = waitResponse(waitResponse);
                if (callback != null) {
                    callback.complete(status);
                }
            } catch (Exception e) {
                LOG.error("Send command() failed", e);
                if (errorCallback != null) {
                    errorCallback.error(e);
                }
            }
            
        }
        
    }
    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#turn(org.kaaproject.kaa.examples.robotrun.controller.robot.TurnDirection)
     */
    @Override
    public void turn(TurnDirection direction) {
        LOG.info("Turn run with direction "+direction.toString());
        
        switch (direction) {
        case LEFT:
            Runner runLeft = new Runner(SEND_COMMAND_TURN_LEFT, COMMAND_TURN_LEFT, turnCallback);
            executor.execute(runLeft);
            break;
        case RIGHT:
            Runner runRight = new Runner(SEND_COMMAND_TURN_RIGHT, COMMAND_TURN_RIGHT, turnCallback);
            executor.execute(runRight);
            break;
        default:
            
        }
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#moveForward()
     */
    @Override
    public void moveForward(boolean withCalibration) {
        LOG.info("Move Forward run... ");
        String sc = SEND_COMMAND_MOVE_FORWARD;
        if (withCalibration) {
            sc = SEND_COMMAND_MOVE_FORWARD_CLBR;
        }
        Runner run = new Runner(sc, COMMAND_MOVE_FORWARD, mfCallback);
        executor.execute(run);
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#ping()
     */
    @Override
    public void ping(PingDirection direction) {
        LOG.info("Ping run... "+direction.toString());
        PingRunner run = new PingRunner(direction);
        executor.execute(run);
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#registerTurnCallback(org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.TurnCallback)
     */
    @Override
    public void registerTurnCallback(TurnCallback callback) {
        this.turnCallback = callback;
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#registerMoveForwardCallback(org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.MoveForwardCallback)
     */
    @Override
    public void registerMoveForwardCallback(MoveForwardCallback callback) {
        this.mfCallback = callback;
    }


    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#registerPingCallback(org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.PingCallback)
     */
    @Override
    public void registerPingCallback(PingCallback callback) {
        this.pingCallback = callback;
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#registerErrorCallback(org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.ErrorCallback)
     */
    @Override
    public void registerErrorCallback(ErrorCallback callback) {
        this.errorCallback = callback;
    }

    /**
     * @return the driver
     */
    public BTDriver getDriver() {
        return driver;
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.adapter.bluetooth.BTManageable#onConected()
     */
    @Override
    public void onConected(String deviceName) {
        LOG.info("BT notify: Connected");
        synchronized (btSync) {
            BtConnected = true;
            btSync.notify();
            if (stateCallback != null) {
                stateCallback.onConnected(deviceName);
            }
        }
        
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.adapter.bluetooth.BTManageable#onDisconnected()
     */
    @Override
    public void onDisconnected() {
        LOG.info("BT notify: Disonnected");
        synchronized (btSync) {
            BtConnected = false;
            if (stateCallback != null) {
                stateCallback.onDisconnected();
            }
        }
        synchronized (waitSync) {
            recivedResponse = null;
            waitStatus = OperationStatus.FAILED;
            waitSync.notify();
            if (stateCallback != null) {
                stateCallback.onDisconnected();
            }
        }
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.adapter.bluetooth.BTManageable#onError(java.lang.Exception)
     */
    @Override
    public void onError(Exception error) {
        LOG.error("BT Driver error", error);
        if (errorCallback != null) {
            errorCallback.error(error);
        }
        
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.adapter.bluetooth.BTManageable#onMessage(java.lang.String)
     */
    @Override
    public void onMessage(String msg) {
        LOG.info("Bot: onMessage({});",msg);
        recivedResponseBuffer += msg;
        synchronized (waitSync) {
            if (waitResponse != null) {
                if (!recivedResponseBuffer.isEmpty()) {
                    if (recivedResponseBuffer.matches(waitResponse)) {
                        waitStatus = OperationStatus.SUCESSFULL;
                        LOG.info("Bot: onMessage({}: sucessfull)",recivedResponseBuffer);
                        flushResponseBuffer();
                    } else if (recivedResponseBuffer.trim().matches(COMMAND_BUSY)){
                        waitStatus = OperationStatus.FAILED;
                        LOG.info("Bot: onMessage({}: robot busy)",recivedResponseBuffer);
                        flushResponseBuffer();
                    } else if (recivedResponseBuffer.trim().matches(COMMAND_INVALID)){
                        waitStatus = OperationStatus.FAILED;
                        LOG.info("Bot: onMessage({}: command invalid)",recivedResponseBuffer);
                        flushResponseBuffer();
                    } else {
                        waitStatus = OperationStatus.FAILED;
                        LOG.info("Bot: onMessage({}: unrecognized)",recivedResponseBuffer);
                    }
                }
            } else {
                waitSync.notify();
            }
        }
        LOG.info("Bot: onMessage({}), waitStatus={};",msg,waitStatus);
    }
    
    private void flushResponseBuffer() {
        recivedResponse = new String(recivedResponseBuffer);
        recivedResponseBuffer = "";
        waitSync.notify();
    }

    /**
     * Wait response function, used to sleep in wait() state while received notify()
     * @param c String received
     */
    private OperationStatus waitResponse(String c) {
        OperationStatus status = OperationStatus.FAILED;
        synchronized (waitSync) {
            waitResponse = c;
            waitStatus = OperationStatus.FAILED;
            try {
                waitSync.wait();
            } catch (InterruptedException e) {
                LOG.error("wait interupted", e);
            } finally {
                waitResponse = null;
                status = waitStatus;
                waitStatus = OperationStatus.FAILED;
            }
        }
        return status;
    }
    
    /**
     * Wait connect function, wait while received notify
     */
    private void waitConnected() {
        synchronized (btSync) {
            if (!BtConnected) {
                try {
                    btSync.wait();
                } catch (InterruptedException e) {
                    LOG.error("WaitConnected() interuppted.",e);
                }
            }
        }
    }

    /**
     * ExecutorService getter.
     * @return the executor
     */
    public ExecutorService getExecutor() {
        return executor;
    }

    /* (non-Javadoc)
     * @see org.kaaproject.kaa.examples.robotrun.controller.robot.Drivable#registerStateCallback(org.kaaproject.kaa.examples.robotrun.controller.robot.callbacks.StateCallback)
     */
    @Override
    public void registerStateCallback(StateCallback callback) {
        this.stateCallback = callback;
    }

    /**
     * @return the operated
     */
    public boolean isOperated() {
        return operated;
    }

    /**
     * @param operated the operated to set
     */
    public void setOperated(boolean operated) {
        this.operated = operated;
    }

}
