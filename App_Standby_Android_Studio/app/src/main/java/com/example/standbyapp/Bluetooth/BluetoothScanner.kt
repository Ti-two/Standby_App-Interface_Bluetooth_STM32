package com.example.stmbluetooth

import android.annotation.SuppressLint
import android.bluetooth.*
import android.bluetooth.le.*
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Handler
import android.os.Looper
import android.util.Log
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.util.*
import java.util.Timer
import java.util.TimerTask

@SuppressLint("MissingPermission")
class BluetoothScanner(
    private val context: Context,
    private val callback: BluetoothScanCallback,
    private var authenticationCode: String = "BT12345"
) {
    companion object {
        private const val TAG = "BluetoothScanner"
        private const val SCAN_TIMEOUT = 12000L // 12 secondes
        private const val MAX_SCANS_PER_WINDOW = 5 // Android 15: max 5 scans
        private const val SCAN_WINDOW_DURATION = 30000L // 30 secondes
        private const val MIN_SCAN_INTERVAL = 6000L // 6 secondes minimum entre scans
        
        // Protocole STM32/BX310X
        const val STM32_PASSWORD = "BT12345"
        const val PASSWORD_SIZE = 8
        
        // Headers de trames selon votre protocole
        private const val WRD_KEY_ASK_BT = "key?"
        private const val WRD_KEY_RESPONSE_BT = "key."
        private const val WRD_DATA_1_BT = "da1="
        private const val WRD_BTN_BT = "btn="
        private const val WRD_CAPP_BT = "CAPP"  // Corrigé en majuscules
        
        // UUID pour SPP (Serial Port Profile) - Compatible BX310X
        private val SPP_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
        // BX310X specific BLE service and characteristic UUIDs
        val BLE_SERVICE_UUID: UUID = UUID.fromString("0000FFE0-0000-1000-8000-00805f9b34fb")
        val BLE_CHAR_UUID: UUID = UUID.fromString("0000FFE1-0000-1000-8000-00805f9B34FB")
        
        private const val MAX_GATT_RETRIES = 3
        // Maximum number of retries for transient GATT error 133
        private const val GATT_MAX_RETRY = 3
        
        // Keep-alive system
        private const val KEEP_ALIVE_INTERVAL = 1500L // 1.5 secondes pour être sûr (STM32 timeout = 2s)
        
        // Connection watchdog system
        private const val CONNECTION_TIMEOUT = 3000L // 3 secondes sans réception de trame = déconnexion
        
        // Connection attempt timeout
        private const val CONNECTION_ATTEMPT_TIMEOUT = 10000L // 10 seconds timeout for connection attempt
    }

    private val bluetoothAdapter: BluetoothAdapter? by lazy {
        val bluetoothManager = context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothManager.adapter
    }
    
    // BLE scanner pour la découverte
    private val bleScanner: BluetoothLeScanner? by lazy { bluetoothAdapter?.bluetoothLeScanner }
    
    // Connexion SPP pour BX310X
    private var bluetoothSocket: BluetoothSocket? = null
    private var inputStream: InputStream? = null
    private var outputStream: OutputStream? = null
    private var connectedDevice: BluetoothDevice? = null
    private var isConnected = false
    private var isAuthenticated = false
    
    // Keep-alive system
    
    /**
     * Update the authentication code dynamically
     */
    fun setAuthenticationCode(newCode: String) {
        authenticationCode = newCode
        Log.d(TAG, "Authentication code updated to: $newCode")
    }
    
    /**
     * Send authentication response with key. header when device asks for key
     */
    private fun sendAuthenticationResponse() {
        Log.d(TAG, "🔑 SENDING AUTHENTICATION RESPONSE")
        val authFrame = createAuthenticationResponseFrame()
        Log.d(TAG, "🔑 Auth response frame created: ${authFrame.size} bytes")
        sendFrame(authFrame)
    }
    
    /**
     * Create authentication response frame with "key." header and selected authentication code
     */
    private fun createAuthenticationResponseFrame(): ByteArray {
        Log.d(TAG, "🔑 Creating authentication response with key: $authenticationCode")
        // Utilise le header de réponse key. avec le code d'authentification sélectionné
        val frame = STM32Commands.buildSPPFrame(
            STM32Commands.BluetoothProtocolCommands.WRD_KEY_BT,
            authenticationCode.toByteArray()
        )
        Log.d(TAG, "🔑 Authentication response frame created: ${frame.size} bytes with header 'key.'")
        return frame
    }
    private var keepAliveTimer: Timer? = null
    
    // Connection watchdog system
    private var connectionWatchdogTimer: Timer? = null
    private var lastFrameReceivedTime: Long = 0
    private val connectionWatchdogHandler = Handler(Looper.getMainLooper())
    
    // ButtonStateViewModel pour les keep-alive avec états réels (ancien système)
    private lateinit var buttonStateViewModel: com.example.standbyapp.ControlPanel.ButtonStateViewModel
    
    // ControlPanelViewModel pour les keep-alive avec le nouveau système
    private lateinit var controlPanelViewModel: com.example.standbyapp.ControlPanel.ControlPanelViewModel
    
    // BLE GATT callback & fields
    private var bluetoothGatt: BluetoothGatt? = null
    private var txCharacteristic: BluetoothGattCharacteristic? = null
    // Counter for GATT retries
    private var gattRetryCount = 0
    private var bleDevice: BluetoothDevice? = null
    private var connectionRetryCount = 0
    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (status != BluetoothGatt.GATT_SUCCESS) {
                // handle transient error 133 with limited retries
                if (status == 133 && gattRetryCount < GATT_MAX_RETRY) {
                    gattRetryCount++
                    Log.w(TAG, "Transient GATT error 133, retrying connection ($gattRetryCount/$GATT_MAX_RETRY)")
                    gatt.close()
                    gatt.device.connectGatt(context, false, this)
                    return
                }
                Log.e(TAG, "GATT connection error: status=$status, state=$newState")
                callback.onError("GATT connection error: $status")
                return
            }
            // reset retry counter on any successful operation
            gattRetryCount = 0
             if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_CONNECTED) {
                 Log.d(TAG, "GATT connected, discovering services")
                 callback.onDeviceConnected(gatt.device)
                 gatt.discoverServices()
             } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                 Log.d(TAG, "GATT disconnected")
                 callback.onDeviceDisconnected()
             }
         }
         override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status != BluetoothGatt.GATT_SUCCESS) {
                Log.e(TAG, "Service discovery failed with status $status")
                // Clean up and notify error
                gatt.disconnect()
                gatt.close()
                callback.onError("GATT service discovery error: $status")
                return
            }
             Log.d(TAG, "onServicesDiscovered: status=$status")
             if (status == BluetoothGatt.GATT_SUCCESS) {
                 val service = gatt.getService(BLE_SERVICE_UUID)
                 if (service == null) {
                     Log.w(TAG, "BX310X service $BLE_SERVICE_UUID not found. Listing all services:")
                     gatt.services.forEach { s ->
                         Log.w(TAG, "Service found: ${s.uuid}")
                         s.characteristics.forEach { c -> Log.w(TAG, " Characteristic: ${c.uuid}") }
                     }
                     callback.onError("BX310X BLE service not found. Module may not be in BLE mode.")
                     gatt.close()
                     return
                 }
                
                txCharacteristic = service.getCharacteristic(BLE_CHAR_UUID)
                if (txCharacteristic == null) {
                    callback.onError("BX310X BLE characteristic not found: $BLE_CHAR_UUID.")
                    gatt.close()
                    return
                }
                
                Log.d(TAG, "Found BX310X service and characteristic")
                // Characteristic found
                txCharacteristic?.let {
                    gatt.setCharacteristicNotification(it, true)
                    // Enable descriptor for notifications
                    it.descriptors?.forEach { desc ->
                        desc.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                        gatt.writeDescriptor(desc)
                    }
                    // Send authentication - just the password for BLE
                    val password = authenticationCode.toByteArray()
                    it.value = password
                    Log.d(TAG, "Sending BLE authentication: $authenticationCode")
                    gatt.writeCharacteristic(it)
                } ?: callback.onError("Characteristic non trouvée")
            } else callback.onError("Services GATT non disponibles")
        }
        override fun onCharacteristicChanged(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic) {
            val data = characteristic.value
            val header = String(data, 0, minOf(4, data.size))
            when (header) {
                WRD_KEY_RESPONSE_BT -> {
                    isAuthenticated = true
                    callback.onAuthenticationSuccess()
                }
                "key!" -> {
                    isAuthenticated = false
                    callback.onAuthenticationFailed()
                    gatt.disconnect()
                }
                else -> callback.onDataReceived(data)
            }
        }
        override fun onCharacteristicWrite(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) {
            Log.d(TAG, "onCharacteristicWrite: uuid=${characteristic.uuid}, status=$status")
             if (status == BluetoothGatt.GATT_SUCCESS) callback.onDataSent()
             else callback.onError("Échec writeCharacteristic: $status")
         }
        override fun onDescriptorWrite(gatt: BluetoothGatt, descriptor: BluetoothGattDescriptor, status: Int) {
            Log.d(TAG, "onDescriptorWrite: uuid=${descriptor.uuid}, status=$status")
        }
    }

    // Callback pour le scan BLE
    private val bleScanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult) {
            // Debug: print advertisement data
            result.scanRecord?.serviceUuids?.let { uuids ->
                Log.d(TAG, "Advertised services: ${uuids.joinToString()}")
            }
            result.scanRecord?.manufacturerSpecificData?.let { msd ->
                for (i in 0 until msd.size()) {
                    val id = msd.keyAt(i)
                    val data = msd.valueAt(i)
                    Log.d(TAG, "Manufacturer data (id=$id): ${data.joinToString()}")
                }
            }
             val device = result.device
             val name = device.name ?: "Unknown"
             Log.d(TAG, "BLE device found: $name - ${device.address}")
            
            // Filtrer les modules BX310X/STM32
            if (isBX310Device(device)) {
                Log.d(TAG, "BX310X device detected: $name")
                callback.onDeviceFound(device)
            }
        }
        
        override fun onScanFailed(errorCode: Int) {
            Log.e(TAG, "BLE scan failed: $errorCode")
            callback.onError("BLE scan failed: $errorCode")
        }
    }
    
    private var isScanning = false
    private val scanTimeoutHandler = Handler(Looper.getMainLooper())
    
    // Limitation des scans pour Android 15
    private val scanTimestamps = mutableListOf<Long>()
    private var lastScanTime = 0L
    
    // Classic discovery receiver for fallback scan
    private val discoveryReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                BluetoothDevice.ACTION_FOUND -> {
                    val device: BluetoothDevice? = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)
                    device?.let {
                        if (isBX310Device(it)) callback.onDeviceFound(it)
                    }
                }
                BluetoothAdapter.ACTION_DISCOVERY_FINISHED -> {
                    Log.d(TAG, "Classic scan finished")
                    callback.onScanStopped()
                    context.unregisterReceiver(this)
                    isScanning = false
                }
            }
        }
    }

    fun startScan(): Boolean {
        Log.d(TAG, "Starting classic Bluetooth discovery...")
        
        val adapter = bluetoothAdapter
        if (adapter == null) {
            callback.onError("Bluetooth non supporté")
            return false
        }
        
        if (!BluetoothPermissionManager.hasBluetoothPermissions(context)) {
            callback.onError("Permissions Bluetooth manquantes")
            return false
        }
        
        if (!adapter.isEnabled) {
            callback.onError("Bluetooth n'est pas activé")
            return false
        }
        
        if (isScanning) {
            Log.d(TAG, "Scan already in progress")
            callback.onError("Un scan est déjà en cours")
            return false
        }
        
        // Vérification des limitations Android 15
        if (!canStartScan()) {
            val remainingTime = getRemainingCooldownTime()
            callback.onError("Limitation de scan: attendez ${remainingTime}s avant le prochain scan")
            return false
        }
        
        try {
            // Use classic Bluetooth discovery for SPP devices
            context.registerReceiver(
                discoveryReceiver,
                IntentFilter(BluetoothDevice.ACTION_FOUND).apply {
                    addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)
                }
            )
            
            recordScanStart()
            isScanning = true
            adapter.startDiscovery()
            callback.onScanStarted()
            
            // Timeout de sécurité
            scanTimeoutHandler.postDelayed({
                if (isScanning) {
                    Log.d(TAG, "Classic scan timeout reached")
                    stopScan()
                }
            }, SCAN_TIMEOUT)
            
            Log.d(TAG, "Classic Bluetooth discovery started successfully")
            return true
        } catch (e: SecurityException) {
            val error = "Permissions insuffisantes: ${e.message}"
            Log.e(TAG, error, e)
            callback.onError(error)
        } catch (e: Exception) {
            Log.e(TAG, "Error starting scan", e)
            callback.onError("Erreur lors du démarrage du scan: ${e.message}")
        }
        return false
    }
    
    private fun canStartScan(): Boolean {
        val currentTime = System.currentTimeMillis()
        
        // Vérifier l'intervalle minimum depuis le dernier scan
        if (currentTime - lastScanTime < MIN_SCAN_INTERVAL) {
            Log.d(TAG, "Scan blocked: minimum interval not reached")
            return false
        }
        
        // Nettoyer les anciens timestamps (plus de 30 secondes)
        cleanOldTimestamps(currentTime)
        
        // Vérifier si on a dépassé la limite de 5 scans en 30 secondes
        if (scanTimestamps.size >= MAX_SCANS_PER_WINDOW) {
            Log.d(TAG, "Scan blocked: too many scans in 30s window (${scanTimestamps.size})")
            return false
        }
        
        return true
    }
    
    private fun recordScanStart() {
        val currentTime = System.currentTimeMillis()
        scanTimestamps.add(currentTime)
        lastScanTime = currentTime
        Log.d(TAG, "Scan recorded: ${scanTimestamps.size}/${MAX_SCANS_PER_WINDOW} scans in window")
    }
    
    private fun cleanOldTimestamps(currentTime: Long) {
        scanTimestamps.removeAll { timestamp ->
            currentTime - timestamp > SCAN_WINDOW_DURATION
        }
    }
    
    private fun getRemainingCooldownTime(): Int {
        val currentTime = System.currentTimeMillis()
        
        // Temps depuis le dernier scan
        val timeSinceLastScan = currentTime - lastScanTime
        val intervalCooldown = (MIN_SCAN_INTERVAL - timeSinceLastScan) / 1000
        
        // Temps jusqu'à ce que le plus ancien scan sorte de la fenêtre
        val windowCooldown = if (scanTimestamps.size >= MAX_SCANS_PER_WINDOW && scanTimestamps.isNotEmpty()) {
            val oldestScan = scanTimestamps.minOrNull() ?: 0L
            ((oldestScan + SCAN_WINDOW_DURATION) - currentTime) / 1000
        } else 0
        
        return maxOf(intervalCooldown.toInt(), windowCooldown.toInt(), 1)
    }
    
    /**
     * Fallback to classic Bluetooth discovery
     */
    private fun startClassicScan() {
        Log.d(TAG, "Starting classic Bluetooth scan (discovery)")
        if (bluetoothAdapter?.isEnabled != true) {
            callback.onError("Bluetooth n'est pas activé pour classic scan")
            return
        }
        // Register receiver for discovery
        context.registerReceiver(
            discoveryReceiver,
            IntentFilter(BluetoothDevice.ACTION_FOUND).apply {
                addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)
            }
        )
        isScanning = true
        callback.onScanStarted()
        bluetoothAdapter?.startDiscovery()
    }

    fun stopScan() {
        if (!isScanning) return
        
        // Stop classic discovery
        bluetoothAdapter?.let { adapter ->
            if (adapter.isDiscovering) adapter.cancelDiscovery()
        }
        
        // Unregister receiver if registered
        try {
            context.unregisterReceiver(discoveryReceiver)
        } catch (e: IllegalArgumentException) {
            // Receiver not registered, ignore
        }
        
        scanTimeoutHandler.removeCallbacksAndMessages(null)
        isScanning = false
        callback.onScanStopped()
        Log.d(TAG, "Classic Bluetooth scan stopped")
    }
    
    fun isScanning(): Boolean = isScanning
    
    /**
     * Vérifie si le device est un module BX310X/STM32
     */
    private fun isBX310Device(device: BluetoothDevice): Boolean {
        val name = device.name ?: return false
        return name.contains("BX310", ignoreCase = true) ||
               name.contains("BX10", ignoreCase = true) || // Include BX10 modules
               name.startsWith("Merc_", ignoreCase = true) || // Include Merc_ device names
               name.contains("STM32", ignoreCase = true) ||
               name.contains("HC-05", ignoreCase = true) ||
               name.contains("HC-06", ignoreCase = true) ||
               name.startsWith("BT", ignoreCase = true)
    }
    
    /**
     * Définit le ButtonStateViewModel pour les keep-alive avec états réels
     */
    fun setButtonStateViewModel(viewModel: com.example.standbyapp.ControlPanel.ButtonStateViewModel) {
        this.buttonStateViewModel = viewModel
        Log.d(TAG, "ButtonStateViewModel set for keep-alive")
    }
    
    /**
     * Définit le ControlPanelViewModel pour les keep-alive avec le nouveau système
     */
    fun setControlPanelViewModel(viewModel: com.example.standbyapp.ControlPanel.ControlPanelViewModel) {
        this.controlPanelViewModel = viewModel
        Log.d(TAG, "ControlPanelViewModel set for keep-alive")
    }
    
    /**
     * Se connecte à un device BX310X via BLE GATT
     */
    /**
     * Connect to device using SPP
     */
    fun connectToDevice(device: BluetoothDevice): Boolean {
        Log.d(TAG, "=== ATTEMPTING SPP CONNECTION ===")
        Log.d(TAG, "Target device: ${device.name} (${device.address})")
        Log.d(TAG, "Device type: ${when(device.type) {
            BluetoothDevice.DEVICE_TYPE_CLASSIC -> "Classic"
            BluetoothDevice.DEVICE_TYPE_LE -> "BLE"
            BluetoothDevice.DEVICE_TYPE_DUAL -> "Dual"
            else -> "Unknown"
        }}")
         Log.d(TAG, "Connecting to device via SPP: ${device.name} (${device.address})")
         stopScan()
         return connectSppToDevice(device)
     }
    
    /**
     * Connexion SPP pour BX310X (legacy)
     */
    @Deprecated("Use connectToDevice which selects BLE GATT or SPP")
    fun connectSppToDevice(device: BluetoothDevice): Boolean {
        Log.d(TAG, "=== STARTING SPP CONNECTION PROCESS ===")
        Log.d(TAG, "Attempting SPP connect to ${device.name} (${device.address})")
        
        // Execute connection on background thread to avoid blocking UI
        Thread {
            var connectionTimeoutHandler: Handler? = null
            var timeoutRunnable: Runnable? = null
            
            try {
                Log.d(TAG, "Stopping any ongoing scan...")
                stopScan()
            
                Log.d(TAG, "Creating RFCOMM socket with SPP UUID...")
                bluetoothSocket = device.createRfcommSocketToServiceRecord(SPP_UUID)
                Log.d(TAG, "Socket created successfully")
                
                // Set up connection timeout
                connectionTimeoutHandler = Handler(Looper.getMainLooper())
                timeoutRunnable = Runnable {
                    Log.w(TAG, "⏰ Connection timeout after ${CONNECTION_ATTEMPT_TIMEOUT}ms")
                    try {
                        bluetoothSocket?.close()
                    } catch (e: Exception) {
                        Log.e(TAG, "Error closing socket on timeout: ${e.message}")
                    }
                    Handler(Looper.getMainLooper()).post {
                        try {
                            callback.onError("Connection timeout: device not responding")
                        } catch (e: Exception) {
                            Log.e(TAG, "Error in timeout callback: ${e.message}", e)
                        }
                    }
                }
                connectionTimeoutHandler.postDelayed(timeoutRunnable, CONNECTION_ATTEMPT_TIMEOUT)
            
                Log.d(TAG, "Attempting socket connection...")
                // This is the blocking operation that was freezing the UI
                bluetoothSocket?.connect()
                
                // Cancel timeout if connection succeeded
                timeoutRunnable?.let { connectionTimeoutHandler.removeCallbacks(it) }
                
                Log.d(TAG, "✅ Socket connected successfully!")
            
                Log.d(TAG, "Initializing streams...")
                inputStream = bluetoothSocket?.inputStream
                outputStream = bluetoothSocket?.outputStream
                connectedDevice = device
                isConnected = true
                Log.d(TAG, "✅ SPP CONNECTION ESTABLISHED to ${device.name}")
                Log.d(TAG, "✅ Streams initialized: IN=${inputStream != null}, OUT=${outputStream != null}")
                
                // Use Handler to ensure callbacks run on main thread
                Handler(Looper.getMainLooper()).post {
                    try {
                        callback.onDeviceConnected(device)
                        callback.onConnectionStatusChanged(true, "Connected via SPP")
                    } catch (e: Exception) {
                        Log.e(TAG, "Error in connection callbacks: ${e.message}", e)
                    }
                }
                
                Log.d(TAG, "Starting data listener...")
                startDataListener()
                
                // Start connection watchdog
                startConnectionWatchdog()
                
                Log.d(TAG, "Sending authentication request...")
                sendAuthenticationRequest()
                
            } catch (e: IOException) {
                // Cancel timeout if still active
                timeoutRunnable?.let { connectionTimeoutHandler?.removeCallbacks(it) }
                
                Log.e(TAG, "❌ SPP CONNECTION FAILED - IOException: ${e.message}", e)
                Log.e(TAG, "Error details: ${e.javaClass.simpleName}")
                disconnect()
                // Use Handler for error callback
                Handler(Looper.getMainLooper()).post {
                    try {
                        callback.onError("SPP connection failed: ${e.message}")
                    } catch (callbackError: Exception) {
                        Log.e(TAG, "Error in error callback: ${callbackError.message}", callbackError)
                    }
                }
            } catch (e: SecurityException) {
                // Cancel timeout if still active
                timeoutRunnable?.let { connectionTimeoutHandler?.removeCallbacks(it) }
                
                Log.e(TAG, "❌ SPP CONNECTION FAILED - SecurityException: ${e.message}", e)
                disconnect()
                // Use Handler for error callback
                Handler(Looper.getMainLooper()).post {
                    try {
                        callback.onError("Permission denied: ${e.message}")
                    } catch (callbackError: Exception) {
                        Log.e(TAG, "Error in error callback: ${callbackError.message}", callbackError)
                    }
                }
            } catch (e: Exception) {
                // Cancel timeout if still active
                timeoutRunnable?.let { connectionTimeoutHandler?.removeCallbacks(it) }
                
                Log.e(TAG, "❌ SPP CONNECTION FAILED - Unexpected error: ${e.message}", e)
                disconnect()
                // Use Handler for error callback
                Handler(Looper.getMainLooper()).post {
                    try {
                        callback.onError("Unexpected connection error: ${e.message}")
                    } catch (callbackError: Exception) {
                        Log.e(TAG, "Error in error callback: ${callbackError.message}", callbackError)
                    }
                }
            }
        }.start()
        
        // Return true immediately as connection is now asynchronous
        return true
    }
    
    /**
     * Envoie une demande d'authentification avec mot de passe
     */
    private fun sendAuthenticationRequest() {
        Log.d(TAG, "🔑 SENDING AUTHENTICATION REQUEST")
        Log.d(TAG, "Sending authentication request")
         
        val authFrame = createAuthenticationFrame()
        Log.d(TAG, "🔑 Auth frame created: ${authFrame.size} bytes")
        sendFrame(authFrame)
    }
    
    /**
     * Crée la trame d'authentification selon le protocole STM32
     */
    private fun createAuthenticationFrame(): ByteArray {
            // Utilise la constante FourCC de STM32Commands
    return STM32Commands.buildSPPFrame(
        STM32Commands.BluetoothProtocolCommands.WRD_KEY_ASK_BT,
        authenticationCode.toByteArray()
    )
    }
    

    private fun sendFrame(frame: ByteArray): Boolean {
         if (!isConnected || outputStream == null) {
            Log.e(TAG, "Not connected, cannot send frame")
        Log.e(TAG, "❌ CANNOT SEND FRAME - Not connected or no output stream")
        Log.e(TAG, "   - isConnected: $isConnected")
        Log.e(TAG, "   - outputStream: ${outputStream != null}")
             return false
         }
         
         try {
        Log.d(TAG, "📤 SENDING SPP FRAME: ${frame.size} bytes")
        Log.d(TAG, "📤 Hex data: ${frame.joinToString(" ") { "%02X".format(it) }}")
             outputStream?.write(frame)
             outputStream?.flush()
            Log.d(TAG, "Frame sent: ${frame.size} bytes")
        Log.d(TAG, "✅ Frame sent successfully: ${frame.size} bytes")
             return true
         } catch (e: IOException) {
            Log.e(TAG, "Error sending frame: ${e.message}")
        Log.e(TAG, "❌ FRAME SEND ERROR - IOException: ${e.message}", e)
             callback.onError("Erreur envoi: ${e.message}")
             return false
         }
     }
    
    /**
     * Démarre l'écoute des données en arrière-plan
     */
    private fun startDataListener() {
        Log.d(TAG, "📡 Starting SPP data listener thread...")
         Thread {
             val buffer = ByteArray(1024)
             
             try {
        Log.d(TAG, "📡 Data listener loop started - waiting for incoming data...")
                 while (isConnected && inputStream != null) {
                     val bytesRead = inputStream?.read(buffer) ?: 0
                     
                     if (bytesRead > 0) {
                         val receivedData = buffer.copyOf(bytesRead)
        Log.d(TAG, "📥 SPP DATA RECEIVED: $bytesRead bytes")
        Log.d(TAG, "📥 Raw hex: ${receivedData.joinToString(" ") { "%02X".format(it) }}")
                         processReceivedFrame(receivedData)
        } else if (bytesRead == 0) {
        Log.d(TAG, "📡 No data available, continuing to listen...")
                     }
                 }
        Log.d(TAG, "📡 Data listener loop ended (disconnected or stream closed)")
             } catch (e: IOException) {
                Log.e(TAG, "Data listener error: ${e.message}")
        Log.e(TAG, "❌ SPP DATA LISTENER ERROR: ${e.message}", e)
        Log.e(TAG, "Connection likely lost or interrupted")
                 if (isConnected) {
                     disconnect()
                     // Use Handler for error callback
                     Handler(Looper.getMainLooper()).post {
                         try {
                             callback.onError("Connection lost")
                         } catch (e: Exception) {
                             Log.e(TAG, "Error in connection lost callback: ${e.message}", e)
                         }
                     }
                 }
             }
         }.start()
     }
    
    /**
     * Traite les trames reçues du STM32
     */
    private fun processReceivedFrame(data: ByteArray) {
        Log.d(TAG, "🔍 PROCESSING RECEIVED FRAME: ${data.size} bytes")
        
        // Update last frame received timestamp for connection watchdog
        lastFrameReceivedTime = System.currentTimeMillis()
        
        if (data.size < 4) return
         
         val header = String(data, 0, 4)
        Log.d(TAG, "Received frame with header: $header")
        Log.d(TAG, "🔍 Frame header: '$header'")
           when (header) {
            WRD_KEY_ASK_BT -> {
                Log.d(TAG, "🔑 Authentication request received - responding with selected key")
                // Send authentication response with the selected authentication code
                sendAuthenticationResponse()
            }
            WRD_KEY_RESPONSE_BT -> {
                isAuthenticated = true
                Log.d(TAG, "Authentication successful!")
                Log.d(TAG, "🔐 AUTHENTICATION SUCCESSFUL!")
                
                // Use Handler to ensure callbacks run on main thread
                Handler(Looper.getMainLooper()).post {
                    try {
                        callback.onAuthenticationSuccess()
                        callback.onConnectionStatusChanged(true, "Authenticated with $authenticationCode")
                    } catch (e: Exception) {
                        Log.e(TAG, "Error in authentication success callbacks: ${e.message}", e)
                    }
                }
                
                // Start keep-alive after successful authentication
                startKeepAlive()
            }
            
            WRD_DATA_1_BT -> {
                Log.d(TAG, "Data frame received")
                Log.d(TAG, "📊 Data frame received")
                callback.onDataReceived(data)
            }
            else -> {
                Log.d(TAG, "Unknown frame type: $header")
                Log.w(TAG, "⚠️ Unknown frame type: '$header'")
                Log.w(TAG, "⚠️ Expected: '$WRD_KEY_RESPONSE_BT' or '$WRD_DATA_1_BT'")
                callback.onDataReceived(data)
            }
        }
     }
    
    /**
     * Envoie des données de contrôle au STM32
     */
    fun sendControlData(commandData: ByteArray): Boolean {
        if (!isConnected || !isAuthenticated) {
            Log.e(TAG, "Not connected or authenticated")
            return false
        }
        
        //val controlFrame = createControlFrame(commandData)
        return sendFrame(commandData)
    }
    
    
    /**
     * Crée une trame de contrôle avec mot de passe (utilise la fonction générique)
     */
    private fun createControlFrame(data: ByteArray): ByteArray {
        //return createFrame(WRD_CAPP_BT, data, includePassword = true)
        return STM32Commands.buildSPPFrame(STM32Commands.BluetoothProtocolCommands.WRD_CAPP_BT, data)
    }

    /**
     * Ferme la connexion
     */
    fun disconnect() {
        // Stop keep-alive first
        stopKeepAlive()
        
        // Stop connection watchdog
        stopConnectionWatchdog()

        // Handle BLE GATT disconnect
        bluetoothGatt?.disconnect()
        bluetoothGatt?.close()
        bluetoothGatt = null
        txCharacteristic = null
        // Handle SPP disconnect
        isConnected = false
        isAuthenticated = false
        try {
            inputStream?.close()
            outputStream?.close()
            bluetoothSocket?.close()
        } catch (e: IOException) {
            Log.e(TAG, "Error closing SPP connection: ${e.message}")
        } finally {
            inputStream = null
            outputStream = null
            bluetoothSocket = null
            connectedDevice = null
        }
        Log.d(TAG, "Disconnected all transports")
        // Use Handler to ensure callbacks run on main thread
        Handler(Looper.getMainLooper()).post {
            try {
                callback.onDeviceDisconnected()
                callback.onConnectionStatusChanged(false, "Disconnected")
            } catch (e: Exception) {
                Log.e(TAG, "Error in disconnection callbacks: ${e.message}", e)
            }
        }
    }

    /**
     * Getters pour l'état de la connexion
     */
    fun isConnected(): Boolean = isConnected
    fun isAuthenticated(): Boolean = isAuthenticated
    fun getConnectedDevice(): BluetoothDevice? = connectedDevice

    /**
     * Start the connection watchdog system
     */
    private fun startConnectionWatchdog() {
        stopConnectionWatchdog() // Stop any existing watchdog
        
        if (!isConnected) {
            Log.w(TAG, "Cannot start connection watchdog: not connected")
            return
        }
        
        Log.d(TAG, "🐕 Starting connection watchdog (timeout: ${CONNECTION_TIMEOUT}ms)")
        lastFrameReceivedTime = System.currentTimeMillis()
        
        connectionWatchdogTimer = Timer("SPP-ConnectionWatchdog", true)
        connectionWatchdogTimer?.scheduleAtFixedRate(object : TimerTask() {
            override fun run() {
                if (!isConnected) {
                    stopConnectionWatchdog()
                    return
                }
                
                val currentTime = System.currentTimeMillis()
                val timeSinceLastFrame = currentTime - lastFrameReceivedTime
                
                if (timeSinceLastFrame > CONNECTION_TIMEOUT) {
                    Log.w(TAG, "🐕 Connection timeout detected! No SPP frame received for ${timeSinceLastFrame}ms")
                    Log.w(TAG, "🐕 Forcing disconnection due to timeout")
                    
                    // Force disconnection on main thread
                    connectionWatchdogHandler.post {
                        if (isConnected) {
                            callback.onError("Connection lost: no data received for 3 seconds")
                            disconnect()
                        }
                    }
                } else {
                    Log.d(TAG, "🐕 Connection alive - last frame: ${timeSinceLastFrame}ms ago")
                }
            }
        }, CONNECTION_TIMEOUT, 1000) // Check every second
    }
    
    /**
     * Stop the connection watchdog system
     */
    private fun stopConnectionWatchdog() {
        connectionWatchdogTimer?.cancel()
        connectionWatchdogTimer = null
        connectionWatchdogHandler.removeCallbacksAndMessages(null)
        Log.d(TAG, "🐕 Connection watchdog stopped")
    }

    /**
     * Start automatic keep-alive system
     */
    /**
     * Start automatic keep-alive system
     */
    private fun startKeepAlive() {
        stopKeepAlive() // Stop any existing timer

        if (!isConnected || !isAuthenticated) {
            Log.w(TAG, "Cannot start keep-alive: not connected or not authenticated")
            return
        }

        Log.d(TAG, "🔄 Starting keep-alive system (every ${KEEP_ALIVE_INTERVAL}ms)")

        keepAliveTimer = Timer("STM32-KeepAlive", true)
        keepAliveTimer?.scheduleAtFixedRate(object : TimerTask() {
            override fun run() {
                if (isConnected && isAuthenticated) {
                    sendKeepAliveCommand()
                } else {
                    Log.d(TAG, "🔄 Keep-alive stopped: connection lost")
                    stopKeepAlive()
                }
            }
        }, KEEP_ALIVE_INTERVAL, KEEP_ALIVE_INTERVAL)
    }

    /**
     * Stop the keep-alive system
     */
    private fun stopKeepAlive() {
        keepAliveTimer?.cancel()
        keepAliveTimer = null
        Log.d(TAG, "🔄 Keep-alive system stopped")
    }

    /**
     * Send a keep-alive command
     */
    private fun sendKeepAliveCommand() {
        try {
            when {
                // New system with ControlPanelViewModel
                ::controlPanelViewModel.isInitialized -> {
                    val (enabledCommands, buttonStates) = controlPanelViewModel.getKeepAliveData()
                    val customCommandsData = STM32Commands.createAppCommandFromCustomCommands(enabledCommands, buttonStates)
                    sendFrame(customCommandsData)
                    Log.d("BluetoothScanner", "Keep-alive sent with custom commands (${enabledCommands.size} commands)")
                }
                // Old system with ButtonStateViewModel (fallback)
                ::buttonStateViewModel.isInitialized -> {
                    val currentStates = buttonStateViewModel.buttonStates.value
                    Log.d("BluetoothScanner", "Keep-alive with ButtonStateViewModel: $currentStates")
                    
                    if (currentStates.any { it.value }) {
                        // There are active buttons, use simple states
                        val commandData = STM32Commands.createAppCommandFromStates(
                            emergencyActive = currentStates[0] ?: false,
                            frontLightsActive = currentStates[1] ?: false,
                            rearLightsActive = currentStates[2] ?: false,
                            sirenActive = currentStates[3] ?: false
                        )
                        sendFrame(commandData)
                    } else {
                        // No active buttons, send simple keep-alive
                        val keepAliveData = STM32Commands.buildSPPFrame(STM32Commands.BluetoothProtocolCommands.WRD_CAPP_BT, "KEEP_ALIVE".toByteArray())
                        sendFrame(keepAliveData)
                    }
                }
                // Fallback - simple keep-alive
                else -> {
                    val keepAliveData = STM32Commands.buildSPPFrame(STM32Commands.BluetoothProtocolCommands.WRD_CAPP_BT, "KEEP_ALIVE".toByteArray())
                    sendFrame(keepAliveData)
                    Log.d("BluetoothScanner", "Simple keep-alive sent (no ViewModel configured)")
                }
            }
        } catch (e: Exception) {
            Log.e("BluetoothScanner", "Error sending keep-alive: ${e.message}")
            // In case of error, send basic keep-alive
            try {
                val keepAliveData = STM32Commands.buildSPPFrame(STM32Commands.BluetoothProtocolCommands.WRD_CAPP_BT, "KEEP_ALIVE".toByteArray())
                sendFrame(keepAliveData)
            } catch (fallbackError: Exception) {
                Log.e("BluetoothScanner", "Critical keep-alive error: ${fallbackError.message}")
            }
        }
    }
}