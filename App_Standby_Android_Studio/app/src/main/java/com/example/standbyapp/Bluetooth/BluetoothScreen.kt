package com.example.standbyapp.Bluetooth

import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.widget.Toast
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.lifecycle.ViewModelProvider
import com.example.standbyapp.Bluetooth.BluetoothViewModel
import com.example.stmbluetooth.BluetoothPermissionManager

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun BluetoothScreen() {
    val context = LocalContext.current
    val viewModel: BluetoothViewModel = viewModel(
        factory = ViewModelProvider.AndroidViewModelFactory.getInstance(context.applicationContext as android.app.Application)
    )

    // Vérifier et demander les permissions Bluetooth
    LaunchedEffect(Unit) {
        if (!BluetoothPermissionManager.hasBluetoothPermissions(context)) {
            BluetoothPermissionManager.requestPermissions(context as android.app.Activity)
        }
    }

    // Gestion des erreurs
    LaunchedEffect(viewModel.errorMessage) {
        viewModel.errorMessage?.let { message ->
            Toast.makeText(context, message, Toast.LENGTH_LONG).show()
            viewModel.clearError()
        }
    }

    Scaffold(
        modifier = Modifier.fillMaxSize(),
        topBar = {
            TopAppBar(
                title = { 
                    Text(
                        text = "Bluetooth Connection",
                        color = Color.White,
                        style = MaterialTheme.typography.headlineMedium,
                        fontWeight = FontWeight.Bold
                    ) 
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = Color(0xFF00041A)
                )
            )
        },
        containerColor = Color(0xFF00041A)
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
                .padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            // Statut de connexion
            Card(
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors(
                    containerColor = if (viewModel.isConnected)
                        Color(0xFF1B4332) // Vert sombre pour connecté
                    else
                        Color(0xFF4A0E0E) // Rouge sombre pour déconnecté
                )
            ) {
                Text(
                    text = "Status: ${viewModel.connectionStatus}",
                    modifier = Modifier.padding(16.dp),
                    style = MaterialTheme.typography.headlineSmall,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )
            }

            // Boutons de contrôle
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Button(
                    onClick = { viewModel.startScan() },
                    enabled = !viewModel.isScanning,
                    modifier = Modifier.weight(1f),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFF1E3A8A),
                        contentColor = Color.White,
                        disabledContainerColor = Color(0xFF374151),
                        disabledContentColor = Color(0xFF9CA3AF)
                    )
                ) {
                    Icon(Icons.Default.Search, contentDescription = null)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(if (viewModel.isScanning) "Scanning..." else "Scan")
                }

                Button(
                    onClick = { viewModel.refreshPairedDevices() },
                    modifier = Modifier.weight(1f),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFF059669),
                        contentColor = Color.White
                    )
                ) {
                    Icon(Icons.Default.Refresh, contentDescription = null)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text("Refresh")
                }
            }

            if (viewModel.isConnected) {
                Button(
                    onClick = { viewModel.disconnect() },
                    modifier = Modifier.fillMaxWidth(),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFFDC2626)
                    )
                ) {
                    Text("Disconnect", color = Color.White)
                }
            } else {
                Button(
                    onClick = { viewModel.showDeviceSelectionDialog() },
                    modifier = Modifier.fillMaxWidth(),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFF7C3AED),
                        contentColor = Color.White
                    )
                ) {
                    Text("Select device")
                }
            }

            // Liste des appareils découverts
            if (viewModel.discoveredDevices.isNotEmpty()) {
                Text(
                    text = "Discovered devices:",
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )

                LazyColumn(
                    verticalArrangement = Arrangement.spacedBy(4.dp)
                ) {
                    items(viewModel.discoveredDevices) { device ->
                        DeviceCard(
                            device = device,
                            onClick = {
                                viewModel.connectToDevice(device)
                            }
                        )
                    }
                }
            }
        }

        // Dialog de sélection d'appareil
        if (viewModel.showDeviceDialog) {
            DeviceSelectionDialog(
                pairedDevices = viewModel.pairedDevices,
                onDeviceSelected = { device ->
                    viewModel.connectToDevice(device)
                    viewModel.hideDeviceSelectionDialog()
                },
                onDismiss = { viewModel.hideDeviceSelectionDialog() }
            )
        }
    }
}

@SuppressLint("MissingPermission")
@Composable
fun DeviceCard(
    device: BluetoothDevice,
    onClick: () -> Unit
) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .clickable { onClick() },
        colors = CardDefaults.cardColors(
            containerColor = Color(0xFF1F2937)
        )
    ) {
        Column(
            modifier = Modifier.padding(12.dp)
        ) {
            Text(
                text = device.name ?: "Appareil inconnu",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold,
                color = Color.White
            )
            Text(
                text = device.address,
                style = MaterialTheme.typography.bodyMedium,
                color = Color(0xFF9CA3AF)
            )
        }
    }
}

@Composable
fun DeviceSelectionDialog(
    pairedDevices: List<BluetoothDevice>,
    onDeviceSelected: (BluetoothDevice) -> Unit,
    onDismiss: () -> Unit
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = { 
            Text(
                "Sélectionner un appareil",
                color = Color.White
            ) 
        },
        text = {
            LazyColumn {
                items(pairedDevices) { device ->
                    DeviceCard(
                        device = device,
                        onClick = { onDeviceSelected(device) }
                    )
                }
            }
        },
        confirmButton = {
            TextButton(
                onClick = onDismiss,
                colors = ButtonDefaults.textButtonColors(
                    contentColor = Color.White
                )
            ) {
                Text("Annuler")
            }
        },
        containerColor = Color(0xFF1F2937),
        textContentColor = Color.White
    )
}