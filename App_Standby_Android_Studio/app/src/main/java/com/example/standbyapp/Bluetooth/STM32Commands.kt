package com.example.stmbluetooth

import android.util.Log

/**
 * Gestionnaire de commandes SPP pour communication avec STM32
 * Implémente le protocole : //[TAILLE] [FOURCC] [UNLOCK_KEY] [CRC] [DONNÉES] [CRC_FINAL]
 */
object STM32Commands {
    private const val TAG = "STM32Commands"
    
    // Codes de commande (FourCC)
    object BluetoothProtocolCommands {
        const val WRD_KEY_ASK_BT = 0x3F79656B  // "key?" en little endian
        const val WRD_KEY_BT = 0x2E79656B      // "key." en little endian
        const val WRD_DATA_1_BT = 0x3D316164   // "da1=" en little endian
        const val WRD_DATA_1_DOT_BT = 0x2E316164  // "da1." en little endian
        const val WRD_BTN_BT = 0x3D6E7462      // "btn=" en little endian
        const val WRD_CAPP_BT = 0x70706163 //0x50504143         // "capp" en little endian
    }
    
    private var unlockKey: ByteArray = byteArrayOf(0x02, 0x02, 0x02, 0x02,0x02, 0x02, 0x02, 0x02)
    
    /**
     * Met à jour la clé de déverrouillage après authentification
     */
    fun setUnlockKey(key: ByteArray) {
        if (key.size == 8) {
            unlockKey = key.copyOf()
            Log.d(TAG, "Unlock key updated: ${key.joinToString(" ") { "%02X".format(it) }}")
        }
        else {
        Log.w(TAG, "Invalid key size: ${key.size}. Expected 8 bytes.")
    }
    }
    /**
 * Construit une trame SPP complète
 * Format: [FourCC binaire] + [Taille+Clé+Data+Checksum en ASCII continu]
 */
fun buildSPPFrame(fourCC: Int, data: ByteArray = byteArrayOf()): ByteArray {
    // 1. Construire d'abord la trame binaire complète pour calculer les valeurs
    val totalSize = 4 + 2 + 8 + data.size + 2 // Header + Size + Key + Data + Checksum
    val binaryFrame = ByteArray(totalSize)
    var offset = 0
    
    // Header FourCC (little endian)
    binaryFrame[offset++] = (fourCC and 0xFF).toByte()
    binaryFrame[offset++] = ((fourCC shr 8) and 0xFF).toByte()
    binaryFrame[offset++] = ((fourCC shr 16) and 0xFF).toByte()
    binaryFrame[offset++] = ((fourCC shr 24) and 0xFF).toByte()
    
    // Taille de la trame (little endian)
    binaryFrame[offset++] = (totalSize and 0xFF).toByte()
    binaryFrame[offset++] = ((totalSize shr 8) and 0xFF).toByte()
    
    // Clé de déverrouillage (8 octets)
    System.arraycopy(unlockKey, 0, binaryFrame, offset, 8)
    offset += 8
    
    // Données (si présentes)
    if (data.isNotEmpty()) {
        System.arraycopy(data, 0, binaryFrame, offset, data.size)
        offset += data.size
    }
    
    // Checksum temporaire (sera recalculé)
    val checksum = calculateCRC16(binaryFrame, 0, binaryFrame.size - 2)
    binaryFrame[offset++] = (checksum and 0xFF).toByte()
    binaryFrame[offset] = ((checksum shr 8) and 0xFF).toByte()
    
    // 2. Construire la trame finale : Header binaire + reste ASCII
    val headerSize = 4 // Seulement le FourCC
    val asciiSize = (binaryFrame.size - headerSize) * 2 // Chaque byte devient 2 caractères ASCII
    val finalFrame = ByteArray(headerSize + asciiSize)
    
    // 3. Copier le header FourCC en binaire
    System.arraycopy(binaryFrame, 0, finalFrame, 0, headerSize)
    
    // 4. Convertir tout le reste en ASCII hexadécimal continu (SANS SÉPARATEURS)
    var asciiOffset = headerSize
    for (i in headerSize until binaryFrame.size) {
        val byte = binaryFrame[i].toInt() and 0xFF
        val highNibble = (byte shr 4) and 0x0F
        val lowNibble = byte and 0x0F
        
        // Convertir en caractères ASCII hexadécimaux
        finalFrame[asciiOffset++] = if (highNibble < 10) {
            (0x30 + highNibble).toByte() // '0' à '9'
        } else {
            (0x41 + highNibble - 10).toByte() // 'A' à 'F'
        }
        
        finalFrame[asciiOffset++] = if (lowNibble < 10) {
            (0x30 + lowNibble).toByte() // '0' à '9'
        } else {
            (0x41 + lowNibble - 10).toByte() // 'A' à 'F'
        }
    }
    
    // 5. Logs de debug
    Log.d(TAG, "=== SPP FRAME BUILT (HYBRID FORMAT) ===")
    Log.d(TAG, "Total frame size: ${finalFrame.size} bytes")
    Log.d(TAG, "Header (binary): ${finalFrame.take(headerSize).joinToString("") { "%02X".format(it) }}")
    Log.d(TAG, "Rest (ASCII continuous): ${String(finalFrame, headerSize, asciiSize)}")
    Log.d(TAG, "Complete frame preview: ${finalFrame.joinToString("") { 
        if (it.toInt() in 32..126) it.toInt().toChar().toString() else "%02X".format(it) 
    }}")
    Log.d(TAG, "=== END SPP FRAME ===")
    
