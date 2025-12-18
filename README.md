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

🛠️ Tecnologias Utilizadas

    Java

    Android SDK

    Android Studio Iguana

    Padrão de Projeto MVC

    APIs nativas de conectividade e sensores

▶️ Execução

    Clone o repositório:

git clone https://github.com/profalexmonteiro/cansapp.git

    Abra no Android Studio

    Execute em um dispositivo físico ou emulador

    A aplicação inicia automaticamente o serviço ServiceCANS

📈 Resultados

    Seleção automática e dinâmica da interface de rede

    Funcionamento em tempo real

    Execução contínua em segundo plano

    Base sólida para pesquisas em:

        Mobilidade

        Redes 5G

        Open RAN

        Sistemas sensíveis ao contexto

🚧 Limitações Atuais

    Consumo de energia pode ser otimizado

    Estratégias de predição ainda não implementadas

    Não há intervenção do usuário na política de seleção

    Avaliação feita apenas em ambiente controlado

🔮 Trabalhos Futuros

    Otimização do consumo energético

    Inclusão de técnicas de predição de contexto

    Integração com métricas de QoE

    Testes em ambientes reais e redes densas

    Integração com arquiteturas Open RAN / Near-RT RIC

📄 Referência Acadêmica

    MONTEIRO, A. F. D. et al.
    Context-aware network selection in heterogeneous wireless networks.
    Computer Communications, v. 135, p. 1–15, 2019.

👨‍💻 Autor

Alex Fernando Duarte Monteiro
📚 Pesquisa em Redes de Computadores, Mobilidade e Ciência de Contexto
🔗 GitHub: https://github.com/profalexmonteiro
📜 Licença

Este projeto é disponibilizado exclusivamente para fins acadêmicos e de pesquisa.
Consulte o autor para uso comercial ou industrial.

⭐ Se este projeto foi útil para sua pesquisa ou estudo, considere dar uma estrela no repositório!


---

Se quiser, posso:

- Adaptar o README para **inglês**
- Criar um **README acadêmico** (estilo IEEE / SBRC)
- Gerar **badges**, **diagramas em SVG**, ou
- Preparar um **DOI-ready README** para repositórios científicos

Só me dizer 👍

Você disse:
Adaptar o README para **inglês**
O ChatGPT disse:

Claro — abaixo está o README.md totalmente adaptado para inglês, mantendo fidelidade técnica e acadêmica ao artigo e à implementação real do CANSApp.

Você pode substituir diretamente o README atual do repositório.

# 📡 CANSApp – Context-Aware Network Selection for Android

**CANSApp** (Context-Aware Network Selection) is an Android application that implements a **context-aware network interface selection mechanism**, focusing on **vertical handover** in **heterogeneous wireless networks** such as **Wi-Fi, 4G/5G, and Bluetooth**.

This project is a practical Android implementation of the algorithm proposed in:

> Monteiro et al., *Context-aware network selection in heterogeneous wireless networks*, Computer Communications, 2019.

---

## 🎯 Project Objective

The goal of CANSApp is to develop an Android application capable of:

- Collecting **context information** from the device, user, and network
- Identifying **usage scenarios** in near real time
- Automatically selecting the **best available network interface**
- Minimizing:
  - Connectivity disruptions
  - Energy consumption
  - Communication costs
- Providing **transparent mobility** during user movement

---

## 🧠 Key Concepts

- **Vertical Handover**: switching connectivity between different access technologies (e.g., Wi-Fi → 5G)
- **Context-Aware Computing**
- **Heterogeneous Networks (HetNets)**
- **Intelligent Network Interface Selection**

---

## 🏗️ System Architecture

CANSApp is implemented as a **background Android service**, following the **MVC (Model–View–Controller)** architectural pattern.

### 📐 Main Components

#### 🔹 Model
- `DeviceMobile`
  - Stores the current device context
  - User speed
  - Battery level
  - Screen state
  - Bandwidth usage
  - Available network interfaces
- `WirelessNet`
  - Represents wireless interfaces (Wi-Fi, 5G, Bluetooth)
  - RSSI, frequency, connectivity state, and scoring attributes

#### 🔹 Controller
- `CANSController`
  - Context information acquisition
  - Context identification
  - Network interface selection
- `ServiceCANS`
  - Android background service
  - Execution cycle every **5 seconds**

