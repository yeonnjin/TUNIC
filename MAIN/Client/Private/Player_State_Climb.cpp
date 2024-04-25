#include "stdafx.h"

#include "Player.h"
#include "Player_State_Climb.h"

CPlayer_State_Climb::CPlayer_State_Climb(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Climb::OnStateEnter()
{
    // 패턴
    // 0. 내려갈 때 : ANIM_CLIMB_ON / 올라갔을 때 :; 바로 ANIM_CLIMB / 다 올라갔을 때 : ANIM_CLIMB_OFF / 사다리 타기 : ANIM_CLIMB
    // 1. 내려가기 / 올라가기 애니메이션 종료 후 조작 안하면 멈춤, 조작 시 애니메이션 재생

    m_pPlayer->Set_Blending(true, CPlayer::ANIM_CLIMB_ON);
}

void CPlayer_State_Climb::OnStateUpdate(_float fTimeDelta)
{
    // 진입 애니메이션이 끝났을 때 체크
    if (true == m_pPlayer->Get_isFinished(CPlayer::ANIM_CLIMB_ON))
        m_isStart = false;

    // 진입 애니메이션이 끝났을 때 위 아래 조작 가능
    if(false == m_isStart)
    {
        if (true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_DOWN) ||
            true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_DOWN))
        {
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_CLIMB);
            m_pPlayer->Set_StopAnimation(false);
        }
        else if (true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_UP) ||
            true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_UP))
        {
            m_pPlayer->Set_StopAnimation(true);
        }

        CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
        _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
        _float fPlayerSpeed = m_pPlayer->Get_Speed();
        if (true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
        {
            vPlayerPosition.m128_f32[1] += fTimeDelta * fPlayerSpeed;
        }
        else if(true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
        {
            vPlayerPosition.m128_f32[1] -= fTimeDelta * fPlayerSpeed;
        }
        pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition);
    }
        
}

void CPlayer_State_Climb::OnStateExit()
{
    m_isStart = true;
    m_isClimb = false;

    m_pPlayer->Set_StopAnimation(false);
}

CPlayer_State_Climb* CPlayer_State_Climb::Create(CPlayer* pPlayer)
{
    CPlayer_State_Climb* pInstance = new CPlayer_State_Climb(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Climb"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Climb::Free()
{
    __super::Free();
}