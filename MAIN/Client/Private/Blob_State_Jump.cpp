#include "stdafx.h"

#include "Player.h"
#include "Monster_Blob.h"
#include "Blob_State_Jump.h"

CBlob_State_Jump::CBlob_State_Jump(CMonster_Blob* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CBlob_State_Jump::OnStateEnter()
{
    m_pMonster->Set_isAggro(true);
    m_pMonster->Set_Blending(true, CMonster_Blob::ANIM_JUMP);
    m_pGameInstance->Play_Once(TEXT("MONSTER_Blob_Aggro.wav"), CSound_Manager::MONSTER1);
}

void CBlob_State_Jump::OnStateUpdate(_float fTimeDelta)
{
    m_fAccJumpTime += fTimeDelta;

    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    
    CTransform* pBlob = (CTransform*)(m_pMonster->Get_Component(g_strTransformTag));
    pBlob->Look_At_For_LandOject(vPlayerPosition, true);

    _vector vMonsterPosition = pBlob->Get_State_Vector(CTransform::STATE_POSITION);
    _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];

    _uint iFrame = m_pMonster->Get_Current_Frame(CMonster_Blob::ANIM_JUMP);
    if(14 == iFrame)
        m_pGameInstance->Play_Once(TEXT("MONSTER_Blob_Jump.wav"), CSound_Manager::MONSTER1);

    if (m_fAttackDistance >= fDistance)
    {
        if(m_fAccJumpTime > m_fJumpTime)
            m_pMonster->Change_State(CMonster_Blob::STATE_ATTACK);
    }
    else if (m_fIdleDistance <= fDistance)
    {
        if (m_fAccJumpTime > m_fJumpTime)
            m_pMonster->Change_State(CMonster_Blob::STATE_IDLE);
    }
    else
    {
        pBlob->Go_Backward(fTimeDelta);
    }
}

void CBlob_State_Jump::OnStateExit()
{
    m_fAccJumpTime = 0.f;
    m_pMonster->Set_isAggro(false);
}

CBlob_State_Jump* CBlob_State_Jump::Create(CMonster_Blob* pMonster, CPlayer* pPlayer)
{
    CBlob_State_Jump* pInstance = new CBlob_State_Jump(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBlob_State_Jump"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBlob_State_Jump::Free()
{
    __super::Free();
}
