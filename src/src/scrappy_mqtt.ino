/**
 * Scrappy Rover - Basic MQTT Connectivity Test
 * 
 * Flashes to ESP32 via Arduino IDE / `pio run --target upload`.
 * Connects to Wi-Fi "dreamland", publishes rover/heartbeat and listens on rover/control.
 * Uses the onboard LED (GPIO 2) as a connectivity indicator:
 *   Fast blink = scanning for WiFi
 *   Slow blink = disconnected from MQTT broker
 *   Solid ON = fully connected
 * 
 * Commands accepted on rover/control:
 *   "HEARTBEAT_ON"  - enable periodic telemetry
 *   "HEARTBEAT_OFF" - disable (default: disabled to save bandwidth)
 */

#include <WiFi.h>
#include <PubSubClient.h>

// ─── Wi-Fi & Broker Credentials (dreamland / chonk) ──────────────────
const char* ssid     = "dreamland";
const char* password = "fed7739605400";
const char* mqtt_server = "10.10.8.60";   // chonk MQTT broker IP
const int   mqtt_port = 1883;

// ─── Topics (rover namespace) ────────────────────────────────────────
const char* TOPIC_HEARTBEAT = "rover/heartbeat";
const char* TOPIC_STATUS    = "rover/status";
const char* TOPIC_CONTROL   = "rover/control";

// ─── MQTT Client Setup ───────────────────────────────────────────────
WiFiClient espClient;
PubSubClient mqtt(espClient);

// ─── LEDs / Indicators ──────────────────────────────────────────────
const int LED_PIN   = 2;       // Onboard ESP32 LED (active low on many boards)
const int STAT_PIN  = 16;      // Optional external status LED (UART_TX)
bool     telemetry_enabled = false;

// ─── Heartbeat Timing ───────────────────────────────────────────────
unsigned long last_heartbeat_ms = 0;
const unsigned long HEARTBEAT_INTERVAL_MS = 5000;   // every 5s when enabled

// ─── WiFi Connection State ──────────────────────────────────────────
bool wifi_connected = false;
bool mqtt_connected = false;

// ══════════════════════════════════════════════════════════════════════
//  SETUP
// ══════════════════════════════════════════════════════════════════════
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(STAT_PIN, OUTPUT);
  Serial.begin(115200);
  delay(10);

  // ── Connect to Wi-Fi ───────────────────────────────────────────
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi "); Serial.println(ssid);

  unsigned long wifi_start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);     // blink fast: scanning
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    
    if (millis() - wifi_start > 30000) {
      Serial.println("\nWiFi connect timeout!");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println("WiFi connected");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
  } else {
    // Keep trying forever in loop() via reconnect logic
    Serial.println("WiFi FAILED — will retry in loop");
  }

  // ── Connect to MQTT ────────────────────────────────────────────
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setBufferSize(512);          // enough for telemetry JSON

  // Try to connect immediately (may fail if WiFi is weak)
  if (wifi_connected) {
    attempt_mqtt_connect();
  }
}

// ══════════════════════════════════════════════════════════════════════
//  MQTT HELPERS
// ══════════════════════════════════════════════════════════════════════
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("CMD ["); Serial.print(topic); Serial.print("]: ");
  Serial.println(msg);

  if (strcmp(topic, TOPIC_CONTROL) == 0) {
    if (msg == "HEARTBEAT_ON") {
      telemetry_enabled = true;
      mqtt.publish(TOPIC_STATUS, "{\"status\":\"telemetry_started\"}", false);
    } else if (msg == "HEARTBEAT_OFF") {
      telemetry_enabled = false;
      mqtt.publish(TOPIC_STATUS, "{\"status\":\"telemetry_stopped\"}", false);
    }
  }
}

bool attempt_mqtt_connect() {
  String client_id = "scrappy-rover-" + String(ESP.getChipModel());
  
  if (mqtt.connect(client_id.c_str())) {
    mqtt_connected = true;
    mqtt.subscribe(TOPIC_CONTROL);
    Serial.print("MQTT connected & subscribed to ");
    Serial.println(TOPIC_CONTROL);
    
    // Blink LED solid on to indicate full connectivity
    digitalWrite(LED_PIN, LOW);   // ON (active low)
    return true;
  } else {
    mqtt_connected = false;
    Serial.print("MQTT connect failed, rc=");
    Serial.println(mqtt.state());
    return false;
  }
}

// ══════════════════════════════════════════════════════════════════════
//  LOOP
// ══════════════════════════════════════════════════════════════════════
void loop() {
  // ── If not connected to WiFi, keep trying ──────────────────────
  if (WiFi.status() != WL_CONNECTED) {
    wifi_connected = false;
    mqtt_connected = false;
    
    // Blink LED fast: scanning for WiFi
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);

    if (millis() - last_heartbeat_ms > 10000) {  // retry every 10s
      WiFi.begin(ssid, password);
      last_heartbeat_ms = millis();
      Serial.println("\nRetrying WiFi...");
    }
    
    mqtt.loop();  // keep MQTT client alive for reconnection callbacks
    return;
  }

  // ── MQTT connection maintenance ────────────────────────────────
  if (!mqtt_connected) {
    digitalWrite(STAT_PIN, HIGH);     // blink: MQTT down
    delay(200);
    digitalWrite(STAT_PIN, LOW);
    delay(200);

    if (millis() - last_heartbeat_ms > 5000) {  // retry every 5s
      if (attempt_mqtt_connect()) {
        last_heartbeat_ms = millis();   // reset to prevent flooding
      } else {
        last_heartbeat_ms = millis();   // count as a "heartbeat" attempt
      }
    }

    mqtt.loop();
    return;
  }

  // ── Fully connected — solid LED, send telemetry if enabled ─────
  digitalWrite(LED_PIN, LOW);   // Solid ON (active low)

  if (telemetry_enabled) {
    unsigned long uptime = millis() / 1000;
    String payload = "{\"uptime\":" + String(uptime) + 
                     ",\"rssi\":" + String(WiFi.RSSI()) +
                     ",\"free_heap\":" + String(ESP.getFreeHeap()) +
                     "}";
    mqtt.publish(TOPIC_HEARTBEAT, payload.c_str(), false);
  }

  mqtt.loop();

  // Brief sleep to keep serial output readable
  delay(100);
}
