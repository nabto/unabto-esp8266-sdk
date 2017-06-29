/**
 * AppMyProduct - www.appmyproduct.com
 *
 * This is an example device application on the Arduino platform
 * to send a push notification to the basestation.
 *
 * Hardware:
 * - ESP8266 module (e.g. Adafruit HUZZAH ESP8266 breakout)
 * - Button (built-in GPIO0 button can be used)
 *
 * Usage:
 * - Configure the notifcation webhook on the basestation.
 * - Enter ssid and password of your WiFi network.
 * - Enter device id and preshared key
 * - Connect button to GPIO0 or use built-in button
 * - Upload and run.
 * - Press the button to send a push notification.
 */
#include <Nabto.h>
#include <unabto/unabto_push.h>

// Enter ssid and password of your WiFi network
const char* WIFI_SSID = "<SSID>";
const char* WIFI_PASSWORD = "<PASSWORD>";

// Enter device id and pre-shared key from portal.appmyproduct.com
const char* DEVICE_ID = "<DEVICE ID>";
const char* PRE_SHARED_KEY = "<PRE-SHARED KEY>";

const uint16_t PUSH_NOTIFICATION_SERVICE_ID = 1;

const int BUTTON_PIN = BUILTIN_LED; // same GPIO on HUZZAH board

uint8_t* unabto_push_notification_get_data(uint8_t* bufStart, const uint8_t* bufEnd, uint32_t seq){
    Serial.print("Push notification data (");
    Serial.print(seq);
    Serial.println("): NONE");
    return bufStart;
}

void unabto_push_notification_callback(uint32_t seq, unabto_push_hint* hint){
    Serial.print("Push notification callback (");
    Serial.print(seq);
    Serial.print("): ");
    switch(*hint){
        case UNABTO_PUSH_HINT_OK:
            Serial.println("OK");
            break;
        case UNABTO_PUSH_HINT_QUEUE_FULL:
            Serial.println("QUEUE FULL");
            break;
        case UNABTO_PUSH_HINT_INVALID_DATA_PROVIDED:
            Serial.println("INVALID DATA PROVIDED");
            break;
        case UNABTO_PUSH_HINT_NO_CRYPTO_CONTEXT:
            Serial.println("NO CRYPTO CONTEXT");
            break;
        case UNABTO_PUSH_HINT_ENCRYPTION_FAILED:
            Serial.println("ENCRYPTION FAILED");
            break;
        case UNABTO_PUSH_HINT_FAILED:
            Serial.println("FAILED");
            break;
        case UNABTO_PUSH_HINT_QUOTA_EXCEEDED:
            Serial.println("QUOTA EXCEEDED");
            break;
        case UNABTO_PUSH_HINT_QUOTA_EXCEEDED_REATTACH:
            Serial.println("QUOTA EXCEEDED REATTACH");
            break;
        default:
            Serial.println("unknown hint!");
    }
}
    
void setup() {
    // Initialize Serial
    Serial.begin(115200);

    // Initialize built-in led
    pinMode(BUTTON_PIN, INPUT);

    // Initialize WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("done");

    // Initialize Nabto
    Serial.println("Init Nabto...");
    Nabto.begin(DEVICE_ID, PRE_SHARED_KEY);

    // Print Nabto version
    char versionString[10];
    Nabto.version(versionString);
    Serial.print("Nabto v");
    Serial.print(versionString);
    Serial.println(" running");

    // Initialize push notifications
    unabto_push_init();
}

uint32_t seq = 0; // push notifcations sequence number

void loop() {
    Nabto.tick();
    delay(10);

    static bool buttonPressedLast = false;
    bool buttonPressedNow = !digitalRead(BUTTON_PIN);
    if (buttonPressedLast && !buttonPressedNow) {
        Serial.println("Button pressed!");
        unabto_push_hint ret = unabto_send_push_notification(PUSH_NOTIFICATION_SERVICE_ID, &seq);
        if (ret != UNABTO_PUSH_HINT_OK){
            Serial.println("Send push notification did not return OK!");
        }
    } 
    buttonPressedLast = buttonPressedNow;
}

application_event_result application_event(application_request* request,
                                           unabto_query_request* query_request,
                                           unabto_query_response* query_response) {
    return AER_REQ_INV_QUERY_ID;
}
