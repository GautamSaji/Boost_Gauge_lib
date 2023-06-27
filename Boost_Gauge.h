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
#include <Preferences.h>

//preferences macros

#define BOOST_LIMIT_FLASH "Boostfl"
#define BUZZER_STATUS_FLASH "Buzzerfl"

// All macros
#define MAX_BOOST_RESET_PIN 13
#define JOYSTICK_CENTER_PIN 26
#define JOYSTICK_LEFT_PIN  32
#define JOYSTICK_RIGHT_PIN 27
#define JOYSTICK_UP_PIN    25
#define JOYSTICK_DOWN_PIN  33

#define BUZZER_PIN 14
#define BUZZER_GAP_DURATION 200
#define BUZZER_BEEP_DURATION 100

#define LED_WARNING_PIN 5
#define LED_INIT_PIN 17


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

//menu highlight options
 #define HIGHLIGHT_BOOST_LIMIT 0
 #define HIGHLIGHT_BOOST_LIMIT_WARNING 1
 #define CHANGE_BOOST_LIMIT_WARNING 2
 #define CHANGE_BOOST_LIMIT 3
 #define SELECTED_BOOST_LIMIT 4
 #define DISPLAY_MAIN 5
 
 
 #define FLAG_RESET 0
 #define FLAG_SET 1

//

void IRAM_ATTR Max_Boost_Reset();
void IRAM_ATTR Joystick_mid_ISR();
void IRAM_ATTR Joystick_up_ISR();
void IRAM_ATTR Joystick_down_ISR();
void IRAM_ATTR Joystick_left_ISR();
void IRAM_ATTR Joystick_right_ISR();

//Strcucture to hold menu button flags
struct Menu_Button{
  bool  IsMenuActive;
  bool IsMenuSubActive;
  volatile uint8_t Joystick_center_FLAG=FLAG_RESET;
  volatile uint8_t Joystick_left_FLAG=FLAG_RESET;
  volatile uint8_t Joystick_right_FLAG=FLAG_RESET;
  volatile uint8_t Joystick_up_FLAG=FLAG_RESET;
  volatile uint8_t Joystick_down_FLAG=FLAG_RESET;

  Menu_Button(){
   IsMenuActive=false;
   IsMenuSubActive=false;
   /*Joystick_center_FLAG=FLAG_RESET;
   Joystick_left_FLAG=FLAG_RESET;
   Joystick_right_FLAG=FLAG_RESET;
   Joystick_up_FLAG=FLAG_RESET;
   Joystick_down_FLAG=FLAG_RESET;*/
  }

};
//

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
volatile uint8_t Max_Boost_Reset_FLAG;

Boost_calculation(){
raw_data=0;
boost_limit=BOOST_LIMIT;
alpha=ALPHA;
psi_val=0;
psi_val_prev=0;
max_value=0;
last_avg=0;
curr_avg=0;
Max_Boost_Reset_FLAG=0;
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
public:
//TFT Component
Boost_display BoostDisplay;
//

//External ADC
Adafruit_ADS1115 ads;
//

//Boost parameters & display boost
Boost_calculation Boost_params;
//

//Menu button
Menu_Button MenuButton;
//

//flash memory obj
Preferences preferences;

//configurations
bool isBuzzerActive;

bool isMenuHbuzzerDisplayed;
bool isMenuHlimitDsiplayed;
bool isMenuSubLimitDisplayed;
//

//buzzer control params
  uint32_t BuzzerBeepDuration;
  uint32_t BuzzerGapDuration;
  uint32_t buzzer_previous_time;
  bool beep_on;
//
/////default constructor
   Boost_Gauge (){
  
  ads=Adafruit_ADS1115();
  isBuzzerActive=true;
  isMenuHbuzzerDisplayed=false;
  isMenuHlimitDsiplayed=false;
  isMenuSubLimitDisplayed=false;
  BuzzerBeepDuration=BUZZER_BEEP_DURATION;
  BuzzerGapDuration=BUZZER_GAP_DURATION;
  buzzer_previous_time=0;
  bool beep_on=false;
   }


void init (void); //initializes the tft lcd and external adc
void DisplaySplashScreen(uint8_t seconds); //displays splash screen stored in image.h in Boost_Gauge_Fonts.h for "seconds" 
void DisplayMain(void); //displays the main boost gauge screen
void Test(void); //displays a test procedure on the display
void DisplayBoost(void); //calculates and displays the boost on the screen
void ResetMaxBoost(void);//resets and diplays max boost
void DisplayMenu(uint8_t MenuParam);//displays menu with highlighter option
bool WaitForButtonPress(void);
};















#endif 
