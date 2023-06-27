#include "Boost_Gauge.h"



static void populate_bar_struct(Bar_pos_struct* bar,uint8_t param); //helper function to fill the bars inside boost gauge obj
static void Draw_bar(Boost_display* BoostDisplay,Boost_calculation* BoostParam);  //helper function to draw empty bars
static void Fill_bar(Boost_display* BoostDisplay,Boost_calculation* BoostParams);  //helper function to fill bars according to psi
static void setBoostFlash(int value, Preferences* prefs);
static int getBoostFlash(Preferences* prefs);

static void setBuzzerFlash(bool value, Preferences* prefs);
static bool getBuzzerFlash(Preferences* prefs);



void setBoostFlash(int value, Preferences* prefs) {
  // Open the preferences with the given key
  prefs->begin("flash", false);

  // Store the boost value
  prefs->putInt(BOOST_LIMIT_FLASH, value);

  // Close the preferences
  prefs->end();
}

int getBoostFlash(Preferences* prefs) {
  // Open the preferences with the given key
  prefs->begin("flash", true);

  // Retrieve the boost limit value, providing a default value of 0 if it doesn't exist
  int value = prefs->getInt(BOOST_LIMIT_FLASH, BOOST_LIMIT);

  // Close the preferences
  prefs->end();

  // Return the boost limit value
  return value;
}

void setBuzzerFlash(bool value, Preferences* prefs) {
  // Open the preferences with the given key
  prefs->begin("flash", false);

  // Store the buzzer value
  prefs->putBool(BUZZER_STATUS_FLASH, value)
  
     

  // Close the preferences
  prefs->end();
}

bool getBuzzerFlash(Preferences* prefs) {
  // Open the preferences with the given key
  prefs->begin("flash", true);

  // Retrieve the buzzer value, providing a default value of 1 if it doesn't exist
  bool value = prefs->getBool(BUZZER_STATUS_FLASH, true);

  // Close the preferences
  prefs->end();

  // Return the buzzer value
  return value;
}

void Boost_Gauge::init (void){

 pinMode(LED_INIT_PIN,OUTPUT);
 pinMode(LED_WARNING_PIN,OUTPUT);
  digitalWrite(LED_INIT_PIN,0);
  digitalWrite(LED_WARNING_PIN,0);



//Max boost reset button & joystick pin config
 pinMode(MAX_BOOST_RESET_PIN,INPUT_PULLUP);

 pinMode(JOYSTICK_CENTER_PIN,INPUT_PULLUP);
 pinMode(JOYSTICK_DOWN_PIN,INPUT_PULLUP);
 pinMode(JOYSTICK_LEFT_PIN,INPUT_PULLUP);
 pinMode(JOYSTICK_RIGHT_PIN,INPUT_PULLUP);
 pinMode(JOYSTICK_UP_PIN,INPUT_PULLUP);

 attachInterrupt(MAX_BOOST_RESET_PIN, Max_Boost_Reset, FALLING);

 attachInterrupt(JOYSTICK_CENTER_PIN, Joystick_mid_ISR, FALLING);
 attachInterrupt(JOYSTICK_DOWN_PIN, Joystick_down_ISR, FALLING);
 attachInterrupt(JOYSTICK_LEFT_PIN, Joystick_left_ISR, FALLING);
 attachInterrupt(JOYSTICK_RIGHT_PIN, Joystick_right_ISR, FALLING);
 attachInterrupt(JOYSTICK_UP_PIN, Joystick_up_ISR, FALLING);

//

isBuzzerActive=getBuzzerFlash(&preferences);
Boost_params.boost_limit=getBoostFlash(&preferences);

//display init
BoostDisplay.display.init();
//

//PWM brightness control
 //ledcSetup(BoostDisplay.backlight_Channel, BoostDisplay.backlight_freq, BoostDisplay.backlight_resolution);
 //ledcAttachPin(TFT_BL, BoostDisplay.backlight_Channel);
 //ledcWrite(BoostDisplay.backlight_Channel, LCD_BRIGHTNESS);
 //

 //fill bar pos structs
 populate_bar_struct(BoostDisplay.draw_bar_pos,DRAW_BAR_PARAM);
 populate_bar_struct(BoostDisplay.fill_bar_pos,FILL_BAR_PARAM);

 //initialize external adc
if (!ads.begin()) {
    BoostDisplay.display.setRotation(3);
    BoostDisplay.display.setTextSize(3);
    BoostDisplay.display.drawString("Failed to initialze ADS1115", 120, 0,1);
    while (1);
  }
//
digitalWrite(LED_INIT_PIN,1); //init successfully completed led indication

}



