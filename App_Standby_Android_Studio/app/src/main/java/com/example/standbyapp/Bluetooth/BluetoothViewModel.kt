package com.example.standbyapp.Bluetooth

import android.Manifest
import android.app.Application
import android.bluetooth.BluetoothAdapter
import android.content.pm.PackageManager
import androidx.compose.runtime.*
import androidx.core.content.ContextCompat
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import android.util.Log
import android.bluetooth.BluetoothDevice
import com.example.stmbluetooth.BluetoothScanCallback
import com.example.stmbluetooth.BluetoothScanner
import com.example.stmbluetooth.STM32Commands
import com.example.standbyapp.ControlPanel.ButtonStateViewModel
import com.example.standbyapp.dataStore
import kotlinx.coroutines.flow.first

class BluetoothViewModel(application: Application) : AndroidViewModel(application),
    BluetoothScanCallback {
    
    private lateinit var bluetoothScanner: BluetoothScanner
    
    // États observables
    var isConnected by mutableStateOf(false)
        private set
    
    var isAuthenticated by mutableStateOf(false)
        private set
    
    var connectionStatus by mutableStateOf("Déconnecté")
        private set
    
    var pairedDevices by mutableStateOf<List<BluetoothDevice>>(emptyList())
        private set
        
    var discoveredDevices by mutableStateOf<List<BluetoothDevice>>(emptyList())
        private set
    
    var isScanning by mutableStateOf(false)
        private set
    
    var errorMessage by mutableStateOf<String?>(null)
        private set
    
    var showDeviceDialog by mutableStateOf(false)
        private set

    init {
        bluetoothScanner = BluetoothScanner(application, this)
        refreshPairedDevices()
        // Load current authentication code from DataStore
        loadCurrentAuthCode()
    }
    
    /**
     * Load the current authentication code from DataStore and update the scanner
     */
    private fun loadCurrentAuthCode() {
        viewModelScope.launch {
            try {
                val prefs = getApplication<Application>().dataStore.data.first()
                val currentCode = prefs[androidx.datastore.preferences.core.stringPreferencesKey("current_auth_code")] ?: "BT12345"
                bluetoothScanner.setAuthenticationCode(currentCode)
                Log.d("BluetoothViewModel", "Loaded authentication code: $currentCode")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error loading auth code: ${e.message}", e)
                // Fallback to default
                bluetoothScanner.setAuthenticationCode("BT12345")
            }
        }
    }
    
    /**
     * Update the authentication code manually
     */
    fun updateAuthenticationCode(newCode: String) {
        bluetoothScanner.setAuthenticationCode(newCode)
        Log.d("BluetoothViewModel", "Authentication code updated to: $newCode")
    }
    
    /**
     * Reload authentication code from DataStore
     */
    fun reloadAuthenticationCode() {
        loadCurrentAuthCode()
    }
    
    /**
     * Définit le ButtonStateViewModel pour les keep-alive avec états réels
     */
    fun setButtonStateViewModel(buttonStateViewModel: ButtonStateViewModel?) {
        buttonStateViewModel?.let {
            bluetoothScanner.setButtonStateViewModel(it)
        }
    }
    
    /**
     * Définit le ControlPanelViewModel pour les keep-alive avec le nouveau système
     */
    fun setControlPanelViewModel(controlPanelViewModel: com.example.standbyapp.ControlPanel.ControlPanelViewModel?) {
        controlPanelViewModel?.let {
            bluetoothScanner.setControlPanelViewModel(it)
        }
    }
    
    fun refreshPairedDevices() {
        viewModelScope.launch {
            try {
                val bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
                if (ContextCompat.checkSelfPermission(
                        getApplication<Application>().applicationContext,
                        Manifest.permission.BLUETOOTH_CONNECT
                    ) == PackageManager.PERMISSION_GRANTED) {
                    pairedDevices = bluetoothAdapter?.bondedDevices?.toList() ?: emptyList()
                } else {
                    errorMessage = "Bluetooth permission not granted"
                    pairedDevices = emptyList()
                }
            } catch (e: Exception) {
                errorMessage = "Erreur lors de la récupération des appareils: ${e.message}"
            }
        }
    }
    
    fun startScan() {
        if (!isScanning) {
            discoveredDevices = emptyList()
            bluetoothScanner.startScan()
        }
    }
    
    fun stopScan() {
        if (isScanning) {
            bluetoothScanner.stopScan()
        }
    }
    
    fun connectToDevice(device: BluetoothDevice) {
        viewModelScope.launch {
            try {
                Log.d("BluetoothViewModel", "Attempting to connect to device: ${device.name ?: "Unknown"}")
                connectionStatus = "Connecting to ${device.name ?: "Unknown"}..."
                
                val connected = bluetoothScanner.connectToDevice(device)
                if (!connected) {
                    Log.e("BluetoothViewModel", "Connection failed for device: ${device.name}")
                    errorMessage = "Unable to connect to ${device.name ?: "Unknown device"}"
                    connectionStatus = "Connection failed"
                }
            } catch (e: SecurityException) {
                Log.e("BluetoothViewModel", "Security exception during connection: ${e.message}", e)
                errorMessage = "Permission denied: ${e.message}"
                connectionStatus = "Permission error"
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Unexpected error during connection: ${e.message}", e)
                errorMessage = "Connection error: ${e.message}"
                connectionStatus = "Connection error"
            }
        }
    }
    
    fun disconnect() {
        bluetoothScanner.disconnect()
    }

    /**
     * Envoie une commande basée sur l'état actuel des boutons du ControlPanel
     */
    fun sendButtonStatesCommand(buttonStateViewModel: ButtonStateViewModel) {
        if (!isConnected || !isAuthenticated) {
            errorMessage = "Non connecté ou non authentifié"
            Log.w("BluetoothViewModel", "Tentative d'envoi sans connexion/authentification")
            return
        }
        
        viewModelScope.launch {
            try {
                val commandData = STM32Commands.createAppCommandFromViewModel(buttonStateViewModel)
                Log.d("BluetoothViewModel", "Envoi de commande boutons: ${commandData.contentToString()}")
                
                val success = bluetoothScanner.sendControlData(commandData)
                if (success) {
                    Log.d("BluetoothViewModel", "Commande boutons envoyée avec succès")
                } else {
                    errorMessage = "Erreur d'envoi de commande boutons"
                    Log.e("BluetoothViewModel", "Échec envoi commande boutons")
                }
            } catch (e: Exception) {
                errorMessage = "Erreur envoi boutons: ${e.message}"
                Log.e("BluetoothViewModel", "Exception lors de l'envoi: ${e.message}")
            }
        }
    }

    /**
     * Envoie une commande simple avec des états spécifiques
     */
    fun sendSimpleButtonCommand(buttonStates: Map<String, Boolean>) {
        if (!isConnected || !isAuthenticated) {
            errorMessage = "Non connecté ou non authentifié"
            return
        }
        
        viewModelScope.launch {
            try {
                val commandData = STM32Commands.createAppCommandFromButtonMap(buttonStates)
                Log.d("BluetoothViewModel", "Envoi de commande simple: ${commandData.contentToString()}")
                
                val success = bluetoothScanner.sendControlData(commandData)
                if (!success) {
                    errorMessage = "Erreur d'envoi de commande simple"
                }
            } catch (e: Exception) {
                errorMessage = "Erreur envoi simple: ${e.message}"
            }
        }
    }

    /**
     * Envoie une commande basée sur les commandes personnalisées
     */
    fun sendCustomCommandsData(customCommands: List<com.example.standbyapp.Settings.CustomCommand>, buttonStates: Map<Int, Boolean>) {
        if (!isConnected || !isAuthenticated) {
            errorMessage = "Non connecté ou non authentifié"
            Log.w("BluetoothViewModel", "Tentative d'envoi sans connexion/authentification")
            return
        }
        
        viewModelScope.launch {
            try {
                val commandData = STM32Commands.createAppCommandFromCustomCommands(customCommands, buttonStates)
                Log.d("BluetoothViewModel", "Envoi de commandes personnalisées: ${commandData.contentToString()}")
                
                val success = bluetoothScanner.sendControlData(commandData)
                if (success) {
                    Log.d("BluetoothViewModel", "Commandes personnalisées envoyées avec succès")
                } else {
                    errorMessage = "Erreur d'envoi de commandes personnalisées"
                    Log.e("BluetoothViewModel", "Échec envoi commandes personnalisées")
                }
            } catch (e: Exception) {
                errorMessage = "Erreur envoi commandes personnalisées: ${e.message}"
                Log.e("BluetoothViewModel", "Exception lors de l'envoi: ${e.message}")
            }
        }
    }
    
    /**
     * Envoie une commande personnalisée
     */
    fun sendCustomCommand(commandData: ByteArray) {
        if (!isConnected || !isAuthenticated) {
            errorMessage = "Non connecté ou non authentifié"
            return
        }
        
        viewModelScope.launch {
            try {
                Log.d("BluetoothViewModel", "Envoi de commande personnalisée: ${commandData.contentToString()}")
                
                val success = bluetoothScanner.sendControlData(commandData)
                if (!success) {
                    errorMessage = "Erreur d'envoi de commande personnalisée"
                }
            } catch (e: Exception) {
                errorMessage = "Erreur envoi commande personnalisée: ${e.message}"
            }
        }
    }
    
//    /**
//     * Commandes rapides
//     */
//    fun turnAllOn(pwmLevel: Int = 50) {
//        val commandData = STM32Commands.createAllOnCommand(pwmLevel)
//        bluetoothScanner.sendControlData(commandData)
//    }
//
//    fun turnAllOff() {
//        val commandData = STM32Commands.createAllOffCommand()
//        bluetoothScanner.sendControlData(commandData)
//    }
//
//    fun controlSingleOutput(outputIndex: Int, isOn: Boolean, pwmValue: Int = 50) {
//        val commandData = STM32Commands.createSingleOutputCommand(outputIndex, isOn, pwmValue)
//        bluetoothScanner.sendControlData(commandData)
//    }
    
    fun showDeviceSelectionDialog() {
        showDeviceDialog = true
    }
    
    fun hideDeviceSelectionDialog() {
        showDeviceDialog = false
    }
    
    fun clearError() {
        errorMessage = null
    }
    
    // Implémentation BluetoothScanCallback
    override fun onScanStarted() {
        viewModelScope.launch {
            try {
                isScanning = true
                Log.d("BluetoothViewModel", "Scan started")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onScanStarted: ${e.message}", e)
            }
        }
    }
    
    override fun onScanStopped() {
        viewModelScope.launch {
            try {
                isScanning = false
                Log.d("BluetoothViewModel", "Scan stopped")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onScanStopped: ${e.message}", e)
            }
        }
    }
    
    override fun onDeviceFound(device: BluetoothDevice) {
        viewModelScope.launch {
            try {
                if (!discoveredDevices.contains(device)) {
                    discoveredDevices = discoveredDevices + device
                    if (ContextCompat.checkSelfPermission(
                            getApplication<Application>().applicationContext,
                            Manifest.permission.BLUETOOTH_CONNECT
                        ) == PackageManager.PERMISSION_GRANTED) {
                        Log.d("BluetoothViewModel", "Device added: ${device.name ?: "Unknown"}")
                    } else {
                        Log.d("BluetoothViewModel", "Device added: Unknown (permission not granted)")
                    }
                }
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onDeviceFound: ${e.message}", e)
            }
        }
    }
    
    override fun onDeviceConnected(device: BluetoothDevice) {
        viewModelScope.launch {
            try {
                isConnected = true
                if (ContextCompat.checkSelfPermission(
                        getApplication<Application>().applicationContext,
                        Manifest.permission.BLUETOOTH_CONNECT
                    ) == PackageManager.PERMISSION_GRANTED) {
                    connectionStatus = "Connected to ${device.name ?: "Unknown"}"
                    Log.d("BluetoothViewModel", "Device connected: ${device.name}")
                } else {
                    connectionStatus = "Connected to Unknown Device"
                    Log.d("BluetoothViewModel", "Device connected: Unknown (permission not granted)")
                }
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onDeviceConnected: ${e.message}", e)
            }
        }
    }
    
    override fun onDeviceDisconnected() {
        viewModelScope.launch {
            try {
                isConnected = false
                isAuthenticated = false
                connectionStatus = "Disconnected"
                Log.d("BluetoothViewModel", "Device disconnected")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onDeviceDisconnected: ${e.message}", e)
            }
        }
    }
    
    override fun onConnectionStatusChanged(isConnected: Boolean, status: String) {
        viewModelScope.launch {
            try {
                this@BluetoothViewModel.isConnected = isConnected
                this@BluetoothViewModel.connectionStatus = status
                Log.d("BluetoothViewModel", "Connection status changed: $status")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onConnectionStatusChanged: ${e.message}", e)
            }
        }
    }
    
    override fun onAuthenticationSuccess() {
        viewModelScope.launch {
            try {
                isAuthenticated = true
                connectionStatus = "Authentication successful"
                Log.d("BluetoothViewModel", "Authentication successful")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onAuthenticationSuccess: ${e.message}", e)
            }
        }
    }
    
    override fun onAuthenticationFailed() {
        viewModelScope.launch {
            try {
                isAuthenticated = false
                errorMessage = "Authentication failed"
                connectionStatus = "Authentication failed"
                Log.e("BluetoothViewModel", "Authentication failed")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onAuthenticationFailed: ${e.message}", e)
            }
        }
    }
    
    override fun onDataReceived(data: ByteArray) {
        viewModelScope.launch {
            try {
                Log.d("BluetoothViewModel", "Data received: ${data.size} bytes")
                // Process received data from STM32 if necessary
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onDataReceived: ${e.message}", e)
            }
        }
    }
    
    override fun onDataSent() {
        viewModelScope.launch {
            try {
                Log.d("BluetoothViewModel", "Data sent successfully")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onDataSent: ${e.message}", e)
            }
        }
    }
    
    override fun onError(message: String) {
        viewModelScope.launch {
            try {
                errorMessage = message
                Log.e("BluetoothViewModel", "Error: $message")
            } catch (e: Exception) {
                Log.e("BluetoothViewModel", "Error in onError callback: ${e.message}", e)
            }
        }
    }
}