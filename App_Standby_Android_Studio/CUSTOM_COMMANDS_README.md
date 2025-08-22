# Système de Commandes Personnalisées - StandbyApp

## Vue d'ensemble

Le système de commandes personnalisées permet aux utilisateurs de configurer des boutons avec des propriétés visuelles et fonctionnelles personnalisées pour contrôler l'équipement STM32 via Bluetooth.

## Architecture

### 1. Modèle de données - `CustomCommand`

```kotlin
data class CustomCommand(
    val id: Int,                    // ID unique de la commande
    val name: String,               // Nom affiché sur le bouton
    val colorHex: String,           // Couleur en format hex (#RRGGBB)
    val iconResourceId: Int?,       // ID ressource d'icône drawable (optionnel)
    val iconName: String?,          // Nom icône Material (optionnel)
    val commandId: Int,             // ID de commande STM32 (0-255)
    val activeCommand: Int = 1,     // Commande envoyée quand actif
    val inactiveCommand: Int = 0,   // Commande envoyée quand inactif
    val description: String = "",   // Description optionnelle
    val isEnabled: Boolean = true   // Si la commande est activée
)
```

### 2. Gestion des données - `CustomCommandViewModel`

- Sauvegarde persistante avec DataStore
- Gestion des commandes (ajouter/modifier/supprimer)
- Support des commandes par défaut
- Validation des entrées

### 3. Interface utilisateur - `SettingsScreen`

#### Fonctionnalités principales :

- **Liste des commandes** : Affichage de toutes les commandes configurées
- **Édition en place** : Modification des commandes existantes
- **Création de nouvelles commandes** : Bouton FAB pour ajouter
- **Activation/désactivation** : Switch pour chaque commande
- **Réinitialisation** : Retour aux valeurs par défaut
- **Sélecteur de couleurs** : Choix parmi des couleurs prédéfinies

#### Interface d'édition :

- **Nom de la commande** : Texte affiché sur le bouton
- **Description** : Information optionnelle
- **Couleur** : Sélection visuelle via cercles colorés
- **ID Commande** : Identifiant STM32 (0-255)
- **Commande Actif** : Valeur envoyée quand le bouton est activé
- **Commande Inactif** : Valeur envoyée quand le bouton est désactivé

### 4. Intégration Bluetooth - `STM32Commands`

Nouvelles méthodes ajoutées :

```kotlin
// Création de commande pour un bouton personnalisé
fun createCustomButtonCommand(customCommand: CustomCommand, isActive: Boolean): ByteArray

// Création de commandes pour multiple boutons personnalisés
fun createAppCommandFromCustomCommands(
    customCommands: List<CustomCommand>, 
    buttonStates: Map<Int, Boolean>
): ByteArray
```

### 5. Panneau de contrôle mis à jour

- `DynamicControlPanel` charge automatiquement les commandes personnalisées
- `ConfigurableButton` supporte les commandes personnalisées
- Envoi automatique des bonnes commandes STM32 selon la configuration
- Fallback vers l'ancien système pour compatibilité

## Utilisation

### Pour l'utilisateur :

1. **Accéder aux paramètres** : Onglet "Settings" dans la navigation
2. **Voir les commandes** : Liste des commandes configurées avec statut
3. **Modifier une commande** : Bouton éditer (crayon) sur chaque carte
4. **Ajouter une commande** : Bouton + flottant en bas à droite
5. **Activer/désactiver** : Switch sur chaque carte de commande
6. **Réinitialiser** : Bouton "Réinitialiser aux valeurs par défaut"

### Configuration d'une commande :

1. **Nom** : Entrer le texte à afficher sur le bouton
2. **Description** : Ajouter une description optionnelle
3. **Couleur** : Sélectionner parmi les couleurs prédéfinies
4. **ID Commande** : Numéro unique pour le STM32 (0-255)
5. **Commande Actif** : Valeur envoyée quand le bouton est activé
6. **Commande Inactif** : Valeur envoyée quand le bouton est désactivé

### Exemples de configuration :

#### Commande Emergency (par défaut) :
- Nom : "EMERGENCY"
- Couleur : Rouge (#F24E4E)
- ID Commande : 0
- Actif : 1, Inactif : 0

#### Commande personnalisée LED :
- Nom : "LED Strip"
- Couleur : Violet (#9932CC)
- ID Commande : 10
- Actif : 255 (pleine intensité), Inactif : 0

## Stockage des données

Les commandes sont sauvegardées dans DataStore avec sérialisation JSON :
- Fichier : `custom_commands.preferences_pb`
- Format : JSON sérialisé des objets `CustomCommand`
- Persistence : Survit aux redémarrages de l'application

## Commandes par défaut

Au premier lancement, 4 commandes sont créées :

1. **EMERGENCY** (ID:0) - Rouge - Commande d'urgence
2. **FRONT LIGHTS** (ID:1) - Vert - Feux avant
3. **REAR LIGHTS** (ID:2) - Bleu - Feux arrière  
4. **SIREN** (ID:3) - Magenta - Sirène

## Protocole STM32

Les commandes suivent le protocole existant :
- **Header** : "CAPP" (0x70706163)
- **Format** : [ID_BOUTON][COMMANDE] pour chaque bouton
- **Taille** : 2 octets par bouton dans la trame

Exemple de trame pour bouton ID=5, commande=3 :
```
[Header CAPP][Taille][Clé][05][03][CRC]
```

## Extension future

Le système est conçu pour être facilement extensible :

- **Icônes personnalisées** : Support des icônes Material et ressources
- **Animations** : Possibilité d'ajouter des effets visuels
- **Groupes de commandes** : Organisation en catégories
- **Import/Export** : Sauvegarde et partage de configurations
- **Commandes temporisées** : Activation automatique après délai
- **Séquences** : Exécution de multiple commandes en série

## Dépendances

- `kotlinx.serialization` : Sérialisation des données
- `androidx.datastore` : Stockage persistant
- `androidx.compose` : Interface utilisateur
- Modules Bluetooth existants : Communication STM32
