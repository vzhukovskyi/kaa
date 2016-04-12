Kaa open-source IoT platform
============================

Welcome to Kaa!

Kaa is an open-source middleware platform for building, managing, and integrating connected products with the Internet of Everything.

[![Kaa logo](logo_kaa_fullsize.png)](http://www.kaaproject.org/)

Kaa in a nutshell
=================

Kaa is a highly flexible open-source platform for building, managing, and integrating applications in the Internet of Things. Kaa offers a holistic approach for implementing rich communication, control, and interoperation capabilities into connected products and smart devices. On top of this, powerful back-end functionality of Kaa greatly speeds up IoT product development, allowing vendors to concentrate on maximizing their product’s unique value to the consumer.

Key features of Kaa are:

* *Events*: Kaa provides a mechanism for delivery of configurable event messages across connected devices. Events may be set as either unicast, that is transmitted to a single recipient, or multicast, that is transmitted to several recipients. For detailed information on managing events, refer to the [Events](http://docs.kaaproject.org/display/KAA/Events) section.
* *Data collection*: Kaa endpoints perform temporary storage of data records ("logs") of any predefined structure. Kaa endpoint SDK implements log upload triggers that initiate periodic logs upload from the endpoint to the server. The server can store logs into the filesystem, big data platforms like Hadoop, MongoDB, etc., or submit the data directly to the stream analytics processors. For example, logs can be used to help debug client applications, analyze user behavior, identify anomalies, etc. For detailed information on log management, refer to the [Logging](http://docs.kaaproject.org/display/KAA/Logging) section.
* *Profiling and grouping*: Kaa introduces the concept of the endpoint profile which consists of client-side and server-side parts. Client-side endpoint profile is a set of data that the client exposes for the access by Kaa application. Server-side endpoint profile is a set of data that is controlled by Kaa server users via Administration UI or by other server applications via REST API. Profiles can then be used to organize the endpoints into groups. Endpoint groups can be used, for example, to send targeted notifications or adjust software behavior. For detailed information on profiles, filtering, and grouping, refer to the the [Endpoint profiling](http://docs.kaaproject.org/display/KAA/Endpoint+profiling) and [Endpoint grouping](http://docs.kaaproject.org/display/KAA/Endpoint+grouping) sections.
* *Notifications delivery*: Kaa features a topic-based notification system that allows the server to deliver messages of any predefined structure to subscribed endpoints. The notification topic access is granted via the endpoint’s group membership. For detailed information about managing notifications, please refer to the [Notifications](http://docs.kaaproject.org/display/KAA/Notifications) section.
* *Data distribution*: Kaa allows performing updates of operational data, such as configuration data, from the Kaa server to endpoints. This feature can be used for centralized configuration management, content distribution, etc. Using Kaa’s data schemas, developers are able to define any type of data structure and constraint data types. For detailed information on data distribution, refer to the [Configuration](http://docs.kaaproject.org/display/KAA/Configuration) section.
* *Transport abstraction*: Kaa’s data channel abstraction architecture presents software vendors with freedom in selecting a networking stack for establishing communication between the server and endpoints - WiFi, Ethernet, Zigbee, mqtt, CoAP, XMPP, TCP, HTTP, etc. A prominent characteristic of Kaa is its ability to build applications that function over any type of network connection and communicate with devices even over intermittent data connections.
* *Support of multi-tenancy and multi-application configuration*: The Kaa server is able to serve multiple business entities and multiple applications independently on a single server instance. For more details, refer to the [Design reference](http://docs.kaaproject.org/display/KAA/Design+reference).

## Resources

* [Kaa website](http://www.kaaproject.org/)
* [Project wiki](http://docs.kaaproject.org/display/KAA/)
* [Task tracker](http://jira.kaaproject.org/browse/KAA/)
* [Google group](https://groups.google.com/forum/#!forum/kaaproject)
* [Documentation](http://docs.kaaproject.org/display/KAA/Kaa+IoT+Platform+Home)

## Getting started

Kaa Sandbox is the quickest and easiest way to get started with Kaa. It's a private Kaa environment which includes demo client applications. Kaa Sandbox includes all necessary Kaa components in a convenient virtual environment that can be set up in just 5 minutes! Checkout [Getting started](http://docs.kaaproject.org/display/KAA/Getting+started) page for more information.

## Installation and configuration

After you get acquainted with Kaa Sandbox and its demo applications, you can take one step further and install Kaa into your environment. Kaa installation is available in the single node mode or in the distributed mode, as described in the [Installation guide](http://docs.kaaproject.org/display/KAA/Installation+guide).

## Supported platforms

Operating systems:

* [Android](http://docs.kaaproject.org/display/KAA/Android)
* [iOS](http://docs.kaaproject.org/display/KAA/iOS)
* [Linux](http://docs.kaaproject.org/display/KAA/Linux)
* [Snappy Ubuntu](http://docs.kaaproject.org/display/KAA/Snappy+Ubuntu+Core)
* [Windows](http://docs.kaaproject.org/display/KAA/Windows)
* [QNX](http://docs.kaaproject.org/display/KAA/QNX+Neutrino+RTOS)

Hardware platforms:

* [Intel Edison](http://docs.kaaproject.org/display/KAA/Intel+Edison)
* [BeagleBone](http://docs.kaaproject.org/display/KAA/BeagleBone)
* [Raspberry Pi](http://docs.kaaproject.org/display/KAA/Raspberry+Pi)
* [Econais](http://docs.kaaproject.org/display/KAA/Econais)
* [Texas Instruments CC3200](http://docs.kaaproject.org/display/KAA/Texas+Instruments+CC3200)
* [ESP8266](http://docs.kaaproject.org/display/KAA/ESP8266)

Supported data processing systems

* [Hadop]()
* [MongoDB]()
* [Oracle NoSQL Database]()
* [Cassandra]()
* [Spark]()
* [Couchbase]()
* [CDAP]()

## License

Kaa is licensed under the [Apache license, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).
