package com.example.standbyapp

import android.util.Log
import androidx.annotation.OptIn
import androidx.camera.core.ExperimentalGetImage
import androidx.compose.foundation.Image
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.*
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.TextButton
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Color.Companion.DarkGray
import androidx.compose.ui.graphics.Color.Companion.LightGray
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.PasswordVisualTransformation
import com.google.mlkit.vision.barcode.common.Barcode
import android.Manifest
import android.content.Context
import android.util.Size
import android.view.ViewGroup
import androidx.camera.core.AspectRatio
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView

import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat
import androidx.lifecycle.compose.LocalLifecycleOwner
import com.google.mlkit.vision.barcode.*
import com.google.mlkit.vision.barcode.BarcodeScannerOptions
import com.google.mlkit.vision.barcode.BarcodeScanning
import com.google.mlkit.vision.common.InputImage
import androidx.datastore.preferences.core.stringSetPreferencesKey
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch


@Composable
// Manual login screen with text input field for access code verification
fun LogicScreen(onLoginSuccess: (String) -> Unit, onBackToChoice: (() -> Unit)? = null) {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()
    val credentials = setCredentialAdmin()
    var accessCode by rememberSaveable { mutableStateOf("") }
    var loginError by rememberSaveable { mutableStateOf(false) }

    Column(
        modifier = Modifier.fillMaxSize(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(text = "Welcome to Login Page", fontSize = 28.sp, fontWeight = FontWeight.Bold)
        Spacer(modifier = Modifier.height(5.dp))
        Text(text = "Enter your access code", fontSize = 20.sp, fontWeight = FontWeight.Thin)

        Spacer(modifier = Modifier.height(20.dp))

        TextField(
            value = accessCode,
            onValueChange = { accessCode = it.lowercase() }, // Convert to lowercase for consistency
            label = { Text(text = "Access Code") },
            singleLine = true,
            colors = TextFieldDefaults.colors(
                unfocusedContainerColor = LightGray,
                focusedContainerColor = LightGray,
                cursorColor = DarkGray,
                focusedIndicatorColor = DarkGray,
                unfocusedIndicatorColor = LightGray,
                focusedLabelColor = DarkGray,
                unfocusedLabelColor = DarkGray,
                focusedTextColor = DarkGray,
                unfocusedTextColor = DarkGray
            )
        )
        Spacer(modifier = Modifier.height(20.dp))

        if (loginError) {
            Text(text = "Invalid access code", color = Color.Red, fontSize = 14.sp)
            Spacer(modifier = Modifier.height(10.dp))
        }

        Button(
            onClick = {
                scope.launch {
                    val prefs = context.dataStore.data.first()
                    val storedCodes = prefs[stringSetPreferencesKey("access_codes")] ?: setOf()
                    val defaultCodes = credentials.map { it.code }.toSet()
                    val allValidCodes = storedCodes + defaultCodes
                    
                    val isValidCode = allValidCodes.contains(accessCode)
                    
                    if (isValidCode) {
                        loginError = false
                        onLoginSuccess(accessCode)
                    } else {
                        loginError = true
                    }
                }
            },
            colors = ButtonDefaults.buttonColors(
                containerColor = LightGray,
                contentColor = DarkGray
            )
        ) {
            Text(text = "Login")
        }
        
        onBackToChoice?.let {
            Spacer(modifier = Modifier.height(10.dp))
            TextButton(onClick = it) {
                Text(
                    text = "Back to Login Options",
                    color = DarkGray,
                    fontSize = 14.sp
                )
            }
        }
        
        Spacer(modifier = Modifier.height(5.dp))
        Text(
            text = "Need help? Contact admin",
            fontSize = 15.sp,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.clickable { }
        )
    }
}



@Composable
// QR code scanner screen using camera preview and ML Kit barcode detection
fun ScanQrScreen(
    modifier: Modifier = Modifier,
    onQrCodeScanned: (String) -> Unit,
    onBackToChoice: (() -> Unit)? = null
) {
    val lifecycleOwner = androidx.lifecycle.compose.LocalLifecycleOwner.current
    val scope = rememberCoroutineScope()

    var hasScanned by remember { mutableStateOf(false) }

    Box(modifier = modifier.fillMaxSize()) {
        AndroidView(
            factory = { ctx ->
                val previewView = PreviewView(ctx).apply {
                    layoutParams = ViewGroup.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT
                    )
                }

                val cameraProviderFuture = ProcessCameraProvider.getInstance(ctx)
                cameraProviderFuture.addListener({
                    val cameraProvider = cameraProviderFuture.get()
                    val preview = Preview.Builder().build().also {
                        it.setSurfaceProvider(previewView.surfaceProvider)
                    }
                    val scanner = BarcodeScanning.getClient()

                    val imageAnalysis = ImageAnalysis.Builder()

                        .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                        .build()

                    imageAnalysis.setAnalyzer(ContextCompat.getMainExecutor(ctx)) { imageProxy: ImageProxy ->
                        if (!hasScanned) {
                            processImageProxy(scanner, imageProxy) { result ->
                                if (result != null && !hasScanned) {
                                    hasScanned = true
                                    scope.launch {
                                        onQrCodeScanned(result)
                                    }
                                }
                            }
                        } else {
                            imageProxy.close()
                        }
                    }

                    val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA
                    cameraProvider.unbindAll()
                    cameraProvider.bindToLifecycle(
                        lifecycleOwner, cameraSelector, preview, imageAnalysis
                    )
                }, ContextCompat.getMainExecutor(ctx))

                previewView
            },
            modifier = Modifier.fillMaxSize()
        )
        
        // Back button overlay
        onBackToChoice?.let { backAction ->
            Box(
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(16.dp)
            ) {
                Button(
                    onClick = backAction,
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color.Black.copy(alpha = 0.7f),
                        contentColor = Color.White
                    ),
                    modifier = Modifier.size(48.dp),
                    contentPadding = PaddingValues(0.dp)
                ) {
                    Text("←", fontSize = 20.sp)
                }
            }
        }
    }
}

@OptIn(ExperimentalGetImage::class)
// Processes camera images to detect and decode QR codes using ML Kit
private fun processImageProxy(
    scanner: BarcodeScanner,
    imageProxy: ImageProxy,
    onScanned: (String?) -> Unit
) {
    Log.d("QR", "processImageProxy called")
    val mediaImage = imageProxy.image
    if (mediaImage != null) {
        val image = InputImage.fromMediaImage(mediaImage, imageProxy.imageInfo.rotationDegrees)
        scanner.process(image)
            .addOnSuccessListener { barcodes ->
                Log.d("QR", "Barcodes found: ${barcodes.size}")
                val result = barcodes.firstOrNull()?.rawValue
                Log.d("QR", "QR Value: $result")
                onScanned(result)
            }
            .addOnFailureListener { e ->
                Log.e("QR", "Scanner error: ${e.message}")
                onScanned(null)
            }
            .addOnCompleteListener {
                imageProxy.close()
            }
    } else {
        Log.e("QR", "mediaImage is null")
        imageProxy.close()
        onScanned(null)
    }
}

