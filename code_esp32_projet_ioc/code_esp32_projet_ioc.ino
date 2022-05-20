/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               BILBIOTHÈQUES                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               CONSTANTES                                                    //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     16 // Reset pin
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define INT_PIN 2

#define MAX_WAIT_FOR_TIMER 2

#define LUM_PIN 36

#define BUZZZER_PIN  17 
#define CHN 0

#define SIZE_RIVER 141
#define SIZE_ELISE 93
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               STRUCTURES                                                    //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum {EMPTY, FULL};

struct mailbox_s {
  int state;
  int val;
};

struct mailbox_s mb_lum_to_serie = {.state = EMPTY};
struct mailbox_s mb_lum_to_led = {.state = EMPTY};
struct mailbox_s mb_serial = {.state = EMPTY};
struct mailbox_s mb_lum_to_wifi = {.state = EMPTY};

struct Lum {
  int timer;
  unsigned long period;
  int pin;
};

struct Led_s {
  int timer;                                              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // etat interne de la led
  int disable;
}; 

struct oled
{
  int timer;
  unsigned long period;
  int cpt;
};

struct wifi
{
  const char* ssid;
  const char* password;
  const char* server;
  int count;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               VARIABLES                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Update these with values suitable for your network.
//const char* mqtt_server = "192.168.43.162";
//const char* mqtt_ssid = "Salut";
//const char* mqtt_password = "pouetsalopette";
const char* mqtt_server = "192.168.1.111";
const char* mqtt_ssid = "ioc";
const char* mqtt_password = "";
//const char* mqtt_server = "192.168.0.27";
//const char* mqtt_ssid = "Numericable-612c";
//const char* mqtt_password = "a16zpl34qevd";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int musicSelect = 0; 
int play = 0; 

int river[] = {
  NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_GS5, 
  NOTE_A5, NOTE_E5, NOTE_A5, NOTE_D5,
  NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_GS5,
  NOTE_A5, NOTE_E5, NOTE_A5, NOTE_D5,
  
  NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_GS5, NOTE_A5, 
  NOTE_A4, NOTE_E4, NOTE_A5, NOTE_A4, NOTE_D4,
  NOTE_CS4, NOTE_D4, NOTE_E4, NOTE_CS4, NOTE_B3,

  NOTE_A4, NOTE_GS4, NOTE_A4, 
  NOTE_E3, NOTE_A4, NOTE_B4, NOTE_CS4, 
  NOTE_CS4, NOTE_D4, NOTE_E4, 
  NOTE_D4, NOTE_CS4, NOTE_B3, //44

  NOTE_A4, NOTE_CS4,
  NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_GS5, NOTE_A5, 
  NOTE_A4, NOTE_E4, NOTE_A5, NOTE_A4, NOTE_D4,
  NOTE_CS4, NOTE_D4, NOTE_E4, NOTE_CS5, NOTE_B4, //61

  NOTE_A4, NOTE_GS4, NOTE_A4, 
  NOTE_E4, NOTE_A4, NOTE_B4, NOTE_CS5, 
  NOTE_CS5, NOTE_D5, NOTE_E5, 
  NOTE_D5, NOTE_CS5, NOTE_B4, // 74

  NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_A4, NOTE_E4, NOTE_A4,
  NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_A4, NOTE_E4, NOTE_A4,
  NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_D6,
  NOTE_E6, NOTE_CS6, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_B4, 

  NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_A4, NOTE_E4, NOTE_A4,
  NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_A4, NOTE_E4, NOTE_A4,
  NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_D6,
  NOTE_E6, NOTE_CS6, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_B4, 

  NOTE_GS4, NOTE_E4,
  NOTE_A4, NOTE_CS4, NOTE_E4, NOTE_A5, 0


};

int riverDurations[] = {
  4, 4, 4, 4, 
  4, 4, 4, 1,
  4, 4, 4, 4, 
  4, 4, 4, 1,

  4, 8, 4, 8, 4, 
  8, 8, 4, 4, 4,
  4, 4, 4, 4, 2, 

  4, 4, 2, 
  4, 4, 4, 2, 
  4, 4, 2, 
  4, 4, 1, //44

  10, 10, 
  4, 8, 4, 8, 4, 
  8, 8, 4, 4, 4,
  4, 4, 4, 4, 2, 

  4, 4, 2, 
  4, 4, 4, 2, 
  4, 4, 2, 
  4, 4, 1, //74
  
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 4, 4, 

  
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 4, 4, // 134

  2, 2,
  4, 4, 4, 1, 1 //141
  
};



int elise[] = {
  NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, 
  NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4,
  NOTE_E4, NOTE_GS4, NOTE_B4, NOTE_C5,
   
  NOTE_E4, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4,
  NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, 
  NOTE_E4, NOTE_C5, NOTE_B4, NOTE_A4, 

  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, 
  NOTE_G4, NOTE_F5, NOTE_E5, NOTE_D5, 
  NOTE_F4, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_E4, NOTE_D5, NOTE_C5, NOTE_B4,

  NOTE_E4, NOTE_E5, NOTE_E4, NOTE_E5, NOTE_E6, 

  NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, 
  NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4,
  NOTE_E4, NOTE_GS4, NOTE_B4, NOTE_C5,
   
  NOTE_E4, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4,
  NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, 
  NOTE_E4, NOTE_C5, NOTE_B4, NOTE_A4, 0 
   
    
};


int eliseDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 2, 
  4, 4, 4, 2,  //17
  
