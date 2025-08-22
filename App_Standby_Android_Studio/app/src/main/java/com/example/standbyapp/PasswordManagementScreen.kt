package com.example.standbyapp

import android.content.Context
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.*
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Color.Companion.DarkGray
import androidx.compose.ui.graphics.Color.Companion.LightGray
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.core.stringSetPreferencesKey
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import com.example.standbyapp.Bluetooth.BluetoothViewModel

@Composable
// Screen for managing access codes with add/delete functionality and bluetooth code generation
fun PasswordManagementScreen(bluetoothViewModel: BluetoothViewModel? = null) {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()
    
    var showAddDialog by remember { mutableStateOf(false) }
    var showQrScanner by remember { mutableStateOf(false) }
    var showManualInput by remember { mutableStateOf(false) }
    var storedCodes by remember { mutableStateOf(setOf<String>()) }
    var currentBluetoothCode by remember { mutableStateOf("") }
    
    // Load stored codes on startup
    LaunchedEffect(key1 = Unit) {
        scope.launch {
            try {
                val datastoreData = context.dataStore.data.first()
                val codes = datastoreData[stringSetPreferencesKey("access_codes")] ?: setOf()
                val bluetoothCode = datastoreData[stringPreferencesKey("current_auth_code")] ?: "BT12345"
                
                // Initialize with default codes if empty
                if (codes.isEmpty()) {
                    val defaultCodes = setCredentialAdmin().map { it.code }.toSet()
                    context.dataStore.edit { editPrefs ->
                        editPrefs[stringSetPreferencesKey("access_codes")] = defaultCodes
                    }
                    storedCodes = defaultCodes
                } else {
                    storedCodes = codes
                }
                
                currentBluetoothCode = bluetoothCode
            } catch (e: Exception) {
                // Handle error
            }
        }
    }
    
    when {
        showQrScanner -> {
            QrScreenWithPermission(
                onQrCodeScanned = { qrValue ->
                    scope.launch {
                        val newCodes = storedCodes + qrValue.uppercase()
                        storedCodes = newCodes
                        context.dataStore.edit { editPrefs ->
                            editPrefs[stringSetPreferencesKey("access_codes")] = newCodes
                        }
                        showQrScanner = false
                    }
                },
                onBackToChoice = { showQrScanner = false }
            )
        }
        showManualInput -> {
            ManualCodeInputDialog(
                onSave = { newCode ->
                    scope.launch {
                        val newCodes = storedCodes + newCode.uppercase()
                        storedCodes = newCodes
                        context.dataStore.edit { editPrefs ->
                            editPrefs[stringSetPreferencesKey("access_codes")] = newCodes
                        }
                        showManualInput = false
                    }
                },
                onDismiss = { showManualInput = false }
            )
        }
        showAddDialog -> {
            AddCodeDialog(
                onQrScan = {
                    showAddDialog = false
                    showQrScanner = true
                },
                onManualInput = {
                    showAddDialog = false
                    showManualInput = true
                },
                onDismiss = { showAddDialog = false }
            )
        }
        else -> {
            // Main password management screen
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(24.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp)
            ) {
                // Header
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Access Codes",
                        style = MaterialTheme.typography.headlineMedium,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                    
                    Button(
                        onClick = { showAddDialog = true },
                        colors = ButtonDefaults.buttonColors(
                            containerColor = Color(0xFF4CAF50)
                        )
                    ) {
                        Icon(Icons.Default.Add, contentDescription = null)
                        Spacer(modifier = Modifier.width(8.dp))
                        Text("Add Code")
                    }
                }
                
                // Bluetooth Authentication Section
                Card(
                    modifier = Modifier.fillMaxWidth(),
                    colors = CardDefaults.cardColors(
                        containerColor = Color(0xFF1E3A8A).copy(alpha = 0.3f)
                    )
                ) {
                    Column(
                        modifier = Modifier.padding(16.dp)
                    ) {
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.SpaceBetween
                        ) {
                            Text(
                                text = "🔗 Bluetooth Authentication",
                                style = MaterialTheme.typography.titleMedium,
                                fontWeight = FontWeight.Bold,
                                color = Color.White
                            )
                        }
                        
                        Spacer(modifier = Modifier.height(8.dp))
                        
                        Text(
                            text = "Current code: $currentBluetoothCode",
                            style = MaterialTheme.typography.bodyMedium,
                            color = Color.White.copy(alpha = 0.8f)
                        )
                        
                        Spacer(modifier = Modifier.height(12.dp))
                        
                        // Dropdown-like selection for Bluetooth code
                        BluetoothCodeSelector(
                            availableCodes = storedCodes + setCredentialAdmin().map { it.code }.toSet(),
                            currentCode = currentBluetoothCode,
                            onCodeSelected = { selectedCode ->
                                scope.launch {
                                    context.dataStore.edit { editPrefs ->
                                        editPrefs[stringPreferencesKey("current_auth_code")] = selectedCode
                                    }
                                    currentBluetoothCode = selectedCode
                                    // Reload the authentication code in BluetoothViewModel
                                    bluetoothViewModel?.reloadAuthenticationCode()
                                }
                            }
                        )
                    }
                }
                
                // Current codes list
                Text(
                    text = "Current access codes (${storedCodes.size})",
                    style = MaterialTheme.typography.titleMedium,
                    color = Color.White.copy(alpha = 0.8f)
                )
                
                if (storedCodes.isEmpty()) {
                    Card(
                        modifier = Modifier.fillMaxWidth(),
                        colors = CardDefaults.cardColors(
                            containerColor = Color(0xFF374151)
                        )
                    ) {
                        Column(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(24.dp),
                            horizontalAlignment = Alignment.CenterHorizontally
                        ) {
                            Text(
                                text = "No access codes configured",
                                color = Color.White.copy(alpha = 0.7f),
                                textAlign = TextAlign.Center
                            )
                            Spacer(modifier = Modifier.height(8.dp))
                            Text(
                                text = "Add your first access code using QR scan or manual input",
                                color = Color.White.copy(alpha = 0.5f),
                                textAlign = TextAlign.Center,
                                fontSize = 14.sp
                            )
                        }
                    }
                } else {
                    LazyColumn(
                        verticalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        items(storedCodes.toList()) { code ->
                            CodeCard(
                                code = code,
                                onDelete = {
                                    scope.launch {
                                        val newCodes = storedCodes - code
                                        storedCodes = newCodes
                                        context.dataStore.edit { editPrefs ->
                                            editPrefs[stringSetPreferencesKey("access_codes")] = newCodes
                                        }
                                    }
                                }
                            )
                        }
                    }
                }
                
                Spacer(modifier = Modifier.height(16.dp))

            }
        }
    }
}

