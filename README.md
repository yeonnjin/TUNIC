# TUNIC – DirectX11 개인 프로젝트

DirectX11 기반 자체 엔진으로 개발한 3D 액션 어드벤처 게임입니다.
게임 프레임워크 설계부터 렌더링, 캐릭터 로직, 콘텐츠 구현까지 전반을 직접 담당했으며,
엔진 구조 설계, 맵 툴 제작, 멀티스레드 리소스 로딩 등을 구현했습니다.

---

## 📌 프로젝트 개요

자체 엔진 구조를 설계하고 렌더링부터 게임 로직까지 전체 파이프라인 이해하는 것을 목표로 작업했습니다.
다양한 콘텐츠 구현을 통한 게임 시스템 전반적인 경험을 쌓았고 맵 툴 제작 및 멀티스레드 리소스 로딩 적용을 했습니다.

- 프로젝트 형태: 개인 프로젝트
- 엔진: DirectX11
- 언어: C++14 / HLSL
- 개발 도구 : Visual Studio 2022, Git, ImGui

---

## 🙋 직접 구현한 주요 시스템

### 1️⃣ CollisionManager

- Bounding Box 기반 충돌 검사 구현
- 충돌 발생 시 최상위 Object 클래스의 `collision_event()` 호출
- 오브젝트 타입별로 서로 다른 충돌 이벤트 처리

---

### 2️⃣ CameraManager

- 카메라 태그(wstring) 기반 다중 카메라 관리
- 카메라 전환 시 이전 카메라 행렬을 다음 카메라에 전달하여 자연스러운 시점 이동 구현
  
---

### 3️⃣ FSM 기반 캐릭터 상태 관리

- 플레이어, 몬스터, NPC를 FSM 구조로 관리
- 플레이어는 이동 / 전투 / 퍼즐 / 상호작용 등 약 14개의 상태 구현
- 보스는 7가지 공격 패턴을 FSM 기반으로 구성
- NPC는 상황별 상태 전환 구조 적용
  
---

### 4️⃣ 상호작용 및 콘텐츠

- 상자 열기
- 몬스터 처치 시 아이템 드랍
- 인벤토리
- 사다리 / 텔레포트 / 망원경
- 퍼즐 맵

---

### 5️⃣ UI 시스템

- Player HUD
- 로딩 UI
- 전투 UI
- 몬스터 / 보스 체력 UI
- 상호작용 안내 UI

---

## 🧠 기술적으로 중점 구현한 부분

### 1️⃣ 애니메이션 시스템

- ASSIMP 라이브러리를 사용하여 모델 및 애니메이션 데이터 로드
- 애니메이션 보간 및 블렌딩 구현
- 루트 모션 처리
- 특정 뼈(Bone)에 무기를 부착하여 캐릭터 애니메이션에 맞춰 자연스럽게 무기 움직임 연동

---

### 2️⃣ 네비게이션 및 지형 태우기 시스템

- 3D 지형을 삼각형 메쉬로 분할
- 각 삼각형의 정점 및 인덱스 데이터 저장
- 플레이어 이동 시 현재 위치한 삼각형을 실시간 추적
- 해당 삼각형 정보를 기반으로 높이 보정 적용

또한 맵 툴을 직접 제작하여:

- 지형 위에 삼각형 데이터를 생성
- 해당 데이터를 파싱하여 네비게이션 정보로 사용

---

## ⭐ 주요 구현 코드

▶ Engine Core

 [CollisionManager]
- Bounding Box 충돌 검사 구현 
- https://github.com/yeonnjin/TUNIC/blob/master/MAIN/Engine/Private/Collision_Manager.cpp
  
 [CameraManager]
- 카메라 태그 기반 다중 카메라 처리
- https://github.com/yeonnjin/TUNIC/blob/master/MAIN/Engine/Private/Camera_Manager.cpp
  
 [StateMachine]
- https://github.com/yeonnjin/TUNIC/blob/master/MAIN/Engine/Private/State_Machine.cpp


▶ Animation

 [AnimatorController]
- 애니메이션 보간 및 루트 모션 처리 
- https://github.com/yeonnjin/TUNIC/blob/master/MAIN/Engine/Private/Animator_Controller.cpp


▶ Navigation + Tool

 [Navigation Tool]
- 맵 상에 점을 찍어 삼각형 메쉬 정보를 만든 후 데이터 저장
- https://github.com/yeonnjin/TUNIC/blob/master/TOOL/Engine/Private/Navigation.cpp


 [Navigation Runtime]
- 삼각형 메쉬 기반 이동/지형 처리
- https://github.com/yeonnjin/TUNIC/blob/master/MAIN/Engine/Private/Navigation.cpp

---

## 💡 이 프로젝트를 통해 얻은 경험

- 렌더링부터 게임 로직까지 전체 파이프라인 직접 설계
- FSM 기반 캐릭터 구조 설계 경험
- 애니메이션 파이프라인 이해
- 네비게이션 및 지형 처리 구현 경험
- 다양한 콘텐츠를 구조적으로 통합하는 경험
- 단일 프로젝트에서 엔진 / 콘텐츠 / 툴을 모두 직접 다뤄본 경험

