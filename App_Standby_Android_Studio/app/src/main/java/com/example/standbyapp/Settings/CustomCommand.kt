package com.example.standbyapp.Settings

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import kotlinx.serialization.Serializable

// Data class representing a custom command with visual and functional properties for STM32 communication
@Serializable
data class CustomCommand(
    val id: Int,
    val name: String,
    val colorHex: String, // Color stored in hex format
    val iconResourceId: Int? = null, // Drawable icon resource ID (optional)
    val iconName: String? = null, // Material icon name (optional)
    val commandId: Int, // STM32 command ID (0-255)
    val activeCommand: Int = 1, // Command sent when button is active
    val inactiveCommand: Int = 0, // Command sent when button is inactive
    val description: String = "", // Optional description
    val isEnabled: Boolean = true // Whether the command is enabled
) {
    // Converts hex color string to Compose Color object
    fun getColor(): Color {
        return try {
            Color(android.graphics.Color.parseColor(colorHex))
        } catch (e: Exception) {
            Color.Gray // Default color in case of error
        }
    }
    
    // Validates command parameters are within acceptable ranges
    fun isValid(): Boolean {
        return name.isNotBlank() && 
               commandId in 0..50 &&
               activeCommand in 0..50 &&
               inactiveCommand in 0..50
    }
}

// Object containing predefined default commands for emergency operations
object DefaultCommands {
    val EMERGENCY = CustomCommand(
        id = 0,
        name = "EMERGENCY",
        colorHex = "#F24E4E",
        iconName = "Warning",
        commandId = 0,
        activeCommand = 1,
        inactiveCommand = 0,
        description = "Emergency command"
    )
    
    val FRONT_LIGHTS = CustomCommand(
        id = 1,
        name = "FRONT LIGHTS",
        colorHex = "#43FF8E",
        iconResourceId = com.example.standbyapp.R.drawable.siren,
        commandId = 1,
        activeCommand = 1,
        inactiveCommand = 0,
        description = "front lights"
    )
    
    val REAR_LIGHTS = CustomCommand(
        id = 2,
        name = "REAR LIGHTS",
        colorHex = "#4EC5FF",
        iconResourceId = com.example.standbyapp.R.drawable.siren,
        commandId = 2,
        activeCommand = 1,
        inactiveCommand = 0,
        description = "rear lights"
    )
    
    val SIREN = CustomCommand(
        id = 3,
        name = "SIREN",
        colorHex = "#F34EFF",
        iconResourceId = com.example.standbyapp.R.drawable.siren,
        commandId = 3,
        activeCommand = 1,
        inactiveCommand = 0,
        description = "siren"
    )
    
    // Returns the complete list of predefined default commands
    fun getDefaultCommands(): List<CustomCommand> {
        return listOf(EMERGENCY, FRONT_LIGHTS, REAR_LIGHTS, SIREN)
    }
}

/**
 * Icônes Material disponibles pour les commandes personnalisées
 */
object AvailableIcons {
    data class IconInfo(val name: String, val icon: ImageVector)
    
    val AVAILABLE_ICONS = listOf(
        IconInfo("Warning", Icons.Default.Warning),
        IconInfo("Home", Icons.Default.Home),
        IconInfo("Settings", Icons.Default.Settings),
        IconInfo("Star", Icons.Default.Star),
        IconInfo("Favorite", Icons.Default.Favorite),
        IconInfo("Phone", Icons.Default.Phone),
        IconInfo("Email", Icons.Default.Email),
        IconInfo("Search", Icons.Default.Search),
        IconInfo("Menu", Icons.Default.Menu),
        IconInfo("Check", Icons.Default.Check),
        IconInfo("Clear", Icons.Default.Clear),
        IconInfo("Add", Icons.Default.Add),
        IconInfo("Delete", Icons.Default.Delete), // Remplacer Remove par Delete
        IconInfo("Edit", Icons.Default.Edit),
        IconInfo("Share", Icons.Default.Share),
        IconInfo("PlayArrow", Icons.Default.PlayArrow),
        IconInfo("Refresh", Icons.Default.Refresh), // Enlever Pause et Stop
        IconInfo("Lock", Icons.Default.Lock),
        IconInfo("AccountCircle", Icons.Default.AccountCircle),
        IconInfo("Notifications", Icons.Default.Notifications),
        IconInfo("LocationOn", Icons.Default.LocationOn),
        IconInfo("Build", Icons.Default.Build),
        IconInfo("Info", Icons.Default.Info)
    )
    
    fun getIconByName(name: String): ImageVector? {
        return AVAILABLE_ICONS.find { it.name == name }?.icon
    }
    
    fun getIconNames(): List<String> {
        return AVAILABLE_ICONS.map { it.name }
    }
}

/**
 * Couleurs prédéfinies disponibles
 */
object PresetColors {
    val COLORS = listOf(
        "#F24E4E", // Rouge
        "#43FF8E", // Vert
        "#4EC5FF", // Bleu
        "#F34EFF", // Magenta
        "#FFD700", // Or
        "#FF8C00", // Orange
        "#9932CC", // Violet
        "#00CED1", // Turquoise
        "#FF69B4", // Rose
        "#32CD32", // Vert citron
    )
}
