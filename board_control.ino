#include <MsTimer2.h> 

#include <OneWire.h> 

#include <Servo.h> 

#include<SoftwareSerial.h> 

 

//Bluetooth pins// 

#define BLE_TxD 6 

#define BLE_RxD 5 

#define BLE_Baud 9600 

 

//GPS pins 

 

//ESC Pins 

#define esc_left_Pin 4

#define esc_right_Pin 8 

 
 

// BLE Setup 

SoftwareSerial bluetooth_Serial(BLE_RxD, BLE_TxD); 

 

//ESC Setup 

Servo esc_left;   

Servo esc_right; 

 


double intData[2]; 

 

int ind1; 

int ind2; 

String  thrust_left_ccw; 

String  thrust_right_cw; 

 



int minPulseRate = 768; 

int maxPulseRate = 2400; 

 

int safety_value =  5; 

 

int stop_RC_Puls = 1060 + safety_value; 

int full_RC_Puls = 1860 - safety_value; 

 

int throttleChangeDelay = 100; 

 

int left_initial_vel = 0; 

int right_initial_vel = 0; 

int new_left_initial_vel = 0; 

int new_right_initial_vel = 0; 

 



float weight_CW = 1; 

float weight_CCW = 1; 

 



int normalizeThrottle(int value) { 

  if( value < -100 ) 

    return -100; 

  if( value > 100 ) 

    return 100; 

  return value; 

} 

 

 

void setup() { 

   

  Serial.begin(9600); 

  Serial.setTimeout(50); 

  //BLE Serial 

  bluetooth_Serial.begin(BLE_Baud); 

  bluetooth_Serial.setTimeout(50); 

   

  esc_left.attach(esc_left_Pin,minPulseRate,maxPulseRate); 

  esc_right.attach(esc_right_Pin,minPulseRate,maxPulseRate); 

     

  esc_left.write(map(0,-100,100,full_RC_Puls,stop_RC_Puls)); 

  esc_right.write(map(0,-100,100,stop_RC_Puls,full_RC_Puls)); 

} 

 

void loop() {   

  if(bluetooth_Serial.available()>0 ) 

  {  

 

    String thrust_value; 

    thrust_value = bluetooth_Serial.readString();    



    ind1 = thrust_value.indexOf(';');  

    thrust_left_ccw = thrust_value.substring(0, ind1); 

    ind2 = thrust_value.indexOf(';',ind1+1); 

    thrust_right_cw = thrust_value.substring(ind1+1); 

     


    new_left_initial_vel = thrust_left_ccw.toInt(); 

    left_initial_vel = normalizeThrottle(new_left_initial_vel); 

    left_initial_vel = map(left_initial_vel,-100,100,full_RC_Puls,stop_RC_Puls); 



    new_right_initial_vel = thrust_right_cw.toInt(); 

    right_initial_vel = normalizeThrottle(new_right_initial_vel); 

    right_initial_vel = map(right_initial_vel,-100,100,stop_RC_Puls,full_RC_Puls); 

 
    esc_left.write((int)wight_CCW*left_initial_vel);       
    esc_right.write((int)wight_CW*right_initial_vel); 


  } 

} 