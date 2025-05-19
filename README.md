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

Add to your compilation the sources from src folder and the headers from the inc folder

---

## ✅ Running Built‑In Tests

A small test suite is provided under `tests/config1/`. To run:

cd tests/config1
make run

---

## 🚀 Roadmap

* [ ] Complete CMSIS‑RTOS2 thread API
* [ ] Implement all synchronization primitives (semaphores, event flags, etc.)
* [ ] Add more platform support (macOS, other Linux distros)
* [ ] Improve test coverage

> This is a community-driven effort—if you need a specific feature, please open an Issue or submit a PR!

---

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/foo`)
3. Commit your changes (`git commit -am 'Add foo'`)
4. Push to the branch (`git push origin feature/foo`)
5. Open a Pull Request

Please try to match the existing formatting and naming conventions.

---

## ⭐️ Show Your Support

If you find CMSIS‑POSIX useful, please give this repo a ★ on GitHub — it helps me know you’re interested and motivates further development!

---

## 📜 License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.
