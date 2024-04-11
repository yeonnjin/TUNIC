#include "stdafx.h"

#include "Player.h"
#include "Player_State_Defense.h"

CPlayer_State_Defense::CPlayer_State_Defense(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Defense::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SHIELD);   
    m_pPlayer->Set_Parrying(true);
}

void CPlayer_State_Defense::OnStateUpdate(_float fTimeDelta)
{
    IF_PLAYER_ISMOVE
    {
        CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

        m_ePreDir = m_eCurDir;
        m_eCurDir = m_pPlayer->Get_Dir();
        _vector vLook = m_pPlayer->Get_Look();

        // 일반 상태일 때
        if ((CPlayer::LOCK_OFF == eLockOn || CPlayer::LOCK_END == eLockOn))
        {
            _vector vTargetLook{};
            vTargetLook = XMVector3Normalize(vLook);
            _vector PlayerVector = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_LOOK);

            if (false == XMVector3Equal(vTargetLook, XMVector3Normalize(PlayerVector)))
            {
                m_isTurn = true;
                m_isFirst = true;
            }

            if (true == m_isTurn)
            {
                /*if (true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(&m_isFirst, &m_vLerpLook, vTargetLook, fTimeDelta))
                {
                    m_isTurn = false;
                    m_isFirst = true;
                }*/

                if (true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(vTargetLook, fTimeDelta))
                {
                    m_isTurn = false;
                    m_isFirst = true;
                }
            }

            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Look(fTimeDelta, vTargetLook);
        }
        // 락온 상태일 때
        else
        {
            // 몬스터가 없을 경우 바라보고 있던 방향을 계속 바라보기 
            // 몬스터가 있을 경우 몬스터 계속 바라보기
            // 시선은 고정한 채 8방향 움직이기 

            if (CPlayer::LOCK_ON_FIND == eLockOn)
            {
                if (CPlayer::STATE_DODGE != m_pPlayer->Get_State())
                {
                    _vector vTargetPosition = m_pPlayer->Get_LockOn_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
                    ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Look_At_For_LandOject(vTargetPosition, true);
                }
            }

            switch (m_eCurDir)
            {
            case Client::CPlayer::DIR_FRONT:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Front(fTimeDelta);
                break;
            case Client::CPlayer::DIR_FL:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_FL(fTimeDelta);
                break;
            case Client::CPlayer::DIR_LEFT:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_L(fTimeDelta);
                break;
            case Client::CPlayer::DIR_BL:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_BL(fTimeDelta);
                break;
            case Client::CPlayer::DIR_BACK:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Back(fTimeDelta);
                break;
            case Client::CPlayer::DIR_BR:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_BR(fTimeDelta);
                break;
            case Client::CPlayer::DIR_RIGHT:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_R(fTimeDelta);
                break;
            case Client::CPlayer::DIR_FR:
                ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_FR(fTimeDelta);
                break;
            case Client::CPlayer::DIR_END:
                break;
            default:
                break;
            }
        }
    }
    
    if (true == m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_UP))
    {
        IF_PLAYER_ISMOVE
            m_pPlayer->Change_State(CPlayer::STATE_MOVE);
        else
            m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }

    



    //// 들고 있을 때 피격 시 -> ANIM_PARRY
    //if (false == m_isParrying && true == m_pPlayer->isParrying())
    //{      
    //    m_isParrying = true;
    //    m_pPlayer->Set_Parrying(false);
    //    m_pPlayer->Set_Blending(true, CPlayer::ANIM_PARRY);
    //}

    //if (true == m_isParrying && m_pPlayer->Get_isFinished(CPlayer::ANIM_PARRY))
    //{
    //    m_isParrying = false;
    //    m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    //}
}

void CPlayer_State_Defense::OnStateExit()
{
    //m_isParrying = false;
    m_pPlayer->Set_Parrying(false);

    m_isFirst = { true };
    m_isTurn = { false };
}

CPlayer_State_Defense* CPlayer_State_Defense::Create(CPlayer* pPlayer)
{
    CPlayer_State_Defense* pInstance = new CPlayer_State_Defense(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Defense"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Defense::Free()
{
    __super::Free();
}

