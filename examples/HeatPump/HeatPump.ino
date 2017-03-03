/**
 * AppMyProduct - www.appmyproduct.com
 *
 * This is an example AppMyProduct device application on the Arduino platform
 * simulating a heatpump. Currently supports the Heat Control Ionic starter app
 * available on <https://github.com/nabto/ionic-starter-nabto>.
 *
 * Hardware:
 * - ESP8266 module (e.g. Adafruit HUZZAH ESP8266 breakout)
 * - LED (built-in LED can be used)
 *
 * Usage:
 * - Enter ssid and password of your WiFi network.
 * - Enter device id and preshared key from portal.appmyproduct.com
 * - Connect LED to LED_PIN (anode) and ground (cathode) or use built-in LED
 * - Upload and run.
 * - Connect to your device using the Heat Control Ionic starter app and see
 *   the LED change its brightness according to the target heat.
 */
#include <Nabto.h>
extern "C" {
#include <modules/fingerprint_acl/fp_acl_ae.h>
#include <modules/fingerprint_acl/fp_acl_memory.h>
}
#include <fp_acl_eeprom.h>

// Enter ssid and password of your WiFi network
const char* WIFI_SSID = "<SSID>";
const char* WIFI_PASSWORD = "<PASSWORD>";

// Enter device id and pre-shared key from portal.appmyproduct.com
const char* DEVICE_ID = "<DEVICE ID>";
const char* PRE_SHARED_KEY = "<PRE-SHARED KEY>";

const int LED_PIN = BUILTIN_LED;

typedef enum {
    HPM_COOL = 0,
    HPM_HEAT = 1,
    HPM_CIRCULATE = 2,
    HPM_DEHUMIDIFY = 3
} heatpump_mode_t;

static uint8_t heatpump_state_ = 1;
static int32_t heatpump_room_temperature_ = 19;
static int32_t heatpump_target_temperature_ = 23;
static uint32_t heatpump_mode_ = HPM_HEAT;

#define DEVICE_NAME_DEFAULT "AMP stub"
#define MAX_DEVICE_NAME_LENGTH 50
static char device_name_[MAX_DEVICE_NAME_LENGTH];
static const char* device_product_ = "ACME 9002 Heatpump";
static const char* device_icon_ = "img/chip-small.png";

static struct fp_acl_db db_;
struct fp_mem_persistence fp_eeprom_;

#define REQUIRES_GUEST FP_ACL_PERMISSION_NONE
#define REQUIRES_OWNER FP_ACL_PERMISSION_ADMIN

void setup() {
    // Initialize built-in led
    pinMode(LED_PIN, OUTPUT);
    analogWrite(LED_PIN, PWMRANGE);

    // Initialize Serial
    Serial.begin(115200);

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

    // Initialize demo application
    demo_init();
    demo_application_set_device_name("ESP8266");
    demo_application_set_device_product("ACME 9002 Heatpump");
    demo_application_set_device_icon_("img/chip-small.png");
}

void loop() {
    Nabto.tick();
    demo_application_tick();
    delay(10);
}

void demo_init() {
    struct fp_acl_settings default_settings;
    default_settings.systemPermissions = FP_ACL_SYSTEM_PERMISSION_PAIRING |
                                         FP_ACL_SYSTEM_PERMISSION_LOCAL_ACCESS;
    default_settings.defaultUserPermissions = FP_ACL_PERMISSION_LOCAL_ACCESS;
    default_settings.firstUserPermissions = FP_ACL_PERMISSION_ADMIN |
                                            FP_ACL_PERMISSION_LOCAL_ACCESS |
                                            FP_ACL_PERMISSION_REMOTE_ACCESS;

    if (fp_acl_eeprom_init(2048, &fp_eeprom_) != FP_ACL_DB_OK) {
        NABTO_LOG_ERROR(("cannot load acl from eeprom"));
    }

    fp_mem_init(&db_, &default_settings, &fp_eeprom_);
    fp_acl_ae_init(&db_);
    snprintf(device_name_, sizeof(device_name_), DEVICE_NAME_DEFAULT);
}

void demo_application_set_device_name(const char* name) {
    strncpy(device_name_, name, MAX_DEVICE_NAME_LENGTH);
}

void demo_application_set_device_product(const char* product) {
    device_product_ = product;
}

void demo_application_set_device_icon_(const char* icon) {
    device_icon_ = icon;
}

void demo_application_tick() {
    // Simulate room temperature converging to target temperature
    static nabto_stamp_t time_last_update_;
    nabto_stamp_t now = nabtoGetStamp();
    if (nabtoStampDiff2ms(nabtoStampDiff(&now, &time_last_update_)) > 2000) {
        if (heatpump_room_temperature_ < heatpump_target_temperature_) {
            heatpump_room_temperature_++;
        } else if (heatpump_room_temperature_ > heatpump_target_temperature_) {
            heatpump_room_temperature_--;
        }
        time_last_update_ = now;
    }

    // Change LED brightness according to target temperature or turn LED off
    // if virtual heat-pump is powered off.
    if (heatpump_state_) {
        double temp = heatpump_target_temperature_;
        if(temp > 30) temp = 30;
        else if (temp < 16) temp = 16;
        double val = (temp - 16) / 14.0;
        analogWrite(LED_PIN, (1 - val) * (0.95 * PWMRANGE));  // 5% - 100%
    } else {
        analogWrite(LED_PIN, PWMRANGE);  // off
    }
}

