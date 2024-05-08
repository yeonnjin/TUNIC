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

    _bool isUpper = m_pPlayer->isUpper();

    // 올라가기 : 바로 등산 모션
    if (true == isUpper)
    {
        m_eAnimation = CPlayer::ANIM_CLIMB;
    }
    // 내려가기 : 내려가기 모션
    else
    {
        m_eAnimation = CPlayer::ANIM_CLIMB_ON;       
    }

    m_pPlayer->Set_Blending(true, m_eAnimation);

    m_isPuzzle = (LEVEL_PUZZLE == m_pPlayer->Get_Level()) ? true : false;

    Compute_Dir();
}

void CPlayer_State_Climb::OnStateUpdate(_float fTimeDelta)
{
    // 진입 애니메이션
    if(true == m_isStart)
    {
        // 내려갈 때 : 해당 애니메이션 종료 후 다음 애니메이션 재생
        if (CPlayer::ANIM_CLIMB_ON == m_eAnimation)
        {
            if (true == m_pPlayer->Get_isFinished(m_eAnimation))
            {
                m_isStart = false;
            }
        }
        // 올라갈 때 : 블렌딩 끝나면 멈춤 상태
        else if (CPlayer::ANIM_CLIMB == m_eAnimation)
        {
            m_fBlendAccTime += fTimeDelta;

            if (m_fBlendAccTime >= m_fBlendTime)
            {
                m_pPlayer->Set_StopAnimation(true);
                m_isStart = false;
            }
        }
    }
    // 진입 애니메이션이 끝났을 때 위 아래 조작 가능
    else
    {
        _bool isArrive = m_pPlayer->isArrive();
        if (true == isArrive)
            m_isStartEndClimb = true;

        // 도착했으면 현 상태 종료
        if (true == m_isStartEndClimb && false == m_isEndClimb)
        {
            m_pPlayer->Set_StopAnimation(false);

            // 올라가는 상태였을 때 : 애니메이션 재생 후 종료 && 마지막 사다리인지 체크
            if (false == m_isEndClimb && CPlayer::CLIMB_UPPER == m_pPlayer->Get_Climb())
            {
                m_pPlayer->Set_Blending(true, CPlayer::ANIM_CLIMB_OFF);
                m_isEndClimb = true;

                if (true == m_pPlayer->isEndLadder())
                    m_isEndLadder = true;
            }
            // 내려가는 상태였을 때 : 바로 상태 종료
            else
            {
                if (true == m_isPuzzle)
                    m_pPlayer->Change_State(CPlayer::STATE_PUZZLE);
                else
                    m_pPlayer->Change_State(CPlayer::STATE_IDLE);
            }                    
        }
        // 올라가는 상태였을 때 && 애니메이션이 종료되었을 때
        else if (true == m_isStartEndClimb && true == m_isEndClimb && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_CLIMB_OFF))
        {
            if (true == m_isPuzzle)
            {
                if (true == m_isEndLadder)
                    m_pPlayer->Change_State(CPlayer::STATE_TOP);
                else
                    m_pPlayer->Change_State(CPlayer::STATE_PUZZLE);
            }
            else
            {
                m_pPlayer->Change_State(CPlayer::STATE_IDLE);
            }
        }       
        // 도착 전 상태일 때
        else if(false == m_isStartEndClimb)
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
                m_pPlayer->Set_Climb(CPlayer::CLIMB_UPPER);
            }
            else if (true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
            {
                vPlayerPosition.m128_f32[1] -= fTimeDelta * fPlayerSpeed * 1.3f;
                m_pPlayer->Set_Climb(CPlayer::CLIMB_LOWER);
            }
            pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition);
        }   

        m_pPlayer->Set_Arrive(false);     
    } 
}

void CPlayer_State_Climb::OnStateExit()
{
    m_isStart = true;
    m_isStartEndClimb = false;
    m_isEndClimb = false;
    m_isEndLadder = false;

    m_fBlendAccTime = 0.f;

    m_eAnimation = CPlayer::ANIM_END;
    m_pPlayer->Set_StopAnimation(false);
    m_pPlayer->Set_Climb(CPlayer::CLIMB_END);
    m_pPlayer->Set_Arrive(false);
    m_pPlayer->Set_AnimationData_Initialize(CPlayer::ANIM_CLIMB);
}

void CPlayer_State_Climb::Compute_Dir()
{
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _vector vOrigin, vDir;

    if (true == m_isPuzzle) 
    {
        vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1], -58.f, 1.f };
        vDir = XMVector3Normalize(vPlayerPosition - vOrigin);
    }
    else
    {
        vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1], 9.f, 1.f };
        vDir = XMVector3Normalize(vPlayerPosition - vOrigin);
    }

    pPlayerTransform->Look_At_Dir(vDir);
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