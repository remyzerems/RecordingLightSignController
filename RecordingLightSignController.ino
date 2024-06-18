#include "arduino_secrets.h"
#include <ArduinoOSCWiFi.h>

// Digital output pin to which the light sign is connected
#define RECORD_TALLY_CONTROL_PIN      D10

// This is a bit field to ask Ardour which feedback we'd like to get
// Please refer to https://manual.ardour.org/using-control-surfaces/controlling-ardour-with-osc/calculating-feedback-and-strip-types-values/#feedback
// Even better, go to Ardour OSC parameters, this value is calculated for you !
#define ARDOUR_FEEDBACK_VALUE         24603

/****** NETWORK PARAMETERS *******/
#define ARDOUR_OSC_PORT               3819
#define OSC_LOCAL_RECEIVE_PORT        8000

char ssid[] = SECRET_WIFI_SSID;
char pwd[] = SECRET_WIFI_PASSWORD;

String host = SECRET_DAW_COMPUTER_HOSTNAME;
uint16_t remote_port = ARDOUR_OSC_PORT;

uint16_t bind_port = OSC_LOCAL_RECEIVE_PORT;
/*********************************/

bool connected = false;
static uint32_t prev_func_ms = millis();

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    Serial.setTimeout(30);

    // Set record tally control pin to output
    pinMode(RECORD_TALLY_CONTROL_PIN, OUTPUT);

    // WiFi stuff
    WiFi.begin(ssid, pwd);

    // Subscribe to Ardour OSC route /rec_enable_toggle
    OscWiFi.subscribe(bind_port, "/rec_enable_toggle",
        [&](int& i) {
            Serial.print("/rec_enable_toggle/args ");
            Serial.print(i); Serial.println();

            // Set record tally control pin to high or low depending on the rec_enable_toggle value
            digitalWrite(RECORD_TALLY_CONTROL_PIN, (i != 0 ? HIGH : LOW));
        }
    );

    // Subscribe to Ardour OSC route /heartbeat
    // This allows us to know if Ardour is (still) there or not and trigger a (re)connection
    OscWiFi.subscribe(bind_port, "/heartbeat",
        [&](int& i) {
            // Keep track of when we got that call
            prev_func_ms = millis();

            // If we were not connected...
            if(connected == false)
            {
              // Declare we are now connected since we successfully received this OSC frame
              Serial.println("Connected to OSC !");
              connected = true;
            }
        }
    );

  /* // This is for debug, it gets all incoming OSC frames
    OscWiFi.subscribe(bind_port, "/*",
    [](const OscMessage& m) {
        connect_done = true;
        prev_func_ms = millis();
        Serial.print(m.remoteIP()); Serial.print(" ");
        Serial.print(m.remotePort()); Serial.print(" ");
        Serial.print(m.size()); Serial.print(" ");
        Serial.print(m.address()); Serial.print(" ");
        Serial.print(m.arg<int>(0)); Serial.println();
    }
);*/

    Serial.println("Ready to receive OSC feedbacks");
}

void loop() {
    // Run the subscribers
    OscWiFi.update();

    // If we did not receive a heartbeat for too long...
    if (millis() > prev_func_ms + 2000) {
        connected = false;
        int feedback = ARDOUR_FEEDBACK_VALUE;
        Serial.println("Not connected : sending set_surface");
        // Send a "connect" OSC frame to Ardour
        // This requests Ardour to send us "feedbacks" OSC frames
        // Ardour does not send anything to a control surface prior calling set_surface
        OscWiFi.send(host, remote_port, "/set_surface/feedback", feedback);

        prev_func_ms = millis();
    }
}
