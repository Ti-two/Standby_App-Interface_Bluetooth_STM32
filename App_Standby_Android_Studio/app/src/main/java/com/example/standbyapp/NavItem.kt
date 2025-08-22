package com.example.standbyapp

import androidx.annotation.DrawableRes
import androidx.compose.ui.graphics.painter.Painter
import androidx.compose.ui.graphics.vector.ImageVector

// Data class representing navigation bar items with label, icon and route
data class NavItem(
    val label: String,
    @DrawableRes val iconRes: Int,
    val route: String

)
