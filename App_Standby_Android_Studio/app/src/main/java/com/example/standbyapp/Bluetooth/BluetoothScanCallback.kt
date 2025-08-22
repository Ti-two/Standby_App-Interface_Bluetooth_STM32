package com.example.stmbluetooth

import android.bluetooth.BluetoothDevice

/**
 * Interface de callback pour le BluetoothScanner avec gestion complète
 * des événements de scan, connexion et authentification
 */
interface BluetoothScanCallback {
    /**
     * Événements de scan
     */
    fun onScanStarted()
    fun onScanStopped()
    fun onDeviceFound(device: BluetoothDevice)
    
    /**
     * Événements de connexion
     */
    fun onDeviceConnected(device: BluetoothDevice)
    fun onDeviceDisconnected()
    
    /**
     * Changement de statut de connexion avec détails
     */
    fun onConnectionStatusChanged(isConnected: Boolean, status: String)
    
    /**
     * Événements d'authentification
     */
    fun onAuthenticationSuccess()
    fun onAuthenticationFailed()
    
    /**
     * Événements de données
     */
    fun onDataReceived(data: ByteArray)
    fun onDataSent()
    
    /**
     * Gestion des erreurs
     */
    fun onError(message: String)
}
