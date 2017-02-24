/**
 * Nabto - simple and secure - www.nabto.com
 *
 * This is an example uNabto application on the Arduino platform.
 * This basic example demonstrates toggling an LED over the internet,
 * but is easily extended to your needs.
 *
 * Hardware required:
 * - ESP8266 module (e.g. Adafruit HUZZAH ESP8266 breakout)
 * - LED (built-in LED can be used)
 *
 * How-To:
 * - Enter ssid and password of your WiFi network.
 * - Enter device id and preshared key from portal.nabto.com
 * - Connect LED to led1_pin (anode) and ground (cathode) or use built-in LED
 * - Upload and run.
 * - Type in the device id in your browser.
 */
#include <Nabto.h>

// Enter ssid and password of your WiFi network
const char* ssid = "...";
const char* password = "...";

// Enter device id and preshared key from developer.nabto.com
const char* nabtoId = "...";
const char* presharedKey = "...";

// Specify LED pin
const int led1_pin = BUILTIN_LED;

void setup() {
  // Initialize built-in led
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, 1);

  // Initialize Serial
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("done");

  // Initialize Nabto
  Serial.println("Init...");
  Nabto.begin(nabtoId, presharedKey);

  // Optionally get nabto firmware version
  char versionString[10];
  Nabto.version(versionString);
  Serial.print("Nabto v");
  Serial.print(versionString);
  Serial.println(" running...");
}

void loop() {
  // Check for new nabto udp packets and send response. Non-blocking
  Nabto.tick();

  // We have chosen to sleep 10 milliseconds between tics
  delay(10);
}

// Set LED and return state.
// Only using ID #1 in this simple example
uint8_t setLed(uint8_t led_id, uint8_t led_on) {
  if (led_id == 1) {
    // inverted
    digitalWrite(led1_pin, !led_on);
    return !digitalRead(led1_pin);
  }
  else {
    return 0;
  }
}

// Return LED state.
// Only using ID #1 in this simple example.
uint8_t readLed(uint8_t led_id) {
  if (led_id == 1) {
    // inverted
    return !digitalRead(led1_pin);
  }
  else {
    return 0;
  }
}

/***************** The uNabto application logic *****************
 * This is where the user implements his/her own functionality
 * to the device. When a Nabto message is received, this function
 * gets called with the message's request id and parameters.
 * Afterwards a user defined message can be sent back to the
 * requesting browser.
 ****************************************************************/
application_event_result application_event(application_request* appreq, unabto_query_request* r_b, unabto_query_response* w_b) {
  switch(appreq->queryId) {
  case 1:
    {
      //  <query name="light_write.json" description="Turn light on and off" id="1">
      //    <request>
      //      <parameter name="light_id" type="uint8"/>
      //      <parameter name="light_on" type="uint8"/>
      //    </request>
      //    <response>
      //      <parameter name="light_state" type="uint8"/>
      //    </response>
      //  </query>

      uint8_t light_id;
      uint8_t light_on;
      uint8_t light_state;

      // Read parameters in request
      if (!unabto_query_read_uint8(r_b, &light_id)) return AER_REQ_TOO_SMALL;
      if (!unabto_query_read_uint8(r_b, &light_on)) return AER_REQ_TOO_SMALL;

      // Set light according to request
      light_state = setLed(light_id, light_on);

      // Write back led state
      if (!unabto_query_write_uint8(w_b, light_state)) return AER_REQ_RSP_TOO_LARGE;

      return AER_REQ_RESPONSE_READY;
    }
  case 2:
    {
      //  <query name="light_read.json" description="Read light status" id="2">
      //    <request>
      //      <parameter name="light_id" type="uint8"/>
      //    </request>
      //    <response>
      //      <parameter name="light_state" type="uint8"/>
      //    </response>
      //  </query>

      uint8_t light_id;
      uint8_t light_state;

      // Read parameters in request
      if (!unabto_query_read_uint8(r_b, &light_id)) return AER_REQ_TOO_SMALL;

      // Read light state
      light_state = readLed(light_id);

      // Write back led state
      if (!unabto_query_write_uint8(w_b, light_state)) return AER_REQ_RSP_TOO_LARGE;

      return AER_REQ_RESPONSE_READY;

    default:
      return AER_REQ_INV_QUERY_ID;
    }
  }
}
