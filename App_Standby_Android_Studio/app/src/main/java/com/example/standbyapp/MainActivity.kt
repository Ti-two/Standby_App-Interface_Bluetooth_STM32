package com.example.standbyapp

import android.Manifest
import android.content.Context
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.OptIn
import androidx.camera.core.ExperimentalGetImage
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalView
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.booleanPreferencesKey
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.core.stringSetPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewmodel.compose.viewModel
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import com.example.standbyapp.ControlPanel.ControlPanelViewModel as NewControlPanelViewModel
import com.example.standbyapp.ui.theme.StandbyAppTheme

class MainActivity : ComponentActivity() {

    private val LOGGED_IN_KEY = booleanPreferencesKey("logged_in")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        
        enableEdgeToEdge()
        setContent {
            val view = LocalView.current
            val window = (view.context as ComponentActivity).window
            SideEffect {
                val controller = WindowInsetsControllerCompat(window, view)
                controller.isAppearanceLightStatusBars = false
                window.statusBarColor = Color.Transparent.toArgb()
                window.navigationBarColor = Color.Transparent.toArgb()
            }
            
            StandbyAppTheme {
                Box(
                    modifier = Modifier
                        .fillMaxSize()
                        .background(Color.Black)
                ) {
                    AppContent()
                }
            }
        }
    }
}

@OptIn(ExperimentalGetImage::class)
@Composable
fun AppContent() {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()
    
    var isLoggedIn by rememberSaveable { mutableStateOf(false) }
    var pinCode by rememberSaveable { mutableStateOf("") }

    LaunchedEffect(Unit) {
        val prefs = context.dataStore.data.first()
        isLoggedIn = prefs[booleanPreferencesKey("logged_in")] ?: false
    }

    if (isLoggedIn) {
        val customCommandViewModel: com.example.standbyapp.Settings.CustomCommandViewModel = viewModel(
            factory = object : ViewModelProvider.Factory { @Suppress("UNCHECKED_CAST") override fun <T : ViewModel> create(modelClass: Class<T>): T = com.example.standbyapp.Settings.CustomCommandViewModel(context.applicationContext) as T })
        val newViewModel: NewControlPanelViewModel = viewModel(
            factory = object : ViewModelProvider.Factory { @Suppress("UNCHECKED_CAST") override fun <T : ViewModel> create(modelClass: Class<T>): T = NewControlPanelViewModel(context.applicationContext, customCommandViewModel) as T })
        MainScreen(viewModel = newViewModel, customCommandViewModel = customCommandViewModel, modifier = Modifier.systemBarsPadding())
    } else {
        LoginChoiceScreen(
            onQrSuccess = { qrValue ->
                // Validate QR code against stored credentials
                scope.launch {
                    val prefs = context.dataStore.data.first()
                    val storedCodes = prefs[stringSetPreferencesKey("access_codes")] ?: setOf()
                    val defaultCodes = setCredentialAdmin().map { it.code }.toSet()
                    val allValidCodes = storedCodes + defaultCodes
                    
                    val isValidCode = allValidCodes.contains(qrValue.uppercase())
                    
                    if (isValidCode) {
                        pinCode = qrValue
                        context.dataStore.edit { prefs ->
                            prefs[booleanPreferencesKey("logged_in")] = true
                            prefs[stringPreferencesKey("current_auth_code")] = qrValue.uppercase()
                        }
                        isLoggedIn = true
                    } else {
                        // QR code invalid - stay on login screen
                        Log.w("MainActivity", "Invalid QR code: $qrValue")
                    }
                }
            },
            onManualLoginSuccess = { usedCode ->
                scope.launch {
                    context.dataStore.edit { prefs ->
                        prefs[booleanPreferencesKey("logged_in")] = true
                        prefs[stringPreferencesKey("current_auth_code")] = usedCode.uppercase()
                    }
                    isLoggedIn = true
                }
            }
        )
    }
}

