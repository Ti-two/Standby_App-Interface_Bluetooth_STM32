package com.example.standbyapp

import androidx.room.Entity
import androidx.room.PrimaryKey

// Data class representing user credentials with ID and access code
data class Credential(
    val id: Int = 0,
    val code: String
)

// Returns a predefined list of admin credentials for authentication
fun setCredentialAdmin() : List<Credential>{
    return listOf<Credential>(
        Credential(1, "standby"),
        Credential(2, "admin"),
        Credential(3, "code00"),
        Credential(4, "33323-00"),
        Credential(5, "33323-00/06/25")
    )
}