void Boost_Gauge::DisplaySplashScreen(uint8_t seconds){

//display splash screen
 BoostDisplay.display.setRotation(2);
 BoostDisplay.display.setSwapBytes(true);
 BoostDisplay.display.fillScreen(TFT_WHITE);
 BoostDisplay.display.pushImage(0,17,240,286,suzuki);
 
 delay(seconds*1000);
 digitalWrite(LED_INIT_PIN,0);

//
}

void Boost_Gauge::DisplayMain(void){

 BoostDisplay.display.fillScreen(TFT_BLACK);
 BoostDisplay.display.setRotation(3);

 BoostDisplay.sprite.createSprite(319,175);
 BoostDisplay.sprite.fillSprite(TFT_PURPLE);
 BoostDisplay.sprite.setTextColor(TFT_WHITE,TFT_BLACK);

   BoostDisplay.sprite.setTextPadding(3);
   BoostDisplay.sprite.setRotation(3);
   BoostDisplay.sprite.setSwapBytes(true);
  

   Draw_bar(&BoostDisplay,&Boost_params);
 //

   BoostDisplay.display.unloadFont();

}

void Boost_Gauge::Test(void){




BoostDisplay.display.setTextPadding(120);
BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);

 for(int i=0;i<=15;i++)
 {
    
    Fill_bar(&BoostDisplay,&Boost_params);
    Boost_params.psi_val_prev=Boost_params.psi_val;
    
 if (Boost_params.psi_val>=Boost_params.max_value)
      {
          //update max value
          Boost_params.max_value=Boost_params.psi_val;
          BoostDisplay.display.loadFont(psi_max_font36);
          if(Boost_params.max_value>Boost_params.boost_limit)
          {
            BoostDisplay.display.setTextColor(TFT_RED,TFT_BLACK);

          }

             
          
            BoostDisplay.display.drawFloat(Boost_params.max_value,1,240,166);
          
          
          BoostDisplay.display.unloadFont();
          BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
          
      }

         
         if(Boost_params.psi_val>Boost_params.boost_limit)
          {
            BoostDisplay.display.setTextColor(TFT_RED,TFT_BLACK);
               
            if(!beep_on && (isBuzzerActive==true))
             {
                     beep_on=true;
                     buzzer_previous_time=millis();
                     analogWrite(BUZZER_PIN, 180);
                     digitalWrite(LED_WARNING_PIN,HIGH);

                    
             }

          }

           if(beep_on && (  (millis()- buzzer_previous_time) >= BuzzerBeepDuration )  && (isBuzzerActive==true)  ){
                    analogWrite(BUZZER_PIN, 0);
                    digitalWrite(LED_WARNING_PIN,LOW);
                    beep_on=false;
                    buzzer_previous_time=millis();


             }

             if(  (!beep_on && ( (millis() - buzzer_previous_time  ) >= BuzzerGapDuration )  ) && (Boost_params.psi_val>Boost_params.boost_limit) && (isBuzzerActive==true)) {
                    beep_on=true;
                    buzzer_previous_time=millis();
                    analogWrite(BUZZER_PIN, 180);
                    digitalWrite(LED_WARNING_PIN,HIGH);


             }
          BoostDisplay.display.loadFont(psi_font59);
         
         BoostDisplay.display.drawFloat(Boost_params.psi_val,1,105,150);
         
           

         
         BoostDisplay.display.unloadFont();
         BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
         Boost_params.psi_val=Boost_params.psi_val+2;



 }

  for(int j=0;j<=15;j++)
  {

     
     Fill_bar(&BoostDisplay,&Boost_params);
     Boost_params.psi_val_prev=Boost_params.psi_val;
     BoostDisplay.display.loadFont(psi_font59);
        BoostDisplay.display.drawFloat(Boost_params.psi_val,1,105,150);
     Boost_params.psi_val=Boost_params.psi_val-2;


  }
Boost_params.max_value=0;
BoostDisplay.display.loadFont(psi_max_font36);
BoostDisplay.display.drawFloat(Boost_params.max_value,1,240,166);
BoostDisplay.display.unloadFont();



}