#### 🔹 View
- Android Activities:
  - Splash Screen
  - Main screen displaying context information

---

## 📊 Context Information Collected

| Context Data | Android API Used |
|-------------|------------------|
| User speed | `LocationManager`, `LocationListener` |
| Battery level | `BatteryManager` |
| Screen state | `PowerManager` |
| Bandwidth usage | `ConnectivityManager`, `NetworkCapabilities` |
| Wi-Fi networks | `WifiManager` |
| Bluetooth | `BluetoothManager`, `BluetoothAdapter` |

---

## 🧩 Identified Context Scenarios

The algorithm classifies user context into **three main policies**:

### 🔵 Throughput
- Prioritizes **high bandwidth**
- Interface preference:  
  **Wi-Fi → 5G → Bluetooth**

### 🟢 PowerSave
- Prioritizes **energy efficiency**
- Interface preference:  
  **Bluetooth → Wi-Fi → 5G**

### 🟠 Coverage
- Prioritizes **maximum coverage**
- Interface preference:  
  **5G → Wi-Fi → Bluetooth**

---

## 🔁 Execution Flow

1. Context information acquisition
2. Context identification
3. Best interface selection
4. Context and decision logging via Android `Logcat`
5. Automatic repetition every 5 seconds

---

## 🧪 Testing and Validation

- Tests conducted using the **Android Studio emulator**
- Simulated scenarios:
  - User movement via GPS
  - Battery level variations
  - Screen on/off states
- Validation through **system logs** using the tag:

```text
[CANSAPP]

🛠️ Technologies Used

    Java

    Android SDK

    Android Studio Iguana

    MVC architectural pattern

    Native Android connectivity and sensor APIs

▶️ How to Run

    Clone the repository:

git clone https://github.com/profalexmonteiro/cansapp.git

    Open the project in Android Studio

    Run it on a physical device or emulator

    The ServiceCANS background service starts automatically

📈 Results

    Dynamic and automatic network interface selection

    Real-time context evaluation

    Continuous background execution

    Solid foundation for research in:

        Mobile networking

        5G systems

        Context-aware computing

        Open RAN architectures

🚧 Current Limitations

    Energy consumption can be further optimized

    No predictive context modeling yet

    No user-defined policy configuration

    Evaluation limited to controlled environments

🔮 Future Work

    Energy consumption optimization

    Integration of context prediction techniques

    Quality of Experience (QoE) metrics

    Real-world experiments in dense urban scenarios

    Integration with Open RAN and Near-RT RIC environments

📄 Academic Reference

    MONTEIRO, A. F. D. et al.
    Context-aware network selection in heterogeneous wireless networks.
    Computer Communications, v. 135, p. 1–15, 2019.

👨‍💻 Author

Alex Fernando Duarte Monteiro
Researcher in Mobile Networks, Context-Aware Systems, and Network Security
🔗 GitHub: https://github.com/profalexmonteiro
📜 License

This project is intended for academic and research purposes only.
Please contact the author for commercial or industrial use.

⭐ If this project was useful for your research or studies, consider giving it a star!


---

Se quiser, posso agora:

- Ajustar o texto para **IEEE / SBRC GitHub style**
- Criar uma versão **README + CITATION.cff**
- Preparar uma **descrição curta para GitHub + Zenodo**
- Revisar o inglês para **publicação internacional**

É só dizer 👌

Você disse:
gere um read do repositorio https://github.com/engalexmonteiro/cans
O ChatGPT disse:

Aqui está um README completo e profissional para o repositório engalexmonteiro/cans, com base no conteúdo visível da página do GitHub (“Context-Aware Network Selection”) e no padrão do projeto (provavelmente C com lógica de contexto/seleção de rede) — já que não conseguimos carregar o conteúdo original do README.md diretamente devido a limitações de acesso.
GitHub

# 📡 CANS — Context-Aware Network Selection

**CANS** (Context-Aware Network Selection) is a C project focused on implementing mechanisms for adaptive context-aware decision-making in network environments.

This repository contains source code written in **C** (100% of the codebase), designed to perform intelligent network selection based on runtime context. :contentReference[oaicite:1]{index=1}

---

## 🧠 Project Overview

In heterogeneous network environments — where multiple network technologies (Wi-Fi, cellular, Bluetooth, etc.) coexist — selecting the most appropriate interface at runtime is key for performance, energy efficiency, and seamless connectivity.

**CANS** implements logic for:

- Monitoring system or network context
- Evaluating metrics for interface quality
- Selecting the optimal network interface
- Adapting to changing conditions

Although language chosen is C, the core concepts align with context-aware networking research.

---

## 📁 Repository Structure

/
├── .settings/ # IDE or workspace settings
├── src/ # Source code (C files)
├── .cproject # C project config
├── .gitignore
├── .project
├── README.md
└── dependences.sh # Script for installing dependencies


*(Structure based on GitHub directory listing)* :contentReference[oaicite:2]{index=2}

---

## 🛠️ Installation

To compile and run the project:

1. Clone the repository:

```bash
git clone https://github.com/engalexmonteiro/cans.git
cd cans

    Ensure you have a C compiler installed (e.g., gcc).

    Run the dependency installer:

