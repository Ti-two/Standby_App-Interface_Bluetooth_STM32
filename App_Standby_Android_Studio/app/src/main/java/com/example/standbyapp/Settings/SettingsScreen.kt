package com.example.standbyapp.Settings

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material.icons.filled.Edit
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.lifecycle.ViewModelProvider

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SettingsScreen(
    customCommandViewModel: CustomCommandViewModel? = null,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    val viewModel: CustomCommandViewModel = customCommandViewModel ?: viewModel(
        factory = object : ViewModelProvider.Factory {
            @Suppress("UNCHECKED_CAST")
            override fun <T : androidx.lifecycle.ViewModel> create(modelClass: Class<T>): T {
                return CustomCommandViewModel(context) as T
            }
        }
    )
    
    val customCommands by viewModel.customCommands.collectAsState()
    val showEditDialog by viewModel.showEditDialog.collectAsState()
    val editingCommand by viewModel.editingCommand.collectAsState()
    val errorMessage by viewModel.errorMessage.collectAsState()
    
    // Afficher les erreurs
    LaunchedEffect(errorMessage) {
        errorMessage?.let { 
            // Ici on pourrait afficher un Snackbar ou Toast
            viewModel.clearError()
        }
    }
    
    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF00041A))
            .padding(16.dp)
    ) {
        // Titre de la page
        Text(
            text = "Command Configuration",
            color = Color.White,
            style = MaterialTheme.typography.headlineMedium,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.padding(bottom = 16.dp)
        )
            
            // Bouton de réinitialisation
            OutlinedButton(
                onClick = { viewModel.resetToDefaults() },
                modifier = Modifier.fillMaxWidth(),
                colors = ButtonDefaults.outlinedButtonColors(
                    contentColor = Color.White
                ),
                border = androidx.compose.foundation.BorderStroke(1.dp, Color.White)
            ) {
                Text("Reset to default values")
            }
            
            Spacer(modifier = Modifier.height(8.dp))
            
            // Bouton d'ajout de commande
            Button(
                onClick = { viewModel.createNewCommand() },
                modifier = Modifier.fillMaxWidth(),
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFF4CAF50)
                )
            ) {
                Icon(
                    imageVector = Icons.Default.Add,
                    contentDescription = "Add",
                    tint = Color.White,
                    modifier = Modifier.size(20.dp)
                )
                Spacer(modifier = Modifier.width(8.dp))
                Text("Add custom command", color = Color.White)
            }
            
            Spacer(modifier = Modifier.height(16.dp))
            
            // Liste des commandes
            LazyColumn(
                verticalArrangement = Arrangement.spacedBy(8.dp),
                modifier = Modifier.weight(1f) // Prendre l'espace restant
            ) {
                items(customCommands) { command ->
                    CommandCard(
                        command = command,
                        onEdit = { viewModel.editCommand(command) },
                        onDelete = { viewModel.deleteCommand(command.id) },
                        onToggleEnabled = { enabled -> 
                            viewModel.toggleCommandEnabled(command.id, enabled) 
                        }
                    )
                }
            }
    }
    
    // Dialog d'édition
    if (showEditDialog && editingCommand != null) {
        CommandEditDialog(
            command = editingCommand!!,
            onSave = { viewModel.saveCommand(it) },
            onDismiss = { viewModel.closeEditDialog() }
        )
    }
}

