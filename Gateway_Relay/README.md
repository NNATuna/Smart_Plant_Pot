# Gateway/Relay Firmware Skeleton (ESP-IDF v5.5 + ESP-MESH)

## 1) High-level architecture & data flow
```
[Mesh RX ISR/Callback] --> raw_queue --> [Decoder/Validator Task] --> decoded_queue
    --> [Registry update] --> storage_queue --> [Spool Append Task] --> LittleFS append-only log
                                                            \--> [Cloud Sync Task] --> HTTPS/REST
                                                               \--> [Downlink Bridge] --> Mesh TX
```
Layers: mesh_if ↔ pipeline ↔ storage ↔ cloud ↔ bridge. Uplink (Wi‑Fi STA + time sync) only when runtime role allows.

## 2) Role/state machine
- **ROLE_GATEWAY_ROOT**: device is mesh root with IP; enables Wi‑Fi STA, time sync, cloud sync, config pull.
- **ROLE_RELAY**: no uplink; participates in mesh, keeps registry and spooling to survive future promotion.
- **ROLE_OFFLINE**: fallback when mesh not formed or no parents; keeps queues/spool but pauses network activity.
- Transitions driven by mesh events `ROOT_GOT_IP` / `ROOT_LOST_IP` and `CONFIG_CAN_UPLINK` Kconfig.

## 3) Message envelope (internal normalized record)
- Fields: `node_id[8]`, `seq`, `ts_ms`, `schema_version`, `payload_type`, `payload_len`, `payload[]`, optional `rssi`, `layer`, `crc16` placeholder.
- Payload default JSON (via cJSON); CBOR/MsgPack can be plugged in later without touching RX callback.

## 4) ESP-IDF v5.5 APIs per module
- **mesh_if**: `esp_mesh`, `esp_wifi`, `esp_netif`, `esp_event`, FreeRTOS tasks/queues.
- **pipeline**: FreeRTOS queues/tasks, `esp_log`, `cJSON` for deferred parse, registry & storage APIs.
- **storage**: `esp_littlefs`, `esp_vfs`, `nvs_flash` for durable spool offsets.
- **uplink**: `esp_wifi` STA, `esp_netif`, `esp_event`, `esp_sntp`, `esp_timer`.
- **cloud**: `esp_http_client`, `esp_tls`, backoff helper, interacts with storage.
- **bridge**: `mesh_if_send_to_node` wrapper.
- **utils**: logging helpers, exponential backoff, metrics counters, watchdog hooks placeholder.

## 5) Folder tree
```
Gateway_Relay/
├── CMakeLists.txt
├── Kconfig.projbuild
├── README.md
├── main/
│   ├── app_main.c
│   ├── app_config.c
│   ├── app_config.h
│   ├── runtime_role.c
│   └── runtime_role.h
└── components/
    ├── bridge/
    │   ├── CMakeLists.txt
    │   ├── bridge.c
    │   └── include/bridge.h
    ├── cloud/
    │   ├── CMakeLists.txt
    │   ├── cloud.c
    │   └── include/cloud.h
    ├── mesh_if/
    │   ├── CMakeLists.txt
    │   ├── mesh_if.c
    │   └── include/mesh_if.h
    ├── pipeline/
    │   ├── CMakeLists.txt
    │   ├── pipeline.c
    │   └── include/pipeline.h
    ├── registry/
    │   ├── CMakeLists.txt
    │   ├── registry.c
    │   └── include/registry.h
    ├── storage/
    │   ├── CMakeLists.txt
    │   ├── storage_spool.c
    │   └── include/storage_spool.h
    ├── uplink/
    │   ├── CMakeLists.txt
    │   ├── uplink.c
    │   └── include/uplink.h
    └── utils/
        ├── CMakeLists.txt
        ├── include/utils_common.h
        ├── include/utils_metrics.h
        ├── utils_common.c
        └── utils_metrics.c
```

## 6) Task/queue design & sizing
- `raw_queue` (`CONFIG_RAW_QUEUE_LEN`, default 64): receives raw mesh frames; enqueue-only from RX callback to avoid heavy work.
- `decoded_queue` (`CONFIG_DECODED_QUEUE_LEN`): holds validated envelopes before spooling.
- `storage_queue` placeholder for backpressure expansion.
- Drop policy: when queue full, drop oldest/raw frame and increment metrics.
- Suggested stack sizes: decoder 4 KB, spool 4 KB, mesh RX 4 KB, cloud sync 4 KB.

## 7) Spool format & crash safety
- Append-only binary log at `/spool/spool.log` on LittleFS partition `CONFIG_SPOOL_PARTITION_LABEL`.
- APIs: `spool_append`, `spool_read_batch`, `spool_commit`, `spool_rotate_if_needed`.
- `fflush()` after each append for power-loss tolerance; commit offset tracked in RAM (extend to NVS later).

## 8) Cloud sync/backoff (concept)
```
loop:
  batch = spool_read_batch(max_records, max_bytes)
  if batch.empty: sleep(window)
  else if send_ok: spool_commit(offset); attempt=0
  else: delay = exp_backoff(attempt++, base=500ms, max=10s, jitter=250ms)
```

## 9) Kconfig/menuconfig hints
- `CONFIG_CAN_UPLINK`: enable uplink features (gateway-capable firmware).
- Queue lengths: `CONFIG_RAW_QUEUE_LEN`, `CONFIG_DECODED_QUEUE_LEN`, `CONFIG_STORAGE_QUEUE_LEN`.
- Spool: `CONFIG_SPOOL_PARTITION_LABEL`, `CONFIG_SPOOL_MAX_BYTES`.
- Cloud: `CONFIG_CLOUD_BATCH_TIME_SEC`, `CONFIG_CLOUD_BATCH_MAX_RECORDS`.

## 10) Build/test checklist
- Install ESP-IDF v5.5 and set `IDF_PATH`.
- From `Gateway_Relay/`: `idf.py set-target esp32` (or esp32s3) then `idf.py build`.
- Flash via `idf.py -p <PORT> flash monitor`.
- Adjust Wi-Fi credentials in `app_config.c` or via NVS later.