void Boost_Gauge::DisplayMenu(uint8_t MenuParam){


  
if(MenuParam==DISPLAY_MAIN)
{
BoostDisplay.display.fillScreen(TFT_BLACK);
   
   BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
   BoostDisplay.display.setTextPadding(120);
   BoostDisplay.display.loadFont(menu_font_large);
   BoostDisplay.display.drawString("Settings",100,10);
   BoostDisplay.display.unloadFont();
   BoostDisplay.display.loadFont(menu_font_small);
   BoostDisplay.display.drawString(">Buzzer",0,100);

   if(isBuzzerActive==true)
   {
    BoostDisplay.display.drawString("ON",248,100);
   }else if(isBuzzerActive==false)
   {
    BoostDisplay.display.drawString("OFF",248,100);
   }
    BoostDisplay.display.drawString(">Boost Limit",0,172);
    BoostDisplay.display.drawNumber(Boost_params.boost_limit,233,172);
    BoostDisplay.display.drawString("psi",263,172);
    BoostDisplay.display.unloadFont();


}else if(MenuParam==HIGHLIGHT_BOOST_LIMIT_WARNING)
{
   
if(isMenuHlimitDsiplayed==true)
{
  BoostDisplay.display.setTextPadding(40);
    //remove highlight from limit
   BoostDisplay.display.loadFont(menu_font_small);
   BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
   BoostDisplay.display.drawString(">Boost Limit",0,172);
   BoostDisplay.display.unloadFont();


}
    BoostDisplay.display.setTextPadding(40);
   BoostDisplay.display.loadFont(menu_font_small);
   BoostDisplay.display.setTextColor(TFT_BLACK,TFT_WHITE);
   BoostDisplay.display.drawString(">Buzzer",0,100);
    BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
    BoostDisplay.display.unloadFont();




}else if(MenuParam==HIGHLIGHT_BOOST_LIMIT)
{
    BoostDisplay.display.setTextPadding(40);
   if(isMenuHbuzzerDisplayed==true)
   {
     //remove highlight from buzzer
   BoostDisplay.display.loadFont(menu_font_small);
   BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
   BoostDisplay.display.drawString(">Buzzer",0,100);
   BoostDisplay.display.unloadFont();


   }
   
   BoostDisplay.display.loadFont(menu_font_small);
    BoostDisplay.display.setTextColor(TFT_BLACK,TFT_WHITE);
    BoostDisplay.display.drawString(">Boost Limit",0,172);
    BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
    BoostDisplay.display.unloadFont();




}else if (MenuParam==SELECTED_BOOST_LIMIT)
{
BoostDisplay.display.setTextPadding(40);
BoostDisplay.display.loadFont(menu_font_small);
    BoostDisplay.display.setTextColor(TFT_BLACK,TFT_DARKGREY);
    BoostDisplay.display.drawString(">Boost Limit",0,172);
    BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
    BoostDisplay.display.unloadFont();




}else if (MenuParam==CHANGE_BOOST_LIMIT_WARNING)
{BoostDisplay.display.setTextPadding(120);
  if(isBuzzerActive==true)
  {
BoostDisplay.display.loadFont(menu_font_small);
    BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
    BoostDisplay.display.drawString("ON",248,100);
    BoostDisplay.display.unloadFont();
  }else if(isBuzzerActive==false)
  {

   BoostDisplay.display.loadFont(menu_font_small);
    BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
    BoostDisplay.display.drawString("OFF",248,100);
    BoostDisplay.display.unloadFont();

  }


}else if (MenuParam==CHANGE_BOOST_LIMIT)
{BoostDisplay.display.setTextPadding(30);
  BoostDisplay.display.loadFont(menu_font_small);
  BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
  BoostDisplay.display.drawNumber(Boost_params.boost_limit,233,172);
  BoostDisplay.display.unloadFont();

}






}



