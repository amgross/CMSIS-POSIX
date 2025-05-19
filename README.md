# CMSIS‑POSIX

[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)

A partial implementation of the CMSIS‑RTOS2 API on a POSIX platform for testing and mocking embedded code.

---

## 📖 Overview

CMSIS‑POSIX allows you to build and test CMSIS‑RTOS2‑based applications on a POSIX system (e.g., Ubuntu Linux) by mapping RTOS2 threads and mutexes to pthreads and related primitives. This is ideal for:

- **Unit testing** and CI pipelines on desktop/server environments  
- **Mocking** RTOS behavior during development  
- Rapid iteration before deploying to real hardware

> **Status:** Partial implementation  
> - Thread creation, deletion, management  
> - Mutex create/lock/unlock  
> - (Other RTOS2 API calls are not yet implemented)

Contributions via Issues or Pull Requests are very welcome—please follow the existing coding style (no strict format rules, just be consistent).

---

## ⚙️ Requirements

- A POSIX‑compliant OS (tested on Ubuntu Linux)  
- POSIX threads (pthread) support  
- GNU Make (or any POSIX `make`)  
- A C compiler of your choice (e.g., `gcc`, `clang`)

---

## 🛠️ Installation & Build

1. **Clone the repo**  
   ```bash
   git clone https://github.com/<your‑org>/CMSIS-POSIX.git
   cd CMSIS-POSIX
