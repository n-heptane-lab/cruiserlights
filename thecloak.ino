#include <SPI.h>
#include <TCL.h>

/*****************************************************************************
 * config
 ****************************************************************************/

const int LEDS = 50;

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
/*
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