void Boost_Gauge::DisplayBoost(void){


if(MenuButton.Joystick_center_FLAG==FLAG_SET)
{
  
  delay(100);
  MenuButton.IsMenuActive=true;
  MenuButton.Joystick_center_FLAG=FLAG_RESET;
   MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      
  
   delay(100);
}

while(MenuButton.IsMenuActive==true) //Menu subroutine active when center button is pressed for the first time
{
        
    delay(100);
   if((isMenuHbuzzerDisplayed==false) && (isMenuHlimitDsiplayed==false))
   {
   
    DisplayMenu(DISPLAY_MAIN);
    DisplayMenu(HIGHLIGHT_BOOST_LIMIT_WARNING);
    isMenuHbuzzerDisplayed=true;
     delay(100);
   }
   
   
   while(WaitForButtonPress()==false){}; //wait for button press
   /*while( !( MenuButton.Joystick_center_FLAG==FLAG_SET || MenuButton.Joystick_left_FLAG==FLAG_SET || 
   MenuButton.Joystick_right_FLAG==FLAG_SET || MenuButton.Joystick_up_FLAG==FLAG_SET || 
   MenuButton.Joystick_down_FLAG==FLAG_SET   ) );*/
   
     delay(100);
    //some button is pressed
    if(MenuButton.Joystick_center_FLAG==FLAG_SET)
    {   

      MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
     
      if(isMenuHbuzzerDisplayed==true)
      {
            
        //change boost limit buzzer config
        if(isBuzzerActive==false)
        {    
             isBuzzerActive=true;
            setBuzzerFlash(isBuzzerActive,&preferences);
            

        }else{
          
             isBuzzerActive=false;
             setBuzzerFlash(isBuzzerActive,&preferences);
             
        }
      
         delay(100);
        DisplayMenu(CHANGE_BOOST_LIMIT_WARNING);// update the changed value
        MenuButton.Joystick_center_FLAG=FLAG_RESET;//finished handling button
        
         delay(100);
      }else if(isMenuHlimitDsiplayed==true)
      {
          
                
                  MenuButton.IsMenuSubActive=true;
                   delay(100);
                 while(MenuButton.IsMenuSubActive==true)
                 {

                 
                   //enter change boost limit sub menu
                   if(isMenuSubLimitDisplayed==false)
                   {

                    DisplayMenu(SELECTED_BOOST_LIMIT);
                    isMenuSubLimitDisplayed=true;
                     delay(100);

                   }
                   MenuButton.Joystick_right_FLAG=FLAG_RESET;
                    delay(100);
                    MenuButton.Joystick_center_FLAG=FLAG_RESET;
   MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
                   
                   
                   while(!((MenuButton.Joystick_center_FLAG==FLAG_SET) || (MenuButton.Joystick_up_FLAG==FLAG_SET) || 
                   (MenuButton.Joystick_down_FLAG==FLAG_SET) || (MenuButton.Joystick_left_FLAG==FLAG_SET) || (MenuButton.Joystick_right_FLAG==FLAG_SET)) ){}; //wait for required input
                     delay(100);

                   if(MenuButton.Joystick_right_FLAG==FLAG_SET)
                   {       
                           MenuButton.Joystick_down_FLAG=FLAG_RESET;
                            MenuButton.Joystick_left_FLAG=FLAG_RESET;
     
                            MenuButton.Joystick_up_FLAG=FLAG_RESET;
                            MenuButton.Joystick_center_FLAG=FLAG_RESET;
                           MenuButton.Joystick_right_FLAG=FLAG_RESET;

                   }

                   if(MenuButton.Joystick_center_FLAG==FLAG_SET)
                   {   


                             MenuButton.Joystick_down_FLAG=FLAG_RESET;
                             MenuButton.Joystick_left_FLAG=FLAG_RESET;
                              MenuButton.Joystick_right_FLAG=FLAG_RESET;
                              MenuButton.Joystick_up_FLAG=FLAG_RESET;
      
                          DisplayMenu(HIGHLIGHT_BOOST_LIMIT);
                          MenuButton.Joystick_center_FLAG=FLAG_RESET;
                           delay(100);
                          MenuButton.IsMenuSubActive=false;
                          isMenuSubLimitDisplayed=false;
                           delay(100);
                         
                          break;
                          


                   }else if(MenuButton.Joystick_up_FLAG==FLAG_SET)
                   {
                                MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
     
      MenuButton.Joystick_center_FLAG=FLAG_RESET;

                            
                             if(Boost_params.boost_limit==30)     //update max value
                             {
                                    Boost_params.boost_limit=0;
                                    setBoostFlash(Boost_params.boost_limit,&preferences);

                             }else
                             {
                                    Boost_params.boost_limit++;
                                    setBoostFlash(Boost_params.boost_limit,&preferences);

                             }
                             DisplayMenu(CHANGE_BOOST_LIMIT);
                             MenuButton.Joystick_up_FLAG=FLAG_RESET;
                              delay(100);


                   }else if(MenuButton.Joystick_down_FLAG==FLAG_SET)
                   {

                   
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;
                         if(Boost_params.boost_limit==0)     //update max value
                             {
                                    Boost_params.boost_limit=30;
                                    setBoostFlash(Boost_params.boost_limit,&preferences);

                             }else
                             {
                                    Boost_params.boost_limit--;
                                    setBoostFlash(Boost_params.boost_limit,&preferences);

                             }
                             DisplayMenu(CHANGE_BOOST_LIMIT);
                             MenuButton.Joystick_down_FLAG=FLAG_RESET;
                              delay(100);


                   }else if(MenuButton.Joystick_left_FLAG==FLAG_SET)
                   {
                               MenuButton.Joystick_down_FLAG=FLAG_RESET;
     
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;
                          DisplayMenu(HIGHLIGHT_BOOST_LIMIT);
                          MenuButton.Joystick_left_FLAG=FLAG_RESET;
                           delay(100);
                          MenuButton.IsMenuSubActive=false;
                          isMenuSubLimitDisplayed=false;
                           delay(100);
                          break;

                   }
                 }



      }


    }else if((MenuButton.Joystick_down_FLAG==FLAG_SET)||(MenuButton.Joystick_up_FLAG==FLAG_SET))
    {
      
      if(MenuButton.Joystick_down_FLAG==FLAG_SET)
      {
      
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;
      }else{

       MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      
      MenuButton.Joystick_center_FLAG=FLAG_RESET;
      }

   
      if(isMenuHbuzzerDisplayed==true)
      {

   
        //change view to highlight limit option
        DisplayMenu(HIGHLIGHT_BOOST_LIMIT);
        isMenuHbuzzerDisplayed=false; //update state
        isMenuHlimitDsiplayed=true;

        if(MenuButton.Joystick_down_FLAG==FLAG_SET)
        {
          
          MenuButton.Joystick_down_FLAG=FLAG_RESET;
           delay(100);
        }else if(MenuButton.Joystick_up_FLAG==FLAG_SET)
        {  
         
          MenuButton.Joystick_up_FLAG=FLAG_RESET; //finished handling button
           delay(100);
        }
    
      }else if(isMenuHlimitDsiplayed==true)
      {
          
          //change view to highlight buzzer option
        DisplayMenu(HIGHLIGHT_BOOST_LIMIT_WARNING);
        isMenuHbuzzerDisplayed=true; //update state
        isMenuHlimitDsiplayed=false;

        if(MenuButton.Joystick_down_FLAG==FLAG_SET)
        {
          
          MenuButton.Joystick_down_FLAG=FLAG_RESET;
           delay(100);
        }else if(MenuButton.Joystick_up_FLAG==FLAG_SET)
        {
          
          MenuButton.Joystick_up_FLAG=FLAG_RESET; //finished handling button
           delay(100);
        }





      }

    }else if(MenuButton.Joystick_left_FLAG==FLAG_SET)
    {
       
       MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;


    
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
       delay(100);
       isMenuHbuzzerDisplayed=false;
       isMenuHlimitDsiplayed=false;
      MenuButton.IsMenuActive=false;
      
      DisplayMain();
       delay(100);
       break; //exit from the menu

    }else if(MenuButton.Joystick_right_FLAG==FLAG_SET)
    {      


       MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;

     
             if(isMenuHlimitDsiplayed==true)
             {    
                
                  MenuButton.IsMenuSubActive=true;
                   delay(100);
                 while(MenuButton.IsMenuSubActive==true)
                 {

                
                   //enter change boost limit sub menu
                   if(isMenuSubLimitDisplayed==false)
                   {

                    DisplayMenu(SELECTED_BOOST_LIMIT);
                    isMenuSubLimitDisplayed=true;
                     delay(100);

                   }
                   MenuButton.Joystick_right_FLAG=FLAG_RESET;
                    delay(100);
                  
                   
                   while(!((MenuButton.Joystick_center_FLAG==FLAG_SET) || (MenuButton.Joystick_up_FLAG==FLAG_SET) || 
                   (MenuButton.Joystick_down_FLAG==FLAG_SET) || (MenuButton.Joystick_left_FLAG==FLAG_SET) || (MenuButton.Joystick_right_FLAG==FLAG_SET)) ){}; //wait for required input
                     delay(100);

                   if(MenuButton.Joystick_right_FLAG==FLAG_SET)
                   {       
                           MenuButton.Joystick_down_FLAG=FLAG_RESET;
                            MenuButton.Joystick_left_FLAG=FLAG_RESET;
     
                            MenuButton.Joystick_up_FLAG=FLAG_RESET;
                            MenuButton.Joystick_center_FLAG=FLAG_RESET;
                           MenuButton.Joystick_right_FLAG=FLAG_RESET;

                   }

                   if(MenuButton.Joystick_center_FLAG==FLAG_SET)
                   {   


                             MenuButton.Joystick_down_FLAG=FLAG_RESET;
                             MenuButton.Joystick_left_FLAG=FLAG_RESET;
                              MenuButton.Joystick_right_FLAG=FLAG_RESET;
                              MenuButton.Joystick_up_FLAG=FLAG_RESET;
      
                          DisplayMenu(HIGHLIGHT_BOOST_LIMIT);
                          MenuButton.Joystick_center_FLAG=FLAG_RESET;
                           delay(100);
                          MenuButton.IsMenuSubActive=false;
                           isMenuSubLimitDisplayed=false;
                           delay(100);
                         
                          break;
                          


                   }else if(MenuButton.Joystick_up_FLAG==FLAG_SET)
                   {
                                MenuButton.Joystick_down_FLAG=FLAG_RESET;
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
     
      MenuButton.Joystick_center_FLAG=FLAG_RESET;

                            
                             if(Boost_params.boost_limit==30)     //update max value
                             {
                                    Boost_params.boost_limit=0;

                             }else
                             {
                                    Boost_params.boost_limit++;

                             }
                             DisplayMenu(CHANGE_BOOST_LIMIT);
                             MenuButton.Joystick_up_FLAG=FLAG_RESET;
                              delay(100);


                   }else if(MenuButton.Joystick_down_FLAG==FLAG_SET)
                   {

                   
      MenuButton.Joystick_left_FLAG=FLAG_RESET;
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;
                         if(Boost_params.boost_limit==0)     //update max value
                             {
                                    Boost_params.boost_limit=30;

                             }else
                             {
                                    Boost_params.boost_limit--;

                             }
                             DisplayMenu(CHANGE_BOOST_LIMIT);
                             MenuButton.Joystick_down_FLAG=FLAG_RESET;
                              delay(100);


                   }else if(MenuButton.Joystick_left_FLAG==FLAG_SET)
                   {
                               MenuButton.Joystick_down_FLAG=FLAG_RESET;
     
      MenuButton.Joystick_right_FLAG=FLAG_RESET;
      MenuButton.Joystick_up_FLAG=FLAG_RESET;
      MenuButton.Joystick_center_FLAG=FLAG_RESET;
                          DisplayMenu(HIGHLIGHT_BOOST_LIMIT);
                          MenuButton.Joystick_left_FLAG=FLAG_RESET;
                           delay(100);
                          MenuButton.IsMenuSubActive=false;
                           isMenuSubLimitDisplayed=false;
                           delay(100);
                          break;

                   }
                 }

             }
             
             else if(isMenuHbuzzerDisplayed==true)
      {
          
        //change boost limit buzzer config
        if(isBuzzerActive==false)
        {    
             isBuzzerActive=true;

        }else{
         
             isBuzzerActive=false;

        }
       
         delay(100);
        DisplayMenu(CHANGE_BOOST_LIMIT_WARNING);// update the changed value
        MenuButton.Joystick_right_FLAG=FLAG_RESET;//finished handling button
        
         delay(100);
            //no action
            
    }


    }






}

//normal boost calculation



if(Boost_params.Max_Boost_Reset_FLAG==1){

  ResetMaxBoost();
 Boost_params.Max_Boost_Reset_FLAG=0;
}


Boost_params.raw_data=ads.readADC_SingleEnded(0);

        
        Boost_params.curr_avg=(Boost_params.raw_data*Boost_params.alpha)+(Boost_params.last_avg*(1-Boost_params.alpha)); //EMA Filter

        Boost_params.psi_val=(0.005812905813487104*Boost_params.curr_avg)-10.508230464276787;   //raw datat to psi calculation
        if(Boost_params.psi_val<0)
        {

          Boost_params.psi_val=0;
        }
   
 
       
    if (Boost_params.psi_val>Boost_params.max_value)
      {
          //update max value
         Boost_params.max_value=Boost_params.psi_val;
          BoostDisplay.display.loadFont(psi_max_font36);
          if(Boost_params.max_value>Boost_params.boost_limit)
          {
            BoostDisplay.display.setTextColor(TFT_RED,TFT_BLACK);
          }
          
            BoostDisplay.display.setTextPadding(120);
            
            BoostDisplay.display.drawFloat(Boost_params.max_value,1,240,166);
          
          
           BoostDisplay.display.unloadFont();
           BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
      }

 
         if(Boost_params.psi_val>Boost_params.boost_limit)
          {
             BoostDisplay.display.setTextColor(TFT_RED,TFT_BLACK);
             if(!beep_on && (isBuzzerActive==true))
             {
                     beep_on=true;
                     buzzer_previous_time=millis();
                     analogWrite(BUZZER_PIN, 180);
                     digitalWrite(LED_WARNING_PIN,HIGH);

                    
             }



             }


             if(beep_on && (  (millis()- buzzer_previous_time) >= BuzzerBeepDuration )  && (isBuzzerActive==true)  ){
                    analogWrite(BUZZER_PIN, 0);
                    digitalWrite(LED_WARNING_PIN,LOW);
                    beep_on=false;
                    buzzer_previous_time=millis();


             }

             if(  (!beep_on && ( (millis() - buzzer_previous_time  ) >= BuzzerGapDuration )  ) && (Boost_params.psi_val>Boost_params.boost_limit) && (isBuzzerActive==true)) {
                    beep_on=true;
                    buzzer_previous_time=millis();
                    analogWrite(BUZZER_PIN, 180);
                    digitalWrite(LED_WARNING_PIN,HIGH);


             }
          
           BoostDisplay.display.loadFont(psi_font59);
          BoostDisplay.display.setTextPadding(120);
          
          BoostDisplay.display.drawFloat(Boost_params.psi_val,1,105,150);
         Fill_bar(&BoostDisplay,&Boost_params);
           

         
          BoostDisplay.display.unloadFont();
          BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);

         Boost_params.psi_val_prev=Boost_params.psi_val;
         Boost_params.last_avg=Boost_params.curr_avg;








}


