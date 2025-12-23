# ghd — GitHub Hook Daemon

## 中文版 (CH)

一个用 **C 语言** 编写的 **极简 GitHub Webhook 守护进程**。  
目标是提供一个 **低依赖、可审计、可扩展** 的 GitHub webhook 接收与处理核心。

该项目刻意避免使用高层框架或完整 HTTP/JSON 库，专注于：

- 最小化攻击面
- 清晰的模块边界
- 可逐步演进的架构
- 行为与 GitHub 官方 webhook 规范保持一致

---

## 特性 (Features)

- 原生 POSIX socket HTTP server
- GitHub `X-Hub-Signature-256` HMAC 校验（SHA-256）
- 模块化设计（HTTP / HMAC / JSON / GitHub 规则 / Exec）
- 使用轻量级 JSON 解析器（jsmn，已修复嵌套对象解析）
- 正确返回 GitHub 期望的 HTTP 状态码
- 支持 webhook push 事件自动执行 `git pull`
- 支持 PR / issue 日志生成
- 适合作为：
  - GitHub webhook daemon
  - mirror / automation trigger
  - CI / P2P glue layer

---

## 项目状态 (Project Status)

当前状态：**可用 / 已实现主要功能** ✅

- HTTP 接收：稳定
- HMAC 校验：正确（与 GitHub 行为一致）
- JSON 解析：已修复（支持嵌套对象，如 `repository.full_name`）
- GitHub 事件规则：基础支持（push + branch + PR + issue）
- 执行动作（exec git pull / PR / issue 日志）：已实现

> 注意：JSON 解析错误不会影响 HTTP 响应，  
> 服务始终返回 `200 OK`，这是 GitHub webhook 的**正确行为**  
>（用于防止 GitHub 重复重试）。

---

## 目录结构 (Structure)

```

include/
├── config.h        # 全局配置（端口、密钥、限制）
├── http.h          # HTTP 接口定义
├── hmac.h          # GitHub HMAC 校验接口
├── json.h          # webhook_event 抽象
├── github.h        # GitHub 事件规则与调度
├── exec.h          # 执行动作接口

src/
├── main.c          # 程序入口（只负责调度）
├── http.c          # HTTP 解析与响应
├── hmac.c          # HMAC SHA-256 实现
├── json.c          # JSON → webhook_event
├── github.c        # GitHub 事件决策
├── exec.c          # 执行 git pull / PR / issue 日志
├── jsmn.c          # 轻量 JSON parser（已修复）

build/              # 编译中间产物（.o）
out/                # 最终可执行文件
scripts/            # 示例 webhook 测试脚本

````

---

## 构建依赖 (Build Requirements)

### Debian / Ubuntu

```bash
sudo apt install gcc make libssl-dev
````

### Arch Linux

```bash
sudo pacman -S gcc make openssl
```

---

## 编译 (Build)

```bash
make
```

生成的可执行文件位于：

```bash
./out/ghd
```

---

## 运行 (Run)

```bash
./out/ghd
```

默认监听端口：`9000`

---

## 测试脚本 (Test Scripts)

示例脚本位于项目根目录，可直接运行以测试 webhook 功能：

```bash
./scripts/test_push.sh
./scripts/test_pr.sh
./scripts/test_issue.sh
```

> 脚本会模拟 GitHub webhook 请求，触发对应仓库目录的 git pull 或 PR/issue 日志处理。

---

## 设计原则 (Design Principles)

* 一个文件 = 一个职责
* `main.c` 只做调度，不做业务逻辑
* 配置集中在 `config.h`
* 不依赖高层框架
* 所有组件可替换（HTTP / JSON / exec）

---

## 路线图 (Roadmap)

* [x] 修正 JSON 解析逻辑（支持嵌套对象）
* [x] 添加 `exec.c`（安全执行 git pull / PR / issue 日志）
* [ ] fork + execve + timeout
* [ ] drop privileges
* [ ] daemon / service mode
* [ ] P2P / mirror backend

