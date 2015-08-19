#include <SPI.h>
#include <TCL.h>

/*****************************************************************************
 * config
 ****************************************************************************/

const int LEDS = 50;
const int HALF_LEDS = 25;

/*****************************************************************************
 * gamma
 ****************************************************************************/
const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

/*****************************************************************************
 * RGB
 ****************************************************************************/

typedef struct {
    int r;
    int g;
    int b;
} RGB;

const RGB blackRGB  = { 0x00, 0x00, 0x00 };
const RGB redRGB    = { 0xff, 0x00, 0x00 };
const RGB greenRGB  = { 0x00, 0xff, 0x00 };
const RGB blueRGB   = { 0x00, 0x00, 0xff };
const RGB purpleRGB = { 0xff, 0x00, 0xff };
const RGB whiteRGB  = { 0xff, 0xff, 0xff };
const RGB yellowRGB = { 0xff, 0x70, 0x00 };
const RGB orangeRGB = { 0xff, 0x20, 0x00 };
const RGB pinkRGB   = { 0xff, 0x00, 0x20 };

const int PRETTY_COLORS = 8;
const RGB prettyColors[PRETTY_COLORS] =
    { redRGB
    , greenRGB
    , blueRGB
    , purpleRGB
    , whiteRGB
    , yellowRGB
    , orangeRGB
    , pinkRGB
    };

RGB randomPrettyColor() {
    return(prettyColors[random(PRETTY_COLORS)]);
}

void sendRGB(RGB rgb) {
    TCL.sendColor(rgb.r, rgb.g, rgb.b);
}

RGB mixColors(RGB color1, RGB color2, float mix) {
    RGB rgb;
    rgb.r = (int)(((float)color1.r * (1.0 - mix)) + ((float)color2.r * mix));
    rgb.g = (int)(((float)color1.g * (1.0 - mix)) + ((float)color2.g * mix));
    rgb.b = (int)(((float)color1.b * (1.0 - mix)) + ((float)color2.b * mix));
    return (rgb);
}

RGB scaleRGB(RGB color, float scale) {
    RGB rgb;
    rgb.r = (int)((float)color.r * scale);
    rgb.g = (int)((float)color.g * scale);
    rgb.b = (int)((float)color.b * scale);
    return(rgb);
}

void blank() {
    TCL.sendEmptyFrame();
    for (int led = 0; led < LEDS; led++) {
        sendRGB(blackRGB);
    }
    TCL.sendEmptyFrame();
}

RGB readRGB() {
    RGB rgb;
    rgb.r = analogRead(TCL_POT3)>>2;
    rgb.b = analogRead(TCL_POT2)>>2;
    rgb.g = analogRead(TCL_POT4)>>2;

    return(rgb);
}

void sendRGBS (RGB rgb[]) {
    TCL.sendEmptyFrame();
    for (int led = 0; led < LEDS; led++) {
        sendRGB(rgb[led]);
    }
    TCL.sendEmptyFrame();
}

/*****************************************************************************
 * typedefs
 ****************************************************************************/

typedef unsigned long time ;
typedef RGB * strand;

/*****************************************************************************
 * pushButton
 ****************************************************************************/

int pushButtonNeedsInit = 1;
RGB pushButtonColor[LEDS];
int pushButtonDown = 0;

void pushButton() {
    if (pushButtonNeedsInit) {
        pushButtonNeedsInit = 0;
        for (int led = 0; led < LEDS; led++) {
            pushButtonColor[led] = blackRGB;
        }
    }

    if ((pushButtonDown == 0) && (digitalRead(TCL_MOMENTARY1) == LOW)) {
        pushButtonDown = 1;
    } else if ((pushButtonDown == 1)  && (digitalRead(TCL_MOMENTARY1) == HIGH)) {
        pushButtonDown = 0;
        int newLed = random(LEDS);
        RGB newColor = prettyColors[random(PRETTY_COLORS)];
        pushButtonColor[newLed] = newColor;

        TCL.sendEmptyFrame();
        for (int led = 0; led < LEDS; led++) {
            sendRGB(pushButtonColor[led]);
        }
        TCL.sendEmptyFrame();
    }
    delay(100);
}


float fadeternate = 0.0;
float fadedelta = 0.01;

void fadeternateLoop() {
  TCL.sendEmptyFrame();
  float a = 255.0 * fadeternate;
  float b = 255.0 * (1 - fadeternate);
  for (int led = 0; led < LEDS; led++)
  {
   if (led % 2) {
     TCL.sendColor(a, 0 ,0);
   } else {
     TCL.sendColor(0, 0, b);
   }
  }

  if (((fadeternate + fadedelta) > 1) || ((fadeternate + fadedelta) < 0))
    fadedelta = -fadedelta;
  fadeternate += fadedelta;
  delay(20);
}

int alt = 0;

void alternate()
{
  TCL.sendEmptyFrame();
  for (int led = 0; led < LEDS; led++)
  {
   if ((led + alt) % 2) {
     TCL.sendColor(0xff, 0, 0);
   } else {
     TCL.sendColor(0, 0, 0xff);
   }
  }

  alt = (alt + 1) % 2;
  delay(200);
}

