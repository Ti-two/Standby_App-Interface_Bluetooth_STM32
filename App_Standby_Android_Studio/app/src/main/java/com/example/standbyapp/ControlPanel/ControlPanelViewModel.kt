package com.example.standbyapp.ControlPanel

import android.content.Context
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Warning
import androidx.compose.ui.graphics.Color
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import com.example.standbyapp.R
import com.example.standbyapp.Settings.CustomCommand
import com.example.standbyapp.Settings.CustomCommandViewModel

// ViewModel gérant les options basé sur les commandes personnalisées
class ControlPanelViewModel(
    private val context: Context,
    private val customCommandViewModel: CustomCommandViewModel
) : ViewModel() {
    
    private val _options = MutableStateFlow<List<Option>>(emptyList())
    val options = _options.asStateFlow()
    
    // États des boutons (ID -> isActive)
    private val _buttonStates = MutableStateFlow<Map<Int, Boolean>>(emptyMap())
    val buttonStates = _buttonStates.asStateFlow()
    
    init {
        // Observer les changements des commandes personnalisées
        viewModelScope.launch {
            customCommandViewModel.customCommands.collect { customCommands ->
                updateOptionsFromCustomCommands(customCommands)
                // Initialiser les états des boutons
                val initialStates = customCommands.associate { it.id to false }
                _buttonStates.value = initialStates
            }
        }
    }
    
    /**
     * Met à jour les options basées sur les commandes personnalisées
     */
    private fun updateOptionsFromCustomCommands(customCommands: List<CustomCommand>) {
        val options = customCommands.filter { it.isEnabled }.map { command ->
            Option(
                id = command.id,
                label = command.name,
                activeColor = command.getColor(),
                inactiveTintColor = command.getColor(),
                icon = command.iconName?.let { 
                    com.example.standbyapp.Settings.AvailableIcons.getIconByName(it) 
                },
                painterResource = command.iconResourceId,
                enabled = true, // Déjà filtré les commandes activées
                customCommand = command
            )
        }
        _options.value = options
    }
    
    /**
     * Permet d'activer ou désactiver une option
     */
    fun toggleOption(optionId: Int, enabled: Boolean) {
        customCommandViewModel.toggleCommandEnabled(optionId, enabled)
    }
    
    /**
     * Récupère la commande personnalisée associée à une option
     */
    fun getCustomCommand(optionId: Int): CustomCommand? {
        return customCommandViewModel.getCommandById(optionId)
    }
    
    /**
     * Met à jour l'état d'un bouton
     */
    fun updateButtonState(buttonId: Int, isActive: Boolean) {
        val currentStates = _buttonStates.value.toMutableMap()
        currentStates[buttonId] = isActive
        _buttonStates.value = currentStates
    }
    
    /**
     * Récupère l'état actuel d'un bouton
     */
    fun getButtonState(buttonId: Int): Boolean {
        return _buttonStates.value[buttonId] ?: false
    }
    
    /**
     * Envoie les commandes personnalisées via Bluetooth
     */
    fun sendBluetoothCommands(bluetoothViewModel: com.example.standbyapp.Bluetooth.BluetoothViewModel) {
        val enabledCommands = customCommandViewModel.customCommands.value.filter { it.isEnabled }
        val currentStates = _buttonStates.value
        
        bluetoothViewModel.sendCustomCommandsData(enabledCommands, currentStates)
    }
    
    /**
     * Récupère les commandes personnalisées activées et leurs états pour le keep-alive
     */
    fun getKeepAliveData(): Pair<List<CustomCommand>, Map<Int, Boolean>> {
        val enabledCommands = customCommandViewModel.customCommands.value.filter { it.isEnabled }
        val currentStates = _buttonStates.value
        return Pair(enabledCommands, currentStates)
    }
}
