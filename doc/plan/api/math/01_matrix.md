## Mátrix

- 4x4 mátrix; alap típusai a `XMFLOATnXm`, ezek vannak csomagolva egy 32 bitre igazított típusra
- Egy `Matrix` osztállyal, a csomagolt [XMMATRIX](https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xmmatrix(v=vs.85).aspx) típus bővítése műveletekkel
- gyakran használt mátrixok (`translate`, `rotation`, ...) felírása, és szorzása, vagy beállítása (`projection`, `ortho` wordmatrix/lookat

### Műveletek
- translate 
- rotation
	- vektor mentén
	- euler szögek mentén - ha kell 
	- RPY szögek mentén
- scale
- scale and translate 

### `XMVECTOR`
- 4 darab 32 bites integer csomagolva (128 bit)
- megvalósítása platformfüggő - transzparens réteg 
- mozgatás `XMFLOAT*` típusból `XMVECTOR` típusba [`XMLoad*()` könyvtári függvényekkel](https://msdn.microsoft.com/en-us/library/windows/desktop/ee415589(v=vs.85).aspx)
- mozgatás `XMVECTOR` típusból  `XMFLOAT*`típusba [`XMStore*()` könyvtári függvényekkel]([XMStore könyvtári függvények](https://msdn.microsoft.com/en-us/library/windows/desktop/ee415635(v=vs.85).aspx)

**Specializálva**

| Típus        | Load               | Store               |
| ------------ | ------------------ | ------------------- |
| `XMFLOAT2`   | `XMLoadFloat2()`   | `XMStoreFloat2()`   |
| `XMFLOAT3`   | `XMLoadFloat3()`   | `XMStoreFloat3()`   |
| `XMFLOAT4`   | `XMLoadFloat4()`   | `XMStoreFloat4()`   |
| `XMCOLOR`    | `XMLoadColor()`    | `XMStoreColor()`    |
| `XMFLOAT4X4` | `XMLoadFloat4x4()` | `XMStoreFloat4x4()` |

### Refernciák
- [XMMATRIX könyvtári függvények](https://msdn.microsoft.com/en-us/library/windows/desktop/ee415594(v=vs.85).aspx)
- [XMVECTOR](https://msdn.microsoft.com/en-us/library/windows/desktop/ee420742(v=vs.85).aspx) adattípus
	- [XMVECTOR operátorok](https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.types.xmvector_methods(v=vs.85).aspx)
	- [XMLload könyvtári függvények](https://msdn.microsoft.com/en-us/library/windows/desktop/ee415589(v=vs.85).aspx)
	- [XMStore könyvtári függvények](https://msdn.microsoft.com/en-us/library/windows/desktop/ee415635(v=vs.85).aspx)