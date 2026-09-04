// SCL VAI NO PINO D22 E SDA VAI NO PINO D21

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LARGURA_TELA 128
#define ALTURA_TELA 64
#define OLED_RESET -1

Adafruit_SSD1306 display(LARGURA_TELA, ALTURA_TELA, &Wire, OLED_RESET);

// Credenciais da rede Wi-Fi do ESP32
const char* ssidAP = "ESP32";
const char* passwordAP = "esp32esp"; 

// Definição dos pinos que serão controlados
const int ledEmbutido = LED_BUILTIN; // LED azul da própria placa
const int pinoExtra1 = 4;
const int pinoExtra2 = 5;

// Inicia o servidor web
WebServer server(80);

// Estrutura HTML da página com os botões
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Controle de Pinos ESP32</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 30px; background-color: #222; color: #fff;}
    h1 { color: #4CAF50; }
    .linha-controle { margin-bottom: 20px; padding: 15px; background-color: #333; border-radius: 10px; display: inline-block; width: 90%; max-width: 400px; }
    .btn { display: inline-block; padding: 12px 20px; margin: 5px; font-size: 16px; font-weight: bold; color: #fff; background-color: #4CAF50; text-decoration: none; border-radius: 5px; width: 100px; }
    .btn-off { background-color: #f44336; }
    .btn:active { opacity: 0.7; }
  </style>
</head>
<body>
  <h1>Painel do ESP32</h1>
  
  <div class="linha-controle">
    <p>LED Embutido (Pino 2)</p>
    <a href="/acionar?pino=2&estado=1" class="btn">LIGAR</a>
    <a href="/acionar?pino=2&estado=0" class="btn btn-off">DESLIGAR</a>
  </div>

  <div class="linha-controle">
    <p>Dispositivo 1 (Pino 4)</p>
    <a href="/acionar?pino=4&estado=1" class="btn">LIGAR</a>
    <a href="/acionar?pino=4&estado=0" class="btn btn-off">DESLIGAR</a>
  </div>

  <div class="linha-controle">
    <p>Dispositivo 2 (Pino 5)</p>
    <a href="/acionar?pino=5&estado=1" class="btn">LIGAR</a>
    <a href="/acionar?pino=5&estado=0" class="btn btn-off">DESLIGAR</a>
  </div>

</body>
</html>
)rawliteral";

// Função para exibir a página principal
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Função executada quando um botão é clicado
void handleAcionar() {
  // Verifica se a URL contém os parâmetros "pino" e "estado"
  if (server.hasArg("pino") && server.hasArg("estado")) {
    int pino = server.arg("pino").toInt();
    int estado = server.arg("estado").toInt();
    
    // Atualiza o estado físico do pino na placa
    digitalWrite(pino, estado);
  }
  
  // Redireciona de volta para a página inicial para evitar que a tela fique em branco
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  // Configura os pinos como saída (OUTPUT) e garante que iniciem desligados (LOW)
  pinMode(ledEmbutido, OUTPUT);
  pinMode(pinoExtra1, OUTPUT);
  pinMode(pinoExtra2, OUTPUT);
  digitalWrite(ledEmbutido, LOW);
  digitalWrite(pinoExtra1, LOW);
  digitalWrite(pinoExtra2, LOW);

  // Inicializa o display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Falha ao inicializar o OLED"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Criando Rede...");
  display.display();

  // Configura o ESP32 como Access Point
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress IP = WiFi.softAPIP(); 

  // Configuração das rotas do Servidor Web
  server.on("/", handleRoot);          // Rota principal (A página HTML)
  server.on("/acionar", handleAcionar); // Rota invisível que processa os cliques dos botões
  server.begin();

  // Atualiza a tela OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("REDE ESP32 ATIVA");
  
  display.setCursor(0, 20);
  display.print("Rede: ");
  display.println(ssidAP);
  
  display.setCursor(0, 35);
  display.print("Pass: ");
  display.println(passwordAP);
  
  display.setCursor(0, 50);
  display.print("IP: ");
  display.println(IP);
  
  display.display();
}

void loop() {
  // Mantém o servidor processando os cliques e acessos
  server.handleClient();
}
