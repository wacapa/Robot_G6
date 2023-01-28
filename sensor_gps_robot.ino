#include <MsTimer2.h> 

 

#include <OneWire.h> 

#include <Servo.h> 

 

#include <TinyGPS++.h> 

#include<SoftwareSerial.h> 

 

//Pines bluetooth

#define TXD_blu 6 

#define RXD_blu 5 

#define baudios_blu 115200 

 

//Pines GPS 

#define GPS_TXD 11 

#define GPS_RXD 12 

#define baudios_gps 4800 

 

//Pines para los sensores

#define Temperature_sensor A0 

#define Turbidity_sensor A1 

#define PH_sensor A2 

#define sensor_voltaje_board A3 

#define sensor_amperaje_board A4 

#define sensor_voltaje_esc A5 

#define sensor_amperaje_esc A6 



TinyGPSPlus gps; 

SoftwareSerial gps_Serial(GPS_RXD, GPS_TXD); 

SoftwareSerial bluetooth_Serial(RXD_blu, TXD_blu); 


int DS18S20_Pin = A0; 

OneWire ds(DS18S20_Pin);  


float sensor_data[9]; 

 

//input data 

double intData[2]; 

 

int ind1; 

int ind2; 

 
//Variables para los sensores
float turbidity_sensor_data; 

float ph_sensor_data; 

float temperature_sensor_data; 

float sensor_vol_board_datos; 

float sensor_amp_board_datos; 

float sensor_vol_esc_datos; 

float sensor_amp_esc_datos; 

 

float gps_log; 

float gps_lng; 

bool  gps_mode=false; 

 

//ESC variables  

int pulso_min = 768; 

int pulso_max = 2400; 

 

int valorE =  5; 

 
float getTemp(){ 


  byte data[12]; 

  byte addr[8]; 

 

  if ( !ds.search(addr)) { 


      ds.reset_search(); 

      return -1000; 

  } 

 

  if ( OneWire::crc8( addr, 7) != addr[7]) { 

  

      return -1000; 

  } 

 

  if ( addr[0] != 0x10 && addr[0] != 0x28) { 

   

      return -1000; 

  } 

 

  ds.reset(); 

  ds.select(addr); 

  ds.write(0x44,1);

 

  byte present = ds.reset(); 

  ds.select(addr);     

  ds.write(0xBE); 

 

   

  for (int i = 0; i < 9; i++) { 

    data[i] = ds.read(); 

  } 

   

  ds.reset_search(); 

   

  byte MSB = data[1]; 

  byte LSB = data[0]; 

 

  float tempRead = ((MSB << 8) | LSB); 

  float TemperatureSum = tempRead / 16; 

   

  return TemperatureSum; 

   

} 

 

 
//función para leer sensores
void sensor_data_reading() 

{


  turbidity_sensor_data = analogRead(Turbidity_sensor)/1024.0*5.0; 

  if(turbidity_sensor_data >= 2.5) 

  { 



    turbidity_sensor_data=-1120.04*turbidity_sensor_data*turbidity_sensor_data+5742.3*turbidity_sensor_data-4352.9; 

  } 

  else  

  { 

    turbidity_sensor_data = 9999; 

  } 

   

  ph_sensor_data = analogRead(PH_sensor)/1024.0*5.0*3.5; 

  temperature_sensor_data = getTemp(); 

  sensor_vol_board_datos = analogRead(sensor_voltaje_board)/12.99; 

  sensor_vol_esc_datos = analogRead(sensor_voltaje_esc)/12.99; 


 

  sensor_data[0] = gps_log; // GPS_log 

  sensor_data[1] = gps_lng; // GPS_lat 

  sensor_data[2] = temperature_sensor_data; 

  sensor_data[3] = turbidity_sensor_data;  

  sensor_data[4] = ph_sensor_data;  

  sensor_data[5] = sensor_vol_board_datos; 

  sensor_data[6] = sensor_amp_board_datos; 

  sensor_data[7] = sensor_vol_esc_datos; 

  sensor_data[8] = sensor_amp_esc_datos;  

 

  bluetooth_Serial.print(sensor_data[0],6); bluetooth_Serial.print(F(";")); // GPS_log 

  bluetooth_Serial.print(sensor_data[1],6); bluetooth_Serial.print(F(";")); // GPS_la 

  bluetooth_Serial.print(sensor_data[2]); bluetooth_Serial.print(F(";")); // Temperature 

  bluetooth_Serial.print(sensor_data[3]); bluetooth_Serial.print(F(";")); // Turbidity_data 

  bluetooth_Serial.print(sensor_data[4]); bluetooth_Serial.print(F(";")); // PH_sensor_data 

  bluetooth_Serial.print(sensor_data[5]); bluetooth_Serial.print(F(";")); 

  bluetooth_Serial.print(sensor_data[7]); bluetooth_Serial.print(F(";"));  



  bluetooth_Serial.print("\n"); 


} 

int normalizeThrottle(int value) { 

  if( value < -100 ) 

    return -100; 

  if( value > 100 ) 

    return 100; 

  return value; 

} 

 

void setup() { 


  //Serial para bluetooth
  Serial.begin(9600); 

  Serial.setTimeout(50); 

 

  bluetooth_Serial.begin(baudios_blu); 

  bluetooth_Serial.setTimeout(50); 

 


  gps_Serial.begin(baudios_gps); 

  gps_Serial.setTimeout(100); 


 //Input para sensores 
 pinMode(Temperature_sensor,INPUT); 

  pinMode(Turbidity_sensor,INPUT); 

  pinMode(PH_sensor,INPUT); 

  pinMode(sensor_voltaje_board,INPUT); 

  pinMode(sensor_amperaje_board,INPUT); 

  pinMode(sensor_voltaje_esc,INPUT); 

  pinMode(sensor_amperaje_esc,INPUT); 



  MsTimer2::set(500, sensor_data_reading); // 100ms period 

  MsTimer2::start(); 

 

} 

 

void loop() {   

    if(gps_Serial.available() > 0){ 

      if (gps.encode(gps_Serial.read())){ 

        if (gps.location.isValid()) 

        { 

          gps_log = gps.location.lat(); 

          gps_lng = gps.location.lng(); 


        } 

        else 

        { 

          gps_log = 99.999999; 

          gps_lng = 99.999999; 


        } 

      } 

    } 

} 
