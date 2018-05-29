Getting started with AWS IoT and MDK
====================================

This tutorial explains how to use **MDK** and the **AWS_IoT_Device** pack to connect your application to the AWS IoT Cloud.

Prerequisites
-------------
 You require a development board with a device that is supported by [CMSIS-Drivers](http://arm-software.github.io/CMSIS_5/Driver/html/index.html). Lots of devices from STMicroelectronics and NXP do have these drivers available. Please consult the [device database](https://www.keil.com/dd2) for a list of devices and development boards.

When you have chosen your device and development board, use ![Pack Installer](./static/icons/packinst.png)[Pack Installer](https://www.keil.com/support/man/docs/uv4/uv4_ca_packinstaller.htm) to download and install the following software packs:

* **MDK-Packs::AWS_IoT_Device**
* **ARM::CMSIS** v5.0.1 or newer
* **ARM::mbedTLS** v1.5.0 or newer
* **Keil::MDK-Middleware** v7.7.0 or newer
* Device family pack for the selected device

**Note**:

* In order to be able to use the AWS IoT Cloud Connector, you require a MDK-Professional license. &micro;Vision allows you to start a [seven day trial](http://www2.keil.com/mdk5/activation/) from within the tool.
* This tutorial assumes that you know how to work with &micro;Vision (creating projects, adding template code, configuring software components). If not, check out the [MDK Getting Started Guide](https://armkeil.blob.core.windows.net/product/gs_MDK5_4_en.pdf).
* In this tutorial, your MDK installation folder will be referenced as \<INSTALL_FOLDER>. Usually, it is C:\\Keil_v5.

In the AWS IoT Console
----------------------

In the AWS IoT console, you need to create a thing, download its certificates, create a policy, and attach the policy to the thing. Here's how to do this:

### Create an IoT thing for your device

1. Browse to the [AWS IoT console](https://console.aws.amazon.com/iotv2/).
2. In the left navigation pane, choose **Manage**, and then choose **Things**.
3. If you do not have any IoT things registered in your account, the **You don't have any things yet** page is displayed. If you see this page, choose **Register** a thing.
4. On the **Creating AWS IoT things** page, choose **Create a single thing**.
5. On the **Add your device to the thing registry** page, type a name for your thing (for example "MyThing"), and then choose **Next**. You will need to add the name later to your C code.
6. On the **Add a certificate for your thing** page, under **One-click certificate creation**, choose **Create certificate**.
7. Download your private key and certificate by choosing the **Download** links for each. Make note of the certificate ID. You need it later to attach a policy to your certificate.
8. Choose **Activate** to activate your certificate. Certificates must be activated prior to use.

### Create a policy and attach it to your thing

1. In the navigation pane of the [AWS IoT console](https://console.aws.amazon.com/iotv2/), choose **Secure**, and then choose **Policies**.
2. On the **Policies** page, choose **Create** (top right corner).
3. On the **Create a policy** page, enter a name for the policy. In the **Action** box, enter **iot:Connect, iot:Publish, iot:Subscribe, iot:Receive**. The **Resource ARN** box will be auto-filled with your credentials. Replace **topic/replaceWithATopic** with **\***. Under **Effect**, check the **Allow** box. Click on **Create**.
4. In the left navigation pane of the [AWS IoT console](https://console.aws.amazon.com/iotv2/), choose **Secure**, and then choose **Certificates**. You should see the certificate that you have created earlier.
5. Click on the three dots in the upper right corner of the certificate and choose **Attach policy**.
6. In the **Attach policies to certificate(s)** window, enable the policy that you have just created and click **Attach**.

### Make a note of your custom endpoint
In the navigation pane of the [AWS IoT console](https://console.aws.amazon.com/iotv2/), choose **Settings**. On the **Settings** page, copy the name of your **Endpoint** (such as `a3xyzzyx.iot.us-east-2.amazonaws.com`) to have it ready for later.

Now you are ready to connect your device to the AWS IoT Cloud.


In &micro;Vision
----------

Follow these steps to create a simple application that connects to the AWS IoT Cloud.

### Create project and select software components
1. Create a new project. Select the device that you are using on your development board.
2. The ![Manage RTE](./static/icons/rte.png) Manage Run-Time Environment window opens. Select the following software components:
    * **IoT Client:AWS**
    * **CMSIS:RTOS2:Keil RTX5**
    * **CMSIS:CORE**
    * **Security:mbed TLS**
    * **Network:** make sure to select the variant **MDK-Pro**
    * **Network:Socket:BSD**
    * **Network:Socket:TCP**
    * **Network:Socket:UDP**
    * **Network:Service:DNS Client**
    * **Network:Interface:ETH**
    * **Network:CORE:IPv4 Release**
    * **CMSIS Driver:Ethernet/MAC/PHY (API)** (depending on your hardware)
    * **Device:Startup** (and any other Device dependent components)
    
    Optional (for `printf` debug output on the console):
    * **Compiler:I/O:STDOUT:ITM**

### Configure the software components
1.  Configure AWS IoT Client: **IoT Client:aws_iot_config.h**
    * `AWS_IOT_MQTT_HOST`: Enter your custom endpoint, as noted down before.
    * `AWS_IOT_MQTT_CLIENT_ID`: Enter the thing's name here (as created in the AWS IoT console)
    * `AWS_IOT_MY_THING_NAME`: Enter the thing's name here (as created in the AWS IoT console)<br>
    Leave all other #defines in this file as they are. No further changes required. 
2.  Configure mbedTLS: **Security:mbedTLS_config.h**
    * In the Project window, double-click this file to open it. It contains generic settings for mbed TLS and its configuration requires a thorough understanding of SSL/TLS. We have prepared an example file that contains all required settings for Amazon AWS. The file available in `<INSTALL_FOLDER>/ARM/Pack/MDK-Packs/AWS_IoT_Device/_version_/config/mbedTLS_config.h`. Copy its contents and replace everything in the project's mbedTLS_config.h file.
3.  If you are using the software components described above, you do not need to configure the Network component. The default settings will work. If you do not have DHCP available in your network, please refer to the [MDK-Middleware documentation](http://www.keil.com/pack/doc/mw/Network/html/index.html) on how to set a static IP address.
4.  Configure RTX5: **CMSIS:RTX_Config.h**
    * If you are using one of the provided templates (see below), you need to set the **System - Global Dynamic Memory size** to at least 10240:<br>
    ![RTX_Config.h](./static/images/rtx_config_h.png)<br>
    This large amount of dynamic memory is not required for custom projects.
5.  Configure Heap: **startup_\<device>.s**
    * Configure at least 48 KB (0xC000) of heap memory.<br> 
    ![Heap](./static/images/heap.png)<br>
    This is required for the mbed TLS certificates.
6.  Configure device specific hardware:
    * Configure the CMSIS Ethernet driver and other device specific components (clock system, I/O, ...) as required. Please consult your device's/board's documentation for more information.

### Add Template code
The **AWS_IoT_Device** pack includes three code templates that can be used to test the connection to AWS (refer to [Samples](https://github.com/MDK-Packs/aws-iot-device-sdk-embedded-C/tree/master/samples)). In the next step, select only one of these examples. In the Project window, right-click "Source Group 1" and select "Add New Item to Group" - "User Code Template", to add user code templates.

1.  AWS sample code (add one of the following):
    * Add **IoT Client:AWS:Subscribe and Publish** or
    * Add **IoT Client:AWS:Shadow** or
    * Add **IoT Client:AWS:Shadow (console echo)** or
    * Add **IoT Client:AWS:Jobs**<br>
    ![Heap](./static/images/uct.png)<br>
2.  Add **CMSIS:RTOS2:Keil RTX5:main** and update:
    * Add an attribute for an increased stack size for the application thread. Each of the samples require a thread stack size of 8 KB:<br>
      `const osThreadAttr_t app_main_attr = { .stack_size = 8192U };`
    * Change application thread creation (include the attribute defined above):<br>
      `osThreadNew(app_main, NULL, &app_main_attr);`
    * Add an include for the Network library (beginning of file):<br>
      `#include "rl_net.h"`
    * Add a prototype for the selected example's top level function (only one):<br>
      `extern int subscribe_publish (void);`<br>
      `extern int shadow (void);`<br>
      `extern int shadow_console_echo (void);`<br>
      `extern int jobs (void);`
    * Add Network initialization in the `app_main` function:<br>
      `uint32_t addr;`<br>
      `netInitialize();`<br>
      `do {`<br>
      &nbsp;&nbsp;`osDelay(500U);`<br>
      &nbsp;&nbsp;`netIF_GetOption(NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, (uint8_t *)&addr, sizeof (addr));`<br>
      `} while (addr == 0U);`
    * Add a call to the selected example's top level function into the `app_main` after Network initialization (only one):<br>
      `subscribe_publish();`<br>
      `shadow();`<br>
      `shadow_console_echo();`<br>
      `jobs();`
3.  AWS Certificates:
    * Add **IoT Client:AWS:Certificates**. This adds the file `aws_certificates.c` to the project. This file needs careful editing. The next step explains what to add here. 
    
### Adding your certificates
Previously, you have saved the certificates and keys for your thing that you have added to your AWS IoT console. You need to paste the content of these files into `aws_certificates.c` and then edit it.

1. Copy the following into the section beginning with<br>
    `const char AWS_RootCA[] =`<br>
    `"-----BEGIN CERTIFICATE-----\n"`<br>
    `"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n"`<br>
    `"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n"`<br>
    `"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n"`<br>
    `"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n"`<br>
    `"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n"`<br>
    `"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n"`<br>
    `"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n"`<br>
    `"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n"`<br>
    `"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n"`<br>
    `"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n"`<br>
    `"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n"`<br>
    `"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n"`<br>
    `"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n"`<br>
    `"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n"`<br>
    `"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n"`<br>
    `"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n"`<br>
    `"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n"`<br>
    `"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n"`<br>
    `"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n"`<br>
    `"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n"`<br>
    `"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n"`<br>
    `"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n"`<br>
    `"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n"`<br>
    `"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n"`<br>
    `"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n"`<br>
    `"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\n"`<br>
    `"-----END CERTIFICATE-----\n";`<br>
2. Open the file `<ID>-certificate.pem.crt` in a text editor. Edit the certificate by adding a leading `"` and a trailing `\n"` to each line. Then, copy the content into the section beginning with `const char AWS_ClientCert[] =`<br>
3. Open the file `<ID>-private.pem.key` in a text editor. Edit the certificate by adding a leading `"` and a trailing `\n"` to each line. Then, copy the content into the section beginning with `const char AWS_ClientPrivateKey[] = `<br>

### Optional: Configure debug
The AWS sample code is prepared to output `printf` statements for debugging purposes. To show these effectively, you can use the software component **Compiler:I/O:STDOUT:ITM** to show the messages in the [Debug printf](http://www.keil.com/support/man/docs/uv4/uv4_db_dbg_serialwin.htm) window. To enable this output, do the following:

1.  Open the ![Options for target](./static/icons/oft.png) Options for target dialog (**Alt+F7**).
2.  Go to the **C/C++** tab and configure debug messages by adding the following global defines to the C pre-processor:<br>
    `ENABLE_IOT_INFO` `ENABLE_IOT_WARN` `ENABLE_IOT_ERROR`<br>
3.  Go to the **Debug** tab. Select the debug adapter that is connected to your target and configure these **Settings**:
    * **Debug** tab: Set **Port** to **SW**.
    * **Trace** tab: Set the **Core Clock** to the system clock of your target, Select **Trace Enable** and set make sure that **ITM Stimulus Port 0** is enabled:<br>
![Debug Setup](./static/images/debug_setup.png)

### Run/debug the application
1.  ![Build target](./static/icons/build.png) Build the application (**F7**) and ![Download to target](./static/icons/Flash.png) download (**F8**) to target.
2.  Enter ![Start/Stop Debug Session](./static/icons/debug.png) debug (**CTRL+F5**) and ![Run](./static/icons/run.png) run (**F5**) the application on the target and monitor the console output via ![Debug (printf) Viewer](./static/icons/uart_window.png) **Serial Window - Debug (printf) Viewer**. You should see something similar:
    ```
    AWS IoT SDK Version 2.2.1-
    
    Connecting...
    Subscribing...
    -->sleep
    Subscribe callback
    sdkTest/sub     hello from SDK QOS0 : 0 
    Subscribe callback
    sdkTest/sub     hello from SDK QOS1 : 1 
    -->sleep
    Subscribe callback
    ... 
    ```