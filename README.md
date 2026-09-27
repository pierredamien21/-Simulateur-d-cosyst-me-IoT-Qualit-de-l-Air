# AP4A — Simulateur d'Écosystème IoT & Qualité de l'Air

Projet AP4A Automne 2026 — Binôme : Nom1 / Nom2 — Groupe TP___.

## Prérequis

- Visual Studio 2022 (workload "Développement Desktop en C++" + CMake tools)
- CMake ≥ 3.20
- Ninja (inclus dans VS 2022)
- Qt6 (optionnel, Phase 4)
- WSL2 + valgrind (pour la vérification mémoire)

## Compilation (Windows)

Ouvrir le dossier dans VS 2022 (`File → Open → Folder`), VS configure
automatiquement le projet CMake. `Ctrl+Shift+B` pour builder, `F5` pour debug.

En ligne de commande :

```bash
cmake --preset windows-debug
cmake --build --preset windows-debug
./build/debug/simulator_core.exe
```

## Compilation (WSL / Linux)

```bash
cmake --preset wsl-debug
cmake --build --preset wsl-debug
./build/wsl-debug/simulator_core
valgrind --leak-check=full ./build/wsl-debug/simulator_core
```

## Structure

- `include/core/` : en-têtes du coeur C++ (sans Qt)
- `src/core/` : implémentations du coeur
- `src/main_core.cpp` : point d'entrée console
- `docs/uml/` : diagrammes PlantUML
- `logs/` : fichiers CSV générés à l'exécution

## Livrables

- `Rapport_AP4A_TP__A_Nom1_Nom2.pdf`
- `Code_AP4A_TP__A_Nom1_Nom2.zip`
- Rendu : 6 novembre 2026, Moodle.