    return finalFrame
}

/**
 * Calcul du CRC16 (algorithme STM32)
 */
private fun calculateCRC16(data: ByteArray, start: Int, length: Int): Int {
    var crc = 0xFFFF
    for (i in start until start + length) {
        crc = crc xor (data[i].toInt() and 0xFF)
        for (j in 0 until 8) {
            if ((crc and 0x0001) != 0) {
                crc = (crc shr 1) xor 0xA001
            } else {
                crc = crc shr 1
            }
        }
    }
    return crc and 0xFFFF
}
  
    /**
     * Crée une demande d'authentification
     */
    fun createAuthRequest(): ByteArray {
        return buildSPPFrame(BluetoothProtocolCommands.WRD_KEY_ASK_BT)
    }
    

    
    /**
     * Crée une commande de bouton
     * @param buttonId ID du bouton (0-255)
     * @param isPressed true si le bouton est pressé
     * @param duration Durée en millisecondes
     */
    fun createButtonCommand(buttonId: Int, isPressed: Boolean, duration: Int = 500): ByteArray {
        val commandData = ByteArray(4)
        commandData[0] = buttonId.toByte()                    // ID bouton
        commandData[1] = if (isPressed) 0x01 else 0x00        // État (1=pressé, 0=relâché)
        commandData[2] = (duration and 0xFF).toByte()         // Durée (little endian)
        commandData[3] = ((duration shr 8) and 0xFF).toByte()
        
        return buildSPPFrame(BluetoothProtocolCommands.WRD_BTN_BT, commandData)
    }


    /**
     * Parse une réponse d'authentification pour extraire la clé
     * @param response Trame de réponse reçue
     * @return true si la clé a été extraite avec succès
     */
    fun parseAuthResponse(response: ByteArray): Boolean {
        if (response.size < 16) {
            Log.w(TAG, "Auth response too short: ${response.size} bytes")
            return false
        }
        
        // Vérifier le FourCC (octets 2-5)
        val fourCC = (response[2].toInt() and 0xFF) or
                    ((response[3].toInt() and 0xFF) shl 8) or
                    ((response[4].toInt() and 0xFF) shl 16) or
                    ((response[5].toInt() and 0xFF) shl 24)
        
        if (fourCC == BluetoothProtocolCommands.WRD_KEY_BT) {
            // Extraire la clé (octets 12-15 typiquement)
            val keyStart = 12
            if (response.size >= keyStart + 4) {
                val key = response.copyOfRange(keyStart, keyStart + 4)
                setUnlockKey(key)
                Log.d(TAG, "Authentication key extracted successfully")
                return true
            }
        }
        
        Log.w(TAG, "Invalid auth response format")
        return false
    }
    
    /**
     * Utilitaires de débogage pour la connexion Bluetooth
     */
    object DebugUtils {
        
        /**
         * Analyse et affiche le contenu d'une trame reçue
         */
        fun analyzeReceivedFrame(data: ByteArray, source: String = "Unknown") {
            Log.d(TAG, "=== FRAME ANALYSIS from $source ===")
            Log.d(TAG, "Raw data (${data.size} bytes): ${data.joinToString(" ") { "%02X".format(it) }}")
            
            if (data.size < 6) {
                Log.w(TAG, "Frame too short for analysis")
                return
            }
            
            // Analyse de la structure SPP
            val frameSize = (data[0].toInt() and 0xFF) or ((data[1].toInt() and 0xFF) shl 8)
            Log.d(TAG, "Declared frame size: $frameSize bytes")
            
            if (data.size >= 6) {
                val fourCC = (data[2].toInt() and 0xFF) or
                            ((data[3].toInt() and 0xFF) shl 8) or
                            ((data[4].toInt() and 0xFF) shl 16) or
                            ((data[5].toInt() and 0xFF) shl 24)
                
                val fourCCString = String(byteArrayOf(
                    (fourCC and 0xFF).toByte(),
                    ((fourCC shr 8) and 0xFF).toByte(),
                    ((fourCC shr 16) and 0xFF).toByte(),
                    ((fourCC shr 24) and 0xFF).toByte()
                ))
                
                Log.d(TAG, "FourCC: 0x${fourCC.toString(16).uppercase()} ('$fourCCString')")
                
                when (fourCC) {
                    BluetoothProtocolCommands.WRD_KEY_ASK_BT -> Log.d(TAG, "-> AUTH REQUEST")
                    BluetoothProtocolCommands.WRD_KEY_BT -> Log.d(TAG, "-> AUTH RESPONSE")
                    BluetoothProtocolCommands.WRD_DATA_1_BT -> Log.d(TAG, "-> DATA COMMAND")
                    BluetoothProtocolCommands.WRD_BTN_BT -> Log.d(TAG, "-> BUTTON COMMAND")
                    else -> Log.w(TAG, "-> UNKNOWN COMMAND")
                }
            }
            
            if (data.size >= 10) {
                val unlockKey = data.copyOfRange(6, 10)
                Log.d(TAG, "Unlock key: ${unlockKey.joinToString(" ") { "%02X".format(it) }}")
            }
            
            Log.d(TAG, "=== END FRAME ANALYSIS ===")
        }
        
        /**
         * Test de la connexion Bluetooth avec ping simple
         */
        fun createConnectionTestFrame(): ByteArray {
            Log.d(TAG, "Creating connection test frame (auth request)")
            return createAuthRequest()
        }
        
        /**
         * Vérifie si les données reçues correspondent à une réponse valide
         */
        fun isValidResponse(data: ByteArray): Boolean {
            if (data.size < 12) {
                Log.w(TAG, "Response too short: ${data.size} bytes")
                return false
            }
            
            // Vérifier la structure minimale
            val frameSize = (data[0].toInt() and 0xFF) or ((data[1].toInt() and 0xFF) shl 8)
            if (frameSize + 2 != data.size) {
                Log.w(TAG, "Frame size mismatch: declared=$frameSize, actual=${data.size}")
                return false
            }
            
            val fourCC = (data[2].toInt() and 0xFF) or
                        ((data[3].toInt() and 0xFF) shl 8) or
                        ((data[4].toInt() and 0xFF) shl 16) or
                        ((data[5].toInt() and 0xFF) shl 24)
            
            val isKnownCommand = fourCC == BluetoothProtocolCommands.WRD_KEY_BT ||
                               fourCC == BluetoothProtocolCommands.WRD_DATA_1_BT ||
                               fourCC == BluetoothProtocolCommands.WRD_BTN_BT
            
            if (!isKnownCommand) {
                Log.w(TAG, "Unknown FourCC: 0x${fourCC.toString(16).uppercase()}")
                return false
            }
            
            Log.d(TAG, "Valid response detected")
            return true
        }
        
        /**
         * Génère un rapport de statut de connexion
         */
        fun generateConnectionReport(
            isConnected: Boolean,
            deviceName: String?,
            deviceAddress: String?,
            lastDataReceived: Long = 0
        ): String {
            val sb = StringBuilder()
            sb.appendLine("=== BLUETOOTH CONNECTION REPORT ===")
            sb.appendLine("Status: ${if (isConnected) "CONNECTED" else "DISCONNECTED"}")
            sb.appendLine("Device: ${deviceName ?: "Unknown"}")
            sb.appendLine("Address: ${deviceAddress ?: "Unknown"}")
            sb.appendLine("Protocol: SPP (Serial Port Profile)")
            sb.appendLine("UUID: 00001101-0000-1000-8000-00805F9B34FB")
            
            if (lastDataReceived > 0) {
                val timeSince = System.currentTimeMillis() - lastDataReceived
                sb.appendLine("Last data: ${timeSince}ms ago")
            } else {
                sb.appendLine("Last data: Never")
            }
            
            sb.appendLine("Unlock key: ${unlockKey.joinToString(" ") { "%02X".format(it) }}")
            sb.appendLine("=== END REPORT ===")
            
            return sb.toString()
        }
    }
    
    /**
     * Crée une commande APP avec header "CAPP"
     * @param buttonCommands Liste des commandes boutons (BOUTON_ID:6 bits + cmd:2 bits)
     */
    fun createAppCommand(buttonCommands: List<ButtonCommand>): ByteArray { 
        
        val buttonData = ByteArray(buttonCommands.size * 2)
        var offset = 0
        
        for (cmd in buttonCommands) {
            // ID du bouton sur 1 octet (hexa)
            buttonData[offset++] = (cmd.buttonId and 0xFF).toByte()
            
            // Commande correspondante sur 1 octet (hexa)
            buttonData[offset++] = (cmd.command and 0xFF).toByte()
        }
        return buildSPPFrame(BluetoothProtocolCommands.WRD_CAPP_BT, buttonData)
    }

    fun createAppCommandSimple(
        button0Id: Int = 0, button0Cmd: Int = 1,
        button1Id: Int = 1, button1Cmd: Int = 1, 
        button2Id: Int = 2, button2Cmd: Int = 5,
        button3Id: Int = 3, button3Cmd: Int = 6
    ): ByteArray {
        val commands = listOf(
            ButtonCommand(button0Id, button0Cmd),
            ButtonCommand(button1Id, button1Cmd),
            ButtonCommand(button2Id, button2Cmd),
            ButtonCommand(button3Id, button3Cmd)
        )
        return createAppCommand(commands)
    }

    /**
     * Crée une commande APP en temps réel basée sur l'état des boutons du ControlPanel
     * @param buttonStates Liste des états des boutons (id, isActive)
     * @return ByteArray avec la trame SPP complète
     */
    fun createAppCommandFromControlPanel(buttonStates: List<Pair<Int, Boolean>>): ByteArray {
        val commands = buttonStates.map { (buttonId, isActive) ->
            // Logique simplifiée : si actif = commande 1, sinon commande 0
            val command = if (isActive) 1 else 0
            ButtonCommand(buttonId, command)
        }
        
        Log.d(TAG, "Creating APP command from ControlPanel states:")
        buttonStates.forEach { (id, isActive) ->
            Log.d(TAG, "  Button $id: ${if (isActive) "ACTIVE (cmd=1)" else "INACTIVE (cmd=0)"}")
        }
        
        return createAppCommand(commands)
    }

    /**
     * Crée une commande APP en récupérant directement les états depuis le ButtonStateViewModel
     * @param buttonStateViewModel Instance du ViewModel contenant les états des boutons
     * @return ByteArray avec la trame SPP complète
     */
    fun createAppCommandFromViewModel(buttonStateViewModel: com.example.standbyapp.ControlPanel.ButtonStateViewModel): ByteArray {
        val buttonStates = buttonStateViewModel.getAllButtonStates()
        
        Log.d(TAG, "Creating APP command from ButtonStateViewModel:")
        buttonStates.forEach { (id, isActive) ->
            Log.d(TAG, "  Button $id: ${if (isActive) "ACTIVE" else "INACTIVE"}")
        }
        
        return createAppCommandFromControlPanel(buttonStates)
    }

    /**
     * Version synchrone pour récupérer les états actuels depuis les StateFlow
     */
    fun createAppCommandFromCurrentStates(
        emergencyFlow: kotlinx.coroutines.flow.StateFlow<Boolean>,
        frontLightsFlow: kotlinx.coroutines.flow.StateFlow<Boolean>,
        rearLightsFlow: kotlinx.coroutines.flow.StateFlow<Boolean>,
        sirenFlow: kotlinx.coroutines.flow.StateFlow<Boolean>
    ): ByteArray {
        val buttonStates = listOf(
            Pair(0, emergencyFlow.value),
            Pair(1, frontLightsFlow.value),
            Pair(2, rearLightsFlow.value),
            Pair(3, sirenFlow.value)
        )
        
        Log.d(TAG, "Creating APP command from current StateFlow values:")
        buttonStates.forEach { (id, isActive) ->
            Log.d(TAG, "  Button $id: ${if (isActive) "ACTIVE" else "INACTIVE"}")
        }
        
        return createAppCommandFromControlPanel(buttonStates)
    }

    /**
     * Crée une commande APP simplifiée avec états des boutons
     * Utilise un mapping d'ID de boutons prédéfini
     */
    fun createAppCommandFromStates(
        emergencyActive: Boolean = false,
        frontLightsActive: Boolean = false, 
        rearLightsActive: Boolean = false,
        sirenActive: Boolean = false
    ): ByteArray {
        val buttonStates = listOf(
            Pair(0, emergencyActive),     // Emergency
            Pair(1, frontLightsActive),   // Front Lights
            Pair(2, rearLightsActive),    // Rear Lights
            Pair(3, sirenActive)          // Siren
        )
        
        return createAppCommandFromControlPanel(buttonStates)
    }
    
    /**
     * Crée une commande APP à partir d'une Map de boutons
     */
    fun createAppCommandFromButtonMap(buttonStates: Map<String, Boolean>): ByteArray {
        // Mapping des noms de boutons vers leurs IDs
        val buttonMappings = mapOf(
            "emergency" to 0,
            "frontLights" to 1,
            "rearLights" to 2,
            "siren" to 3
        )
        
        val buttonStatesList = mutableListOf<Pair<Int, Boolean>>()
        
        buttonStates.forEach { (buttonName, isActive) ->
            val buttonId = buttonMappings[buttonName.lowercase()]
            if (buttonId != null) {
                buttonStatesList.add(Pair(buttonId, isActive))
                Log.d(TAG, "Button $buttonName (ID: $buttonId): ${if (isActive) "ACTIVE" else "INACTIVE"}")
            } else {
                Log.w(TAG, "Unknown button name: $buttonName")
            }
        }
        
        return createAppCommandFromControlPanel(buttonStatesList)
    }
    
    /**
     * Crée une commande APP à partir d'une liste de commandes personnalisées
     */
    fun createAppCommandFromCustomCommands(customCommands: List<com.example.standbyapp.Settings.CustomCommand>, buttonStates: Map<Int, Boolean>): ByteArray {
        val commands = customCommands.map { customCommand ->
            val isActive = buttonStates[customCommand.id] ?: false
            val commandValue = if (isActive) customCommand.activeCommand else customCommand.inactiveCommand
            ButtonCommand(customCommand.commandId, commandValue)
        }
        
        Log.d(TAG, "Creating APP command from custom commands:")
        commands.forEach { cmd ->
            Log.d(TAG, "  Button ID ${cmd.buttonId}: command=${cmd.command}")
        }
        
        return createAppCommand(commands)
    }
    
    /**
     * Crée une commande APP pour un bouton spécifique avec commande personnalisée
     */
    fun createCustomButtonCommand(customCommand: com.example.standbyapp.Settings.CustomCommand, isActive: Boolean): ByteArray {
        val commandValue = if (isActive) customCommand.activeCommand else customCommand.inactiveCommand
        val buttonCommand = ButtonCommand(customCommand.commandId, commandValue)
        
        Log.d(TAG, "Creating custom button command: ID=${customCommand.commandId}, command=$commandValue, active=$isActive")
        
        return createAppCommand(listOf(buttonCommand))
    }
}


/**
 * Représente une commande bouton pour la trame CAPP
 * @param buttonId ID du bouton (0-63, 6 bits)
 * @param command Commande (0-3, 2 bits)
 */
data class ButtonCommand(
    val buttonId: Int,
    val command: Int
) {
    init {
        require(buttonId in 0..255) { "buttonId doit être entre 0 et 255" }
        require(command in 0..255) { "command doit être entre 0 et 255" }
    }
}