void Boost_Gauge::ResetMaxBoost (void){

  Boost_params.max_value=0;
  BoostDisplay.display.loadFont(psi_max_font36);
  BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
  BoostDisplay.display.setTextPadding(120);
  BoostDisplay.display.drawFloat(Boost_params.max_value,1,240,166);
  BoostDisplay.display.unloadFont();



}

 bool Boost_Gauge:: WaitForButtonPress(void)
{
   
   if(MenuButton.Joystick_center_FLAG==FLAG_SET || MenuButton.Joystick_left_FLAG==FLAG_SET || 
   MenuButton.Joystick_right_FLAG==FLAG_SET || MenuButton.Joystick_up_FLAG==FLAG_SET || 
   MenuButton.Joystick_down_FLAG==FLAG_SET)
   {

       return true; //button is pressed

   }else{

     return false;//button is not pressed

   }




}

static void populate_bar_struct(Bar_pos_struct* bar,uint8_t param)
{

if(param==DRAW_BAR_PARAM)
{
  
  bar[0].y=RECT_Y;
  bar[1].y=RECT_Y-20;
  bar[2].y=RECT_Y-40;
  bar[3].y=RECT_Y-56;
  bar[4].y=RECT_Y-64;

  for(int i=0;i<15;i++)
  {
          if(i>=5)
            {
               bar[i].y=RECT_Y-69;

            }

       bar[i].x=RECT_X+(i*RECT_WIDTH);
       bar[i].width=RECT_WIDTH;
       bar[i].height=RECT_HEIGHT;
  }

}else if(param==FILL_BAR_PARAM)
{
  bar[0].y=RECT_Y+1;
  bar[1].y=RECT_Y-19;
  bar[2].y=RECT_Y-39;
  bar[3].y=RECT_Y-55;
  bar[4].y=RECT_Y-63;
  
  for(int i=0;i<15;i++)
    {
          if(i>=5)
            {
               bar[i].y=RECT_Y-68;

            }

       bar[i].x=RECT_X+(i*RECT_WIDTH)+1;
       bar[i].width=RECT_WIDTH-1;
       bar[i].height=RECT_HEIGHT-1;
    }

  }

}


