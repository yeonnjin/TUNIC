#include "stdafx.h"

#include "Player.h"
#include "Player_State_Top.h"

#include "UI_Arrow.h"
#include "Easing.h"

#include "Camera_Top.h"

CPlayer_State_Top::CPlayer_State_Top(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
    Set_Axis();
    m_pEasing = CEasing::Get_Instance();

#ifndef _DEBUG
    Safe_AddRef(m_pEasing);
#endif // _DEBUG  
}

void CPlayer_State_Top::OnStateEnter()
{
    // 패턴
    // 0. 꼭대기 도착 시 해당 상태 진입
    // 1. 카메라 다르게 세팅 : 돌아가는 게 다름 (4모서리 이동)
    // 2. a, d : 카메라 돌리기 / <-, -> : 정답 입력
    // 3. 정답 : 왼, 위 / 오, 위 / 오, 아 / 오, 위 / 오, 위 / 왼, 왼 / 왼 , 위 / 오, 위

    m_pGameInstance->Change_Camera(TEXT("Camera_Top"));

    m_pUIArrow = dynamic_cast<CUI_Arrow*>(m_pGameInstance->Get_GameObject(LEVEL_PUZZLE, TEXT("Layer_UI_Arrow")));
    Safe_AddRef(m_pUIArrow);
}

void CPlayer_State_Top::OnStateUpdate(_float fTimeDelta)
{
    if(false == m_isIdle)
    {
        if (true == dynamic_cast<CCamera_Top*>(m_pGameInstance->Get_Camera(TEXT("Camera_Top")))->Get_isIdle())
            m_isIdle = true;
    }
    else
    {
        Player_Move(fTimeDelta);

        CUI_Arrow::ANSWER eAnswer = m_pUIArrow->Get_Answer();
        if (false == m_isSuccess && (CUI_Arrow::ANSWER_OK == eAnswer || CUI_Arrow::ANSWER_END == eAnswer))
        {
            Input_Arrow();
        }
        else if(true == m_isSuccess)
        {
            if (CUI_Arrow::ANSWER_END == eAnswer)
            {
                Success(fTimeDelta);
            }
        }

        m_pUIArrow->Tick(fTimeDelta);
        m_pUIArrow->Late_Tick(fTimeDelta);
    }
}

void CPlayer_State_Top::OnStateExit()
{
    m_isPreMove = m_isMove = false;

    m_isIdle = false;
    m_isSuccess = false;
    m_isFly = false;

    m_fAccEnterTime = 0.f;
    m_fAccFlyTime = 0.f;
    m_fAccChangeTime = 0.f;
}

void CPlayer_State_Top::Player_Move(_float fTimeDelta)
{
    m_isPreMove = m_isMove;

    // 애니메이션 제어
    IF_PLAYER_ISMOVE
        m_isMove = true;
    else
        m_isMove = false;

        if (m_isPreMove != m_isMove)
        {
            if (true == m_isMove)
                m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
            else
                m_pPlayer->Set_Blending(true, CPlayer::ANIM_IDLE);
        }

        // 움직임 제어
        CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
        _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
        _vector vCamPosition = m_pGameInstance->Get_CamPosition_Vector();

        // 플레이어와 맵 중점의 각도에 따라서 vOrigin이 바뀜
        _vector vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1], -58.f, 1.f };
        _vector vDiff = vPlayerPosition - vOrigin;
        _vector vDir = XMVector3Normalize(vDiff);

        _vector vCross;
        for (size_t i = 0; i < 4; i++)
        {
            _vector vDot = XMVector3Dot(m_vAxis[i], vDir);

            if (cosf(XMConvertToRadians(45.f)) <= vDot.m128_f32[0] && cosf(XMConvertToRadians(-45.f)) < vDot.m128_f32[0])
            {
                //vCross = { -1.f, 0.f, 0.f, 0.f };
                vCross = { -1.f * m_vAxis[i].m128_f32[2], 0.f, m_vAxis[i].m128_f32[0], 0.f };
                vCross = XMVector3Normalize(vCross);
                break;
            }
        }

        if (true == m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
            pPlayerTransform->Puzzle_FL(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
            pPlayerTransform->Puzzle_FR(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
            pPlayerTransform->Puzzle_BL(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
            pPlayerTransform->Puzzle_BR(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
            pPlayerTransform->Puzzle_Straight(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
            pPlayerTransform->Puzzle_Backward(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
            pPlayerTransform->Puzzle_Right(fTimeDelta, vCross);
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
            pPlayerTransform->Puzzle_Left(fTimeDelta, vCross);
}

void CPlayer_State_Top::Input_Arrow()
{
    if (true == m_pGameInstance->Get_DIKeyState(DIK_LEFT, KEY_DOWN))
    {
        m_pUIArrow->Input_Arrow(CUI_Arrow::ARROW_LEFT);
    }
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_RIGHT, KEY_DOWN))
    {
        m_pUIArrow->Input_Arrow(CUI_Arrow::ARROW_RIGHT);
    }
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_UP, KEY_DOWN))
    {
        m_pUIArrow->Input_Arrow(CUI_Arrow::ARROW_UP);
    }
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_DOWN, KEY_DOWN))
    {
        m_pUIArrow->Input_Arrow(CUI_Arrow::ARROW_DOWN);
    }

    // 성공했을 때
    if (CUI_Arrow::ANSWER_SUCCESS == m_pUIArrow->Get_Answer())
    {
        m_isSuccess = true;
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
        m_vOriginPosition = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        m_vFlyPosition = m_vOriginPosition;
        m_vFlyPosition.m128_f32[1] += 5.f;
        m_vEndPosition = m_vFlyPosition;
        m_vEndPosition.m128_f32[1] += 10.f;
    }
}

