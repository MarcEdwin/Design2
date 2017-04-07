# Lesson 3: Part 2

## Run a sample application to send device-to-cloud messages
Visit this [Microsoft](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-adafruit-feather-m0-wifi-kit-arduino-lesson3-run-azure-blink) site for more info.

### Get your IoT hub and device connection strings
The device connection string is used to connect your Arduino board to your IoT hub. The IoT hub connection string is used to connect your IoT hub to the device identity that represents your Arduino board in the IoT hub.

- List all your IoT hubs in your resource group by running the following Azure CLI command:

```bash
az iot hub list -g {resource group name} --query [].name
```

Use `iot-sample` as the value of `{resource group name}` if you didn't change the value.

The following output was displayed after running command:

```
[
  "smart-vibes-project"
]
```

- Get the IoT hub connection string by running the following Azure CLI command:

```bash
az iot hub show-connection-string --name {my hub name}
```

`{my hub name}` is the name that you specified when you created your IoT hub and registered your Arduino board. Use `smart-vibes-project` as the value of `{my hub name}` if you didn't change the value.

The following output was displayed after running command:

```
{
  "connectionString": "HostName=smart-vibes-project.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=fai8+CA7VvEd9LYP4+Vta/VIMQHf9Iotyj9nZ7N0KgY="
}
```

- Get the device connection string by running the following command:

```bash
az iot device show-connection-string --hub-name {my hub name} --device-id {device id}
```

Use `feather1` as the value of `{device id}` if you didn't change the value.

The following output was displayed after running command:

```
{
  "connectionString": "HostName=smart-vibes-project.azure-devices.net;DeviceId=feather1;SharedAccessKey=zpbfO4ZTRjNfJZ9lKvobL9XINnxnm+7yvCINY44Nd1s="
}
```

### Configure the device connection

To configure the device connection, follow these steps:

1. Obtain the serial port of the device with the device discovery cli:

```bash
devdisco list --usb
```

2. Open the file config.json in the lesson folder and add the value of the found COM port number to the `"device_port"` line".

3. Initialize the configuration file by running the following commands:

```bash
npm install
gulp init
gulp install-tools
```

4. Open the device configuration file config-arduino.json in Visual Studio Code by running the following command:

```bash
# For Windows command prompt
code %USERPROFILE%\.iot-hub-getting-started\config-arduino.json

# For MacOS or Ubuntu
code ~/.iot-hub-getting-started/config-arduino.json
```

5. Make the following replacements in the config-arduino.json file:

- Replace [Wi-Fi SSID] with your Wi-Fi SSID that connected to the Internet.
- Replace [Wi-Fi password] with your Wi-Fi password. Remove the string if your Wi-Fi doesn't require password.
- Replace [IoT device connection string] with the device connection string you obtained.
- Replace [IoT hub connection string] with the iot hub connection string you obtained.

### Deploy and run the sample application

Deploy and run the sample application on your Arduino board by running the following command:

```bash
gulp run
# You can monitor the serial port by running listen task:
gulp listen

# Or you can combine above two gulp tasks into one:
gulp run --listen
```

# Lesson 3: Part 3

```bash
az storage account list -g iot-sample --query [].name
# The following output was displayed after running command:
# [
#   "iotstorage3mr6ib6fbpg3m"
# ]

az storage account show-connection-string -g iot-sample -n {storage name}
# The following output was displayed after running command:
# {
#   "connectionString": "DefaultEndpointsProtocol=https;EndpointSuffix=core.windows.net;AccountName=iotstorage3mr6ib6fbpg3m;AccountKey=0OmNYuB4wkTYIoGTd2i9bTte13LaD0tOrkDzXs2xvOMYJlkyt2C8+Bz1RSTOqPi0h00E9cg2KNuWR0LZqjJwnA=="
# }

gulp run --read-storage --listen
# [Azure Table] ERROR: Table not found. Something might be wrong. Please go to troubleshooting page for more information.
```