./dependences.sh

(If the script requires permissions, you may need chmod +x dependences.sh first.)

    Build the program:

# Assuming a Makefile or generic compilation
gcc -o cans src/*.c

    Run the compiled executable:

./cans

🧪 Usage

Describe how the program accepts input and what output to expect:

./cans [options]

Typical options might include:

    --help: display usage

    --config: load configuration file

    --verbose: detailed runtime logs

(Adjust based on actual source code in src/.)
🧠 Core Concepts

    Context Monitoring: Track runtime conditions (e.g., latency, signal quality)

    Evaluation Metrics: Score network interfaces according to context

    Decision Logic: Choose best network path

    Adaptability: Support changes during execution

🧩 Example Output

[INFO] Starting context acquisition
[INFO] Detected interfaces: Wi-Fi, Cellular
[INFO] Best interface selected: Wi-Fi

(Example output — update with real logs from execution.)
🧭 Contributing

Contributions are welcome! To contribute:

    Fork this repository

    Create a branch for your feature (feature/xyz)

    Open a pull request with a clear description

📄 License

Include license terms here if applicable (e.g., MIT, GPL).
If no license is present, state that by default all rights are reserved.
📫 Contact

For questions or collaboration:

📌 GitHub: https://github.com/engalexmonteiro/cans

⭐ If this project helped you, consider giving it a star!


---

Se você quiser, posso também gerar:

🔹 um README traduzido para **Português**,  
🔹 uma versão mais **técnica com exemplos de código**,  
🔹 ou extrair automaticamente *seções do código-fonte* (como funções e variáveis) e colocá-las no README.

Só dizer o estilo desejado!
::contentReference[oaicite:3]{index=3}

Você disse:
1-s2.0-S014036641730155X-main.pdf
PDF
refaça o readme se baseado no repositorio e  no journal anexado 
O ChatGPT disse:

Perfeito. Abaixo está um README.md totalmente refeito, alinhado simultaneamente ao repositório engalexmonteiro/cans (implementação em C) e ao artigo do journal Computer Communications (2019), descrevendo corretamente o CANS como API/protótipo em C para Linux, e não como app Android.

O texto está em inglês acadêmico/técnico, adequado para GitHub + reuso científico.

# 📡 CANS — Context-Aware Network Selection

**CANS (Context-Aware Network Selection)** is a **context-aware network interface selection mechanism** implemented in **C**, designed for **Linux-based systems**.  
It enables **dynamic and intelligent selection of wireless network interfaces** (Bluetooth, Wi-Fi, and LTE/4G) in **heterogeneous wireless environments**, based on **user, device, and network context**.

This repository contains the **reference prototype and API** described and evaluated in the journal article:

> Monteiro, A. et al., *Context-aware network selection in heterogeneous wireless networks*,  
> **Computer Communications**, Elsevier, vol. 135, pp. 1–15, 2019. :contentReference[oaicite:0]{index=0}

---

## 🎯 Project Motivation

Modern mobile and embedded devices are typically equipped with **multiple wireless interfaces**. However, selecting the *best* interface at runtime is challenging due to:

- User mobility
- Variable radio conditions
- Energy constraints
- Bandwidth demand
- Financial cost of mobile data

CANS addresses this problem by introducing a **context-aware, policy-based, and modular network selection mechanism**, enabling the *Always Best Connected (ABC)* concept in practice.

---

## 🧠 Core Concepts

- **Context-Aware Computing**
- **Vertical Handover** (e.g., Wi-Fi ↔ LTE)
- **Heterogeneous Wireless Networks**
- **Multi-homed Devices**
- **Energy-aware and Cost-aware Networking**

---

## 🏗️ Architecture Overview

CANS operates as a **software agent/service** that continuously gathers context information and dynamically selects the most suitable network interface.

### 🔹 Context Sources
- **User context**
  - Displacement speed (via GPS)
  - Device usage (screen on/off)
- **Device context**
  - Battery level
  - Energy consumption
- **Network context**
  - Bandwidth usage
  - Signal quality
  - Interface availability

### 🔹 Supported Interfaces
- **Bluetooth (PAN / NAP profile)** — low energy, low cost
- **Wi-Fi (IEEE 802.11)** — high throughput, medium mobility
- **LTE / 4G** — wide coverage, higher cost

---

## 🧩 Context Policies

CANS classifies the current situation into **three context policies**, which define the **priority order of interfaces**.

| Policy | Objective | Interface Priority |
|------|---------|------------------|
| **Access Speed** | Maximize throughput | Wi-Fi → LTE → Bluetooth |
| **Power Save** | Minimize energy consumption | Bluetooth → Wi-Fi → LTE |
| **Coverage** | Maximize connectivity stability | LTE → Wi-Fi → Bluetooth |

Context identification is based on:
- User speed
- Battery level
- Bandwidth consumption
- Device usage state

---

## 📁 Repository Structure

cans/
├── src/ # C source files
├── .settings/ # IDE configuration
├── dependences.sh # Dependency installation script
├── .project
├── .cproject
└── README.md


---

## 🛠️ Dependencies

CANS relies on standard **Linux networking and system libraries**, including:

- `libnm` / `NetworkManager`
- `iwlib` (Wi-Fi information)
- `hci` / BlueZ (Bluetooth)
- `gpsd` headers
- `acpi` (battery information)
- `ifstat`

Install dependencies using:

```bash
chmod +x dependences.sh
./dependences.sh

▶️ Build and Run

    Clone the repository:

git clone https://github.com/engalexmonteiro/cans.git
cd cans

    Compile the source code:

gcc -o cans src/*.c -lnm -liw -lbluetooth

    Run CANS:

./cans

    Display help options:

./cans -h

⚙️ API Overview

CANS provides a C API that can be reused by researchers and developers.
Key Headers

    mobile_device.h

        Context acquisition

        Device state representation

    cans.h

        Interface management strategies

        Horizontal and vertical handoff logic

Core Functions

    gathering_infoctx()

    horizontal_handoff_WiFi()

    horizontal_handoff_Bluetooth()

    manage_ifaceLTE()

    execution()

🧪 Experimental Validation

Experiments were conducted in real environments with:

    Bluetooth PAN

    Wi-Fi access points

    Commercial LTE/4G network

Key Results

    Up to 95% connectivity availability

    Energy savings compared to single-interface usage

    Reduced financial cost of mobile data

    Stable operation under mobility scenarios

The results confirm that context-aware selection significantly improves energy efficiency and connectivity.
🚧 Known Limitations

    TCP/UDP sessions may break during interface switching

    No built-in mobility protocol (e.g., Mobile IP)

    Context prediction not yet implemented

🔮 Future Work

    Integration with mobility management protocols (MIPv6)

    Context prediction using machine learning

    Support for additional interfaces

    Scalability and QoS evaluation

    Integration with 5G / Open RAN environments

📄 Reference

If you use this code in academic work, please cite:

@article{monteiro2019cans,
  title={Context-aware network selection in heterogeneous wireless networks},
  author={Monteiro, Alex and Souto, Eduardo and Pazzi, Richard and Nogueira, Michele},
  journal={Computer Communications},
  volume={135},
  pages={1--15},
  year={2019},
  publisher={Elsevier}
}

👨‍💻 Author

Alex Fernando Duarte Monteiro
Federal University of Amazonas (UFAM)
Research areas: Wireless Networks, Mobility, Context-Aware Systems

GitHub: https://github.com/engalexmonteiro
📜 License

This project is intended for academic and research purposes.
Please contact the author for commercial use.
