#include <MsTimer2.h> 

#include <OneWire.h> 

#include <Servo.h> 

#include<SoftwareSerial.h> 

 

// Pines para comunicación bluetooth

#define TXD_blu 6 

#define RXD_blu 5 

#define Baudios_blu 9600 

 


//Pines para módulo ESC

#define esc_der 4

#define esc_iz 8 

 
 


SoftwareSerial bluetooth_Serial(RXD_blu, TXD_blu); 



Servo esc_izquierdo;   

Servo esc_derecho; 

 


double intData[2]; 

 

int ind1; 

int ind2; 

String  giro_iz; 

String  giro_der; 

 

//Pulsos minimo y máximo 

int pulsomin = 768; 

int pulsomax = 2400; 

 

int valorE =  5; 

 
//rango para pulso de paro y limite maximo encendido
int pararPulso = 1060 + valorE; 

int pulsoLimite = 1860 - valorE; 

 
 
//variables para velocidad de los propulsores 
int vel_iz = 0; 

int vel_der = 0; 

int vel_iz_nuevo = 0; 

int vel_der_nuevo = 0; 

 
float peso1 = 1; 

float peso2 = 1; 

 


//funcion de normalizacion
int normalizeThrottle(int value) { 

  if( value < -100 ) 

    return -100; 

  if( value > 100 ) 

    return 100; 

  return value; 

} 

 

 

void setup() { 

   
  //Inicilización de las variables en void setup()
  Serial.begin(9600); 

  Serial.setTimeout(50); 

  //BLE Serial 

  bluetooth_Serial.begin(Baudios_blu); 

  bluetooth_Serial.setTimeout(50); 

   
  //control de modulos esc con pulso minimo y maximo
  esc_izquierdo.attach(esc_der,pulsomin,pulsomax); 

  esc_derecho.attach(esc_iz,pulsomin,pulsomax); 

     
  //escritura de los pulsos 
  esc_izquierdo.write(map(0,-100,100,pulsoLimite,pararPulso)); 

  esc_derecho.write(map(0,-100,100,pararPulso,pulsoLimite)); 

} 

 

void loop() {   

  if(bluetooth_Serial.available()>0 ) 

  {  

 
    //inicialiazión de variables 
    String valorA; 

    valorA = bluetooth_Serial.readString();    



    ind1 = valorA.indexOf(';');  

    giro_iz = valorA.substring(0, ind1); 

    ind2 = valorA.indexOf(';',ind1+1); 

    giro_der = valorA.substring(ind1+1); 

     

    //control propulsor izquierdo
    vel_iz_nuevo = giro_iz.toInt(); 

    vel_iz = normalizeThrottle(vel_iz_nuevo); 

    vel_iz = map(vel_iz,-100,100,pulsoLimite,pararPulso); 



   //control propulsor derecho 
   vel_der_nuevo = giro_der.toInt(); 

    vel_der = normalizeThrottle(vel_der_nuevo); 

    vel_der = map(vel_der,-100,100,pararPulso,pulsoLimite); 

 
    esc_izquierdo.write((int)peso1*vel_iz);       
    esc_derecho.write((int)peso2*vel_der); 


  } 

} 