@Composable
fun QrScreenWithPermission(
    onQrCodeScanned: (String) -> Unit,
    onBackToChoice: (() -> Unit)? = null
) {
    var permissionGranted by remember { mutableStateOf(false) }
    val permissionLauncher = rememberLauncherForActivityResult(
        ActivityResultContracts.RequestPermission()
    ) { granted ->
        permissionGranted = granted
    }

    LaunchedEffect(Unit) {
        permissionLauncher.launch(Manifest.permission.CAMERA)
    }

    if (permissionGranted) {
        ScanQrScreen(onQrCodeScanned = onQrCodeScanned, onBackToChoice = onBackToChoice)
    } else {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(32.dp),
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "Camera permission is required to scan QR code.",
                style = MaterialTheme.typography.bodyLarge,
                color = Color.White,
                textAlign = TextAlign.Center
            )
            Spacer(modifier = Modifier.height(24.dp))
            Button(
                onClick = { permissionLauncher.launch(Manifest.permission.CAMERA) },
                modifier = Modifier.fillMaxWidth()
            ) {
                Text("Allow Camera Access")
            }
            
            onBackToChoice?.let {
                Spacer(modifier = Modifier.height(16.dp))
                OutlinedButton(
                    onClick = it,
                    modifier = Modifier.fillMaxWidth(),
                    border = BorderStroke(1.dp, Color.White.copy(alpha = 0.5f))
                ) {
                    Text("Back to Login Options", color = Color.White)
                }
            }
        }
    }
}

@Composable
fun LoginChoiceScreen(
    onQrSuccess: (String) -> Unit,
    onManualLoginSuccess: (String) -> Unit
) {
    var showQrScanner by remember { mutableStateOf(false) }
    var showManualLogin by remember { mutableStateOf(false) }
    
    when {
        showQrScanner -> {
            QrScreenWithPermission(
                onQrCodeScanned = onQrSuccess,
                onBackToChoice = { showQrScanner = false }
            )
        }
        showManualLogin -> {
            LogicScreen(
                onLoginSuccess = onManualLoginSuccess,
                onBackToChoice = { showManualLogin = false }
            )
        }
        else -> {
            // Écran de choix principal
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(32.dp),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(
                    text = "Welcome to StandBy",
                    style = MaterialTheme.typography.headlineLarge,
                    fontWeight = FontWeight.Bold,
                    color = Color.White,
                    textAlign = TextAlign.Center
                )
                
                Spacer(modifier = Modifier.height(16.dp))
                
                Text(
                    text = "Choose your login method",
                    style = MaterialTheme.typography.bodyLarge,
                    color = Color.White.copy(alpha = 0.8f),
                    textAlign = TextAlign.Center
                )
                
                Spacer(modifier = Modifier.height(48.dp))
                
                // Bouton QR Code
                Button(
                    onClick = { showQrScanner = true },
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(56.dp),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFF1E3A8A),
                        contentColor = Color.White
                    ),
                    shape = RoundedCornerShape(12.dp)
                ) {
                    Text(
                        text = "📷",
                        fontSize = 24.sp
                    )
                    Spacer(modifier = Modifier.width(12.dp))
                    Text(
                        text = "Scan QR Code",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.Medium
                    )
                }
                
                Spacer(modifier = Modifier.height(16.dp))
                
                // Bouton Login Manuel
                OutlinedButton(
                    onClick = { showManualLogin = true },
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(56.dp),
                    border = BorderStroke(2.dp, Color.White.copy(alpha = 0.5f)),
                    shape = RoundedCornerShape(12.dp)
                ) {
                    Text(
                        text = "👤",
                        fontSize = 24.sp,
                        color = Color.White
                    )
                    Spacer(modifier = Modifier.width(12.dp))
                    Text(
                        text = "Manual Login",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.Medium,
                        color = Color.White
                    )
                }
                
                Spacer(modifier = Modifier.height(32.dp))
                
                Text(
                    text = "QR Code: Quick access with camera\nManual: Enter access code",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color.White.copy(alpha = 0.6f),
                    textAlign = TextAlign.Center,
                    lineHeight = 16.sp
                )
            }
        }
    }
}