@Composable
fun CodeCard(
    code: String,
    onDelete: () -> Unit
) {
    Card(
        modifier = Modifier.fillMaxWidth(),
        colors = CardDefaults.cardColors(
            containerColor = Color(0xFF374151)
        )
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Column(
                modifier = Modifier.weight(1f)
            ) {
                Text(
                    text = code,
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )
                Text(
                    text = "Access Code",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color.White.copy(alpha = 0.7f)
                )
            }
            
            IconButton(onClick = onDelete) {
                Icon(
                    Icons.Default.Delete,
                    contentDescription = "Delete",
                    tint = Color.Red
                )
            }
        }
    }
}

@Composable
fun AddCodeDialog(
    onQrScan: () -> Unit,
    onManualInput: () -> Unit,
    onDismiss: () -> Unit
) {
    Dialog(onDismissRequest = onDismiss) {
        Card(
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF1F2937)
            )
        ) {
            Column(
                modifier = Modifier.padding(24.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(
                    text = "Add New Access Code",
                    style = MaterialTheme.typography.titleLarge,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )
                
                Spacer(modifier = Modifier.height(24.dp))
                
                Button(
                    onClick = onQrScan,
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(56.dp),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFF1E3A8A)
                    )
                ) {
                    Text(text = "📷", fontSize = 24.sp)
                    Spacer(modifier = Modifier.width(12.dp))
                    Text("Scan QR Code")
                }
                
                Spacer(modifier = Modifier.height(12.dp))
                
                OutlinedButton(
                    onClick = onManualInput,
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(56.dp),
                    border = BorderStroke(1.dp, Color.White.copy(alpha = 0.5f))
                ) {
                    Text(text = "✏️", fontSize = 24.sp, color = Color.White)
                    Spacer(modifier = Modifier.width(12.dp))
                    Text("Manual Input", color = Color.White)
                }
                
                Spacer(modifier = Modifier.height(16.dp))
                
                OutlinedButton(
                    onClick = onDismiss,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("Cancel", color = Color.White)
                }
            }
        }
    }
}

