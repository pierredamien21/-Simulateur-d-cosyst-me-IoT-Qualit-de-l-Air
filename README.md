# AP4A — Simulateur d'Écosystème IoT & Qualité de l'Air

> Projet de TP — Automne 2026
> Binôme : Nom1 — Nom2
> Groupe : TP__A
> Rendu : 6 novembre 2026, Moodle

---

## Table des matières

1. [Présentation du projet](#1-présentation-du-projet)
2. [Objectifs](#2-objectifs)
3. [Architecture logicielle](#3-architecture-logicielle)
4. [Fonctionnalités](#4-fonctionnalités)
5. [Design patterns](#5-design-patterns)
6. [Structure du projet](#6-structure-du-projet)
7. [Prérequis](#7-prérequis)
8. [Compilation](#8-compilation)
9. [Exécution](#9-exécution)
10. [Répartition du travail](#10-répartition-du-travail)
11. [Problèmes rencontrés et solutions](#11-problèmes-rencontrés-et-solutions)
12. [Validation et tests](#12-validation-et-tests)
13. [Perspectives](#13-perspectives)
---

## 1. Présentation du projet

Le projet consiste à concevoir et implémenter un **simulateur d'écosystème IoT** orienté objet en **C++17**, modélisant le monitoring de la qualité de l'air d'un espace de travail.

Le système reproduit, via appels de méthodes (métaphore objet), la communication entre :
- Un **serveur central** (`Server`) qui orchestre la simulation.
- Un ensemble hétérogène de **capteurs autonomes** (`Sensor` et 5 spécialisations).
- Un **ordonnanceur** (`Scheduler`) qui anime les processus capteurs.
- Des **actionneurs virtuels** (`Actuator`) qui modifient l'environnement.
- Un **module d'analyse** (`DataAnalyzer` + modèles prédictifs).
- Une **interface graphique Qt** (`DashboardView`) pour la visualisation.

---

## 2. Objectifs

### 2.1 Objectifs principaux (cahier des charges AP4A)

- Modéliser un écosystème objet complet : `Server`, `Scheduler`, `Sensor`.
- Implémenter la **forme canonique de Coplien** pour `Server`.
- Fournir les services `consoleWrite` et `fileWrite` (logs CSV par type de capteur).
- Surcharger `operator<<` (friend) avec redirection console / fichier.
- Mettre en œuvre **héritage**, **polymorphisme**, **classes abstraites**.
- Utiliser la **STL** (conteneurs, itérateurs, algorithmes).
- Utiliser `<chrono>` et `<random>`.
- Assurer une compilation multi-plateforme via **CMake**.
- Livrer un **diagramme UML complet** (PlantUML) et un **rapport PDF**.

### 2.2 Objectifs secondaires (Portfolio IA / Data)

- Module d'analyse intelligent : indice de qualité de l'air (AQI).
- Modèles prédictifs interchangeables (régression linéaire, détection d'anomalies).
- Actionneurs autonomes pilotés par décision.
- Interface graphique **Qt6** selon une architecture **MVC stricte**.
- Design patterns justifiés : **Factory**, **Strategy**, **Singleton**, **Observer**.

---

## 3. Architecture logicielle

### 3.1 Séparation Modèle / Vue / Contrôleur

```
┌──────────────────────────────────────────────────────────────┐
│                        MODELE (C++ pur)                      │
│                                                              │
│   Scheduler ──► Sensor ──► Server ──► DataAnalyzer           │
│                  (5)         │                               │
│                              ├──► PredictiveModel            │
│                              ├──► Actuator (Ventilation, ...)│
│                              └──► RoomState (physique)       │
└──────────────────────────────────────────────────────────────┘
                              ▲
                              │ lecture seule
                              │
┌──────────────────────────────────────────────────────────────┐
│                    VUE + CONTROLEUR (Qt)                     │
│                                                              │
│   DashboardController (QObject) ◄──► DashboardView           │
│            ▲                              │                  │
│            │ signaux/slots                ├──► ChartWidget   │
│            │                              └──► KPI / Actions │
└──────────────────────────────────────────────────────────────┘
```

**Règle stricte** : le Core (`src/core/`) ne contient **aucun `#include <Q...>`**.
Qt est cantonné à `src/ui/`. Le core est compilable sans Qt installé.

### 3.2 Structure des composants

| Composant | Type | Rôle |
|---|---|---|
| **`Server`** | Classe (Coplien) | Reçoit les mesures, écrit les logs, orchestre analyse et décision |
| **`Sensor`** | Classe abstraite | Processus autonome de mesure |
| **5 spécialisations** | Héritage | `TemperatureSensor`, `HumiditySensor`, `Co2Sensor`, `SoundSensor`, `LightSensor` |
| **`Scheduler`** | Singleton | Anime les capteurs selon une base de temps globale |
| **`Actuator`** | Interface | Actionneurs (`VentilationSystem`, `AirPurifier`) |
| **`DataAnalyzer`** | Classe | Fenêtre glissante, statistiques, AQI |
| **`PredictiveModel`** | Interface | Stratégies d'analyse (`LinearRegressionModel`, `AnomalyDetectorModel`) |
| **`RoomState`** | Singleton | État physique de la pièce (CO2, température, occupants, ...) |
| **`SensorFactory`** | Classe utilitaire | Instanciation dynamique par type textuel |

### 3.3 Double base de temps

- **Base globale** (`Scheduler`) : 1 tick = 100 ms simulés.
- **Base interne** (`Sensor`) : chaque capteur a sa propre `period` (500, 800, 1000, 1500, 2000 ms).

Le `Scheduler` appelle `update()` sur **tous** les capteurs à chaque tick global.
Chaque capteur décide d'appeler `execute()` selon sa propre période interne.

### 3.4 Physique simulée (`RoomState`)

À chaque tick, la classe `RoomState` applique des règles réalistes :

| Grandeur | Règle |
|---|---|
| **CO2** | +0.15 ppm/occupant (respiration) — 5 ppm si ventilation ON |
| **Température** | Tend vers 26 °C sans ventilation / 20 °C avec |
| **Humidité** | Tend vers 55 % sans ventilation / 40 % avec |
| **Son** | 40 dB de jour + 1.5 dB/occupant ; 30 dB la nuit |
| **Lumière** | 1.0 si jour, 0.0 si nuit |
| **Occupants** | Varie selon l'heure simulée (cycle jour/nuit) |

**Résultat observé** : cycles crédibles de montée → décision → correction → stabilisation.

---

## 4. Fonctionnalités

### 4.1 Simulation

- Génération de données hétérogènes (FLOAT, INTEGER, BOOLEAN).
- Cycle temporel simulé (jour/nuit, heures de pointe).
- Physique réaliste (CO2 qui monte avec les occupants, ventilation qui le fait baisser).

### 4.2 Analyse et décision

- Calcul d'un **AQI** (indice de qualité de l'air) pondéré (0-100).
- **Régression linéaire** sur le CO2 pour prédire le *Time-to-Threshold*.
- **Détection d'anomalies** par z-score (|z| > 3).
- **Décision autonome** : activation automatique de la ventilation et du purificateur.
- **Notification** des événements critiques (pattern Observer).

### 4.3 Logs

Un fichier CSV par type de capteur, plus deux fichiers d'analyse :

```
logs/
├── temperature.csv     # timestamp,id,type,value,unit
├── humidity.csv
├── co2.csv
├── sound.csv
├── light.csv
├── predictions.csv     # timestamp,kind,value,unit (AQI, TTT)
└── actuators.csv       # timestamp,actuator,state (ON/OFF)
```

Format **ISO 8601** pour les timestamps → exploitable par Excel, Python, pandas.

### 4.4 Interface graphique Qt

- **Panneau KPI** : AQI, CO2 moyen, Time-to-Threshold, Température moyenne.
- **Panneau Mesures en direct** : valeurs instantanées des 5 capteurs.
- **Panneau Actionneurs** : état ON/OFF de la ventilation et du purificateur.
- **Grille 2×3 de graphiques** temps réel avec échelles Y adaptées.
- **Boutons Play / Pause / Reset**.

---

## 5. Design patterns

| Pattern | Rôle | Implémentation |
|---|---|---|
| **Factory** | Création dynamique de capteurs | `SensorFactory::create(type, server)` |
| **Strategy** | Algorithmes IA interchangeables | `PredictiveModel` + `setPredictor()` sur `Server` |
| **Singleton** | Instance unique globale | `Scheduler` et `RoomState` (idiome de Meyers) |
| **Observer** | Notification d'événements critiques | `Server::notify()` déclenché par `Sensor` |

---

## 6. Structure du projet

```
AP4A_IoT_AirQuality/
├── CMakeLists.txt              # Build principal multi-plateforme
├── .gitignore
├── README.md                   
│
├── include/
│   ├── core/                   # En-têtes C++ purs (sans Qt)
│   │   ├── Config.h.in         # Template CMake pour les chemins absolus
│   │   ├── Server.h
│   │   ├── Sensor.h
│   │   ├── Scheduler.h
│   │   ├── SensorFactory.h
│   │   ├── TemperatureSensor.h
│   │   ├── HumiditySensor.h
│   │   ├── Co2Sensor.h
│   │   ├── SoundSensor.h
│   │   ├── LightSensor.h
│   │   ├── Actuator.h
│   │   ├── VentilationSystem.h
│   │   ├── AirPurifier.h
│   │   ├── DataAnalyzer.h
│   │   ├── PredictiveModel.h
│   │   ├── LinearRegressionModel.h
│   │   ├── AnomalyDetectorModel.h
│   │   └── RoomState.h
│   │
│   └── ui/                     # En-têtes Qt
│       ├── ChartWidget.h
│       ├── DashboardView.h
│       └── DashboardController.h
│
├── src/
│   ├── core/                   # Implémentations C++ pures
│   │   └── *.cpp
│   ├── ui/                     # Implémentations Qt
│   │   └── *.cpp
│   ├── main_core.cpp           # Point d'entrée console
│   └── main_qt.cpp             # Point d'entrée GUI
|
│
└── logs/                       # Fichiers CSV générés (non versionnés)
    
```

---

## 7. Prérequis

### Système

- **Windows 10/11** ou **Linux** (testé sous WSL Ubuntu).
- **Visual Studio 2022** (workload « Développement Desktop en C++ » + CMake tools).
- **CMake ≥ 3.20** (inclus dans VS).
- **Ninja** (inclus dans VS).
- **Qt6 ≥ 6.5** avec modules `Widgets` et `Charts` (pour la GUI).

### Optionnel (validation)

- **WSL2 + Ubuntu** pour `valgrind`.
- **Java JRE 8+** + **PlantUML** pour régénérer les diagrammes.

---

## 8. Compilation

### 8.1 Visual Studio 2022 (méthode recommandée)

1. `File → Open → Folder` → sélectionner la racine du projet.
2. VS détecte `CMakeLists.txt` et configure automatiquement.
3. `Ctrl+Shift+B` → build.
4. Deux binaires sont produits :
   - `out/build/x64-Debug/simulator_core.exe`
   - `out/build/x64-Debug/simulator_gui.exe`

### 8.2 Ligne de commande (Windows)

```powershell
cmake -B out/build/x64-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/x64-Debug
```

### 8.3 Ligne de commande (WSL / Linux)

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### 8.4 Configuration Qt

Le `CMakeLists.txt` localise Qt6 automatiquement. Si Qt est dans un chemin non standard :

```bash
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.11.2/msvc2022_64"
```

Si Qt n'est pas trouvé, seule la cible console est compilée (`simulator_gui` est désactivée).

### 8.5 Ajout de Qt au PATH (Windows, pour exécution)

Pour que `simulator_gui.exe` trouve les DLLs Qt au démarrage :

```
C:\Qt\6.11.2\msvc2022_64\bin
```

À ajouter dans les **Variables d'environnement utilisateur → Path**.

---

## 9. Exécution

### 9.1 Console (`simulator_core`)

```powershell
./out/build/x64-Debug/simulator_core.exe
```

Sortie type :
```
==============================================
 AP4A - Simulateur IoT Qualite de l'Air
 Version console - Phase 3 finale
==============================================

[1] Server initialise : 7 fichiers de logs
[2] IA + actionneurs injectes
    Server [logs=7, recentMessages=0, actuators=2, hasPredictor=yes]
[3] 5 capteurs crees via SensorFactory
[4] Simulation de 500 ticks (50 s simulees)
...
```

### 9.2 Interface graphique (`simulator_gui`)

```powershell
./out/build/x64-Debug/simulator_gui.exe
```

Une fenêtre Qt s'ouvre avec :
- Le dashboard complet (KPI, mesures, actionneurs).
- 5 graphiques temps réel.
- Les boutons Play / Pause / Reset.

### 9.3 Vérification mémoire (valgrind sous WSL)

```bash
valgrind --leak-check=full --show-leak-kinds=all ./build/simulator_core
```

Résultat attendu : `All heap blocks were freed -- no leaks are possible`.

---

## 10. Répartition du travail

Le projet a été développé en **binôme** avec une répartition claire :

| Domaine | Nom1 | Nom2 |
|---|---|---|
| **Conception UML** | Diagrammes 1-4 | Diagrammes 5-8 |
| **Core C++** | Server, Sensor, Scheduler | DataAnalyzer, IA, RoomState |
| **Design patterns** | Factory, Singleton | Strategy, Observer |
| **Tests** | Console + CSV | Qt + valgrind |
| **Rapport PDF** | Sections 1-3 | Sections 4-6 |
| **Relecture** | Croisée | Croisée |

Chaque commit a été discuté et validé en binôme. Le versioning Git a permis de suivre la contribution de chacun.

---

## 11. Problèmes rencontrés et solutions

### 11.1 Qt Visual Studio Tools — conflit de configuration

**Problème** : Visual Studio 2022, dès qu'il détecte Qt installé, crée automatiquement une configuration `Qt-Debug` et régénère `CMakePresets.json` avec un vendor `qt-project.org`. Cela écrase nos presets et casse le build.

**Solution** :
1. Désactiver l'extension **Qt Visual Studio Tools** (Extensions → Gérer).
2. Supprimer `CMakePresets.json` régénéré et `out/`.
3. Retirer tout bloc `find_package(Qt6)` du `CMakeLists.txt` **pendant les Phases 1-3**.
4. Ne réintroduire Qt qu'en Phase 4, proprement, avec une cible séparée.

### 11.2 LNK2001 — symboles metaObject non résolus

**Problème** : `ChartWidget` utilise `Q_OBJECT` mais AUTOMOC ne génère pas les fichiers `moc_*.cpp`.
Cause : `ChartWidget.h` est dans `include/ui/`, `ChartWidget.cpp` dans `src/ui/`. AUTOMOC ne fait pas le lien.

**Solution** : déclarer explicitement les headers dans la cible CMake :

```cmake
file(GLOB UI_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ui/*.h"
)
add_executable(simulator_gui
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${UI_HEADERS}     # AUTOMOC scanne ces fichiers
    src/main_qt.cpp
)
```

### 11.3 C2027 / C2338 — unique_ptr sur type incomplet

**Problème** : `std::unique_ptr<Sensor>` dans `main_core.cpp` provoque :
```
C2027 : utilisation du type non défini 'Sensor'
C2338 : static assertion failed: can't delete an incomplete type
```

**Cause** : `SensorFactory.h` fait une forward declaration de `Sensor` (`class Sensor;`), suffisante pour **transférer** le pointeur mais pas pour le **détruire**.

**Solution** : inclure la **définition complète** dans tout fichier qui détruit un `unique_ptr<T>` :

```cpp
#include "core/Sensor.h"   // obligatoire dès qu'on manipule unique_ptr<Sensor>
```

### 11.4 LNK1168 — impossible d'écrire simulator_gui.exe

**Problème** : le linker ne peut pas écraser l'exécutable car une instance tourne encore.

**Solution** :

```powershell
Get-Process simulator_gui | Stop-Process -Force
Get-Process simulator_core | Stop-Process -Force
```

Puis rebuild. Règle : **toujours fermer la fenêtre Qt avant de compiler**.

### 11.5 Bug silencieux de casse sur std::map

**Problème** : `std::map` est sensible à la casse. Les constructeurs utilisent `"Temperature"`, `"CO2"` (capitalisés) mais `computeAQI()` cherchait `"co2"`, `"temperature"` (minuscules). Résultat : statistiques toujours à 0, AQI erroné.

**Solution** : normalisation systématique des clés à l'entrée et à la sortie du `DataAnalyzer` via une méthode `normalize()` :

```cpp
static std::string normalize(const std::string& type)
{
    std::string key = type;
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return key;
}
```

**Principe** : *« Une classe doit être défensive vis-à-vis de ses entrées : elle ne fait pas confiance à l'appelant pour respecter une convention interne. »*

### 11.6 Signature const sur méthode virtuelle

**Problème** : `override` exige une signature **strictement identique**, y compris les qualificateurs. `predict()` déclarée `const` dans `PredictiveModel` doit l'être aussi dans les dérivées.

**Solution** :

```cpp
// Interface
virtual double predict(const std::deque<double>& history) const = 0;

// Dérivée
double predict(const std::deque<double>& history) const override;
```

### 11.7 Chemin de logs dépendant du répertoire d'exécution

**Problème** : le binaire écrit dans `./logs/` relatif au **répertoire courant** d'exécution. Résultat : les CSV se retrouvent dans `out/build/x64-Debug/logs/` au lieu de la racine.

**Solution** : générer `Config.h` via CMake avec un chemin **absolu** :

```cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core/Config.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core/Config.h"
    @ONLY
)
```

Et dans `Config.h.in` :

```cpp
#define AP4A_PROJECT_ROOT "@CMAKE_CURRENT_SOURCE_DIR@"
```

**Avantage** : fonctionne partout (VS, ligne de commande, WSL) indépendamment du répertoire courant.

### 11.8 Qt6 DLL introuvables au démarrage

**Problème** : `simulator_gui.exe` compile mais Windows affiche :
> « Qt6Widgetsd.dll est introuvable »

**Cause** : Windows cherche les DLLs dans le **PATH**, pas dans le dossier Qt d'installation.

**Solution** : ajouter `C:\Qt\6.11.2\msvc2022_64\bin` au **PATH utilisateur**.

**Solution pour le livrable final** : utiliser `windeployqt.exe` pour rendre l'exe autonome :

```powershell
C:\Qt\6.11.2\msvc2022_64\bin\windeployqt.exe simulator_gui.exe
```

---

## 12. Validation et tests

### 12.1 Tests unitaires informels

Chaque phase a été validée par un `main` de test dédié :

| Phase | Test | Validation |
|---|---|---|
| 1 | Server Coplien + CSV | 9 étapes, 5 fichiers créés |
| 2 | Sensor + Scheduler | Double base de temps vérifiée (CO2 = 20 mesures) |
| 3.2 | 5 spécialisations | Plages de valeurs conformes |
| 3.3 | SensorFactory | Création dynamique OK |
| 3.4 | Actuator | Polymorphisme OK |
| 3.5 | DataAnalyzer | Fenêtre glissante + AQI |
| 3.6 | Modèles IA | Régression + anomalie |
| 3.7 | Scheduler Singleton | Adresses identiques |
| 3.8 | Intégration | Cycle complet |
| 4.x | Qt | Dashboard temps réel |

### 12.2 Vérification mémoire (valgrind)

```bash
valgrind --leak-check=full ./build/simulator_core
```

**Résultat** : 0 erreur, 0 fuite.

### 12.3 Compilation multi-plateforme

Testée :
- MSVC 2022 sous Windows 11.
- GCC sous WSL Ubuntu.

### 12.4 Compilation sans warning

`-Wall -Wextra -Wpedantic` (GCC) / `/W4 /permissive-` (MSVC).

---

## 13. Perspectives

### 13.1 Améliorations possibles

- **Persistance** : charger/sauvegarder la configuration de la pièce.
- **Multi-pièces** : gérer plusieurs espaces indépendants.
- **Threads** : paralléliser les capteurs (au lieu du Scheduler séquentiel).
- **IA avancée** : réseaux de neurones légers (perceptron) pour la prédiction.
- **Statistiques** : export des logs vers Python/pandas pour analyse externe.

### 13.2 Ouverture Machine Learning

Le module IA actuel (régression linéaire, z-score) est un **premier pas** vers un pipeline ML complet :
- Préparation des données : CSV normalisé par type.
- Extraction de features : moyennes glissantes, variances.
- Modèles : régression linéaire → polynômiale → réseau de neurones.
- Évaluation : MAE, RMSE sur la prédiction du Time-to-Threshold.

Une intégration Python serait possible via sous-processus ou binding `pybind11`, mais le projet actuel privilégie un **C++ pur** pour respecter le cadre pédagogique.

---


