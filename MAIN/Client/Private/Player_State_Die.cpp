#include "stdafx.h"

#include "Player.h"
#include "Player_State_Die.h"

#include "UI_Loading.h"

CPlayer_State_Die::CPlayer_State_Die(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Die::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_DIE);

    m_pGameInstance->Play_Once(TEXT("PLAYER_Footstep.wav"), CSound_Manager::PLAYER);

    m_eLevel = m_pPlayer->Get_Level();

    Set_RespawnPosition();

    m_pPlayer->Set_isImmune(true);
}

void CPlayer_State_Die::OnStateUpdate(_float fTimeDelta)
{
    CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));

    if (false == m_isAnimFinish && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_DIE))
    {
        m_isAnimFinish = true;       
        pUILoading->Set_Using(true, 6);
    }

    if(true == m_isAnimFinish)
    {
        if (true == pUILoading->Get_isFinish())
        {
            pUILoading->Set_Using(false);
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
            pTransform->Set_State(CTransform::STATE_POSITION, m_vRespawnPosition);
            m_pPlayer->Set_FullHP();
            m_pPlayer->Set_Respawan(true);
            m_pPlayer->Change_State(CPlayer::STATE_IDLE);
        }
    }
}

void CPlayer_State_Die::OnStateExit()
{
    m_pPlayer->Set_AnimationData_Initialize(CPlayer::ANIM_DIE);
    m_isAnimFinish = false;
}

void CPlayer_State_Die::Set_RespawnPosition()
{
    if (LEVEL_BEACH == m_eLevel)
        m_vRespawnPosition = _vector{ 65.f, 2.f, -62.f, 1.f };
    //else if(LEVEL_BOSS == m_eLevel)

}

CPlayer_State_Die* CPlayer_State_Die::Create(CPlayer* pPlayer)
{
    CPlayer_State_Die* pInstance = new CPlayer_State_Die(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Die::Free()
{
    __super::Free();
}