#include <MsTimer2.h> 

 

#include <OneWire.h> 

#include <Servo.h> 

 

#include <TinyGPS++.h> 

#include<SoftwareSerial.h> 

 

//Bluetooth pins// 

#define BLE_TxD 6 

#define BLE_RxD 5 

#define BLE_Baud 115200 

 

//GPS pins 

#define GPS_TxD 11 

#define GPS_RxD 12 

#define GPS_Baud 4800 

 

//Water sensor Pins 

#define Temperature_sensor A0 

#define Turbidity_sensor A1 

#define PH_sensor A2 

#define Battery_Voltage_sensor_for_board A3 

#define Battery_Ampere_sensor_for_board A4 

#define Battery_Voltage_sensor_for_esc A5 

#define Battery_Ampere_sensor_for_esc A6 

 



 



 

//GPS Setup 

TinyGPSPlus gps; 

SoftwareSerial gps_Serial(GPS_RxD, GPS_TxD); 

 

// BLE Setup 

SoftwareSerial bluetooth_Serial(BLE_RxD, BLE_TxD); 

 

 //Temperature chip i/o 

int DS18S20_Pin = A0; 

OneWire ds(DS18S20_Pin);  

 

// Global variables 

//[GPS_lat,GPS_lng, sensor_1,sensor_2,sensor_3] 

float sensor_data[9]; 

 

//input data 

double intData[2]; 

 

int ind1; // , locations 

int ind2; 

 

float turbidity_sensor_data; 

float ph_sensor_data; 

float temperature_sensor_data; 

float battery_voltage_sensor_data_for_board; 

float battery_ampere_sensor_data_for_board; 

float battery_voltage_sensor_data_for_esc; 

float battery_ampere_sensor_data_for_esc; 

 

float gps_log; 

float gps_lng; 

bool  gps_mode=false; 

 

//ESC::variables  

int minPulseRate = 768; 

int maxPulseRate = 2400; 

 

int safety_value =  5; 

 

 

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

  battery_voltage_sensor_data_for_board = analogRead(Battery_Voltage_sensor_for_board)/12.99; 

  battery_voltage_sensor_data_for_esc = analogRead(Battery_Voltage_sensor_for_esc)/12.99; 


 

  sensor_data[0] = gps_log; // GPS_log 

  sensor_data[1] = gps_lng; // GPS_lat 

  sensor_data[2] = temperature_sensor_data; // Temperature  

  sensor_data[3] = turbidity_sensor_data; // Turbidity_data 

  sensor_data[4] = ph_sensor_data; // PH_sensor_data 

  sensor_data[5] = battery_voltage_sensor_data_for_board; // voltage_sensor_data 

  sensor_data[6] = battery_ampere_sensor_data_for_board; // ampere_sensor_data 

  sensor_data[7] = battery_voltage_sensor_data_for_esc; 

  sensor_data[8] = battery_ampere_sensor_data_for_esc;  

 

  bluetooth_Serial.print(sensor_data[0],6); bluetooth_Serial.print(F(";")); // GPS_log 

  bluetooth_Serial.print(sensor_data[1],6); bluetooth_Serial.print(F(";")); // GPS_la 

  bluetooth_Serial.print(sensor_data[2]); bluetooth_Serial.print(F(";")); // Temperature 

  bluetooth_Serial.print(sensor_data[3]); bluetooth_Serial.print(F(";")); // Turbidity_data 

  bluetooth_Serial.print(sensor_data[4]); bluetooth_Serial.print(F(";")); // PH_sensor_data 

  bluetooth_Serial.print(sensor_data[5]); bluetooth_Serial.print(F(";")); // voltage_sensor_data_for_board 

  bluetooth_Serial.print(sensor_data[7]); bluetooth_Serial.print(F(";")); // voltage_sensor_data_for_esc 



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

   



  Serial.begin(9600); 

  Serial.setTimeout(50); 

 

  //BLE Serial 

  bluetooth_Serial.begin(BLE_Baud); 

  bluetooth_Serial.setTimeout(50); 

 


  gps_Serial.begin(GPS_Baud); 

  gps_Serial.setTimeout(100); 



 



  pinMode(Temperature_sensor,INPUT); 

  pinMode(Turbidity_sensor,INPUT); 

  pinMode(PH_sensor,INPUT); 

  pinMode(Battery_Voltage_sensor_for_board,INPUT); 

  pinMode(Battery_Ampere_sensor_for_board,INPUT); 

  pinMode(Battery_Voltage_sensor_for_esc,INPUT); 

  pinMode(Battery_Ampere_sensor_for_esc,INPUT); 



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