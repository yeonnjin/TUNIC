#include "stdafx.h"

#include "Player.h"
#include "Player_State_Move.h"

CPlayer_State_Move::CPlayer_State_Move(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Move::OnStateEnter()
{
    // TODO:연달아서 누르면 블렌딩 안되는 듯

    m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);

    //m_eCurDir = m_ePreDir = m_pPlayer->Get_Dir();
    //switch (m_eCurDir)
    //{
    //case CPlayer::DIR_FRONT:
    //    m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
    //    break;
    //case CPlayer::DIR_BACK:
    //    m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_BACKWARD);
    //    break;
    //case CPlayer::DIR_LEFT:
    //    //m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
    //    m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_LEFT);
    //    break;
    //case CPlayer::DIR_RIGHT:
    //    //m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
    //    m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_RIGHT);
    //    break;
    //case CPlayer::DIR_END:
    //    break;
    //default:
    //    break;
    //}
}

void CPlayer_State_Move::OnStateUpdate(_float fTimeDelta)
{
   // _vector vLook = {};
    m_ePreDir = m_eCurDir;
    m_eCurDir = m_pPlayer->Get_Dir();

    switch (m_eCurDir)
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
    }

    

    _vector vTargetLook{};
    vTargetLook = XMVector3Normalize(m_vLook);
    _vector PlayerVector = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_LOOK);

    if (false == XMVector3Equal(vTargetLook, XMVector3Normalize(PlayerVector)))
    {
        m_isTurn = true;
        /*vTargetLook = XMVector3Normalize(vLook);

        m_iAngle = m_eCurDir - m_ePreDir;

        if (m_ePreDir == CPlayer::DIR_FR)
            m_iAngle = abs(m_eCurDir);*/
    }
    else
    {
        int a = 0;
    }

    //vLook = XMVector3Normalize(vLook);
    //_float3 vLerpLook{};

    if (true == m_isTurn)
    {
        if (true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(&m_vLerpLook, vTargetLook, fTimeDelta))
            m_isTurn = false;
    }

    ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Look(fTimeDelta, XMLoadFloat3(&m_vLerpLook));

    /*if (false == m_isTurn)
    {
        if(true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Angle(_vector{ 0.f, 1.f, 0.f }, 90, fTimeDelta))
            m_isTurn = true;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_M, KEY_DOWN))
        m_isTurn = false;*/

    /*if (m_ePreDir != m_eCurDir)
    {
        
    }*/

    /*if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
    {
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
    {
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
    {
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn(_vector{ 0.f, 1.f, 0.f }, -1 * fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
    {
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn(_vector{ 0.f, 1.f, 0.f }, fTimeDelta);
    }*/


}

void CPlayer_State_Move::OnStateExit()
{
}

CPlayer_State_Move* CPlayer_State_Move::Create(CPlayer* pPlayer)
{
    CPlayer_State_Move* pInstance = new CPlayer_State_Move(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Move"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Move::Free()
{
    __super::Free();
}