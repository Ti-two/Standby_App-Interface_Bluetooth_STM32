package com.example.standbyapp

import android.content.Context
import androidx.datastore.preferences.preferencesDataStore

// Extension property to create and access user preferences datastore
val Context.dataStore by preferencesDataStore(name = "user_prefs")
