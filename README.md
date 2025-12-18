# 📡 CANSApp – Context-Aware Network Selection for Android

**CANSApp** (Context-Aware Network Selection) é um aplicativo Android que implementa um **mecanismo de seleção dinâmica de interfaces de rede** baseado em **análise de contexto**, com foco em **handover vertical** em **redes sem fio heterogêneas** (Wi-Fi, 4G/5G e Bluetooth).

O projeto é uma implementação prática, em Android, do algoritmo proposto em:

> Monteiro et al., *Context-aware network selection in heterogeneous wireless networks*, Computer Communications, 2019.

---

## 🎯 Objetivo do Projeto

Desenvolver um aplicativo Android capaz de:

- Coletar informações de **contexto do dispositivo, usuário e rede**
- Identificar **cenários de uso** em tempo quase real
- Selecionar automaticamente a **melhor interface de rede**
- Minimizar:
  - Perdas de conectividade
  - Consumo de energia
  - Custo de comunicação
- Garantir **mobilidade transparente** durante o deslocamento do usuário

---

## 🧠 Conceitos-Chave

- **Handover Vertical**: troca de conexão entre tecnologias distintas (ex.: Wi-Fi → 5G)
- **Ciência de Contexto (Context-Aware Computing)**
- **Redes Heterogêneas (HetNets)**
- **Seleção Inteligente de Interface de Rede**

---

## 🏗️ Arquitetura do Sistema

O CANSApp é implementado como um **serviço Android em segundo plano**, organizado segundo o padrão **MVC (Model–View–Controller)**.

### 📐 Componentes Principais

#### 🔹 Model
- `DeviceMobile`
  - Armazena o contexto atual do dispositivo
  - Velocidade de deslocamento
  - Nível de bateria
  - Estado da tela
  - Consumo de banda
  - Interfaces disponíveis
- `WirelessNet`
  - Representa interfaces de rede (Wi-Fi, 5G, Bluetooth)
  - RSSI, frequência, conectividade e pontuação

#### 🔹 Controller
- `CANSController`
  - Aquisição de informações de contexto
  - Identificação do cenário
  - Seleção da melhor interface de rede
- `ServiceCANS`
  - Serviço Android executado em background
  - Ciclo de execução a cada **5 segundos**

#### 🔹 View
- Activities Android:
  - Splash Screen
  - Tela principal com informações de contexto

---

## 📊 Informações de Contexto Coletadas

| Contexto | API Android Utilizada |
|--------|----------------------|
| Velocidade do usuário | `LocationManager`, `LocationListener` |
| Nível de bateria | `BatteryManager` |
| Estado da tela | `PowerManager` |
| Consumo de banda | `ConnectivityManager`, `NetworkCapabilities` |
| Wi-Fi | `WifiManager` |
| Bluetooth | `BluetoothManager`, `BluetoothAdapter` |

---

## 🧩 Cenários de Contexto Identificados

O algoritmo classifica o contexto do usuário em **três políticas principais**:

### 🔵 Throughput
- Prioriza **alta largura de banda**
- Preferência:  
  **Wi-Fi → 5G → Bluetooth**

### 🟢 PowerSave
- Prioriza **economia de energia**
- Preferência:  
  **Bluetooth → Wi-Fi → 5G**

### 🟠 Coverage
- Prioriza **alta cobertura**
- Preferência:  
  **5G → Wi-Fi → Bluetooth**

---

## 🔁 Fluxo de Execução

1. Coleta das informações de contexto
2. Identificação do cenário atual
3. Seleção da melhor interface disponível
4. Impressão dos dados no log do Android (`Logcat`)
5. Repetição automática a cada 5 segundos

---

## 🧪 Testes e Validação

- Testes realizados no **emulador do Android Studio**
- Simulação de:
  - Deslocamento via GPS
  - Alteração do nível de bateria
  - Ativação/desativação da tela
- Validação via **logs do sistema** com a tag:

```text
[CANSAPP]
