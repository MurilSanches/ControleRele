
//inclusão das bibliotecas
#include "WiFiEsp.h"
#include "SoftwareSerial.h"


//definição do pino de rele
#define pinRele 10

//definindo uma porta serial virtual
SoftwareSerial Serial1(8,9);

//definindo o nome, senha da rede e crinado
//uma variavel para o estado da rede
char ssid[] = "TSE_MURILO";
char pass[] = "00018187";
int status = WL_IDLE_STATUS;

//cria o servidor e o leitor de resposta
WiFiEspServer server(80);
RingBuffer buf(8);

void setup() {
  // inicializa os pinos e portas, desliga o rele.
  pinMode(pinRele,OUTPUT);
  digitalWrite(pinRele,HIGH);  
  
  Serial.begin(115200);  // porta de debug
  Serial1.begin(9600);
  
  // inicializa o mudolo wifi
  WiFi.init(&Serial1);
   
  //checa se o modulo esta conectado
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Sem modulo");
    while(true);  //trava a execução 
  }
 

 // IPAddress localIp(192, 168, 1203, 1);
 // WiFi.configAP(localIp);

  status = WiFi.beginAP(ssid, 10, pass, ENC_TYPE_WPA2_PSK);

  Serial.println("Access point started");
  printWifiStatus(); 
  
  server.begin();
  Serial.println("Server started");
}

void loop() {
  //cria um cliente
  WiFiEspClient client = server.available();
  
  //checa se o cliente esta no servidor e
  // inicializa o leitor de resposta
  if (client)
  {
    Serial.println("Novo cliente");
    buf.init();
    // enquanto o cliente esta conectado, 
    //cada resposta que o cliente passar sera lida
    while (client.connected()){
      if (client.available()){
        char c = client.read();
        buf.push(c);
        Serial.print(c);
        //reação para cada resposta do servidor
        //desconecta o cliente
        if (buf.endsWith("\r\n\r\n")){
          sendHttpResponse(client);
          break;
        }
        // liga o rele
        if (buf.endsWith("GET /LG?")){
          digitalWrite(pinRele, LOW);
          buf.reset();
          Serial.println(" Ligado");
        }
        // desliga o rele
        if (buf.endsWith("GET /DL?")){
          digitalWrite(pinRele, HIGH);
          buf.reset();
          Serial.println("deligado");
        }
        
        
      }
    } // while
    client.stop();
    Serial.println("Desconectado");
  }
}

void printWifiStatus(){
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address ");
  Serial.println(ip);

  Serial.print("To see this page in action, connect to ");
  Serial.print(ssid);
  Serial.print(" and open a browser to http://");
  Serial.print(ip);
  Serial.println();
}


void sendHttpResponse(WiFiEspClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE HTML><html>");
  client.println("<head><title>Ligar rele");
  client.println("</title></head>");
  client.println("<body> <br><br><center>");
  client.println("<H1> LIGANDO RELEINS</H1>");
  client.println("<form action=\"LG\" method=\"get\"><button type='submit'>Ligar</button></form><form action=\"DL\" method=\"get\"><button type='submit'>Desligar</button></form>");
  client.println("</center></body></html>");
  client.println();
  
}
