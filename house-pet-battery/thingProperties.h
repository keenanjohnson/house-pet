#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>


const char THING_ID[] = "FIXME";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)


float batteryvoltage;
float loadCurrent;
float chargeCurrent;
float currentRatio;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(batteryvoltage, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(loadCurrent, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(chargeCurrent, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(currentRatio, READ, 1 * SECONDS, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
