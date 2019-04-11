/* ------------------------------------------------------------------------------

Besturing Scorebord

------------------------------------------------------------------------------ */

// INITIALISATIE
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <FastLED.h>

#define DATA_PIN_1      13 // DATA PIN voor het eerste cijfer
#define DATA_PIN_2      12 // DATA PIN voor het tweede cijfer
#define DATA_PIN_3      14 // DATA PIN voor het derde cijfer
#define DATA_PIN_4      27 // DATA PIN voor het vierde cijfer
#define DATA_PIN_5      5 // DATA PIN voor speleraanduiding

#define LED_TYPE           WS2812B
#define KLEUR_VOLGORDE     GRB
#define AANTAL_LEDS_CIJFER 42
#define AANTAL_LEDS_SPELER 80
#define MILLI_AMPS         1500
#define FRAMES_PER_SEC     140
#define HELDERHEID         100

CRGB leds_cijfer1[AANTAL_LEDS_CIJFER];
CRGB leds_cijfer2[AANTAL_LEDS_CIJFER];
CRGB leds_cijfer3[AANTAL_LEDS_CIJFER];
CRGB leds_cijfer4[AANTAL_LEDS_CIJFER];
CRGB leds_speler[AANTAL_LEDS_SPELER];

int cijfers[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

const char* ssid = "";
const char* password = "";

WebServer server(80);

/* ------------------------------------------------------------------------------ */

void setup() {

  delay(1000); 
  Serial.begin(115200);

  
  //WiFi.hostname("scorebord");
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Verbonden");// met ");
  //Serial.println(ssid);
  Serial.println("IP adres: ");
  Serial.println(WiFi.localIP());
   
  server.on("/zetscore", ontvangScoreData);
  
  server.begin();
  Serial.println("Web server gestart!");


  FastLED.addLeds<LED_TYPE, DATA_PIN_1, KLEUR_VOLGORDE>(leds_cijfer1, AANTAL_LEDS_CIJFER);
  FastLED.addLeds<LED_TYPE, DATA_PIN_2, KLEUR_VOLGORDE>(leds_cijfer2, AANTAL_LEDS_CIJFER);
  FastLED.addLeds<LED_TYPE, DATA_PIN_3, KLEUR_VOLGORDE>(leds_cijfer3, AANTAL_LEDS_CIJFER);
  FastLED.addLeds<LED_TYPE, DATA_PIN_4, KLEUR_VOLGORDE>(leds_cijfer4, AANTAL_LEDS_CIJFER);
  FastLED.addLeds<LED_TYPE, DATA_PIN_5, KLEUR_VOLGORDE>(leds_speler, AANTAL_LEDS_SPELER);
  
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(HELDERHEID);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);

  fill_solid(leds_cijfer1, AANTAL_LEDS_CIJFER, CRGB::Black);
  fill_solid(leds_cijfer2, AANTAL_LEDS_CIJFER, CRGB::Black);
  fill_solid(leds_cijfer3, AANTAL_LEDS_CIJFER, CRGB::Black);
  fill_solid(leds_cijfer4, AANTAL_LEDS_CIJFER, CRGB::Black);
  fill_solid(leds_speler, AANTAL_LEDS_SPELER, CRGB::Black);

  FastLED.show();
  
}

/* ------------------------------------------------------------------------------ */

void loop(void){
  server.handleClient();
  
  FastLED.show();
  delay(500);   
}


/* ------------------------------------------------------------------------------ */
void ontvangScoreData() {
  
  String score_a = server.arg("ScoreA");
  String score_b = server.arg("ScoreB");
  String speler  = server.arg("Speler");
  String msg = "";
  int httpcode;
  
  Serial.println("");
  Serial.print("Score A: ");
  Serial.println(score_a);
  Serial.print("Score B: ");
  Serial.println(score_b);
  Serial.print("Speler: ");
  Serial.println(speler);

  
  if(score_a != "" && score_b != "") {
    zetScore(score_a.toInt(), score_b.toInt(), speler);
    msg = "DATA ONTVANGEN: Score A = " + score_a + " / Score B = " + score_b + " / Speler = " + speler;
    httpcode = 200;
  }
  else {
    msg = "ONVOLLEDIGE DATA ONTVANGEN";
    httpcode = 422;
  }


  server.send(httpcode, "text/plain", msg);

}
/* ------------------------------------------------------------------------------ */

void zetScore (int score_a, int score_b, String speler) {
  
  int score_1 = score_a / 10;
  int score_2 = score_a % 10;
  int score_3 = score_b / 10;
  int score_4 = score_b % 10;
  CRGB kleur_spelerA = CRGB::Red;
  CRGB kleur_spelerB = CRGB::Red;

  if (speler == "A") {
    kleur_spelerA = CRGB::Green;
  } else if (speler == "B") {
    kleur_spelerB = CRGB::Green;
  }

  zetCijfer(leds_cijfer1, score_1, kleur_spelerA);
  zetCijfer(leds_cijfer2, score_2, kleur_spelerA);
  zetCijfer(leds_cijfer3, score_3, kleur_spelerB);
  zetCijfer(leds_cijfer4, score_4, kleur_spelerB);
}

void zetCijfer (CRGB leds[AANTAL_LEDS_CIJFER], int waarde, CRGB kleur)
{
  int count = 0;
  for(int j=0; j<7; j++) { 
    for(int k=0; k<6; k++) {
      if(cijfers[waarde][j] == 1) leds[count] = kleur;
      else leds[count] = CRGB::Black;

      count++;
    }
  }
}

/* ------------------------------------------------------------------------------ */
