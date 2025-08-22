package com.example.standbyapp.ControlPanel

import androidx.lifecycle.ViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update

// ViewModel managing button states for control panel with individual and collective state tracking
class ButtonStateViewModel : ViewModel() {
    
    // États des boutons
    private val _buttonStates = MutableStateFlow(mapOf<Int, Boolean>())
    val buttonStates = _buttonStates.asStateFlow()
    
    // États individuels pour faciliter l'accès
    private val _emergencyActive = MutableStateFlow(false)
    val emergencyActive = _emergencyActive.asStateFlow()
    
    private val _frontLightsActive = MutableStateFlow(false)
    val frontLightsActive = _frontLightsActive.asStateFlow()
    
    private val _rearLightsActive = MutableStateFlow(false)
    val rearLightsActive = _rearLightsActive.asStateFlow()
    
    private val _sirenActive = MutableStateFlow(false)
    val sirenActive = _sirenActive.asStateFlow()
    
    // Updates the state of a specific button and syncs individual state variables
    fun updateButtonState(buttonId: Int, isActive: Boolean) {
        _buttonStates.update { currentStates ->
            currentStates.toMutableMap().apply {
                put(buttonId, isActive)
            }
        }
        
        // Mettre à jour les états individuels selon l'ID
        when (buttonId) {
            0 -> _emergencyActive.value = isActive
            1 -> _frontLightsActive.value = isActive
            2 -> _rearLightsActive.value = isActive
            3 -> _sirenActive.value = isActive
        }
    }
    
    // Returns the current state of a specific button by ID
    fun getButtonState(buttonId: Int): Boolean {
        return _buttonStates.value[buttonId] ?: false
    }
    
    // Returns all button states as pairs for STM32 command transmission
    fun getAllButtonStates(): List<Pair<Int, Boolean>> {
        return listOf(
            Pair(0, _emergencyActive.value),
            Pair(1, _frontLightsActive.value),
            Pair(2, _rearLightsActive.value),
            Pair(3, _sirenActive.value)
        )
    }
    
    // Resets all buttons to inactive state
    fun resetAllButtons() {
        _emergencyActive.value = false
        _frontLightsActive.value = false
        _rearLightsActive.value = false
        _sirenActive.value = false
        
        _buttonStates.update { 
            mapOf(0 to false, 1 to false, 2 to false, 3 to false)
        }
    }
}
