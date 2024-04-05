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
}

void CPlayer_State_Defense::OnStateUpdate(_float fTimeDelta)
{
    m_ePreDir = m_eCurDir;
    //m_eCurDir = m_pPlayer->Get_Dir();

    _vector vLook = m_pPlayer->Get_Look();

   /* switch (m_eCurDir)
    {
    case CPlayer::DIR_FRONT:
        m_vLook = { 0.f, 0.f, 1.f };
        break;
    case CPlayer::DIR_BACK:
        m_vLook = { 0.f, 0.f, -1.f };
        break;
    case CPlayer::DIR_LEFT:
        m_vLook = { 1.f, 0.f, 0.f };
        break;
    case CPlayer::DIR_RIGHT:
        m_vLook = { -1.f, 0.f, 0.f };
        break;
    case CPlayer::DIR_FL:
        m_vLook = { 1.f, 0.f, 1.f };
        break;
    case CPlayer::DIR_FR:
        m_vLook = { -1.f, 0.f, 1.f };
        break;
    case CPlayer::DIR_BL:
        m_vLook = { 1.f, 0.f, -1.f };
        break;
    case CPlayer::DIR_BR:
        m_vLook = { -1.f, 0.f, -1.f };
        break;
    case CPlayer::DIR_END:
        break;
    default:
        break;
    }*/

    _vector vTargetLook{};
    vTargetLook = XMVector3Normalize(vLook);
    _vector PlayerVector = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_LOOK);

    if (false == XMVector3Equal(vTargetLook, XMVector3Normalize(PlayerVector)))
    {
        m_isTurn = true;
    }

    if (true == m_isTurn)
    {
       /* if (true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(&m_vLerpLook, vTargetLook, fTimeDelta))
            m_isTurn = false;*/
    }

    /*if(CPlayer::DIR_END != m_pPlayer->Get_Dir())
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Look(fTimeDelta, XMLoadFloat3(&m_vLerpLook));*/


    // ==================================================================================
    if (true == m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_UP))
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);

    // 들고 있을 때 피격 시 -> ANIM_PARRY
    if (false == m_isParrying && true == m_pPlayer->isParrying())
    {      
        m_isParrying = true;
        m_pPlayer->Set_Parrying(false);
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_PARRY);
    }

    if (true == m_isParrying && m_pPlayer->Get_isFinished(CPlayer::ANIM_PARRY))
    {
        m_isParrying = false;
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Defense::OnStateExit()
{
    m_isParrying = false;
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

