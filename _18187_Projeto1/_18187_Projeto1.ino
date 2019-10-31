
//inclusão das bibliotecas
#include "WiFiEsp.h"
#include "SoftwareSerial.h"


//definição do pino de rele
#define pinRele 10

//definindo uma porta serial virtual
SoftwareSerial Serial1(8,9);

//definindo o nome, senha da rede e criando
//uma variavel para o estado da rede
char ssid[] = "TSE_MURILO";
char pass[] = "00018187";
int status = WL_IDLE_STATUS;

//cria o servidor e o leitor de resposta
WiFiEspServer server(80);
RingBuffer buf(8);

void setup() {
  // inicializa os pinos, desliga o rele.
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, HIGH);  
  
  // inicializa as portas
  Serial.begin(115200);  // porta de debug
  Serial1.begin(9600);
  
  // inicializa o modolo wifi
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
  
  // checa se o cliente esta no servidor e
  // inicializa o leitor de resposta
  if (client)
  {
    Serial.println("Novo cliente");
    buf.init();
    // enquanto o cliente esta conectado, 
    // cada resposta que o cliente passar sera lida
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        buf.push(c);
        Serial.print(c);
        //reação para cada resposta do servidor
        //desconecta o cliente
        
        if (buf.endsWith("\r\n\r\n"))
        {
          sendHttpResponse(client);
          break;
        }
        
        // liga o rele
        if (buf.endsWith("GET /LG?"))
        {
          digitalWrite(pinRele, LOW); // acende o led
          buf.reset();
          Serial.println(" Ligado");
        }
        
        // desliga o rele
        if (buf.endsWith("GET /DL?"))
        {
          digitalWrite(pinRele, HIGH); // apaga o led
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
  // printa o status do wifi, como também o ip e a porta
  
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
  // pagina web
  
  client.println(F(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"  // a conexão será fechada depois de concluir
    "Refresh: 20\r\n"        // a pagina ira atualizar automaticamente a cada 20 sec
    "\r\n"
    "\r \n"
    "\r \n"
    "<!DOCTYPE HTML><html>\r \n"
    "<head><title>Ligar rele"
    "</title></head>\r \n"
    "<body> <br><br><center>\r \n"
    "<H1> LIGANDO RELE</H1>\r \n"
    "<form action=\"LG\" method=\"get\"><button style='background-color: green; padding: 10px 10px 10px 10px; border-color: green; color:  white' type='submit'>Ligar</button></form><br /><form action=\"DL\" method=\"get\"><button type='submit' style='background-color: red; border-color: red; padding: 10px 10px 10px 10px; color: white'>Desligar</button></form>\r \n"
    "</center></body></html>\r \n"));
  client.println();
  
}
