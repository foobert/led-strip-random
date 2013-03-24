#include "SPI.h"
#include "Adafruit_WS2801.h"

typedef struct Pixel {
  uint32_t color;
  byte mode;
  float level;
  float delta;
  unsigned int wait;
} Pixel;

Adafruit_WS2801 strip = Adafruit_WS2801(25, 3, 2);

Pixel* pixels;

uint32_t color;
float level = 0.0;
float delta = 0.01;

void setup() {
  randomSeed(analogRead(0));
  strip.begin();
  fill(Color(0, 0, 0));
  strip.show();
  pixels = (Pixel*)calloc(strip.numPixels(), sizeof(Pixel));
  for (int i = 0; i < strip.numPixels(); i++) {
    pixels[i].mode = 0;
  }
  color = Color(0x91, 0x37, 0x02);
}

uint32_t palette[] = {
  /*
  Color(0xFF, 0x00, 0x00),
  Color(0x00, 0xFF, 0x00),
  Color(0x00, 0x00, 0xFF),
  Color(0xFF, 0xFF, 0xFF),
  Color(0x00, 0x00, 0x00),
  /* */
  /*
  Color(0xED, 0xF3, 0x93),
  Color(0xF5, 0xE6, 0x65),
  Color(0xFF, 0xC4, 0x72),
  Color(0x4C, 0x02, 0x00),
  Color(0x89, 0xBA, 0xBE)
  /* */
  
  Color(0x61, 0x2E, 0x34),
  Color(0xE2, 0x7A, 0x4D),
  Color(0xFF, 0xBB, 0x81),
  Color(0xFF, 0xEA, 0xD9),
  Color(0xBE, 0x82, 0xAF),
  Color(0x19, 0x91, 0x9E),
  Color(0xFF, 0x31, 0x00),
  Color(0x72, 0x4C, 0x75)
  /* */
};

const byte palette_size = 8;

void loop() {
   for (int i = 0; i < strip.numPixels(); i++) {
     Pixel* p = &pixels[i];
     switch (p->mode) {
       case 0: // start
        p->wait = random(100, 500);
        p->mode = 1;
        break;
      case 1: // black, wait
        p->wait--;
        if (p->wait <= 0) {
          //p->color = palette[random(0, palette_size)];
          p->color = HSVtoRGB(random(0, 255) / 255.0, 190, 210);
          p->level = 0.0;
          p->delta = random(1, 5) / 200.0;
          p->mode = 2;
        }
        break;
      case 2: // get brighter
        p->level += p->delta;
        if (p->level > 1.0) {
          p->mode = 3;
          p->wait = random(100, 1000);
          p->level = 1.0;
        }
        strip.setPixelColor(i, dim(p->color, p->level));
        break;
      case 3: // fully lit
        p->wait--;
        if (p->wait <= 0) {
          p->mode = 4;
          p->level = 1.0;
          p->delta = random(1, 5) / 200.0;
        }
        break;
      case 4: // get darker again
        p->level -= p->delta;
        if (p->level <= 0.0) {
          p->mode = 0;
          p->level = 0.0;
        }
        strip.setPixelColor(i, dim(p->color, p->level));
        break;
     }
   }
   strip.show();
   delay(10);
}

void fill(uint32_t c) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}

uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t dim(uint32_t color, float level) {
  uint32_t c;
  c = (byte)((color >> 16 & 0xFF) * level);
  c <<= 8;
  c |= (byte)(((color >> 8) & 0xFF) * level);
  c <<= 8;
  c |= (byte)(((color >> 0) & 0xFF) * level);
  return c;
}

uint32_t HSVtoRGB(float h, float s, float v) 
{
  float r, g, b;
  float f, p, q, t;
  int i;

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  return Color((byte)(r * 255), (byte)(g * 255), (byte)(b * 255));
}
