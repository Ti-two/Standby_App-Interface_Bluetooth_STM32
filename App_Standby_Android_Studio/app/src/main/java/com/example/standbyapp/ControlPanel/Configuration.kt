package com.example.standbyapp.ControlPanel

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.painter.Painter
import androidx.compose.ui.graphics.vector.ImageVector

// Data class defining control panel option configuration with visual and functional properties
data class Option(
    val id: Int,
    val label: String,
    val activeColor: Color,
    val inactiveTintColor: Color,
    val icon: ImageVector? = null,
    val painterResource: Int? = null, // Changé en Int pour les ressources
    var enabled: Boolean = true, // option active ou non par défaut
    val customCommand: com.example.standbyapp.Settings.CustomCommand? = null // Commande personnalisée associée
)