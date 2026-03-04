<div align="center">

# CODE80

### Historical Event Manager

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Database](https://img.shields.io/badge/database-SQLite-003B57)
![Build](https://img.shields.io/badge/build-Makefile-success)
![UI](https://img.shields.io/badge/UI-ImGui%2FGLFW-orange)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey)

[Presentation](./CODE80.pptx)

[Documentation](./CODE80Documentation.docx)

</div>

---

## 📜 About

**CODE80** is a C++ application for recording, organizing, and exploring historical events.  
It supports persistent storage through SQLite, event editing, filtering, and report generation.

The project includes:
- a console workflow (`app/main.cpp`),
- a GUI workflow (`app/gui.cpp`, with Dear ImGui when available),
- a reusable static library with core logic (`libtimeline`).

---

## 🏛️ How does it work?

The system stores events in a SQLite table and provides operations to:
- add new events,
- update existing records,
- delete by ID,
- search by year,
- search by theme,
- print/export report-style outputs from stored data.

The in-memory linked-list layer is used for ordered traversal and report generation, while SQLite acts as the source of truth.

---

## 🛠️ Languages, Frameworks, and Tools

### 💻 Core Stack
- C++17
- SQLite3
- Make

### 🖼️ GUI Stack (optional)
- Dear ImGui
- GLFW3
- OpenGL

### 🧰 Development Environment
- GCC / G++
- Visual Studio project files (`visual-studio/`)
- `pkg-config` (for ImGui/GLFW detection in Makefile)

---

## 💡 Features

- Add historical events with full metadata
- Edit existing events by ID
- Delete events by ID
- Search events by year
- Search events by theme keyword
- Show all events in chronological order
- Generate textual reports from stored entries
- Persist all records in a local SQLite database

---

## 🗂️ Project Structure

```text
CODE80/
├─ app/
│  ├─ main.cpp          # Console menu application
│  └─ gui.cpp           # GUI application (ImGui mode when available)
├─ include/             # Public headers
├─ src/                 # Core implementation
├─ visual-studio/       # Visual Studio solution/projects
├─ Makefile
└─ vcpkg.json
```

---

## ⚙️ Build and Run

### 1) Build everything

```bash
make all
```

Artifacts:
- `lib/libtimeline.a`
- `bin/timeline_gui`
- `bin/test_runner` *(if tests source is present)*

### 2) Run GUI app

```bash
make run-gui
```

### 3) Run tests

```bash
make test
```

### 4) Clean build outputs

```bash
make clean
```

> Note: If `pkg-config` cannot find ImGui/GLFW packages, the Makefile still builds a fallback `timeline_gui` binary without ImGui mode.

---

## 🗃️ Event Data Model

Each event contains:
- `id`
- `month`, `year`
- `theme`
- `title`
- `place`
- `leader`
- `participants`
- `result`

Default database file: `events.db` in the project root.

---

## 🚀 Quick Start

1. Install dependencies (`g++`, `sqlite3`; optionally `glfw3` + `imgui`).
2. Build with `make all`.
3. Run with `make run-gui`.
4. Add, filter, and manage historical events.

---

## 🪟 Visual Studio

For Windows development, use:
- `visual-studio/Timeline.sln`
- `timeline_console.vcxproj`
- `timeline_gui.vcxproj`
- `timeline_lib.vcxproj`
- `timeline_tests.vcxproj`