void CPlayer_State_Top::Success(_float fTimeDelta)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));

    if(false == m_isFly)
    {
        m_fAccFlyTime += fTimeDelta;
        _float fRatio = m_fAccFlyTime / m_fFlyTime;
        if (fRatio >= 1.f)
        {
            fRatio = 1.f;
            m_fAccFlyTime = 0.f;
            m_isFly = true;
            //m_pPlayer->Set_Blending(true, CPlayer::ANIM_FALLING);
        }

        // 위치
        _vector vPosition;
        vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_vOriginPosition.m128_f32[0], m_vFlyPosition.m128_f32[0], fRatio);
        vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_vOriginPosition.m128_f32[1], m_vFlyPosition.m128_f32[1], fRatio);
        vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_vOriginPosition.m128_f32[2], m_vFlyPosition.m128_f32[2], fRatio);
        vPosition.m128_f32[3] = 1.f;

        pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPosition);
    }
    else
    {
        m_fAccChangeTime += fTimeDelta;
        _float fRatio = m_fAccChangeTime / m_fChangeTime;
        if (fRatio >= 1.f)
        {
            fRatio = 1.f;
            m_fAccChangeTime = 0.f;
            pPlayer->Change_State(CPlayer::STATE_IDLE);
        }

        _vector vPosition;
        vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vFlyPosition.m128_f32[0], m_vEndPosition.m128_f32[0], fRatio);
        vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vFlyPosition.m128_f32[1], m_vEndPosition.m128_f32[1], fRatio);
        vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vFlyPosition.m128_f32[2], m_vEndPosition.m128_f32[2], fRatio);
        vPosition.m128_f32[3] = 1.f;

        pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPosition);
    }
}

void CPlayer_State_Top::Set_Axis()
{
    _vector vAxisZPlus = { 0.f, 0.f, 1.f, 0.f };
    _vector vAxisXPlus = { 1.f, 0.f, 0.f, 0.f };
    _vector vAxisZMinus = { 0.f, 0.f, -1.f, 0.f };
    _vector vAxisXMinus = { -1.f, 0.f, 0.f, 0.f };

    m_vAxis.push_back(vAxisZPlus);
    m_vAxis.push_back(vAxisXPlus);
    m_vAxis.push_back(vAxisZMinus);
    m_vAxis.push_back(vAxisXMinus);
}

CPlayer_State_Top* CPlayer_State_Top::Create(CPlayer* pPlayer)
{
    CPlayer_State_Top* pInstance = new CPlayer_State_Top(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Top"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Top::Free()
{
    __super::Free();

    Safe_Release(m_pUIArrow);
}