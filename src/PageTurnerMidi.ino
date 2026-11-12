#include <Arduino.h>

// ==============================================================================
// CONFIGURAÇÃO DO BACKEND BLUETOOTH:
// - Padrão: Usa o Bluedroid nativo do ESP32. Não requer nenhuma biblioteca externa
//   além do próprio Control Surface.
// - Opcional (NimBLE): Se quiser economizar RAM e Flash, instale a biblioteca
//   "NimBLE-Arduino" (por h2zero) e descomente a linha abaixo.
// ==============================================================================
// #define CS_USE_NIMBLE

#include <Control_Surface.h>
#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>
#include <esp_system.h>
#include <esp_bt.h>

#if defined(CS_USE_NIMBLE)
#include <NimBLEDevice.h>
#endif

// ==========================================
// Constantes e Definições de Hardware e MIDI
// ==========================================
const uint8_t BUTTON_DOWN = 13;
const uint8_t BUTTON_UP = 15;
const uint8_t LED_STATUS = 16;

const uint8_t NOTA_A3 = 69; // Botão DOWN (Normalmente Próxima Página)
const uint8_t NOTA_B3 = 70; // Botão UP (Normalmente Página Anterior)
const Channel MIDI_CHANNEL = Channel_1; // Canal MIDI 1
const uint8_t DEFAULT_VELOCITY = 127;

const unsigned long BLINK_INTERVAL = 500;
const unsigned long REBOOT_HOLD_TIME = 5000;

// ==========================================
// Instâncias do Control Surface
// ==========================================
// Instancia a interface MIDI via Bluetooth Low Energy (BLE)
BluetoothMIDI_Interface midi;

// Instancia os botões NoteButton do Control Surface:
// O Control Surface configura automaticamente o pino como INPUT_PULLUP,
// realiza o debounce em software e envia Note On ao pressionar e Note Off ao soltar.
NoteButton buttonDown {BUTTON_DOWN, {NOTA_A3, MIDI_CHANNEL}, DEFAULT_VELOCITY};
NoteButton buttonUp   {BUTTON_UP,   {NOTA_B3, MIDI_CHANNEL}, DEFAULT_VELOCITY};

// ==========================================
// Variáveis de Controle de Estado e Tempo
// ==========================================
unsigned long rebootPressTime = 0;
unsigned long lastBlinkTime = 0;
bool wasConnected = false;

// ==========================================
// Funções Auxiliares
// ==========================================

// Pisca o LED três vezes (mantido para compatibilidade/teste inicial)
void initializeBlink() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_STATUS, HIGH);
    delay(BLINK_INTERVAL);
    digitalWrite(LED_STATUS, LOW);
    if (i < 2) {
      delay(BLINK_INTERVAL);
    }
  }
}

// Gerencia o LED indicador e os eventos de conexão/desconexão
void handleConnectionStatus() {
  bool isConnected = midi.isConnected();

  // Detecta transição de status de conexão
  if (isConnected != wasConnected) {
    wasConnected = isConnected;
    if (isConnected) {
      digitalWrite(LED_STATUS, HIGH);
      Serial.println("Connected");
    } else {
      digitalWrite(LED_STATUS, LOW);
      Serial.println("Disconnected");
    }
  }

  // Se não estiver conectado, pisca o LED no intervalo definido
  if (!isConnected) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = currentMillis;
      digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
    }
  }
}

// Emite logs de debug no Serial quando os botões forem acionados
void handleButtonDebug() {
  // AH::Button::Falling indica o momento exato em que o botão foi pressionado (com debounce)
  if (buttonDown.getButtonState() == AH::Button::Falling) {
    Serial.println("Button DOWN");
  }
  if (buttonUp.getButtonState() == AH::Button::Falling) {
    Serial.println("Button UP");
  }
}

// Reinicia o ESP32 caso ambos os botões fiquem pressionados por 5 segundos
void handleReboot() {
  if (digitalRead(BUTTON_DOWN) == LOW && digitalRead(BUTTON_UP) == LOW) {
    if (rebootPressTime == 0) {
      rebootPressTime = millis();
      Serial.println("Reboot: Keep pressing 5s...");
    } else if (millis() - rebootPressTime >= REBOOT_HOLD_TIME) {
      Serial.println("Rebooting...");
      delay(100);
      esp_restart();
    }
  } else {
    rebootPressTime = 0;
  }
}

// ==========================================
// Setup e Loop
// ==========================================
void setup() {
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW);

  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Initializing bluetooth with Control Surface...");

  // Define o nome de anúncio BLE antes de inicializar o Control Surface
  midi.setName("PageTurnerMIDI");

  // Inicializa o Control Surface e as interfaces MIDI ativas
  Control_Surface.begin();

  // Ajusta a potência de transmissão do rádio BLE para +9dBm (máximo do ESP32)
  #if defined(CS_USE_NIMBLE)
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  #else
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  #endif

  Serial.println("Waiting for connections...");
}

void loop() {
  // Atualiza o Control Surface (faz a leitura dos botões, debounce e envio de mensagens MIDI)
  Control_Surface.loop();

  // Gerencia o LED e estado da conexão Bluetooth
  handleConnectionStatus();

  // Mensagens no monitor serial
  handleButtonDebug();

  // Monitoramento da função de reboot
  handleReboot();
}
