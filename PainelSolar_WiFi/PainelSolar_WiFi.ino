#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// definições: IO Adafruit
#define IO_USERNAME  "alicebarros" // <----------------- Quando forem testar, mudem para o de vocês
#define IO_KEY       "aio_TCSn74Hvyx5P72KNKch668S682hr"  // <--------------- Quando forem testar, mudem para o de vocês

// Definições das Publicações no IO Adafruit:
#define PUBLISH_TEMP      "alicebarros/feeds/projeto.temp" // <----------------- Quando forem testar, mudem para o de vocês
#define PUBLISH_LDR       "alicebarros/feeds/projeto.ldr"  // <----------------- Quando forem testar, mudem para o de vocês
#define PUBLISH_EFFICACY  "alicebarros/feeds/projeto.temp-efficacy" // <----------------- Quando forem testar, mudem para o de vocês
#define PUBLISH_VOLTAGE   "alicebarros/feeds/projeto.voltage" // <----------------- Quando forem testar, mudem para o de vocês

// Atualizar para os valores de acordo com sua rede WiFi.
const char* ssid = "Casa";  // <----------------- Quando forem testar, mudem para o de vocês
const char* password = "12345678"; // <-----------Quando forem testar, mudem para o de vocês

// Para conectar ao IO Adafruit.
const char* mqttServer    = "io.adafruit.com";
const char* mqttUser      = IO_USERNAME;
const char* mqttPassword  = IO_KEY;
const int mqttPort        = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// Entradas Analógicas e Variáveis:
int LDR = 36;
int TEMP = 34;
int leituraADC;
float temperatura;

//eficiencia luminosa em lumens por wat da luz solar https://en.wikipedia.org/wiki/Luminous_efficacy
int l_efficacy = 93;

//eficiencia ou ineficiencia gerada pela variação de temperatura.
float t_efficacy;

//tamanho da placa em metros quadrados, neste caso de exemplo estou usando 2, mas poderia ser 1, 3...
int metros2 = 2;

//================================ wifi_setup() ================================
void wifi_setup(){
  delay(10);
  
  // Iniciando a conexão à rede WiFi.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

// ================================== Reconnect ====================================
void reconnect() {
  // Laço até que haja a reconexão..
  while (!client.connected()) {
    Serial.print("Tentando uma conexão MQTT: ");
    Serial.println(mqttServer);

    // Criando um ID de cliente aleatório.
    String clientId = "Painel Solar";
    clientId += String(random(0xffff), HEX);
    
    // Se conseguiu conectar:
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {  
      Serial.println("conectado");
      // Uma vez conectado, publicar um anúncio..
      client.publish(PUBLISH_TEMP, "Iniciando Comunicação");
      client.publish(PUBLISH_LDR, "Iniciando Comunicação");
      client.publish(PUBLISH_EFFICACY, "Iniciando Comunicação");
      client.publish(PUBLISH_VOLTAGE, "Iniciando Comunicação");
    }
    else {
      Serial.print("falhou, rc = ");
      Serial.print(client.state());
      Serial.println(" Tente novamente em 5 segundos");
      delay(5000);
    }
  }
}

//================================ setup() =====================================
void setup() {
  Serial.begin(115200);
  //analogSetPinAttenuation(TEMP, ADC_0db);

  // Serve tanto para o LDR quanto para o LM35.
  analogSetAttenuation(ADC_0db); // Para que o valor da temperatura dê certo.. (no meu caso o valor é mais exato) (ALICE)

  wifi_setup();

  client.setServer(mqttServer, mqttPort);
}

//================================== loop() ====================================
void loop() {
  float voltage;

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(10000); //Intervalo de 10s entre leituras
  
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
  
    leituraADC = analogRead(TEMP);
    temperatura = leituraADC*0.02686203;
    
    char s_temp[10];
    dtostrf(temperatura, 1, 2, s_temp); // transformando 'temperatura' de float para string 's_temp'
    Serial.print("Temperatura é: ");
    // Serial.print(temperatura);
    Serial.print( s_temp );
    Serial.println(" ºC");
    // delay(2000);
    client.publish( PUBLISH_TEMP, s_temp );
      
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
       
    // na formula eu usei o lux 2 pois os valores obtidos nele são mais coerentes 
    // com o que eu vi na internet.
    float p_wat = ((lux2*metros2)/l_efficacy);
   
    Serial.println ("Valor em watts obtidos: ");
    Serial.println (p_wat);
    delay(2000);

    // Publicando o valor do ldr em Watts.
    char s_ldr_watt[10];
    dtostrf( p_wat, 1, 2, s_ldr_watt); // transformando 'p_watt' de float para string 's_ldr_watt'
 
    Serial.println(s_ldr_watt); // valor do p_watt em string.
    //Publicando o valor em Watts obtido no IO Adafruit.
    client.publish( PUBLISH_LDR, s_ldr_watt );
        
    // esse caso considera o valor lux muito baixo, ou seja, está muito escuro
    // e incapaz de gerar energia considerável.
    if(lux2 < 600){
      voltage = 0;
    }
    else {
      voltage = ((p_wat*38)/600) + t_efficacy;
    }
    
    Serial.println("=====================");
    Serial.print("VOLTAGEM: ");
    Serial.println(voltage);
    Serial.print("EFICIENCIA DA TEMP: ");
    Serial.println(t_efficacy);
    Serial.println("=====================");


    // Publicando a voltagem no IO Adafruit
    char s_voltagem[10];
    dtostrf( voltage, 1, 2, s_voltagem );
    client.publish( PUBLISH_VOLTAGE, s_voltagem );

    // Publicando a eficácia da temperatura no IO Adafruit
    char s_temp_efficacy[10];
    dtostrf( t_efficacy, 1, 2, s_temp_efficacy );
    client.publish( PUBLISH_EFFICACY, s_temp_efficacy );
    
  }
}
