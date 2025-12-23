# ghd — GitHub Hook Daemon

一个用 **C 语言** 编写的 **极简 GitHub Webhook 守护进程**。  
目标是提供一个 **低依赖、可审计、可扩展** 的 GitHub webhook 接收与处理核心。

该项目刻意避免使用高层框架或完整 HTTP/JSON 库，专注于：
- 最小化攻击面
- 清晰的模块边界
- 可逐步演进的架构

---

## 特性 (Features)

- 原生 POSIX socket HTTP server
- GitHub `X-Hub-Signature-256` HMAC 校验（SHA-256）
- 模块化设计（HTTP / HMAC / JSON / GitHub 规则）
- 使用 jsmn（或等价逻辑）进行最小 JSON 解析
- 正确返回 GitHub 期望的 HTTP 状态码
- 适合作为：
  - GitHub webhook daemon
  - mirror / automation trigger
  - P2P / CI glue layer

---

## 项目状态 (Project Status)

当前状态：**可用 / 进行中（WIP）**

- HTTP 接收：稳定
- HMAC 校验：正确（与 GitHub 行为一致）
- JSON 解析：部分字段解析中（不影响 webhook 接收）
- GitHub 事件规则：基础支持（push + branch）
- 执行动作（exec）：尚未实现

> 注意：即使 JSON 解析失败，服务仍然返回 `200 OK`，  
> 这是 GitHub webhook 的**正确行为**（防止重复重试）。

---

## 目录结构 (Structure)

```

include/
config.h        # 全局配置（端口、密钥等）
http.h          # HTTP 接口
hmac.h          # GitHub HMAC 校验
json.h          # webhook event 抽象
github.h        # GitHub 事件规则

src/
main.c          # 程序入口（只负责调度）
http.c          # HTTP 解析与响应
hmac.c          # HMAC SHA-256 实现
json.c          # JSON → webhook_event
github.c        # GitHub 事件决策
jsmn.c          # 轻量 JSON parser（第三方）

build/            # 编译产物（.o）
out/              # 最终可执行文件

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

## 测试 (Test with curl)

单行测试命令（包含真实 HMAC）：

```bash
curl -i -X POST http://localhost:9000/hook \
  -H "Content-Type: application/json" \
  -H "X-Hub-Signature-256: sha256=$(printf '%s' '{"action":"push","repository":{"full_name":"username/ghd"},"ref":"refs/heads/main"}' | openssl dgst -sha256 -hmac 'supersecret' | sed 's/^.* //')" \
  --data-binary '{"action":"push","repository":{"full_name":"username/ghd"},"ref":"refs/heads/main"}'
```

预期返回：

```
HTTP/1.1 200 OK
OK
```

---

## 设计原则 (Design Principles)

* 一个文件 = 一个职责
* `main.c` 只做调度，不做解析
* 配置集中在 `config.h`
* 不依赖框架
* 可替换组件（HTTP / JSON / exec）

---

## 路线图 (Roadmap)

* [ ] 修正 / 替换 JSON 解析逻辑
* [ ] 添加 `exec.c`（安全执行 hook）
* [ ] fork + execve + timeout
* [ ] drop privileges
* [ ] P2P / mirror backend
* [ ] daemon / service mode

---

# ghd — GitHub Hook Daemon

A **minimal GitHub webhook daemon written in C**.
The goal is to provide a **low-dependency, auditable, extensible** core for receiving and handling GitHub webhooks.

This project intentionally avoids high-level frameworks and full HTTP/JSON stacks, focusing on:

* Minimal attack surface
* Clear module boundaries
* Progressive, controlled evolution

---

## Features

* Native POSIX socket HTTP server
* GitHub `X-Hub-Signature-256` HMAC validation (SHA-256)
* Modular architecture (HTTP / HMAC / JSON / GitHub rules)
* Lightweight JSON parsing (jsmn or equivalent logic)
* Correct GitHub-compatible HTTP responses
* Suitable as:

  * GitHub webhook daemon
  * mirror / automation trigger
  * P2P / CI glue layer

---

## Project Status

Current state: **usable / work in progress**

* HTTP handling: stable
* HMAC validation: correct
* JSON parsing: partial (non-blocking)
* GitHub rules: basic (push + branch)
* Exec hooks: not implemented yet

Even on JSON parsing failure, the daemon returns `200 OK`,
which is the **correct GitHub webhook behavior**.

---

## Philosophy

This project prioritizes:

* correctness over convenience
* explicit code over abstraction
* control over automation

It is intentionally low-level.
