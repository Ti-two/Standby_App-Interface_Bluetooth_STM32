package com.example.standbyapp.Settings

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import android.util.Log

private val Context.customCommandDataStore: DataStore<Preferences> by preferencesDataStore(name = "custom_commands")

/**
 * ViewModel pour gérer les commandes personnalisées
 */
class CustomCommandViewModel(private val context: Context) : ViewModel() {
    
    companion object {
        private const val TAG = "CustomCommandViewModel"
        private val CUSTOM_COMMANDS_KEY = stringPreferencesKey("custom_commands")
    }
    
    private val json = Json { 
        ignoreUnknownKeys = true
        encodeDefaults = true
    }
    
    // État des commandes personnalisées
    private val _customCommands = MutableStateFlow<List<CustomCommand>>(DefaultCommands.getDefaultCommands())
    val customCommands: StateFlow<List<CustomCommand>> = _customCommands.asStateFlow()
    
    // État pour la boîte de dialogue d'édition
    private val _showEditDialog = MutableStateFlow(false)
    val showEditDialog: StateFlow<Boolean> = _showEditDialog.asStateFlow()
    
    private val _editingCommand = MutableStateFlow<CustomCommand?>(null)
    val editingCommand: StateFlow<CustomCommand?> = _editingCommand.asStateFlow()
    
    // État pour les erreurs
    private val _errorMessage = MutableStateFlow<String?>(null)
    val errorMessage: StateFlow<String?> = _errorMessage.asStateFlow()
    
    init {
        // Démarrer le chargement de manière asynchrone
        initializeCommands()
    }
    
    /**
     * Initialise les commandes de manière asynchrone
     */
    private fun initializeCommands() {
        viewModelScope.launch {
            loadCustomCommands()
        }
    }
    
    /**
     * Charge les commandes personnalisées depuis DataStore
     */
    private suspend fun loadCustomCommands() {
        try {
            val prefs = context.customCommandDataStore.data.first()
            val commandsJson = prefs[CUSTOM_COMMANDS_KEY]
            
            if (commandsJson != null) {
                val commands = json.decodeFromString<List<CustomCommand>>(commandsJson)
                _customCommands.value = commands
                Log.d(TAG, "Loaded ${commands.size} custom commands")
            } else {
                // Première utilisation : initialiser avec les commandes par défaut
                val defaultCommands = DefaultCommands.getDefaultCommands()
                _customCommands.value = defaultCommands
                saveCustomCommands(defaultCommands)
                Log.d(TAG, "Initialized with default commands")
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error loading custom commands", e)
            _errorMessage.value = "Erreur lors du chargement des commandes: ${e.message}"
            // En cas d'erreur, utiliser les commandes par défaut
            val defaultCommands = DefaultCommands.getDefaultCommands()
            _customCommands.value = defaultCommands
            Log.d(TAG, "Fallback to default commands due to error")
        }
    }
    
    /**
     * Sauvegarde les commandes personnalisées dans DataStore
     */
    private fun saveCustomCommands(commands: List<CustomCommand>) {
        viewModelScope.launch {
            try {
                val commandsJson = json.encodeToString(commands)
                context.customCommandDataStore.edit { prefs ->
                    prefs[CUSTOM_COMMANDS_KEY] = commandsJson
                }
                Log.d(TAG, "Saved ${commands.size} custom commands")
            } catch (e: Exception) {
                Log.e(TAG, "Error saving custom commands", e)
                _errorMessage.value = "Erreur lors de la sauvegarde: ${e.message}"
            }
        }
    }
    
    /**
     * Ajoute ou met à jour une commande personnalisée
     */
    fun saveCommand(command: CustomCommand) {
        if (!command.isValid()) {
            _errorMessage.value = "Commande invalide: vérifiez tous les champs"
            return
        }
        
        val currentCommands = _customCommands.value.toMutableList()
        val existingIndex = currentCommands.indexOfFirst { it.id == command.id }
        
        if (existingIndex >= 0) {
            // Mise à jour d'une commande existante
            currentCommands[existingIndex] = command
            Log.d(TAG, "Updated command ${command.id}: ${command.name}")
        } else {
            // Nouvelle commande
            currentCommands.add(command)
            Log.d(TAG, "Added new command ${command.id}: ${command.name}")
        }
        
        _customCommands.value = currentCommands
        saveCustomCommands(currentCommands)
        closeEditDialog()
    }
    
    /**
     * Supprime une commande personnalisée
     */
    fun deleteCommand(commandId: Int) {
        val currentCommands = _customCommands.value.toMutableList()
        val removed = currentCommands.removeAll { it.id == commandId }
        
        if (removed) {
            _customCommands.value = currentCommands
            saveCustomCommands(currentCommands)
            Log.d(TAG, "Deleted command $commandId")
        }
    }
    
    /**
     * Active/désactive une commande
     */
    fun toggleCommandEnabled(commandId: Int, enabled: Boolean) {
        val currentCommands = _customCommands.value.toMutableList()
        val commandIndex = currentCommands.indexOfFirst { it.id == commandId }
        
        if (commandIndex >= 0) {
            currentCommands[commandIndex] = currentCommands[commandIndex].copy(isEnabled = enabled)
            _customCommands.value = currentCommands
            saveCustomCommands(currentCommands)
            Log.d(TAG, "Command $commandId enabled: $enabled")
        }
    }
    
    /**
     * Ouvre la boîte de dialogue pour éditer une commande
     */
    fun editCommand(command: CustomCommand) {
        _editingCommand.value = command
        _showEditDialog.value = true
    }
    
    /**
     * Ouvre la boîte de dialogue pour créer une nouvelle commande
     */
    fun createNewCommand() {
        val nextId = getNextAvailableId()
        val newCommand = CustomCommand(
            id = nextId,
            name = "New command",
            colorHex = PresetColors.COLORS.first(),
            iconName = "Star", // Icône par défaut
            commandId = nextId,
            activeCommand = 1,
            inactiveCommand = 0
        )
        _editingCommand.value = newCommand
        _showEditDialog.value = true
    }
    
    /**
     * Ferme la boîte de dialogue d'édition
     */
    fun closeEditDialog() {
        _showEditDialog.value = false
        _editingCommand.value = null
    }
    
    /**
     * Efface le message d'erreur
     */
    fun clearError() {
        _errorMessage.value = null
    }
    
    /**
     * Réinitialise aux commandes par défaut
     */
    fun resetToDefaults() {
        val defaultCommands = DefaultCommands.getDefaultCommands()
        _customCommands.value = defaultCommands
        saveCustomCommands(defaultCommands)
        Log.d(TAG, "Reset to default commands")
    }
    
    /**
     * Trouve le prochain ID disponible
     */
    private fun getNextAvailableId(): Int {
        val existingIds = _customCommands.value.map { it.id }.toSet()
        var nextId = 0
        while (existingIds.contains(nextId)) {
            nextId++
        }
        return nextId
    }
    
    /**
     * Récupère les commandes activées pour le panneau de contrôle
     */
    fun getEnabledCommands(): List<CustomCommand> {
        return _customCommands.value.filter { it.isEnabled }
    }
    
    /**
     * Récupère une commande par son ID
     */
    fun getCommandById(id: Int): CustomCommand? {
        return _customCommands.value.find { it.id == id }
    }
}
