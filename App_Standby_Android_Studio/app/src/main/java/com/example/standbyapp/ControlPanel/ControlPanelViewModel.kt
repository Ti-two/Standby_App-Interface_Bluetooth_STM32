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

// ViewModel managing control panel options based on custom commands with Bluetooth integration
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
    
    // Updates options list based on enabled custom commands from settings
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
    
    // Toggles the enabled state of a control option
    fun toggleOption(optionId: Int, enabled: Boolean) {
        customCommandViewModel.toggleCommandEnabled(optionId, enabled)
    }
    
    // Returns the custom command associated with a specific option ID
    fun getCustomCommand(optionId: Int): CustomCommand? {
        return customCommandViewModel.getCommandById(optionId)
    }
    
    // Updates the active state of a specific button
    fun updateButtonState(buttonId: Int, isActive: Boolean) {
        val currentStates = _buttonStates.value.toMutableMap()
        currentStates[buttonId] = isActive
        _buttonStates.value = currentStates
    }
    
    // Returns the current state of a specific button
    fun getButtonState(buttonId: Int): Boolean {
        return _buttonStates.value[buttonId] ?: false
    }
    
    // Sends custom commands via Bluetooth with current button states
    fun sendBluetoothCommands(bluetoothViewModel: com.example.standbyapp.Bluetooth.BluetoothViewModel) {
        val enabledCommands = customCommandViewModel.customCommands.value.filter { it.isEnabled }
        val currentStates = _buttonStates.value
        
        bluetoothViewModel.sendCustomCommandsData(enabledCommands, currentStates)
    }
    
    // Returns enabled commands and current states for Bluetooth keep-alive transmission
    fun getKeepAliveData(): Pair<List<CustomCommand>, Map<Int, Boolean>> {
        val enabledCommands = customCommandViewModel.customCommands.value.filter { it.isEnabled }
        val currentStates = _buttonStates.value
        return Pair(enabledCommands, currentStates)
    }
}
