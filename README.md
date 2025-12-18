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
