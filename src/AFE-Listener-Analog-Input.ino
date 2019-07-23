/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC

/* Main code for analog input listener */
void analogInputEventsListener() {
  {
    if (Device.configuration.isAnalogInput) {
      AnalogInput.listener();
      if (AnalogInput.isReady()) {
#if AFE_CONFIG_HARDWARE_NUMBER_OF_LEDS > 0
        Led.on();
#endif
        ADCINPUT_DATA data = AnalogInput.get();
        MQTTPublishAnalogInputData(data);
        DomoticzPublishAnalogInputData(data);
#if AFE_CONFIG_HARDWARE_NUMBER_OF_LEDS > 0
        Led.off();
#endif
      }
    }
  }
}
#endif
