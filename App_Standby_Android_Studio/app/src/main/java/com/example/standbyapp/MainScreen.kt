package com.example.standbyapp

import android.content.Context
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
//import androidx.compose.material.icons.filled.Bluetooth
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.Checkbox
import androidx.compose.material3.CheckboxDefaults
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.NavigationBarItemDefaults
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.*
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.ColorFilter
import androidx.compose.ui.graphics.painter.Painter
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.datastore.preferences.core.edit
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch



import androidx.datastore.preferences.core.stringSetPreferencesKey
import com.example.standbyapp.Bluetooth.BluetoothScreen
import com.example.standbyapp.Bluetooth.BluetoothViewModel
import com.example.standbyapp.ControlPanel.ControlPanelViewModel as NewControlPanelViewModel
import androidx.lifecycle.viewmodel.compose.viewModel

import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
@Composable
fun MainScreen(
    viewModel: NewControlPanelViewModel, 
    customCommandViewModel: com.example.standbyapp.Settings.CustomCommandViewModel,
    modifier: Modifier = Modifier
) {
    val navItemList = listOf(
        NavItem("Settings", R.drawable.align, "settings"),
        NavItem("Control", R.drawable.home, "control"),
        NavItem("Connection", R.drawable.telephone, "Connection"),
        NavItem("Codes", R.drawable.check, "codes")
    )

    var selectedIndex by rememberSaveable { mutableStateOf(1) }

    Scaffold(
        modifier = Modifier
            .fillMaxSize()
            .statusBarsPadding(),
        bottomBar = {
            NavigationBar(containerColor = Color.Black) {
                navItemList.forEachIndexed { index, navItem ->
                    NavigationBarItem(
                        selected = selectedIndex == index,
                        onClick = { selectedIndex = index },
                        icon = {
                            Image(
                                painter = painterResource(id = navItem.iconRes),
                                contentDescription = navItem.label,
                                modifier = Modifier.size(20.dp),
                                colorFilter = ColorFilter.tint(Color.White)
                            )
                        },
                        label = {
                            Text(text = navItem.label, color = Color.White)
                        },
                        colors = NavigationBarItemDefaults.colors(
                            indicatorColor = Color.Gray, //
                            selectedIconColor = Color.White,
                            unselectedIconColor = Color.White,
                            selectedTextColor = Color.White,
                            unselectedTextColor = Color.White
                        )
                    )

                }
            }
        }
    ) { innerPadding ->
        ContentScreen(
            modifier = Modifier.padding(innerPadding),
            selectedIndex = selectedIndex,
            viewModel = viewModel,
            customCommandViewModel = customCommandViewModel
        )
    }
}

@Composable
fun ContentScreen(
    modifier: Modifier = Modifier, 
    selectedIndex: Int, 
    viewModel: NewControlPanelViewModel,
    customCommandViewModel: com.example.standbyapp.Settings.CustomCommandViewModel
) {
    // Créer le BluetoothViewModel pour les pages qui en ont besoin
    val bluetoothViewModel: BluetoothViewModel = viewModel()
    
    when (selectedIndex) {
        0 -> OptionSelectorScreen(viewModel, customCommandViewModel, modifier)
        1 -> DynamicControlPanel(viewModel, bluetoothViewModel)
        2 -> BluetoothScreen()
        3 -> PasswordManagementScreen(bluetoothViewModel)
        else -> DynamicControlPanel(viewModel, bluetoothViewModel)
    }
}

// Modèle représentant une option possible
data class Option(
    val id: Int,
    val label: String,
    val activeColor: Color,
    val inactiveTintColor: Color = activeColor,
    val icon: ImageVector? = null,
    val painterResource: Int? = null,
    var enabled: Boolean = false, // option active ou non
    val customCommand: com.example.standbyapp.Settings.CustomCommand? = null // Commande personnalisée associée
)



class ControlPanelViewModel(private val context: Context) : ViewModel() {

    private val ENABLED_OPTIONS_KEY = stringSetPreferencesKey("enabled_options")

    private val _options = MutableStateFlow<List<Option>>(emptyList())
    val options: StateFlow<List<Option>> = _options

    init {
        loadOptions()
    }

    private fun loadOptions() {
        viewModelScope.launch {
            val prefs = context.dataStore.data.first()
            val enabledIds = prefs[ENABLED_OPTIONS_KEY]

            val baseOptions = listOf(
                Option(2, "FRONT LIGHTS", Color.Green, painterResource= R.drawable.siren, enabled=true),
                Option(3, "REAR LIGHTS", Color.Blue, Color.White, painterResource = R.drawable.siren, enabled=true),
                Option(1, "EMERGENCY", activeColor=Color.Red, icon = Icons.Default.Warning, enabled=true),
                Option(4, "SIREN", Color.Magenta, painterResource = R.drawable.megaphone),
                Option(5, "REAR LIGHTS", Color(0xFF988905), Color.White, painterResource = R.drawable.siren),
                Option(6, "CALL", Color(0xFFE3400B), painterResource = R.drawable.phonecall),
                Option(7, "REAR R", Color(0xFFF30D56), painterResource = R.drawable.siren),
            )

            // Cas 1 : DataStore vide, initialiser avec valeurs par défaut
            if (enabledIds == null) {
                saveEnabledOptions(baseOptions.filter { it.enabled }.map { it.id.toString() }.toSet())
                _options.value = baseOptions
            } else {
                // Cas 2 : DataStore existe déjà
                _options.value = baseOptions.map {
                    it.copy(enabled = enabledIds.contains(it.id.toString()))
                }
            }
        }
    }

    fun toggleOption(optionId: Int, enabled: Boolean) {
        val updated = _options.value.map {
            if (it.id == optionId) it.copy(enabled = enabled) else it
        }
        _options.value = updated

        saveEnabledOptions(updated.filter { it.enabled }.map { it.id.toString() }.toSet())
    }

    private fun saveEnabledOptions(enabledIds: Set<String>) {
        viewModelScope.launch {
            context.dataStore.edit { prefs ->
                prefs[ENABLED_OPTIONS_KEY] = enabledIds
            }
        }
    }
}



@Composable
fun OptionSelectorScreen(
    viewModel: NewControlPanelViewModel,
    customCommandViewModel: com.example.standbyapp.Settings.CustomCommandViewModel,
    modifier: Modifier = Modifier
) {
    com.example.standbyapp.Settings.SettingsScreen(
        customCommandViewModel = customCommandViewModel,
        modifier = modifier
    )
}