@Composable
fun CommandCard(
    command: CustomCommand,
    onEdit: () -> Unit,
    onDelete: () -> Unit,
    onToggleEnabled: (Boolean) -> Unit
) {
    Card(
        modifier = Modifier.fillMaxWidth(),
        colors = CardDefaults.cardColors(
            containerColor = Color(0xFF1F2937)
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            // Indicateur de couleur
            Box(
                modifier = Modifier
                    .size(24.dp)
                    .clip(CircleShape)
                    .background(command.getColor())
                    .border(1.dp, Color.White, CircleShape)
            )
            
            Spacer(modifier = Modifier.width(8.dp))
            
            // Icône de la commande
            Box(
                modifier = Modifier
                    .size(32.dp)
                    .clip(RoundedCornerShape(6.dp))
                    .background(Color(0xFF374151)),
                contentAlignment = Alignment.Center
            ) {
                // Priorité aux icônes Material (iconName)
                if (command.iconName != null) {
                    AvailableIcons.getIconByName(command.iconName)?.let { icon ->
                        Icon(
                            imageVector = icon,
                            contentDescription = command.iconName,
                            tint = Color.White,
                            modifier = Modifier.size(20.dp)
                        )
                    }
                } else if (command.iconResourceId != null) {
                    // Utiliser les icônes drawable si pas d'icône Material
                    Icon(
                        painter = painterResource(id = command.iconResourceId),
                        contentDescription = command.name,
                        tint = Color.White,
                        modifier = Modifier.size(20.dp)
                    )
                }
            }
            
            Spacer(modifier = Modifier.width(12.dp))
            
            // Informations de la commande
            Column(modifier = Modifier.weight(1f)) {
                Text(
                    text = command.name,
                    color = Color.White,
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "ID: ${command.commandId} | Active: ${command.activeCommand} | Inactive: ${command.inactiveCommand}",
                    color = Color.Gray,
                    style = MaterialTheme.typography.bodySmall
                )
                if (command.description.isNotBlank()) {
                    Text(
                        text = command.description,
                        color = Color.LightGray,
                        style = MaterialTheme.typography.bodySmall
                    )
                }
            }
            
            // Switch d'activation
            Switch(
                checked = command.isEnabled,
                onCheckedChange = onToggleEnabled,
                colors = SwitchDefaults.colors(
                    checkedThumbColor = Color.White,
                    checkedTrackColor = command.getColor(),
                    uncheckedThumbColor = Color.Gray,
                    uncheckedTrackColor = Color.DarkGray
                )
            )
            
            Spacer(modifier = Modifier.width(8.dp))
            
            // Boutons d'action
            IconButton(onClick = onEdit) {
                Icon(Icons.Default.Edit, contentDescription = "Edit", tint = Color.White)
            }
            
            IconButton(onClick = onDelete) {
                Icon(Icons.Default.Delete, contentDescription = "Delete", tint = Color.Red)
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CommandEditDialog(
    command: CustomCommand,
    onSave: (CustomCommand) -> Unit,
    onDismiss: () -> Unit
) {
    var name by remember { mutableStateOf(command.name) }
    var description by remember { mutableStateOf(command.description) }
    var selectedColor by remember { mutableStateOf(command.colorHex) }
    var selectedIcon by remember { mutableStateOf(command.iconName ?: "Warning") }
    var commandId by remember { mutableStateOf(command.commandId.toString()) }
    var activeCommand by remember { mutableStateOf(command.activeCommand.toString()) }
    var inactiveCommand by remember { mutableStateOf(command.inactiveCommand.toString()) }
    
    Dialog(onDismissRequest = onDismiss) {
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF1F2937)
            )
        ) {
            Column(
                modifier = Modifier.padding(20.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp)
            ) {
                Text(
                    text = if (command.id >= 0) "Edit command" else "New command",
                    color = Color.White,
                    style = MaterialTheme.typography.headlineSmall,
                    fontWeight = FontWeight.Bold
                )
                
                // Nom de la commande
                OutlinedTextField(
                    value = name,
                    onValueChange = { name = it },
                    label = { Text("Command name", color = Color.Gray) },
                    colors = OutlinedTextFieldDefaults.colors(
                        focusedTextColor = Color.White,
                        unfocusedTextColor = Color.White,
                        focusedBorderColor = Color(0xFF4CAF50),
                        unfocusedBorderColor = Color.Gray
                    ),
                    modifier = Modifier.fillMaxWidth()
                )
                
                // Description
                OutlinedTextField(
                    value = description,
                    onValueChange = { description = it },
                    label = { Text("Description (optional)", color = Color.Gray) },
                    colors = OutlinedTextFieldDefaults.colors(
                        focusedTextColor = Color.White,
                        unfocusedTextColor = Color.White,
                        focusedBorderColor = Color(0xFF4CAF50),
                        unfocusedBorderColor = Color.Gray
                    ),
                    modifier = Modifier.fillMaxWidth()
                )
                
                // Sélecteur de couleur
                Text(
                    text = "Color",
                    color = Color.White,
                    style = MaterialTheme.typography.labelLarge
                )
                
                LazyRow(
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    items(PresetColors.COLORS) { colorHex ->
                        val color = Color(android.graphics.Color.parseColor(colorHex))
                        Box(
                            modifier = Modifier
                                .size(40.dp)
                                .clip(CircleShape)
                                .background(color)
                                .border(
                                    2.dp,
                                    if (selectedColor == colorHex) Color.White else Color.Transparent,
                                    CircleShape
                                )
                                .clickable { selectedColor = colorHex }
                        )
                    }
                }
                
                // Sélecteur d'icône
                Text(
                    text = "Icon",
                    color = Color.White,
                    style = MaterialTheme.typography.labelLarge
                )
                
                LazyRow(
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    items(AvailableIcons.AVAILABLE_ICONS) { iconInfo ->
                        Box(
                            modifier = Modifier
                                .size(48.dp)
                                .clip(RoundedCornerShape(8.dp))
                                .background(
                                    if (selectedIcon == iconInfo.name) 
                                        Color(0xFF4CAF50) 
                                    else 
                                        Color(0xFF374151)
                                )
                                .border(
                                    2.dp,
                                    if (selectedIcon == iconInfo.name) Color.White else Color.Transparent,
                                    RoundedCornerShape(8.dp)
                                )
                                .clickable { selectedIcon = iconInfo.name },
                            contentAlignment = Alignment.Center
                        ) {
                            Icon(
                                imageVector = iconInfo.icon,
                                contentDescription = iconInfo.name,
                                tint = Color.White,
                                modifier = Modifier.size(24.dp)
                            )
                        }
                    }
                }
                
                // Paramètres de commande
                Row(
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    OutlinedTextField(
                        value = commandId,
                        onValueChange = { commandId = it },
                        label = { Text("Command ID", color = Color.Gray) },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedTextColor = Color.White,
                            unfocusedTextColor = Color.White,
                            focusedBorderColor = Color(0xFF4CAF50),
                            unfocusedBorderColor = Color.Gray
                        ),
                        modifier = Modifier.weight(1f)
                    )
                    
                    OutlinedTextField(
                        value = activeCommand,
                        onValueChange = { activeCommand = it },
                        label = { Text("Active Cmd", color = Color.Gray) },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedTextColor = Color.White,
                            unfocusedTextColor = Color.White,
                            focusedBorderColor = Color(0xFF4CAF50),
                            unfocusedBorderColor = Color.Gray
                        ),
                        modifier = Modifier.weight(1f)
                    )
                    
                    OutlinedTextField(
                        value = inactiveCommand,
                        onValueChange = { inactiveCommand = it },
                        label = { Text("Inactive Cmd", color = Color.Gray) },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedTextColor = Color.White,
                            unfocusedTextColor = Color.White,
                            focusedBorderColor = Color(0xFF4CAF50),
                            unfocusedBorderColor = Color.Gray
                        ),
                        modifier = Modifier.weight(1f)
                    )
                }
                
                // Boutons d'action
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.End
                ) {
                    TextButton(onClick = onDismiss) {
                        Text("Cancel", color = Color.Gray)
                    }
                    
                    Spacer(modifier = Modifier.width(8.dp))
                    
                    Button(
                        onClick = {
                            try {
                                val updatedCommand = command.copy(
                                    name = name.trim(),
                                    description = description.trim(),
                                    colorHex = selectedColor,
                                    iconName = selectedIcon,
                                    commandId = commandId.toIntOrNull() ?: 0,
                                    activeCommand = activeCommand.toIntOrNull() ?: 1,
                                    inactiveCommand = inactiveCommand.toIntOrNull() ?: 0
                                )
                                onSave(updatedCommand)
                            } catch (e: Exception) {
                                // Gérer l'erreur de validation
                            }
                        },
                        colors = ButtonDefaults.buttonColors(
                            containerColor = Color(0xFF4CAF50)
                        )
                    ) {
                        Text("Save", color = Color.White)
                    }
                }
            }
        }
    }
}
