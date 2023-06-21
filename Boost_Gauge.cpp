#include "Boost_Gauge.h"


static void populate_bar_struct(Bar_pos_struct* bar,uint8_t param); //helper function to fill the bars inside boost gauge obj
static void Draw_bar(Boost_display* BoostDisplay);  //helper function to draw empty bars
static void Fill_bar(Boost_display* BoostDisplay,Boost_calculation* BoostParams);  //helper function to fill bars according to psi



void Boost_Gauge::init (void){

//initialize external adc
ads.begin();
//

//display init
BoostDisplay.display.init();
//

//brightness control
 ledcSetup(BoostDisplay.backlight_Channel, BoostDisplay.backlight_freq, BoostDisplay.backlight_resolution);
 ledcAttachPin(TFT_BL, BoostDisplay.backlight_Channel);
 ledcWrite(BoostDisplay.backlight_Channel, LCD_BRIGHTNESS);
 //

 //fill bar pos structs
 populate_bar_struct(BoostDisplay.draw_bar_pos,DRAW_BAR_PARAM);
 populate_bar_struct(BoostDisplay.fill_bar_pos,FILL_BAR_PARAM);

}



void Boost_Gauge::DisplaySplashScreen(uint8_t seconds){

//display splash screen
 BoostDisplay.display.setRotation(2);
 BoostDisplay.display.setSwapBytes(true);
 BoostDisplay.display.fillScreen(TFT_WHITE);
 BoostDisplay.display.pushImage(0,17,240,286,suzuki);
 delay(seconds*1000);
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
  

   Draw_bar(&BoostDisplay);
 //

   BoostDisplay.display.unloadFont();

}

void Boost_Gauge::Test(void){




BoostDisplay.display.setTextPadding(120);
BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);

 for(int i=0;i<15;i++)
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
          }
          BoostDisplay.display.loadFont(psi_font59);
         
         BoostDisplay.display.drawFloat(Boost_params.psi_val,1,105,150);
         
           

         
         BoostDisplay.display.unloadFont();
         BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
         Boost_params.psi_val=Boost_params.psi_val+2;



 }

  for(int j=0;j<15;j++)
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


void Boost_Gauge::DisplayBoost(void){

Boost_params.raw_data=ads.readADC_SingleEnded(0);

        
        Boost_params.curr_avg=(Boost_params.raw_data*Boost_params.alpha)+(Boost_params.last_avg*(1-Boost_params.alpha)); //EMA Filter

        Boost_params.psi_val=(0.005812905813487104*Boost_params.curr_avg)-10.463230464276787;   //raw datat to psi calculation
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
          
            BoostDisplay.display.drawFloat(Boost_params.max_value,1,240,166);
          
          
           BoostDisplay.display.unloadFont();
           BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);
      }

 
         if(Boost_params.psi_val>Boost_params.boost_limit)
          {
             BoostDisplay.display.setTextColor(TFT_RED,TFT_BLACK);
          }
           BoostDisplay.display.loadFont(psi_font59);
         
          BoostDisplay.display.drawFloat(Boost_params.psi_val,1,105,150);
         Fill_bar(&BoostDisplay,&Boost_params);
           

         
          BoostDisplay.display.unloadFont();
          BoostDisplay.display.setTextColor(TFT_WHITE,TFT_BLACK);

         Boost_params.psi_val_prev=Boost_params.psi_val;
         Boost_params.last_avg=Boost_params.curr_avg;








}

void populate_bar_struct(Bar_pos_struct* bar,uint8_t param)
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


void Draw_bar(Boost_display* BoostDisplay)
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
   BoostDisplay->display.setTextPadding(40);



}



void Fill_bar(Boost_display* BoostDisplay,Boost_calculation* BoostParams)
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






