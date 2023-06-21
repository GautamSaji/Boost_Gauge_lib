//program menu to change boost_limit
//create interrupt program to clear psi_max
//user configurable brightness control
//move to ads1115


#include <TFT_eSPI.h>
#include <spi.h>
#include <Boost_Gauge.h>

bool demo_mode=false;
//Create boost gauge object
Boost_Gauge BoostGauge=Boost_Gauge();

void setup() {

BoostGauge.init();
BoostGauge.DisplaySplashScreen(3);
BoostGauge.DisplayMain();
BoostGauge.Test();
 
}

void loop() {
  
if(demo_mode==false){

      BoostGauge.DisplayBoost();

 }else{


       BoostGauge.Test();
      
     }


}
