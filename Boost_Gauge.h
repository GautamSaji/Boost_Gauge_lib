#ifndef BOOST_GAUGE_H
#define BOOST_GAUGE_H

// All required header files
#include <TFT_eSPI.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_ADS1X15.h>

#include <Boost_Gauge_Fonts.h>
#include <SplashScreen.h>

// All macros
#define RECT_X 8       //bar starting point
#define RECT_Y 96
#define RECT_WIDTH  20   //bar width
#define RECT_HEIGHT 64  //bar height

#define DIGIT_X 13   //digits origin point
#define DIGIT_Y 80

#define BOOST_LIMIT 23
#define ALPHA 0.7

#define DRAW_BAR_PARAM 0
#define FILL_BAR_PARAM 1

#define TFT_LIGHTYELLOW          0xFFFC
#define TFT_LIGHTGOLDENRODYELLOW 0xFFDA
#define TFT_DARKORANGE           0xFC60

//TFT Backlight PWM control parameters
#define BACKLIGHT_FREQ       5000  
#define BACKLIGHT_CHANNEL    0
#define BACKLIGHT_RESOLUTION 8
#define LCD_BRIGHTNESS 40   //lcd brightness value

//Structure to hold the position of each bar in the bar gauge
struct Bar_pos_struct {

 public:

int32_t x;
int32_t y;
int32_t width;
int32_t height;

// Default constructor
    Bar_pos_struct(){
        x=0;
        y=0;
        width=0;
        height=0;
        
    }

};
//

//Part of main Boost Gauge obj
struct Boost_calculation{
uint16_t raw_data;
uint8_t boost_limit;

float alpha,psi_val,psi_val_prev,max_value;
double last_avg,curr_avg;

Boost_calculation(){
raw_data=0;
boost_limit=BOOST_LIMIT;
alpha=ALPHA;
psi_val=0;
psi_val_prev=0;
max_value=0;
last_avg=0;
curr_avg=0;
}

};

struct Boost_display{
//display
TFT_eSPI display=TFT_eSPI();
TFT_eSprite sprite=TFT_eSprite(&display);
//

//Display parameters
const uint16_t    bar_color[15]={TFT_GREEN, TFT_GREEN,TFT_GREEN,TFT_GREENYELLOW, TFT_GREENYELLOW,TFT_GREENYELLOW, TFT_GREENYELLOW, TFT_YELLOW, TFT_YELLOW,TFT_YELLOW,
  TFT_ORANGE, TFT_ORANGE,TFT_DARKORANGE, TFT_DARKORANGE, TFT_RED};
Bar_pos_struct  draw_bar_pos[15]; //filled using bar_pos_fill method
Bar_pos_struct  fill_bar_pos[15]; //filled using bar_pos_fill method
int backlight_freq,backlight_Channel,backlight_resolution;
//

Boost_display(){

 
 backlight_freq = BACKLIGHT_FREQ;
 backlight_Channel = BACKLIGHT_CHANNEL;
 backlight_resolution = BACKLIGHT_RESOLUTION;


}

};
//

//Main Boost Gauge obj
class Boost_Gauge{
private:
//TFT Component
Boost_display BoostDisplay;
//

//External ADC
Adafruit_ADS1115 ads;
//

//Boost parameters & display boost
Boost_calculation Boost_params;
//
public:
/////default constructor
   Boost_Gauge (){

  ads=Adafruit_ADS1115();
   }


void init (void); //initializes the tft lcd and external adc
void DisplaySplashScreen(uint8_t seconds); //displays splash screen stored in image.h in Boost_Gauge_Fonts.h for "seconds" 
void DisplayMain(void); //displays the main boost gauge screen
void Test(void); //displays a test procedure on the display
void DisplayBoost(void); //calculates and displays the boost on the screen

};















#endif 