static void Draw_bar(Boost_display* BoostDisplay,Boost_calculation* BoostParam)
{

BoostDisplay->sprite.loadFont(top_font_12);
  BoostDisplay->display.loadFont(NotoSansBold15);

   for(int i=0;i<15;i++)
   {

         BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);


   }


   BoostDisplay->sprite.setTextSize(1);
   BoostDisplay->sprite.drawNumber(0,DIGIT_X-9,(DIGIT_Y+RECT_HEIGHT+18));
   BoostDisplay->sprite.drawNumber(2,DIGIT_X,DIGIT_Y);
   BoostDisplay->sprite.drawNumber(4,DIGIT_X+(RECT_WIDTH),DIGIT_Y-20);//20
   BoostDisplay->sprite.drawNumber(6,DIGIT_X+(2*RECT_WIDTH),DIGIT_Y-40);//40
   BoostDisplay->sprite.drawNumber(8,(DIGIT_X)+(3*RECT_WIDTH),DIGIT_Y-56);//56
   BoostDisplay->sprite.drawNumber(10,DIGIT_X+(4*RECT_WIDTH)-5,DIGIT_Y-64);//64

   //9 more

   for(int j=0;j<10;j++)
   {
       BoostDisplay->sprite.drawNumber( ( (2*j)+12) ,( ( (DIGIT_X-4) )+( ((j)+5)*RECT_WIDTH) ),DIGIT_Y-69);


   }
    BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
   //display.setTextSize(2);
   BoostDisplay->display.setTextColor(TFT_CYAN,TFT_BLACK);
   BoostDisplay->display.drawString("psi",130,210);

   BoostDisplay->display.drawString("psi max",239,210);
    
   BoostDisplay->display.setTextColor(TFT_WHITE,TFT_BLACK);
   BoostDisplay->display.setTextPadding(120);

    BoostDisplay->display.unloadFont();
     BoostDisplay->display.loadFont(psi_max_font36);
     
   BoostDisplay->display.drawFloat(BoostParam->max_value,1,240,166);
          
          
           BoostDisplay->display.unloadFont();
           

}