  4, 4, 4, 4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 2,
  4, 4, 4, 2, // 35

  4, 4, 4, 2, 
  2, 4, 4, 2, 
  2, 4, 4, 2, 
  2, 4, 4, 2, //51

  4, 4, 4, 4, 2, 
  
  4, 4, 4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 2, 
  4, 4, 4, 2,  
  
  4, 4, 4, 4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 2,
  4, 4, 4, 1, 1 // 93

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               FONCTIONS                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void open_serial(){
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(INT_PIN, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(INT_PIN, LOW);
    }
  }
  if (String(topic) == "esp32/musiques") {
    Serial.print("Musiques : ");
    if(messageTemp == "1"){
      Serial.println("changement de musique : 1");
      musicSelect = 1;
    }
    else if(messageTemp == "2"){
      Serial.println("changement de musique : 2");
      musicSelect = 2;
    }
    else if(messageTemp == "play"){
      Serial.println("lecture");
      play = 1;
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void playRiver() {
  for (int thisNote = 0; thisNote < SIZE_RIVER; thisNote++) {
    int noteDuration = 1000 / riverDurations[thisNote];
    ledcWriteTone(CHN, river[thisNote]);
  
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
}


void playElise() {
    for (int thisNote = 0; thisNote < SIZE_ELISE; thisNote++) {
      int noteDuration = 1000 / eliseDurations[thisNote];
      ledcWriteTone(CHN, elise[thisNote]);
  
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               SETUPS                                                        //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup_lum(struct Lum* lum, int pin, int timer, unsigned long period){
  lum->pin = pin;
  lum->timer = timer;
  lum->period = period;
  pinMode(pin, INPUT);
}

void setup_Led( struct Led_s * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->etat = 0;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, ctx->etat);
}

void setup_oled(struct oled* ctx, int timer, unsigned long period){

  Wire.begin(4, 15); // pins SDA , SCL

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  ctx->timer = timer;
  ctx->period = period;
  
  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

}

void setup_wifi(struct wifi* ctx,const char* ssid,const char* password,const char* server){
  ctx->ssid = ssid;
  ctx->password = password;
  ctx->server = server;
  ctx->count = 0;
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(INT_PIN, OUTPUT);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               LOOPS                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop_lum(struct Lum* lum, struct mailbox_s * mb_lum_to_serie, struct mailbox_s * mb_lum_to_led, struct mailbox_s * mb_lum_to_wifi) {
  if (!(waitFor(lum->timer,lum->period))) return;
  if (mb_lum_to_serie->state == EMPTY){ // si la mailbox lum à serie est vide
    mb_lum_to_serie->val = analogRead(lum->pin);
    mb_lum_to_serie->state = FULL;
    mb_lum_to_led->val = mb_lum_to_serie->val;
    mb_lum_to_led->state = FULL;
    mb_lum_to_wifi->val = map(mb_lum_to_serie->val, 0, 4095, 100, 0);
    mb_lum_to_wifi->state = FULL;
  } 
}

void loop_serie(struct mailbox_s* mb_lum_to_serie){
  int a;
  if (mb_lum_to_serie->state != FULL) return; // attend que la mailbox soit pleine
  //Serial.print("luminisosite : ");
  //a = map(mb_lum_to_serie->val, 0, 4095, 0, 100);
  //Serial.print(a);
  //Serial.println("%");
  mb_lum_to_serie->state = EMPTY;
}

void loop_Led( struct Led_s * ctx, struct mailbox_s* mb_lum_to_led, struct mailbox_s* mb_serial) {
  /*if (!waitFor(ctx->timer, ctx->period)) return;          // sort s'il y a moins d'une période écoulée
  digitalWrite(ctx->pin,ctx->etat);                       // ecriture
  ctx->etat = 1 - ctx->etat;                              // changement d'état
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  ctx->period = map(mb->val, 0, 4095, 100000, 1000000);
  Serial.print("debug : ");
  Serial.println(ctx->period);
  mb->state = EMPTY;*/
  if(mb_serial->state == FULL) {
    ctx->disable = mb_serial->val;
    mb_serial->state = EMPTY;
  }

  if(!(ctx->disable)) {
    if(waitFor(ctx->timer, ctx->period)) {
        digitalWrite(ctx->pin,ctx->etat);                       // ecriture
        ctx->etat = 1 - ctx->etat;                              // changement d'état
    }
  
    if(mb_lum_to_led->state == FULL) {
      ctx->period = map(mb_lum_to_led->val, 0, 4095, 100000, 1000000);
      //Serial.print("debug : ");
      //Serial.println(ctx->period);
      mb_lum_to_led->state = EMPTY;
    }
  }
}

void serialEvent() {  
  //Serial.println("appel serial evnt");

  char c;
  if ((c = Serial.read()) == -1) return;
  if (mb_serial.state != EMPTY) return; // attend que la mailbox soit vide
  if (c == 's') { 
    mb_serial.val = 1;
    Serial.println("lecture de s");
    
  }
  else if (c == 'r') {
    mb_serial.val = 0;
    Serial.println("lecture de r");
    
  }
  mb_serial.state = FULL;
}

void loop_oled(struct oled* ctx){
  if (!(waitFor(ctx->timer,ctx->period))) return;
  ctx->cpt++;

  // Clear the buffer
  display.clearDisplay();

  display.setCursor(25,10); // On remet en haut à gauche
  display.print(F("Compteur : "));
  display.print(ctx->cpt); //Affichage du compteur
  display.print(F("s"));

  //Update screen
  display.display();
}

void loop_wifi(struct wifi* ctx, struct mailbox_s* mb_lum_to_wifi){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
//////////////////////////////////////////////////////////////    
/*    // Temperature in Celsius
    temperature = bme.readTemperature();   
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);*/

///////////////////////////////////////////////////////////////
/*    // Humidity
    humidity = bme.readHumidity();
    
    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("esp32/humidity", humString);*/

//////////////////////////////////////////////////////////////
      // Luminosité

    // Convert the value to a char array
    char lumString[8];
    dtostrf(mb_lum_to_wifi->val, 1, 2, lumString);
    //Serial.print("Luminosity: ");
    //Serial.println(mb_lum_to_wifi->val);
    client.publish("esp32/luminosity", lumString );
    mb_lum_to_wifi->state = EMPTY;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               APPEL DE STRUCT                                               //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Lum lum;
struct Led_s led;
struct oled oled;
struct wifi wifi;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                               MAINS                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  open_serial();
  setup_oled(&oled, 1, 1000000);
  setup_lum(&lum, LUM_PIN, 0, 500000);
  setup_Led(&led, 0, 10000, LED_BUILTIN);
  setup_wifi(&wifi, mqtt_ssid, mqtt_password, mqtt_server);
  pinMode(BUZZZER_PIN, OUTPUT);
  ledcSetup(CHN, 0, 10);
  ledcAttachPin(BUZZZER_PIN, CHN);
}

void loop()
{ 
  loop_serie(&mb_lum_to_serie);
  loop_lum(&lum, &mb_lum_to_serie, &mb_lum_to_led, &mb_lum_to_wifi);
  //loop_Led(&led, &mb_lum_to_led, &mb_serial);
  loop_oled(&oled);
  loop_wifi(&wifi, &mb_lum_to_wifi);
 
  if (Serial.available())
    serialEvent();

  while(play){
    
        if(musicSelect == 1) {
          playRiver();
          play = 0;

        }
        else if(musicSelect == 2) {
          playElise();
          play = 0;
        }
    }
}
