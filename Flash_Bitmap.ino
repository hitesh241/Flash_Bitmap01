// Code partly derived from ILI9341_Due library example

// Draws the 3 icons across the middle of the screen and pauses.
// Then draws 300 icons at random locations, clears screen and repeats
//
// This demonstrates drawing icons from FLASH

// Icons are stored in tabs, e.g. Alert.h etc
// more than one icon can be in a header file.

/*
 This sketch demonstrates loading images from arrays stored in program (FLASH) memory.

 Works with TFT_HX8357 library here:
 https://github.com/Bodmer/TFT_HX8357_Due

 This sketch does not use/need any fonts at all...

 Arrays containing FLASH images can be created with UTFT library tool:
 (libraries\UTFT\Tools\ImageConverter565.exe)
 Convert to .c format then copy into a new tab

 The number and size of icons is limited by available FLASH memory. The icon array will
 use width x height x 2 bytes of FLASH, i.e. 32 x 32 icon uses ~2048 bytes

*/
#include <Key.h>
#include <TFT_HX8357_Due.h> // Hardware-specific library

TFT_HX8357_Due tft = TFT_HX8357_Due();       // Invoke custom library

// Include the header files that contain the icons
#include "Home.h"
#include "Settings.h"
#include "Values.h"
#include <Keypad.h>
#include "Free_Fonts.h"

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

const uint16_t IconHeight = 64;
const uint16_t IconWidth = 64;

char hexaKeys[ROWS][COLS] = {
	{ '1','2','3','A' },
	{ '4','5','6','B' },
	{ '7','8','9','C' },
	{ '*','0','#','D' }
};
byte rowPins[ROWS] = { 6, 7, 8, 9 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 10, 11, 12, 13 }; //connect to the column pinouts of the keypad

long count = 0; // Loop count
uint16_t RGB565Color;
uint16_t buffer[4096];

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);	// landscape
}

void loop(){

	RGB565Color = rgb565_from_triplet(196, 196, 196);
	tft.fillScreen(RGB565Color);

	// Draw the icons
	drawIcon(Values, (tft.width() - ValuesWidth) / 2 - 100, (tft.height() - ValuesHeight) / 2, ValuesWidth, ValuesHeight);
	drawIcon(Home, (tft.width() - HomeWidth) / 2, (tft.height() - HomeHeight) / 2, HomeWidth, HomeHeight);
	drawIcon(Settings, (tft.width() - SettingsWidth) / 2 + 100, (tft.height() - SettingsHeight) / 2, SettingsWidth, SettingsHeight);

	tft.setTextDatum(MC_DATUM);
	tft.setTextColor(TFT_BLACK, RGB565Color);
	tft.setFreeFont(FF44);                 // Select the font

	uint8_t Pos = 0;

	while (1)
	{
		char Key = customKeypad.getKey();
		if (Key == '4' || Key == '6')
		{
			if (Key == '6' && Pos < 2) {
				Pos++;
			}
			else if (Key == '4' && Pos > 0) {
				Pos--;
			}
			switch (Pos) {
			case 0 :
				tft.drawRoundRect((tft.width() - (HomeWidth+10)) / 2, (tft.height() - (HomeHeight+10)) / 2, HomeWidth+10, HomeHeight+10, 5, RGB565Color);
				tft.drawRoundRect((tft.width() - (SettingsWidth+10)) / 2 + 100, (tft.height() - (SettingsHeight+10)) / 2, SettingsWidth+10, SettingsHeight+10, 5, RGB565Color);
				tft.drawRoundRect((tft.width() - (ValuesWidth+10)) / 2 - 100, (tft.height() - (ValuesHeight+10)) / 2, ValuesWidth+10, ValuesHeight+10, 5, TFT_RED);
				tft.setCursor(240, 260);
				tft.fillRect(0, 240, 480, 50, RGB565Color);
				tft.drawString("Parameters", 240, 260, GFXFF);
				break;
			case 1:
				tft.drawRoundRect((tft.width() - (ValuesWidth+10)) / 2 - 100, (tft.height() - (ValuesHeight+10)) / 2, ValuesWidth+10, ValuesHeight+10, 5, RGB565Color);
				tft.drawRoundRect((tft.width() - (SettingsWidth+10)) / 2 + 100, (tft.height() - (SettingsHeight+10)) / 2, SettingsWidth+10, SettingsHeight+10, 5, RGB565Color);
				tft.drawRoundRect((tft.width() - (HomeWidth + 10)) / 2, (tft.height() - (HomeHeight + 10)) / 2, HomeWidth + 10, HomeHeight + 10, 5, TFT_RED);
				tft.setCursor(240, 260);
				tft.fillRect(0, 240, 480, 50, RGB565Color);
				tft.drawString("Home", 240, 260, GFXFF);
				break;
			case 2:
				tft.drawRoundRect((tft.width() - (ValuesWidth+10)) / 2 - 100, (tft.height() - (ValuesHeight+10)) / 2, ValuesWidth+10, ValuesHeight+10, 5, RGB565Color);
				tft.drawRoundRect((tft.width() - (HomeWidth+10)) / 2, (tft.height() - (HomeHeight+10)) / 2, HomeWidth+10, HomeHeight+10, 5, RGB565Color);
				tft.drawRoundRect((tft.width() - (SettingsWidth + 10)) / 2 + 100, (tft.height() - (SettingsHeight + 10)) / 2, SettingsWidth + 10, SettingsHeight + 10, 5, TFT_RED);
				tft.setCursor(240, 260);
				tft.fillRect(0, 240, 480, 50, RGB565Color);
				tft.drawString("Settings", 240, 260, GFXFF);
				break;
			default:

				break;
			}

			//tft.setCursor(240, 260);
			//tft.fillRect(0, 220, 480, 50, RGB565Color);
			//tft.print(Key);
			//tft.drawString("Settings", 240, 260, GFXFF);
		}
	}
}


//====================================================================================
// This is the function to draw the icon stored as an array in program memory (FLASH)
//====================================================================================

// To speed up rendering we use a 64 pixel buffer
#define BUFF_SIZE 64

// Draw array "icon" of defined width and height at coordinate x,y
// Maximum icon size is 255x255 pixels to avoid integer overflow

void drawIcon(const unsigned short* icon, int16_t x, int16_t y, int8_t width, int8_t height) {

  uint16_t  pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  // Set up a window the right size to stream pixels into
  tft.setWindow(x, y, x + width - 1, y + height - 1);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)height * width) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)height * width) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }
}

uint16_t rgb565_from_triplet(uint8_t red, uint8_t green, uint8_t blue)
{
  red   >>= 3;
  green >>= 2;
  blue  >>= 3;
  return (red << 11) | (green << 5) | blue;
}
