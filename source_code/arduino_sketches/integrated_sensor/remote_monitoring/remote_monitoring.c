// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iot_configs.h"

#include "AzureIoTHub.h"
#include "sdk/schemaserializer.h"
#include "lsm303dlhc.h"

/* CODEFIRST_OK is the new name for IOT_AGENT_OK. The follow #ifndef helps during the name migration. Remove it when the migration ends. */
#ifndef  IOT_AGENT_OK
#define  IOT_AGENT_OK CODEFIRST_OK
#endif // ! IOT_AGENT_OK

#define MAX_DEVICE_ID_SIZE  20

// Define the Model
BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(SystemProperties,
    ascii_char_ptr, DeviceID,
    _Bool, Enabled
);

DECLARE_STRUCT(DeviceProperties,
ascii_char_ptr, DeviceID,
_Bool, HubEnabledState
);

DECLARE_MODEL(Accelerometer,

    /* Event data (x-axis acceleration, y-axis acceleration, and z-axis acceleration) */
    WITH_DATA(float, AccelerationX),
    WITH_DATA(float, AccelerationZ),
    WITH_DATA(float, AccelerationY),
    WITH_DATA(ascii_char_ptr, DeviceId),

    /* Device Info - This is command metadata + some extra fields */
    WITH_DATA(ascii_char_ptr, ObjectType),
    WITH_DATA(_Bool, IsSimulatedDevice),
    WITH_DATA(ascii_char_ptr, Version),
    WITH_DATA(DeviceProperties, DeviceProperties),
    WITH_DATA(ascii_char_ptr_no_quotes, Commands),

    /* Commands implemented by the device */
    WITH_ACTION(SetAccelerationX, float, accel_x),
    WITH_ACTION(SetAccelerationY, float, accel_y),
    WITH_ACTION(SetAccelerationZ, float, accel_z)
);

END_NAMESPACE(Contoso);