static void Fill_bar(Boost_display* BoostDisplay,Boost_calculation* BoostParams)
{
if(BoostParams->psi_val>BoostParams->boost_limit)
{
   //display overboost
   BoostDisplay->sprite.setTextColor(TFT_RED,TFT_BLACK);
   BoostDisplay->sprite.loadFont(NotoSansBold15);
   BoostDisplay->sprite.drawString("Overboost",230,93);
   BoostDisplay->sprite.unloadFont();

}else{

  BoostDisplay->sprite.fillRect(230, 93, 90, 20, TFT_BLACK);

}

//3 conditions, current value can be equal to, greater than or less than previous value

if(BoostParams->psi_val>BoostParams->psi_val_prev)
{
 //increasing

  if( BoostParams->psi_val >= 30)
  {

     for(int i=0;i<15;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width,BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val >=28)
  {
     //fil upto 28 psi bar incl
      for(int i=0;i<14;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width,BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

     BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=26)
  {
    for(int i=0;i<13;i++)
     {
         BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
    
    BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=24)
  {
      for(int i=0;i<12;i++)
     {

         BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

    BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if (BoostParams->psi_val >= 22)
  {
      for(int i=0;i<11;i++)
     {
          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

    BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
  }else if(BoostParams->psi_val>= 20)
  {

 for(int i=0;i<10;i++)
     {
          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
    BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
  }else if(BoostParams->psi_val >= 18)
  {
      for(int i=0;i<9;i++)
     {
          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=16)
  {
      for(int i=0;i<8;i++)
     {
          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=14)
  {
 for(int i=0;i<7;i++)
     {
          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
   BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=12)
  {

     for(int i=0;i<6;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
//9 more
BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
   
  }else if(BoostParams->psi_val>=10)
  {
     for(int i=0;i<5;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
  }else if(BoostParams->psi_val>=8)
  {
for(int i=0;i<4;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
  
  }else if(BoostParams->psi_val>=6)
  {
for(int i=0;i<3;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=4)
  {
for(int i=0;i<2;i++)
     {

          BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

  }else if(BoostParams->psi_val>=2)
  {

    for(int i=0;i<1;i++)
     {

         BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_BLACK);
          BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, BoostDisplay->bar_color[i]);

     }
    BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
 
  }

}else if(BoostParams->psi_val<BoostParams->psi_val_prev)
{
//decreasing
  if(BoostParams->psi_val<2)
  {

    for(int i=0;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }
//delete all bars
BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
  }else if(BoostParams->psi_val<4)
{
   
 for(int i=1;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<6)
{

   for(int i=2;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);

}else if(BoostParams->psi_val<8)
{

   for(int i=3;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<10)
{

  for(int i=4;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<12)

{

    for(int i=5;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<14)
{

 for(int i=6;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<16)
{
 for(int i=7;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<18)
{
 for(int i=8;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<20)
{
 for(int i=9;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }
  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<22)
{

 for(int i=10;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }
  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<24)

{
 for(int i=11;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }
  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<26)
{
 for(int i=12;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }

  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<28)
{
 for(int i=13;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
       BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }
  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}else if(BoostParams->psi_val<30)
{
 for(int i=14;i<15;i++)
    {
       BoostDisplay->sprite.fillRect(BoostDisplay->fill_bar_pos[i].x, BoostDisplay->fill_bar_pos[i].y, BoostDisplay->fill_bar_pos[i].width, BoostDisplay->fill_bar_pos[i].height, TFT_BLACK);
      BoostDisplay->sprite.drawRect(BoostDisplay->draw_bar_pos[i].x, BoostDisplay->draw_bar_pos[i].y, BoostDisplay->draw_bar_pos[i].width, BoostDisplay->draw_bar_pos[i].height, TFT_WHITE);

    }
  BoostDisplay->sprite.pushSprite(0,0,TFT_PURPLE);
}


}


}






