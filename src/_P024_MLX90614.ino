#include "_Plugin_Helper.h"
#ifdef USES_P024

// #######################################################################################################
// #################################### Plugin 024: MLX90614 IR temperature I2C 0x5A)  ###############################################
// #######################################################################################################

/** Changelog:
 * 2023-11-23 tonhuisman: Add Device flag for I2CMax100kHz as this sensor won't work at 400 kHz
 * 2023-11-23 tonhuisman: Add Changelog
*/

# include "src/PluginStructs/P024_data_struct.h"

// MyMessage *msgTemp024; // Mysensors

# define PLUGIN_024
# define PLUGIN_ID_024 24
# define PLUGIN_NAME_024 "Environment - MLX90614"
# define PLUGIN_VALUENAME1_024 "Temperature"

boolean Plugin_024(uint8_t function, struct EventStruct *event, String& string)
{
  boolean success = false;

  // static uint8_t portValue = 0;
  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number           = PLUGIN_ID_024;
      Device[deviceCount].Type               = DEVICE_TYPE_I2C;
      Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_SINGLE;
      Device[deviceCount].Ports              = 16;
      Device[deviceCount].PullUpOption       = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption      = true;
      Device[deviceCount].SendDataOption     = true;
      Device[deviceCount].ValueCount         = 1;
      Device[deviceCount].TimerOption        = true;
      Device[deviceCount].GlobalSyncOption   = true;
      Device[deviceCount].PluginStats        = true;
      Device[deviceCount].I2CMax100kHz       = true; // Max 100 kHz allowed/supported
      break;
    }

    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_024);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_024));
      break;
    }

    case PLUGIN_I2C_HAS_ADDRESS:
    {
      success = (event->Par1 == 0x5a);
      break;
    }

    # if FEATURE_I2C_GET_ADDRESS
    case PLUGIN_I2C_GET_ADDRESS:
    {
      event->Par1 = 0x5a;
      success     = true;
      break;
    }
    # endif // if FEATURE_I2C_GET_ADDRESS

    case PLUGIN_WEBFORM_LOAD:
    {
      # define MLX90614_OPTION 2

      const __FlashStringHelper *options[MLX90614_OPTION] = {
        F("IR object temperature"),
        F("Ambient temperature")
      };
      const int optionValues[MLX90614_OPTION] = {
        (0x07),
        (0x06)
      };
      addFormSelector(F("Option"), F("option"), MLX90614_OPTION, options, optionValues, PCONFIG(0));

      success = true;
      break;
    }

    case PLUGIN_WEBFORM_SAVE:
    {
      PCONFIG(0) = getFormItemInt(F("option"));
      success    = true;
      break;
    }

    case PLUGIN_INIT:
    {
      const uint8_t unit    = CONFIG_PORT;
      const uint8_t address = 0x5A + unit;

      success = initPluginTaskData(event->TaskIndex, new (std::nothrow) P024_data_struct(address));
      break;
    }

    case PLUGIN_READ:
    {
      P024_data_struct *P024_data =
        static_cast<P024_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P024_data) {
        UserVar.setFloat(event->TaskIndex, 0, P024_data->readTemperature(PCONFIG(0)));

        if (loglevelActiveFor(LOG_LEVEL_INFO)) {
          addLog(LOG_LEVEL_INFO, concat(F("MLX90614  : Temperature: "), formatUserVarNoCheck(event->TaskIndex, 0)));
        }

        //        send(msgObjTemp024->set(UserVar[event->BaseVarIndex], 1)); // Mysensors
        success = true;
      }
      break;
    }
  }
  return success;
}

#endif // USES_P024