EXECUTE_COMMAND_RESULT SetAccelerationX(Accelerometer* accelerometer, float accel_x)
{
    LogInfo("Received x-axis acceleration %d\r\n", accel_x);
    accelerometer->AccelerationX = accel_x;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT SetAccelerationY(Accelerometer* accelerometer, float accel_y)
{
    LogInfo("Received y-axis acceleration %d\r\n", accel_y);
    accelerometer->AccelerationY = accel_y;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT SetAccelerationZ(Accelerometer* accelerometer, float accel_z)
{
    LogInfo("Received z-axis acceleration %d\r\n", accel_z);
    accelerometer->AccelerationZ = accel_z;
    return EXECUTE_COMMAND_SUCCESS;
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL)
    {
        LogInfo("unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
        {
            LogInfo("failed to hand over the message to IoTHubClient");
        }
        else
        {
            LogInfo("IoTHubClient accepted the message for delivery\r\n");
        }

        IoTHubMessage_Destroy(messageHandle);
    }
    free((void*)buffer);
}

static size_t GetDeviceId(const char* connectionString, char* deviceID, size_t size)
{
    size_t result;
    const char* runStr = connectionString;
    char ustate = 0;
    char* start = NULL;

    if (runStr == NULL)
    {
        result = 0;
    }
    else
    {
        while (*runStr != '\0')
        {
            if (ustate == 0)
            {
                if (strncmp(runStr, "DeviceId=", 9) == 0)
                {
                    runStr += 9;
                    start = runStr;
                }
                ustate = 1;
            }
            else
            {
                if (*runStr == ';')
                {
                    if (start == NULL)
                    {
                        ustate = 0;
                    }
                    else
                    {
                        break;
                    }
                }
                runStr++;
            }
        }

        if (start == NULL)
        {
            result = 0;
        }
        else
        {
            result = runStr - start;
            if (deviceID != NULL)
            {
                for (size_t i = 0; ((i < size - 1) && (start < runStr)); i++)
                {
                    *deviceID++ = *start++;
                }
                *deviceID = '\0';
            }
        }
    }

    return result;
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result;
    const unsigned char* buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        LogInfo("unable to IoTHubMessage_GetByteArray\r\n");
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        /*buffer is not zero terminated*/
        char* temp = malloc(size + 1);
        if (temp == NULL)
        {
            LogInfo("failed to malloc\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            EXECUTE_COMMAND_RESULT executeCommandResult;

            memcpy(temp, buffer, size);
            temp[size] = '\0';
            executeCommandResult = EXECUTE_COMMAND(userContextCallback, temp);
            result =
                (executeCommandResult == EXECUTE_COMMAND_ERROR) ? IOTHUBMESSAGE_ABANDONED :
                (executeCommandResult == EXECUTE_COMMAND_SUCCESS) ? IOTHUBMESSAGE_ACCEPTED :
                IOTHUBMESSAGE_REJECTED;
            free(temp);
        }
    }
    return result;
}

void remote_monitoring_run(void)
{
        initLsm();

        srand((unsigned int)time(NULL));
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            LogInfo("Failed on serializer_init\r\n");
        }
        else
        {
            IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

#if defined(IOT_CONFIG_MQTT)
            iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, MQTT_Protocol);
#elif defined(IOT_CONFIG_HTTP)
            iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, HTTP_Protocol);
#else
            iotHubClientHandle = NULL;
#endif

            if (iotHubClientHandle == NULL)
            {
                LogInfo("Failed on IoTHubClient_CreateFromConnectionString\r\n");
            }
            else
            {
#ifdef MBED_BUILD_TIMESTAMP
                // For mbed add the certificate information
                if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
                {
                    LogInfo("failure to set option \"TrustedCerts\"\r\n");
                }
#endif // MBED_BUILD_TIMESTAMP

                Accelerometer* accelerometer = CREATE_MODEL_INSTANCE(Contoso, Accelerometer);
                if (accelerometer == NULL)
                {
                    LogInfo("Failed on CREATE_MODEL_INSTANCE\r\n");
                }
                else
                {
                    STRING_HANDLE commandsMetadata;

                    if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, accelerometer) != IOTHUB_CLIENT_OK)
                    {
                        LogInfo("unable to IoTHubClient_SetMessageCallback\r\n");
                    }
                    else
                    {

                        char deviceId[MAX_DEVICE_ID_SIZE];
                        if (GetDeviceId(IOT_CONFIG_CONNECTION_STRING, deviceId, MAX_DEVICE_ID_SIZE) > 0)
                        {
                            LogInfo("deviceId=%s", deviceId);
                        }

                        /* send the device info upon startup so that the cloud app knows
                        what commands are available and the fact that the device is up */
                        accelerometer->ObjectType = "DeviceInfo";
                        accelerometer->IsSimulatedDevice = false;
                        accelerometer->Version = "1.0";
                        accelerometer->DeviceProperties.HubEnabledState = true;
                        accelerometer->DeviceProperties.DeviceID = (char*)deviceId;

                        commandsMetadata = STRING_new();
                        if (commandsMetadata == NULL)
                        {
                            LogInfo("Failed on creating string for commands metadata\r\n");
                        }
                        else
                        {
                            /* Serialize the commands metadata as a JSON string before sending */
                            if (SchemaSerializer_SerializeCommandMetadata(GET_MODEL_HANDLE(Contoso, Accelerometer), commandsMetadata) != SCHEMA_SERIALIZER_OK)
                            {
                                LogInfo("Failed serializing commands metadata\r\n");
                            }
                            else
                            {
                                unsigned char* buffer;
                                size_t bufferSize;
                                accelerometer->Commands = (char*)STRING_c_str(commandsMetadata);

                                /* Here is the actual send of the Device Info */
                                if (SERIALIZE(&buffer, &bufferSize, accelerometer->ObjectType, accelerometer->Version, accelerometer->IsSimulatedDevice, accelerometer->DeviceProperties, accelerometer->Commands) != IOT_AGENT_OK)
                                {
                                    LogInfo("Failed serializing\r\n");
                                }
                                else
                                {
                                    sendMessage(iotHubClientHandle, buffer, bufferSize);
                                }

                            }

                            STRING_delete(commandsMetadata);
                        }

                        accelerometer->DeviceId = (char*)deviceId;
                        int sendCycle = 10;
                        int currentCycle = 0;
                        while (1)
                        {
                            if(currentCycle >= sendCycle) {
                                float AccelX;
                                float AccelY;
                                float AccelZ;
                                getNextSample(&AccelX, &AccelY, &AccelZ);
                                accelerometer->AccelerationX = AccelX;
                                accelerometer->AccelerationY = AccelY;
                                accelerometer->AccelerationZ = AccelZ;
                                currentCycle = 0;
                                unsigned char*buffer;
                                size_t bufferSize;

                                LogInfo("Sending sensor value AccelerationX = %d, AccelerationY = %d, AccelerationZ = %d\r\n", accelerometer->AccelerationX, accelerometer->AccelerationY, accelerometer->AccelerationZ);

                                if (SERIALIZE(&buffer, &bufferSize, accelerometer->DeviceId, accelerometer->AccelerationX, accelerometer->AccelerationY, accelerometer->AccelerationZ) != IOT_AGENT_OK)
                                {
                                    LogInfo("Failed sending sensor value\r\n");
                                }
                                else
                                {
                                    sendMessage(iotHubClientHandle, buffer, bufferSize);
                                }
                            }

                            IoTHubClient_LL_DoWork(iotHubClientHandle);
                            ThreadAPI_Sleep(100);
                            currentCycle++;
                        }
                    }

                    DESTROY_MODEL_INSTANCE(accelerometer);
                }
                IoTHubClient_LL_Destroy(iotHubClientHandle);
            }
            serializer_deinit();

    }
}