int dripPos = 0;

void dripDecay() {
  TCL.sendEmptyFrame();
  for (int led = 0; led < LEDS; led++)
  {
   if (led == dripPos) {
     TCL.sendColor((0xff / LEDS) * (LEDS - dripPos), 0, 0);
   } else {
     TCL.sendColor(0, 0, 0);
   }
  }
  TCL.sendEmptyFrame();
  dripPos = (dripPos + 1) % LEDS;
  delay(100);
}




void randomLights() {
  TCL.sendEmptyFrame();
  for (int led = 0; led < LEDS; led++)
  {
    int red = random(0xff);
    int blue = random(0xff);
    int green = random(0xff);
    TCL.sendColor(red, 0, blue );
  }
  TCL.sendEmptyFrame();

  delay(500);
}

/*****************************************************************************
 * color cycle
 ****************************************************************************/

time colorCycleLastChange = 0;
RGB colorCycleColor1[LEDS];
RGB colorCycleColor2[LEDS];
float colorCyclePosition = 0.0;
bool needsInit = true;

void colorCycle(time now) {
    if (needsInit) {
        for (int led; led < LEDS; led++) {
            colorCycleColor1[led] = randomPrettyColor();
            colorCycleColor2[led] = randomPrettyColor();
        }
    }
    if (now < colorCycleLastChange + 5)
        return;

    colorCycleLastChange = now;

    if (colorCyclePosition > 1.0) {
        colorCyclePosition = 0.0;
        for (int led; led < LEDS; led++) {
            colorCycleColor1[led] = colorCycleColor2[led];
            colorCycleColor2[led] = randomPrettyColor();
        }
    }

    TCL.sendEmptyFrame();
    for (int led = 0; led < LEDS; led++) {
        sendRGB(mixColors(colorCycleColor1[led], colorCycleColor2[led], colorCyclePosition));
    }
    TCL.sendEmptyFrame();

    colorCyclePosition += 0.03;
}

/*****************************************************************************
 * lightening
 ****************************************************************************/

const int LIGHTENING_COLORS = 1;
const RGB lighteningColors[LIGHTENING_COLORS] =
  { {0xff, 0xff, 0xff}
//  , {0x00, 0xff, 0xf00}
  };

void lightening() {
    if (random(5000) >= 4990) {
        int pos     = random(LEDS);
        int color   = 0; // random(LIGHTENING_COLORS);
        int flashes = random(4) + 1;
        while (flashes--) {
            TCL.sendEmptyFrame();
            for (int led = 0; led < LEDS; led++) {
                if (led == pos) {
                    sendRGB(lighteningColors[color]);
                } else {
                    sendRGB(blackRGB);
                }
            }
            TCL.sendEmptyFrame();
            delay(30);
            blank();
            delay(30);
        }
    }
}

/*****************************************************************************
 * decay
 ****************************************************************************/
float decayLeds[LEDS];
time  decayLastChange = 0;

void initDecay() {
  for (int led = 0; led < LEDS; led++) {
    decayLeds[led] = 0;
  }
}

void decay(time now) {
    if (decayLastChange + 10 > now)
        return;
    decayLastChange = now;

    TCL.sendEmptyFrame();
    for (int led = 0; led < LEDS; led++)
    {
        if (random(1000) >= 995) {
            decayLeds[led] = 1.0;
        }
        sendRGB(scaleRGB(orangeRGB, decayLeds[led]));
//        TCL.sendColor(decayLeds[led] * 0xff, 0, 0); /* decayLeds[led] * 0xff); */
        decayLeds[led] = decayLeds[led] * 0.95;
    }
    TCL.sendEmptyFrame();
}


/*****************************************************************************
 * wave
 ****************************************************************************/
time  waveLastChange = 0;
float wavePhase = 0;
void wave(time now) {
    if (waveLastChange + 10 > now)
        return;
    waveLastChange = now;

    TCL.sendEmptyFrame();
    for (int led = 0; led < LEDS; led++)
    {
        int i = 128.0 + 128.0 * sin ( 2.0 * PI * ((float)led/(float)LEDS) + wavePhase);
        TCL.sendColor(i, 0, i);
    }
    TCL.sendEmptyFrame();
    wavePhase = wavePhase + 0.1;
    if (wavePhase >= 2.0 * PI) {
        wavePhase = wavePhase - (2.0 * PI);
    }
}

int fillTop = LEDS;
int fillPos = 0;
RGB fillColor = pinkRGB;
RGB fillArray[LEDS];
int fillBlank = true;

void fillUp () {

    if (fillBlank) {
        for (int led = 0; led < LEDS; led++) {
            fillArray[led] = blackRGB;
        }
        fillBlank = false;
    }

    sendRGBS(fillArray);

    if ((fillPos + 1) < fillTop) {
        fillArray[fillPos] = blackRGB;
    }

    fillPos++;
    if (fillPos >= fillTop) {
        fillColor = prettyColors[random(PRETTY_COLORS)];
        fillTop--;
        fillPos = 0;
        if (fillTop < 0) {
            fillTop = LEDS;
            fillBlank = true;
        }
    }
    fillArray[fillPos] = fillColor;

    delay(40);
}

