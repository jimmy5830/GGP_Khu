# Homework 2 — DirectX 12 Lighting Implementation Report

**과목:** SWCON311 Game Graphics Programming  
**학번/이름:** JInyoung_ko  
**제출일:** 2026-05-17

---

## 목차

1. [과제 개요](#1-과제-개요)
2. [Task 4 — Point Light & Spot Light 구현](#2-task-4--point-light--spot-light-구현)
3. [Hemisphere Light 추가 구현](#3-hemisphere-light-추가-구현)
4. [LIGHT_OPTION별 렌더링 비교](#4-light_option별-렌더링-비교)
5. [파일별 수정 요약](#5-파일별-수정-요약)

---

## 1. 과제 개요

이 과제는 DirectX 12 기반 렌더링 파이프라인에 **Blinn-Phong 조명 모델**을 적용하는 것을 목표로 한다.  
`LightingUtil.hlsl`에 조명 함수를 구현하고, `config.h`의 `LIGHT_OPTION` 매크로만 변경하여  
다양한 조명 조합을 지원하는 구조를 설계하였다.

추가로, 기존 세 가지 광원 유형(Directional / Point / Spot)에 더해  
**Hemisphere Light**를 새로운 광원 유형으로 설계 및 구현하였다.

---

## 2. Task 4 — Point Light & Spot Light 구현

### 2-1. Directional Light (기구현)

방향광은 위치 없이 방향만 가지며, 감쇠 없이 일정한 강도로 씬 전체를 조명한다.

```hlsl
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    float3 lightVec = -L.Direction;
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}
```

### 2-2. Point Light

점광원은 공간상의 한 지점에서 전방향으로 빛을 방출한다.  
광원과 표면 간의 거리 `d`에 따라 **선형 감쇠(Linear Attenuation)** 를 적용한다.

```hlsl
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Position - pos;
    float d = length(lightVec);
    lightVec /= d;                                          // 정규화

    float att  = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    float ndotl = max(dot(lightVec, normal), 0.0f);         // Lambert's cosine law
    float3 lightStrength = L.Strength * ndotl * att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}
```

**감쇠 함수:**

```
att = saturate( (FalloffEnd - d) / (FalloffEnd - FalloffStart) )
```

`FalloffStart`~`FalloffEnd` 구간에서 1→0으로 선형 감소하며, 범위를 벗어나면 0으로 clamp된다.

### 2-3. Spot Light

스포트라이트는 점광원에 **원뿔 인수(Spot Factor)** 를 추가하여 특정 방향으로만 빛을 집중시킨다.

```hlsl
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Position - pos;
    float d = length(lightVec);
    lightVec /= d;

    float att        = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    float ndotl      = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl * att * spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}
```

**Spot Factor:**

```
spotFactor = max( dot(-lightVec, L.Direction), 0 ) ^ SpotPower
```

`SpotPower`가 클수록 원뿔이 좁아지고 빛의 집중도가 높아진다.

### 2-4. Blinn-Phong 반사 모델

모든 광원은 공통적으로 `BlinnPhong` 함수를 통해 **확산(Diffuse) + 반사(Specular)** 를 계산한다.

```hlsl
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    float3 halfVec = normalize(toEye + lightVec);
    const float m = mat.Shininess * 256.0f;
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 specFactor = mat.specAlbedo * roughnessFactor;
    specFactor = specFactor / (specFactor + 1.0f);           // LDR tone-mapping

    return (mat.diffAlbedo + specFactor) * lightStrength;
}
```

Half-Vector `h = normalize(toEye + L)` 를 사용해 Phong 대비 연산량을 줄이면서  
유사한 품질의 하이라이트를 얻는다.

### 2-5. 다중 광원 지원 — config.h 구조

`config.h`의 `LIGHT_OPTION` 하나만 변경하면 광원 조합이 전환된다.

```
LIGHT_OPTION 0 → Directional only
LIGHT_OPTION 1 → Point only
LIGHT_OPTION 2 → Spot only
LIGHT_OPTION 3 → Dir + Point + Spot
LIGHT_OPTION 4 → Hemisphere only        ← 신규
LIGHT_OPTION 5 → Dir + Point + Spot + Hemisphere ← 신규
```

`NUM_DIR_LIGHTS`, `NUM_POINT_LIGHTS`, `NUM_SPOT_LIGHTS`, `NUM_HEMI_LIGHTS` 가  
전처리기 조건으로 자동 결정되어 `PassConstants`에 업로드된다.  
`ComputeLighting`은 이 카운트를 받아 광원 배열을 순회하므로 코드 변경 없이 조합이 가능하다.

---

## 3. Task 5 — Hemisphere Light 추가 구현

### 3-1. 개념

Hemisphere Light는 **하늘(Sky)** 과 **지면(Ground)** 두 방향에서 오는 환경광을 표면 법선 방향에 따라 보간하는 광원이다.  
기존 단일 색상 Ambient Light의 한계를 극복하여, 위를 향하는 면과 아래를 향하는 면에 서로 다른 색감을 부여한다.

```
                       Sky Color (Strength)
                            ↑  ↑  ↑
        ─────────────────────────────────
              surface (normal varies)
        ─────────────────────────────────
                            ↓  ↓  ↓
                     Ground Color (Position)
```

### 3-2. 수식

```
t         = 0.5 * (dot(normal, upAxis) + 1.0)   // [-1,1] → [0,1]
hemiColor = lerp(groundColor, skyColor, t)
result    = hemiColor * mat.diffAlbedo
```

- `t = 1.0` : 법선이 정확히 위를 향함 → 순수 Sky Color
- `t = 0.5` : 법선이 수평 → Sky/Ground 50:50 혼합
- `t = 0.0` : 법선이 정확히 아래를 향함 → 순수 Ground Color

### 3-3. 구현 — LightingUtil.hlsl

```hlsl
float3 ComputeHemisphereLight(Light L, Material mat, float3 normal)
{
    // Strength  = sky color
    // Position  = ground color (Position 필드 재활용)
    // Direction = world up axis
    float t = 0.5f * (dot(normal, L.Direction) + 1.0f);
    float3 hemiColor = lerp(L.Position, L.Strength, t);
    return hemiColor * mat.diffAlbedo;
}
```

> **Light 구조체 필드 재활용**  
> Hemisphere Light는 공간적 위치가 없으므로 `Position` 필드를 Ground Color 저장에 재사용한다.  
> 구조체 변경 없이 기존 `MaxLights` 배열을 그대로 활용할 수 있다.

### 3-4. ComputeLighting 시그니처 확장

```hlsl
// 변경 전
float3 ComputeLighting(Light gLights[MaxLights],
                       int numDirLights, int numPointLights, int numSpotLights,
                       Material mat, float3 pos, float3 normal, float3 toEye)

// 변경 후
float3 ComputeLighting(Light gLights[MaxLights],
                       int numDirLights, int numPointLights, int numSpotLights, int numHemiLights,
                       Material mat, float3 pos, float3 normal, float3 toEye)
```

디스패처 루프에 Hemisphere 구간이 추가된다:

```hlsl
// [0, numDir)          → ComputeDirectionalLight
// [numDir, +numPoint)  → ComputePointLight
// [+numPoint, +numSpot)→ ComputeSpotLight
// [+numSpot, +numHemi) → ComputeHemisphereLight  ← 추가
```

### 3-5. 상수 버퍼 레이아웃 변경

`cbPerPassPad3` 자리를 `NumHemiLights`로 대체하여 패딩 증가 없이 카운트를 전달한다.

| 오프셋 | C++ (PassConstants) | HLSL (cbPass) | 크기 |
|--------|--------------------|--------------------|------|
| 144 | `AmbientLight` | `gAmbientLight` | 12 B |
| 156 | `cbPerPassPad2` | `cbPerPassPad2` | 4 B |
| 160 | `NumDirLights` | `gNumDirLights` | 4 B |
| 164 | `NumPointLights` | `gNumPointLights` | 4 B |
| 168 | `NumSpotLights` | `gNumSpotLights` | 4 B |
| 172 | `NumHemiLights` | `gNumHemiLights` | 4 B |
| 176 | `Lights[MaxLights]` | `gLights[MaxLights]` | 768 B |

4개의 `int`가 정확히 16바이트를 채워 별도 패딩이 필요 없다.

---

## 4. LIGHT_OPTION별 렌더링 비교

### LIGHT_OPTION 3 — Directional + Point + Spot

| 특성 | 내용 |
|------|------|
| 광원 수 | Dir × 1, Point × 1, Spot × 1 |
| 방향광 | `{0.577, -0.577, 0.577}` 방향, 강도 `{0.6, 0.6, 0.6}` (회백색) |
| 점광원 | 위치 `(2, 3, 0)`, 강도 `{0.8, 0.8, 0.0}` (노란빛) |
| 스포트 | 위치 `(-2, 4, 0)`, 강도 `{0.2, 1.0, 4.0}` (청색 강조) |
| 특징 | 씬의 기본 음영 구조가 형성되며, 점광원의 노란 하이라이트와 스포트의 파란 원뿔이 뚜렷하게 나타남 |
| Hemisphere | 없음 → 그림자 영역(아래쪽 면)은 Ambient 단일 색상으로만 채워짐 |

### LIGHT_OPTION 4 — Hemisphere only

| 특성 | 내용 |
|------|------|
| 광원 수 | Hemi × 1 |
| Sky Color | `{0.0, 0.3, 0.5}` (차가운 파랑) |
| Ground Color | `{0.1, 0.08, 0.0}` (따뜻한 갈색) |
| Up Axis | `{0, 1, 0}` (월드 Y축) |
| 특징 | 직사광이나 하이라이트 없이 표면 법선 방향에 따라 Sky/Ground 색상이 부드럽게 그라디언트처럼 나타남 |
| 비교 포인트 | 기둥 상단(법선 위)은 파란 계열, 바닥 면(법선 아래)은 갈색 계열로 채색됨 |

### LIGHT_OPTION 5 — Directional + Point + Spot + Hemisphere

| 특성 | 내용 |
|------|------|
| 광원 수 | Dir × 1 + Point × 1 + Spot × 1 + Hemi × 1 |
| OPTION 3과의 차이 | Hemisphere가 추가되어 그림자 영역에도 Sky/Ground 색상 기반 환경광이 더해짐 |
| 효과 | 직사광이 닿지 않는 면(아래쪽 면, 틈)이 단순 Ambient 회색 대신 하늘빛/지면빛으로 채워져 보다 자연스러운 간접광 느낌을 줌 |
| 특징 | 씬 전체의 **Color Tone**이 통일되어 보임. 특히 하얀 Skull의 윗면(파랑 기조)과 아랫면(갈색 기조) 차이가 명확 |

### 핵심 차이 요약

```
OPTION 3:  그림자 영역 = Ambient (단일 회색)
OPTION 4:  모든 조명  = Hemisphere 만 (하늘↕지면 그라디언트)
OPTION 5:  그림자 영역 = Ambient + Hemisphere (색감 풍부)
                ↑ 이 차이가 Hemisphere Light의 핵심 기여
```

---

## 5. 파일별 수정 요약

### Shaders/LightingUtil.hlsl

| 변경 내용 | 설명 |
|-----------|------|
| `ComputePointLight` 구현 | 거리 계산, 감쇠, Lambert, BlinnPhong |
| `ComputeSpotLight` 구현 | Point 기반에 Spot Factor 추가 |
| `ComputeHemisphereLight` 추가 | Sky/Ground lerp 공식 적용 |
| `ComputeLighting` 시그니처 변경 | `numHemiLights` 파라미터 추가 및 루프 추가 |

### Shaders/color.hlsl

| 변경 내용 | 설명 |
|-----------|------|
| `Light gLights[1]` → `gLights[MaxLights]` | 다중 광원 수용 |
| `float toEyeW` → `float3 toEyeW` | 타입 오류 수정 |
| `cbPerPassPad3` → `gNumHemiLights` | 광원 카운트 전달 |
| PS: `ComputeDirectionalLight` → `ComputeLighting` | 전체 조명 디스패치로 교체 |

### FrameResource.h

| 변경 내용 | 설명 |
|-----------|------|
| `Light Lights[1]` → `Lights[MaxLights]` | 상수 버퍼 크기 확장 |
| `NumDirLights` / `NumPointLights` / `NumSpotLights` 추가 | 광원 카운트 필드 |
| `cbPerPassPad3` → `NumHemiLights` | 패딩 재활용으로 Hemi 카운트 추가 |

### LitColumnsApp.cpp

| 변경 내용 | 설명 |
|-----------|------|
| 하드코딩 광원 제거 | `Lights[0]` 직접 기입 코드 삭제 |
| `#if NUM_DIR_LIGHTS > 0` 블록 | config.h 배열로부터 방향광 업로드 |
| `#if NUM_POINT_LIGHTS > 0` 블록 | config.h 배열로부터 점광원 업로드 |
| `#if NUM_SPOT_LIGHTS > 0` 블록 | config.h 배열로부터 스포트 업로드 |
| `#if NUM_HEMI_LIGHTS > 0` 블록 | config.h 배열로부터 Hemisphere 업로드 |

### config.h

| 변경 내용 | 설명 |
|-----------|------|
| `LIGHT_OPTION` 4, 5 추가 | Hemisphere 단독 및 전체 조합 옵션 |
| `NUM_HEMI_LIGHTS` 매크로 | 전처리 조건으로 자동 결정 |
| Hemisphere 파라미터 배열 추가 | Sky/Ground/Direction 설정값 |
| `static_assert` 수정 | Hemi 카운트 포함하도록 갱신 |

---

*본 보고서는 Task 5의 구현 과정 및 결과를 정리한 것입니다.*
