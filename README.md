# CMSIS‑POSIX

[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)

An implementation of the CMSIS‑RTOS2 API on a POSIX platform for testing and mocking embedded code.

🔗 [Project Website (GitHub Pages)](https://amgross.github.io/CMSIS-POSIX/)

📂 [Source Code on GitHub](https://github.com/amgross/CMSIS-POSIX)

---

## 📖 Overview

CMSIS‑POSIX allows you to build and test CMSIS‑RTOS2‑based applications on a POSIX system (e.g., Ubuntu Linux) by mapping RTOS2 threads and mutexes to pthreads and related primitives. This is ideal for:

- **Unit testing** and CI pipelines on desktop/server environments  
- **Mocking** RTOS behavior during development  
- Rapid iteration before deploying to real hardware

> **Status:** Partial implementation
> For full function implementation list please refer to implemented.txt
> - Thread basics
> - Mutex
> - Semaphore
> - memory pool
> - Message queues

Contributions via Issues or Pull Requests are very welcome—please follow the existing coding style (no strict format rules, just be consistent).

---

## ⚙️ Requirements

- A POSIX‑compliant OS (tested on Ubuntu Linux)  
- POSIX threads (pthread) support  
- GNU Make (or any POSIX `make`)  
- A C compiler of your choice (e.g., `gcc`, `clang`)

---

## 🛠️ Installation, configuration and Build

Add to your compilation the sources from src folder and the headers from the inc folder

The project supports user-defined overrides via a separate configuration file cmsisPosix_UserConfig.h in case CP_USE_USER_CONFIG is defined, see cmsisPosix_UserConfig_sample.h.

---

## ✅ Running Built‑In Tests

A small test suite is provided under `tests/config1/`. To run:

cd tests/config1
make run

---

## 🚀 Roadmap

* [ ] Implement all missinf APIs (see implemented.txt)
* [ ] Add debug flag that deeper check status (for example checking memory pool problems)
* [ ] Add optionally error prints.
* [ ] Add object IDs, for checking valid object while usage. 

> This is a community-driven effort—if you need a specific feature, please open an Issue or submit a PR!

---

## 🤝 Contributing

PRs are highly welcome.

Please try to match the existing formatting and naming conventions. No strict convention.

---

## ⭐️ Show Your Support

If you find CMSIS‑POSIX useful, please give this repo a ★ on GitHub — it helps me know you’re interested and motivates further development!

---

## 📜 License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.
