/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#include "AFE-Analog-Input.h"

AFEAnalogInput::AFEAnalogInput(){};

void AFEAnalogInput::begin() {
  AFEDataAccess Data;
   Data.getConfiguration(&configuration);
  _initialized = true;

#ifndef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (strlen(configuration.mqtt.topic) > 0) {
    sprintf(mqttCommandTopic, "%s/cmd", configuration.mqtt.topic);
  } else {
    mqttCommandTopic[0] = '\0';
  }
#endif

#ifdef DEBUG
  Serial << endl << endl << F("------------ AC VCC Input ------------");
  Serial << endl
         << F("- Initialized") << endl
         << F("- GPIO: ") << configuration.gpio << endl
         << F("- Interval: ") << configuration.interval << endl
         << F("- Max VCC: ") << configuration.maxVCC << endl
         << F("- No of Samples: ") << configuration.numberOfSamples << endl
         << F("- R[A]: ") << configuration.divider.Ra << endl
         << F("- R[B]: ") << configuration.divider.Rb << endl
         << F("-------------------------------------");

#endif
}


boolean AFEAnalogInput::isReady() {
  if (ready) {
    ready = false;
    return true;
  } else {
    return false;
  }
}

void AFEAnalogInput::listener() {

  if (_initialized) {
    unsigned long time = millis();

    if (startTime == 0) {
      startTime = time;
    }

    if (time - startTime >= configuration.interval * 1000) {

      if (counterOfSamplings < configuration.numberOfSamples) {
        temporaryAnalogData += analogRead(configuration.gpio);
        counterOfSamplings++;
      } else {
        data.raw = (uint16_t)(temporaryAnalogData / configuration.numberOfSamples);

        data.percent = (float)data.raw  * 100 / 1024;
        data.voltage = (double)(configuration.maxVCC * data.raw  / 1024);
        if (configuration.divider.Rb > 0) {
          data.voltageCalculated =
            (data.voltage * (configuration.divider.Ra + configuration.divider.Rb)) /
            configuration.divider.Rb;
        } else {
            data.voltageCalculated = data.voltage;
        }
            

#ifdef DEBUG
        Serial << endl
               << F(" - Number of samples: ") << counterOfSamplings << endl
               << F(" - Analog value = ") << data.raw  << endl
               << F(" - Percent = ") << data.percent  << endl
               << F(" - Voltage = ") << data.voltage  << endl
               << F(" - VoltageCalculated = ") << data.voltageCalculated  << endl
               << F(" - Sampling time = ")
               << millis() - startTime - configuration.interval * 1000
               << F("msec.");
#endif

        counterOfSamplings = 0;
        temporaryAnalogData = 0;
        ready = true;
        startTime = 0;

#ifdef DEBUG
        Serial << endl << F("-------------------------------------------");
#endif
      }
    }
  }
}


void AFEAnalogInput::getJSON(char *json) {
 sprintf(json,
          "{\"raw\":%d,\"percent\":%f,\"voltage\":%f,\"voltageCalculated\":%f}",
          data.raw, data.percent,data.voltage,data.voltageCalculated);
}