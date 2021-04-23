#include <math.h>
int LDR=36;

//eficiencia luminosa em lumens por wat da luz solar https://en.wikipedia.org/wiki/Luminous_efficacy
int l_efficacy = 93;

//tamanho da placa em metros quadrados, neste caso de exemplo estou usando 1, mas poderia ser 2, 3...
int metros2 = 1;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

}

void loop() {
  // Para calcular o Lux:
  int vout1 = analogRead(LDR);
  float vout = vout1/204.6;
  float R = (11000-vout*2200)/vout;
  float lux = (pow(R, (1/-0.8616)))/(pow( 10, (5.118/-0.8616)));
  Serial.println("Lux f1: ");
  Serial.println(lux);

  float lux2 = 65.9 * (pow( vout1, 0.352));
  Serial.println("Lux fdois: ");
  Serial.println(lux2);
  delay(2000);

  //agora, convertemos de Lux para watts:
  //https://www.rapidtables.com/calc/light/lux-to-watt-calculator.html

  //na formula eu usei o lux 2 pois os valores obtidos nele são mais coerentes 
  //com o que eu vi na internet.
  float p_wat = ((lux2*metros2)/l_efficacy);
  Serial.println ("Valor em watts obtidos: ");
  Serial.println (p_wat);
  delay(2000);
}

// site que eu usei para calcular o Lux:
//https://www.instructables.com/Measuring-Light-Using-Light-Sensor/
//outros sites que eu tentei usar, mas não deram certo:
//https://invootech.blogspot.com/2017/06/how-to-convert-ldr-dependent-resistor.html
//https://arduinodiy.wordpress.com/2013/11/03/measuring-light-with-an-arduino/
