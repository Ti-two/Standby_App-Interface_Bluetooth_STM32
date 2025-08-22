package com.example.stmbluetooth

interface BluetoothCallback {
    fun onConnectionStatusChanged(isConnected: Boolean, status: String)
    fun onError(message: String)
    fun onDataReceived(data: ByteArray)
}