---

## Checklist

* [x] HTTP 接收与解析
* [x] HMAC 校验（SHA-256）
* [x] JSON 解析与嵌套对象支持
* [x] GitHub push 事件支持 + 自动 git pull
* [x] PR / issue 日志生成
* [ ] fork + timeout

---

# ghd — GitHub Hook Daemon

## English Version (EN)

A **minimal GitHub webhook daemon written in C**.
The goal is to provide a **low-dependency, auditable, extensible** core for receiving and handling GitHub webhooks.

This project intentionally avoids high-level frameworks and full HTTP/JSON stacks, focusing on:

* Minimal attack surface
* Clear module boundaries
* Explicit, low-level control
* GitHub-compatible behavior

---

## Features

* Native POSIX socket HTTP server
* GitHub `X-Hub-Signature-256` HMAC validation (SHA-256)
* Modular architecture (HTTP / HMAC / JSON / GitHub rules / Exec)
* Lightweight JSON parsing (jsmn, with nested object support)
* Correct GitHub-compatible HTTP responses
* Supports push events and automatic `git pull`
* Supports PR / issue logging
* Suitable as:

  * GitHub webhook daemon
  * mirror / automation trigger
  * CI / P2P glue layer

---

## Project Status

Current state: **usable / main features implemented** ✅

* HTTP handling: stable
* HMAC validation: correct
* JSON parsing: fixed (supports nested objects like `repository.full_name`)
* GitHub rules: basic (push + branch + PR + issue)
* Exec hooks (git pull / PR / issue logging): implemented

JSON parsing errors do not affect the HTTP response;
the daemon always returns `200 OK`, which is the
**correct GitHub webhook behavior** to prevent retries.

---

## Structure

```
include/
├── config.h        # Global config (port, secrets, limits)
├── http.h          # HTTP interface definitions
├── hmac.h          # GitHub HMAC validation interface
├── json.h          # webhook_event abstraction
├── github.h        # GitHub rules and dispatch
├── exec.h          # Execution hooks

src/
├── main.c          # Entry point (only dispatches)
├── http.c          # HTTP parsing and response
├── hmac.c          # HMAC SHA-256 implementation
├── json.c          # JSON → webhook_event
├── github.c        # GitHub event decisions
├── exec.c          # Execute git pull / PR / issue logs
├── jsmn.c          # Lightweight JSON parser (fixed)

build/              # Object files (.o)
out/                # Executable
scripts/            # Example webhook test scripts
```

---

## Build Requirements

### Debian / Ubuntu

```bash
sudo apt install gcc make libssl-dev
```

### Arch Linux

```bash
sudo pacman -S gcc make openssl
```

---

## Build

```bash
make
```

Executable is generated at:

```bash
./out/ghd
```

---

## Run

```bash
./out/ghd
```

Default listening port: `9000`

---

## Test Scripts

Example scripts are provided in the project root to test webhook functionality:

```bash
./scripts/test_push.sh
./scripts/test_pr.sh
./scripts/test_issue.sh
```

> Scripts simulate GitHub webhook requests, triggering git pull or PR/issue logging in the configured repository directories.

---

## Design Principles

* One file = one responsibility
* `main.c` only dispatches, no business logic
* Config centralized in `config.h`
* No high-level frameworks
* All components replaceable (HTTP / JSON / exec)

---

## Roadmap

* [x] Fix JSON parsing (support nested objects)
* [x] Add `exec.c` (safe git pull / PR / issue logging)
* [ ] fork + execve + timeout
* [ ] drop privileges
* [ ] daemon / service mode
* [ ] P2P / mirror backend

---

## Checklist

* [x] HTTP handling
* [x] HMAC validation (SHA-256)
* [x] JSON parsing (nested objects supported)
* [x] GitHub push events + auto git pull
* [x] PR / issue logging
* [ ] fork + timeout