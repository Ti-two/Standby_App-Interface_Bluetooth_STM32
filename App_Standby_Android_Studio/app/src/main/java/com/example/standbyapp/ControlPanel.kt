package com.example.standbyapp

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Checkbox
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

import androidx.compose.runtime.getValue
import androidx.compose.runtime.*
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.graphics.painter.Painter
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.semantics.Role.Companion.Checkbox
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.TextUnit
import androidx.lifecycle.ViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update

import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.lazy.items
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.standbyapp.ControlPanel.ButtonStateViewModel
import com.example.standbyapp.ControlPanel.ControlPanelViewModel
import com.example.standbyapp.Bluetooth.BluetoothViewModel
import android.util.Log
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.ui.text.style.TextAlign





@Composable
fun DynamicControlPanel(
    viewModel: ControlPanelViewModel, 
    bluetoothViewModel: BluetoothViewModel? = null
) {
    val options by viewModel.options.collectAsState()
    val buttonStates by viewModel.buttonStates.collectAsState()
    val activeOptions = options.filter { it.enabled }
    
    // Configurer le ControlPanelViewModel dans le BluetoothViewModel pour les keep-alive
    LaunchedEffect(bluetoothViewModel, viewModel) {
        if (bluetoothViewModel != null) {
            bluetoothViewModel.setControlPanelViewModel(viewModel)
            Log.d("ControlPanel", "ControlPanelViewModel configured in BluetoothViewModel for keep-alive")
        } else {
            Log.w("ControlPanel", "BluetoothViewModel is null, unable to configure keep-alive")
        }
    }

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF00041A))
            .padding(16.dp)
            .navigationBarsPadding()
    ) {
        // Connection status indicator in top right
        bluetoothViewModel?.let { btViewModel ->
            ConnectionStatusIndicator(
                isConnected = btViewModel.isConnected,
                connectionStatus = btViewModel.connectionStatus,
                modifier = Modifier.align(Alignment.TopEnd)
            )
        }
        
        // Main content centered
        Column(
            modifier = Modifier.fillMaxSize(),
            verticalArrangement = Arrangement.spacedBy(16.dp, Alignment.CenterVertically),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            // Only show the car image if there are 10 or fewer active buttons
            if (activeOptions.size <= 10) {
                Image(
                    painter = painterResource(id = R.drawable.car),
                    contentDescription = "Emergency Car",
                    modifier = Modifier.size(200.dp)
                )
            }

            activeOptions.chunked(2).forEach { rowOptions ->
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(16.dp)
                ) {
                    rowOptions.forEachIndexed { _, option ->
                        val painter = option.painterResource?.let { painterResource(id = it) }
                        val buttonModifier = if (rowOptions.size == 1) {
                            Modifier
                                .fillMaxWidth()
                                .padding(vertical = 8.dp)

                        } else {
                            Modifier
                                .weight(1f)
                                .padding(vertical = 8.dp)
                        }
                        ConfigurableButton(
                            buttonId = option.id,
                            controlPanelViewModel = viewModel,
                            label = option.label,
                            activeColor = option.activeColor,
                            inactiveTintColor = option.inactiveTintColor,
                            icon = option.icon,
                            painterResource = painter,
                            bluetoothViewModel = bluetoothViewModel,
                            onToggle = { _ ->
                                Log.d("ControlPanel", "Button toggle handled")
                            },
                            modifier = buttonModifier
                        )
                    }
                    // Si la ligne a un seul bouton, on n'ajoute pas de Spacer !
                }
            }
        }
    }
}


@Composable
fun ConfigurableButton(
    buttonId: Int,
    controlPanelViewModel: ControlPanelViewModel,
    label: String,
    activeColor: Color,
    inactiveTintColor: Color,
    onToggle: (Boolean) -> Unit,
    modifier: Modifier = Modifier,
    icon: ImageVector? = null,
    painterResource: Painter? = null,
    iconSize: Dp = 28.dp,
    fontSize: TextUnit = 15.sp,
    bluetoothViewModel: BluetoothViewModel? = null
) {
    val buttonStates by controlPanelViewModel.buttonStates.collectAsState()
    val currentState = buttonStates[buttonId] ?: false

    val interactionSource = remember { MutableInteractionSource() }
    val isPressed by interactionSource.collectIsPressedAsState()

    val buttonColor = when {
        isPressed -> activeColor.copy(alpha = 0.7f)
        currentState -> activeColor
        else -> Color(0xFF23272F)
    }

    Button(

        onClick = {
            val newState = !currentState
            
            // Mettre à jour l'état dans le ControlPanelViewModel
            controlPanelViewModel.updateButtonState(buttonId, newState)
            onToggle(newState)
            
            // Envoyer toutes les commandes personnalisées via Bluetooth
            bluetoothViewModel?.let { btViewModel ->
                Log.d("ControlPanel", "Sending Bluetooth commands for button $buttonId: $newState")
                controlPanelViewModel.sendBluetoothCommands(btViewModel)
            }
        },
        modifier = modifier
            .fillMaxWidth(),
        colors = ButtonDefaults.buttonColors(containerColor = buttonColor),
        interactionSource = interactionSource,
        shape = RoundedCornerShape(16.dp),
        elevation = ButtonDefaults.buttonElevation(defaultElevation = 8.dp)
    ) {
        Column(horizontalAlignment = Alignment.CenterHorizontally) {
            if (icon != null) {
                Icon(
                    imageVector = icon,
                    contentDescription = null,
                    tint = if (currentState || isPressed) Color.White else inactiveTintColor,
                    modifier = Modifier.size(iconSize)
                )
            } else if (painterResource != null) {
                Icon(
                    painter = painterResource,
                    contentDescription = null,
                    tint = if (currentState || isPressed) Color.Black else inactiveTintColor,
                    modifier = Modifier.size(iconSize)
                )
            }
            Text(
                text = label,
                color = if (currentState || isPressed) Color.White else inactiveTintColor,
                fontWeight = FontWeight.Bold,
                fontSize = fontSize,
                letterSpacing = 1.sp
            )
        }
    }
}

@Composable
fun ConnectionStatusIndicator(
    isConnected: Boolean,
    connectionStatus: String,
    modifier: Modifier = Modifier
) {
    Card(
        modifier = modifier,
        shape = RoundedCornerShape(12.dp),
        colors = CardDefaults.cardColors(
            containerColor = if (isConnected) Color(0xFF2E7D32) else Color(0xFF757575)
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
    ) {
        Row(
            modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(6.dp)
        ) {
            // Status indicator dot
            Box(
                modifier = Modifier
                    .size(8.dp)
                    .background(
                        color = if (isConnected) Color(0xFF4CAF50) else Color(0xFFFF5722),
                        shape = CircleShape
                    )
            )
            
            // Status text
            Text(
                text = if (isConnected) "Connected" else "Disconnected",
                color = Color.White,
                fontSize = 12.sp,
                fontWeight = FontWeight.Medium,
                textAlign = TextAlign.Center
            )
        }
    }
}
