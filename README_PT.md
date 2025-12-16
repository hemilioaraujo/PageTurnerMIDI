# PageTurnerMIDI - Pedal Passador de Páginas Bluetooth DIY

**PageTurnerMIDI** é um pedal passador de páginas sem fio "faça-você-mesmo" para músicos. Ele utiliza um microcontrolador ESP32 para transmitir sinais MIDI via Bluetooth Low Energy (BLE), permitindo virar páginas em aplicativos como MobileSheets, forScore e outros que suportam mapeamento MIDI.

![Foto do Pedal](img/pedal-picture.jpg)

## ✨ Funcionalidades

* **Conectividade Sem Fio:** Usa MIDI via BLE para baixa latência e operação sem cabos.
* **Controle Duplo:**
  * **Botão DOWN (Baixo):** Envia Nota MIDI **(69)**.
  * **Botão UP (Cima):** Envia Nota MIDI **(70)**.
* **Indicação de Status:** LED indicador de conexão (Piscando = Aguardando, Aceso = Conectado).
* **Função de Reinício:** Segure ambos os pedais simultaneamente por 5 segundos para reiniciar o dispositivo.
* **Gabinete Personalizado:** Arquivos para corte a laser incluídos.

## 🛠️ Hardware e Pinagem

O projeto é construído em torno de uma placa de desenvolvimento **ESP32**.

| Componente | Pino ESP32 (GPIO) | Descrição |
| :--- | :--- | :--- |
| **Botão DOWN** | 13 | Envia Nota **69** (Geralmente Próxima Página) |
| **Botão UP** | 15 | Envia Nota **70** (Geralmente Página Anterior) |
| **LED de Status** | 16 | Visualização do estado da conexão |

### Diagrama do Circuito

Você pode encontrar o diagrama detalhado do circuito na pasta `diagram/`.  
Obs.: Foi usado um resistor de 100 ohm para o LED, pode variar dependendo do LED usado.

![Diagrama do Circuito](diagram/diagram.png)

## 📦 Gabinete / Case

A pasta `laser-cut/` contém os arquivos de design para o gabinete do pedal (Obs.: Foi usado um material de 9mm).

* `page-turner.svg`: Arquivo vetorial para corte a laser.
* `pedal-page-turner.lbrn2`: Arquivo de projeto do LightBurn.

## 🚀 Instalação e Uso

1. **Montagem do Hardware:** Siga o diagrama do circuito para conectar os botões e o LED ao seu ESP32.
2. **Upload do Firmware:**
    * Abra o projeto (Recomendado PlatformIO ou Arduino IDE).
    * Instale as bibliotecas necessárias (ex: `BLE-MIDI`).
    * Faça o upload do `src/PageTurnerMidi.ino` para o seu ESP32.
3. **Pareamento:**
    * Ligue o pedal. O LED deve piscar.
    * No seu tablet/computador, procure por dispositivos Bluetooth.
    * Conecte-se a **"PageTurner MIDI"**. O LED ficará aceso fixo.
4. **Configuração do App:**
    * Abra seu aplicativo de partituras (ex: MobileSheets).
    * Vá para as Configurações MIDI.
    * Mapeie a Nota MIDI **69** para "Rolar para Baixo" ou "Próxima Página".
    * Mapeie a Nota MIDI **70** para "Rolar para Cima" ou "Página Anterior".