void debug_dump_acl() {
    void* it = db_.first();
    while (it != NULL) {
        struct fp_acl_user user;
        fp_acl_db_status res = db_.load(it, &user);
        if (res != FP_ACL_DB_OK) {
            NABTO_LOG_WARN(("ACL error %d\n", res));
            return;
        }
        NABTO_LOG_INFO(("%s [%02x:%02x:%02x:%02x:...]: %04x", user.name,
                        user.fp[0], user.fp[1], user.fp[2], user.fp[3],
                        user.permissions));
        it = db_.next(it);
    }
}

bool allow_client_access(nabto_connect* connection) {
    bool local = connection->isLocal;
    bool allow = fp_acl_is_connection_allowed(connection) || local;
    NABTO_LOG_INFO(("Allowing %s connect request: %s",
                    (local ? "local" : "remote"), (allow ? "yes" : "no")));
    debug_dump_acl();
    return allow;
}

application_event_result application_event(application_request* request,
                                           unabto_query_request* query_request,
                                           unabto_query_response* query_response) {

    //NABTO_LOG_INFO(("Nabto application_event: %u", request->queryId));
    //debug_dump_acl();

    // handle requests as defined in interface definition shared with
    // client - for the default demo, see
    // https://github.com/nabto/ionic-starter-nabto/blob/master/www/nabto/unabto_queries.xml

    application_event_result res;

    switch (request->queryId) {
    case 10000:
        // get_public_device_info.json
        if (!Nabto.write_string(query_response, device_name_)) return AER_REQ_RSP_TOO_LARGE;
        if (!Nabto.write_string(query_response, device_product_)) return AER_REQ_RSP_TOO_LARGE;
        if (!Nabto.write_string(query_response, device_icon_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint8(query_response, fp_acl_is_pair_allowed(request))) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint8(query_response, fp_acl_is_user_paired(request))) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint8(query_response, fp_acl_is_user_owner(request))) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 10010:
        // set_device_info.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_OWNER)) return AER_REQ_NO_ACCESS;
        res = Nabto.copy_string(query_request, device_name_, sizeof(device_name_));
        if (res != AER_REQ_RESPONSE_READY) return res;
        if (!Nabto.write_string(query_response, device_name_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 11000:
        // get_users.json
        return fp_acl_ae_users_get(request, query_request, query_response); // implied admin priv check

    case 11010:
        // pair_with_device.json
        if (!fp_acl_is_pair_allowed(request)) return AER_REQ_NO_ACCESS;
        res = fp_acl_ae_pair_with_device(request, query_request, query_response);
        debug_dump_acl();
        return res;

    case 11020:
        // get_current_user.json
        return fp_acl_ae_user_me(request, query_request, query_response);

    case 11030:
        // get_system_security_settings.json
        return fp_acl_ae_system_get_acl_settings(request, query_request, query_response); // implied admin priv check

    case 11040:
        // set_system_security_settings.json
        return fp_acl_ae_system_set_acl_settings(request, query_request, query_response); // implied admin priv check

    case 11050:
        // set_user_permissions.json
        return fp_acl_ae_user_set_permissions(request, query_request, query_response); // implied admin priv check

    case 11060:
        // set_user_name.json
        return fp_acl_ae_user_set_name(request, query_request, query_response); // implied admin priv check

    case 11070:
        // remove_user.json
        return fp_acl_ae_user_remove(request, query_request, query_response); // implied admin priv check

    case 20000:
        // heatpump_get_full_state.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_write_uint8(query_response, heatpump_state_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint32(query_response, heatpump_mode_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint32(query_response, (uint32_t)heatpump_target_temperature_)) return AER_REQ_RSP_TOO_LARGE;
        if (!unabto_query_write_uint32(query_response, (uint32_t)heatpump_room_temperature_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 20010:
        // heatpump_set_activation_state.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_read_uint8(query_request, &heatpump_state_)) return AER_REQ_TOO_SMALL;
        if (!unabto_query_write_uint8(query_response, heatpump_state_)) return AER_REQ_RSP_TOO_LARGE;
        NABTO_LOG_INFO(("Got (and returned) state %d", heatpump_state_));
        return AER_REQ_RESPONSE_READY;

    case 20020:
        // heatpump_set_target_temperature.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_read_uint32(query_request, (uint32_t*)(&heatpump_target_temperature_))) return AER_REQ_TOO_SMALL;
        if (!unabto_query_write_uint32(query_response, (uint32_t)heatpump_target_temperature_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    case 20030:
        // heatpump_set_mode.json
        if (!fp_acl_is_request_allowed(request, REQUIRES_GUEST)) return AER_REQ_NO_ACCESS;
        if (!unabto_query_read_uint32(query_request, &heatpump_mode_)) return AER_REQ_TOO_SMALL;
        if (!unabto_query_write_uint32(query_response, heatpump_mode_)) return AER_REQ_RSP_TOO_LARGE;
        return AER_REQ_RESPONSE_READY;

    default:
        NABTO_LOG_WARN(("Unhandled query id: %u", request->queryId));
        return AER_REQ_INV_QUERY_ID;
    }
}