@Composable
fun ManualCodeInputDialog(
    onSave: (String) -> Unit,
    onDismiss: () -> Unit
) {
    var code by remember { mutableStateOf("") }
    var errorMessage by remember { mutableStateOf("") }

    Dialog(onDismissRequest = onDismiss) {
        Card(
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF1F2937)
            )
        ) {
            Column(
                modifier = Modifier.padding(24.dp)
            ) {
                Text(
                    text = "Enter Access Code",
                    style = MaterialTheme.typography.titleLarge,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )
                
                Spacer(modifier = Modifier.height(16.dp))
                
                TextField(
                    value = code,
                    onValueChange = { 
                        code = it.lowercase()
                        errorMessage = ""
                    },
                    label = { Text("Access Code", color = Color.Gray) },
                    placeholder = { Text("Enter your code...", color = Color.Gray) },
                    singleLine = true,
                    colors = TextFieldDefaults.colors(
                        focusedTextColor = Color.White,
                        unfocusedTextColor = Color.White,
                        focusedContainerColor = Color(0xFF374151),
                        unfocusedContainerColor = Color(0xFF374151),
                        focusedIndicatorColor = Color(0xFF4CAF50),
                        unfocusedIndicatorColor = Color.Gray
                    ),
                    modifier = Modifier.fillMaxWidth()
                )
                
                if (errorMessage.isNotEmpty()) {
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = errorMessage,
                        color = Color.Red,
                        fontSize = 14.sp
                    )
                }
                
                Spacer(modifier = Modifier.height(24.dp))
                
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    OutlinedButton(
                        onClick = onDismiss,
                        modifier = Modifier.weight(1f)
                    ) {
                        Text("Cancel", color = Color.White)
                    }
                    
                    Button(
                        onClick = {
                            if (code.trim().isEmpty()) {
                                errorMessage = "Code cannot be empty"
                            } else if (code.trim().length < 3) {
                                errorMessage = "Code must be at least 3 characters"
                            } else {
                                onSave(code.trim())
                            }
                        },
                        modifier = Modifier.weight(1f),
                        colors = ButtonDefaults.buttonColors(
                            containerColor = Color(0xFF4CAF50)
                        )
                    ) {
                        Text("Save")
                    }
                }
            }
        }
    }
}

@Composable
fun BluetoothCodeSelector(
    availableCodes: Set<String>,
    currentCode: String,
    onCodeSelected: (String) -> Unit
) {
    var expanded by remember { mutableStateOf(false) }
    
    Column {
        // Current selection button
        OutlinedButton(
            onClick = { expanded = !expanded },
            modifier = Modifier.fillMaxWidth(),
            border = BorderStroke(1.dp, Color.White.copy(alpha = 0.5f)),
            colors = ButtonDefaults.outlinedButtonColors(
                contentColor = Color.White
            )
        ) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = if (currentCode.isNotEmpty()) currentCode.lowercase() else "Select code...",
                    style = MaterialTheme.typography.bodyMedium
                )
                Text(
                    text = if (expanded) "▲" else "▼",
                    fontSize = 12.sp
                )
            }
        }
        
        // Dropdown menu
        if (expanded) {
            Card(
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors(
                    containerColor = Color(0xFF374151)
                )
            ) {
                LazyColumn(
                    modifier = Modifier
                        .fillMaxWidth()
                        .heightIn(max = 200.dp)
                ) {
                    items(availableCodes.toList()) { code ->
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable {
                                    onCodeSelected(code)
                                    expanded = false
                                }
                                .padding(16.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            // Radio button indicator
                            Box(
                                modifier = Modifier
                                    .size(16.dp)
                                    .background(
                                        color = if (code == currentCode) Color(0xFF4CAF50) else Color.Transparent,
                                        shape = CircleShape
                                    )
                                    .border(
                                        2.dp,
                                        if (code == currentCode) Color(0xFF4CAF50) else Color.White.copy(alpha = 0.5f),
                                        CircleShape
                                    )
                            ) {
                                if (code == currentCode) {
                                    Box(
                                        modifier = Modifier
                                            .size(8.dp)
                                            .background(Color.White, CircleShape)
                                            .align(Alignment.Center)
                                    )
                                }
                            }
                            
                            Spacer(modifier = Modifier.width(12.dp))
                            
                            Column(modifier = Modifier.weight(1f)) {
                                Text(
                                    text = code.lowercase(),
                                    color = Color.White,
                                    style = MaterialTheme.typography.bodyMedium,
                                    fontWeight = FontWeight.Medium
                                )
                                Text(
                                    text = if (code == currentCode) "Currently selected" else "Available for selection",
                                    color = Color.White.copy(alpha = 0.6f),
                                    style = MaterialTheme.typography.bodySmall
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}
