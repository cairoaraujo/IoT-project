#include <math.h>
int LDR=36;
int TEMP=34;
int leituraADC;
float temperatura;

//eficiencia luminosa em lumens por wat da luz solar https://en.wikipedia.org/wiki/Luminous_efficacy
int l_efficacy = 93;

//eficiencia ou ineficiencia gerada pela variação de temperatura.
float t_efficacy;

//tamanho da placa em metros quadrados, neste caso de exemplo estou usando 2, mas poderia ser 1, 3...
int metros2 = 2;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
analogSetPinAttenuation(TEMP, ADC_0db);

}

void loop() {
  float voltage;
 
  
  leituraADC=analogRead(TEMP);
  temperatura = leituraADC*0.02686203;
  Serial.print("Temperatura é: ");
  Serial.print(temperatura);
  Serial.println(" C");
  delay (1000);

  
  if(temperatura >= 55) {
    t_efficacy = -0.6; 
    }
  if(temperatura >=45 && temperatura < 55) {
    t_efficacy = -0.3;
    }
  if(temperatura >=35 && temperatura < 45) {
    t_efficacy = 0;
    }
  if(temperatura >=25 && temperatura < 35) {
    t_efficacy = 0.3;
    }
  if(temperatura < 25) {
    t_efficacy = 0.6;
    }
  
  
  // Para calcular o Lux:
  int vout1 = analogRead(LDR);
  float vout = vout1/204.6;
  float R = (11000-vout*2200)/vout;
  float lux = (pow(R, (1/-0.8616)))/(pow( 10, (5.118/-0.8616)));
  //Serial.println("Lux f1: ");
  //Serial.println(lux);

  float lux2 = 65.9 * (pow( vout1, 0.352));
  Serial.println("Lux fdois: ");
  Serial.println(lux2);
  delay(2000);

  //agora, convertemos de Lux para watts:
 
  
  //na formula eu usei o lux 2 pois os valores obtidos nele são mais coerentes 
  //com o que eu vi na internet.
  float p_wat = ((lux2*metros2)/l_efficacy);
  Serial.println ("Valor em watts obtidos: ");
  Serial.println (p_wat);
  delay(2000);


  //esse caso, considera o valor lux muito baixo, ou seja, está muito escuro
  //incapaz de gerar energia considerável.
  if(lux2<600){
    voltage = 0;
    } else {
  voltage = ((p_wat*38)/600) + t_efficacy;
    }
  Serial.println("=====================");
  Serial.print("VOLTAGEM: ");
  Serial.println(voltage);
  Serial.print("EFICIENCIA DA TEMP: ");
  Serial.println(t_efficacy);
  Serial.println("=====================");
    
}