void flashlight () {
    RGB rgb;

    rgb = readRGB();

    TCL.sendEmptyFrame();
    for (int led = 0; led < LEDS; led++) {
        sendRGB(rgb);
    }
    TCL.sendEmptyFrame();
}

void sendGamma(int r, int g, int b) {
  TCL.sendColor(pgm_read_byte(&gamma[r]),pgm_read_byte(&gamma[g]),pgm_read_byte(&gamma[b]));
}

/*****************************************************************************
 * loop left/right
 ****************************************************************************/
int loopLeftRightPos = 0;
time loopLeftRightTime = 0;
int loopLeftRightSubPos = 0;
void loopLeftRight(time now) {

  if  (loopLeftRightTime + 2 > now)
    return;

  loopLeftRightTime = now;

  loopLeftRightSubPos++;
  loopLeftRightSubPos %= 255;

  if (loopLeftRightSubPos == 0) {
    loopLeftRightPos++;
    loopLeftRightPos %= HALF_LEDS;
  }

  TCL.sendEmptyFrame();
  for (int led = 0; led < HALF_LEDS; led++) {
    if (led == loopLeftRightPos) {
      TCL.sendColor(0xff, 0x00, 0x00);
    } else {
      TCL.sendColor(0x00, 0x00, 0x00);
    }
  }

  for (int led = 0; led < HALF_LEDS; led++) {
    if (led == loopLeftRightPos) {
      sendGamma(0x00, loopLeftRightSubPos, 0x00);
    } else if ((led + 1) % HALF_LEDS == loopLeftRightPos) {
      sendGamma(0x00, 0xff - loopLeftRightSubPos, 0x00);
    } else {
      TCL.sendColor(0x00, 0x00, 0x00);
    }
  }

#if 0
  for (int led = 0; led < HALF_LEDS; led++) {
    if (led == loopLeftRightPos) {
      TCL.sendColor(0x00, 0x10, 0x00);
    } else if ((led + 1) % HALF_LEDS == loopLeftRightPos) {
      TCL.sendColor(0x00, 0xff, 0x00);
    } else if ((led + 2) % HALF_LEDS == loopLeftRightPos) {
      TCL.sendColor(0x00, 0x10, 0x00);
    } else {
      TCL.sendColor(0x00, 0x00, 0x00);
    }
  }
  #endif
  TCL.sendEmptyFrame();

}

/*****************************************************************************
 * pulse
 ****************************************************************************/
int pulseLevel = 30;
time pulseTime = 0;
int pulseDelta = 1;

void pulse(time now) {
  if (pulseTime + 12 > now) return;
  pulseTime = now;

  pulseLevel += pulseDelta;
  if (pulseLevel >= 100) {
    pulseDelta *= -1;
  }
  if (pulseLevel <= 27) {
    pulseDelta *= - 1;
  }
  TCL.sendEmptyFrame();
  for (int led = 0; led < HALF_LEDS; led++) {
      sendGamma(pulseLevel, 0x00, 0x00);
  }

  for (int led = 0; led < HALF_LEDS; led++) {
    sendGamma(pulseLevel, 0x00, 0x00);
  }
  TCL.sendEmptyFrame();
}

/*****************************************************************************
 * setup
 ****************************************************************************/

void setup() {

    TCL.begin();
    TCL.setupDeveloperShield();
    initDecay();
}

/*****************************************************************************
 * cycle
 ****************************************************************************/

time lastChange = millis();
int sequence = 3;
const int SEQUENCES = 4;
bool momentary1Down = false;
bool momentary2Down = false;

void cycle () {
    time now = millis();
    if (digitalRead(TCL_SWITCH1) || true) {
        if (now > (lastChange + ((time)60 * (time)1000)))
        {
            lastChange = now;
            sequence = (sequence + 1) % SEQUENCES;
        }
    }

    if (!momentary1Down && (digitalRead(TCL_MOMENTARY1) == LOW)) {
        momentary1Down = true;
    } else if (momentary1Down && (digitalRead(TCL_MOMENTARY1) == HIGH)) {
        momentary1Down = false;
        lastChange = now;

        sequence = (sequence + 1) % SEQUENCES;
    }

    if (!momentary2Down && (digitalRead(TCL_MOMENTARY2) == LOW)) {
        momentary2Down = true;
    } else if (momentary2Down && (digitalRead(TCL_MOMENTARY2) == HIGH)) {
        momentary2Down = false;
        lastChange = now;

        if (--sequence < 0) {
            sequence = SEQUENCES - 1;
        }
    }
    //    loopLeftRight(now);
    pulse(now);
    /*
    switch(sequence) {

    case 0 :
        wave(now);
        break;
    case 1 :
        decay(now);
        break;
    case 2 :
        lightening();
        break;
    case 3 :
        colorCycle(now);
        break;
    }

    case 3 :
        alternate();
        break;
    case 4 :
        decay();
        break;
    case 5 :
        fillUp();
        break;
    }
    */
}


void loop () {
    if (digitalRead(TCL_SWITCH2)) {
        flashlight();
    } else {
        cycle();
    }
}
