#include "stdafx.h"
#include "Camera_LockOn.h"

#include "Easing.h"
#include "Player.h"

#include "Camera_Follow.h"

CCamera_LockOn::CCamera_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
    , m_pEasing{ CEasing::Get_Instance() }
{
    //Safe_AddRef(m_pEasing);
}

CCamera_LockOn::CCamera_LockOn(const CCamera_LockOn& rhs)
	: CCamera{ rhs }
    , m_pEasing{ rhs.m_pEasing }
{
    Safe_AddRef(m_pEasing);
}

HRESULT CCamera_LockOn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_LockOn::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));

    m_pPlayerTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag, 0));

    m_vDistance = CAM_DISTANCE;

	return S_OK;
}

HRESULT CCamera_LockOn::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

    if (true == m_isFind && (nullptr == m_pTargetTransform || true == m_pTargetTransform->Get_Dead()))
    {
        m_isExit = true;
        Safe_Release(m_pTargetTransform);
        m_pTargetTransform = nullptr;       
    }

    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

    if (false == m_isExit)
    {
        // 타겟 몬스터가 있으면 몬스터와 플레이어 중간 보기 : X, Z && 몬스터 거리에 따라 시야각 조절
        if (nullptr != m_pTargetTransform)
        {
            m_fMidTime += fTimeDelta * 0.1f;
            _float fRatio = m_fMidTime / 0.2f;
            if (1.f <= fRatio)
            {
                fRatio = 1.f;
            }

            _vector vMonsterPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);

            m_vMidPosition = { (vPlayerPosition.m128_f32[0] + vMonsterPosition.m128_f32[0]) * 0.5f,
                                0.f,
                               (vPlayerPosition.m128_f32[2] + vMonsterPosition.m128_f32[2]) * 0.5f + m_vDistance.m128_f32[2], 1.f };

            vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[0], m_vMidPosition.m128_f32[0], fRatio);
            vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[2], m_vMidPosition.m128_f32[2], fRatio);

            m_vLookAt.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vPlayerPosition.m128_f32[2], (vPlayerPosition.m128_f32[2] + vMonsterPosition.m128_f32[2]) * 0.5f, fRatio);

            // 시야각 조절
            // 40 ~ 60 ~ 80
            _float fRadian = 40.f;

            // 최소 거리 1일 때 -> 40.f ( + 0.f)
            // 최대 거리 20일 때 -> 80.f ( + 40.f)
            _float fLength = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];
            if (1.f > fLength)
                fLength = 1.f;
            else if (20.f < fLength)
            {
                fLength = 20.f;
                m_isExit = true;
                m_pPlayer->Set_LockOn(CPlayer::LOCK_OFF);
            }

            fRadian += fLength * 2.f;
            m_fFovy = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_fFovy, XMConvertToRadians(fRadian), fRatio);
            m_fRadian = XMConvertToRadians(fRadian);
        }
        else
        {
            _vector vTargetPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) + m_vDistance;

            if (true == m_pPlayer->isMove())
            {
                m_fMoveTime += fTimeDelta;
                vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fMoveTime, 0.5f));
                vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fMoveTime, 0.5f));
                vCamPosition.m128_f32[3] = 1.f;

                m_fIdleTime = 0.f;
            }
            else
            {
                m_fIdleTime += fTimeDelta * 0.01f;
                vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fIdleTime, 1.f));
                vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fIdleTime, 1.f));
                vCamPosition.m128_f32[3] = 1.f;

                m_fMoveTime = 0.f;
            }

            m_vLookAt.m128_f32[2] = vPlayerPosition.m128_f32[2];
        }

        // 각도 높이기 : Y
        if (false == m_isHigh)
        {
            m_fHighTime += fTimeDelta * 0.1f;
            _float fRatio = m_fHighTime / 0.2f;
            if (1.f <= fRatio)
            {
                fRatio = 1.f;
                m_isHigh = true;
            }

            vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[1], m_fHeight, fRatio);
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

        // 카메라 바라보는 방향 조절
        m_vLookAt.m128_f32[0] = vCamPosition.m128_f32[0];
        m_vLookAt.m128_f32[1] = vPlayerPosition.m128_f32[1];
        m_vLookAt.m128_f32[3] = 1.f;

        m_pTransformCom->Look_At(m_vLookAt);

    }
    else
    {
        m_fOriginTime += fTimeDelta * 0.1f;
        _float fRatio = m_fOriginTime / 0.2f;
        if (1.f <= fRatio)
            fRatio = 1.f;

        _vector vOriginPosition = vPlayerPosition + m_vDistance;

        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vCamPosition.m128_f32[0], vOriginPosition.m128_f32[0], fRatio);
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vCamPosition.m128_f32[1], vOriginPosition.m128_f32[1], fRatio);
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vCamPosition.m128_f32[2], vOriginPosition.m128_f32[2], fRatio);
        vCamPosition.m128_f32[3] = 1.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

        // 카메라 바라보는 방향 조절
        _vector vTargetLookAt;
        vTargetLookAt.m128_f32[0] = vCamPosition.m128_f32[0];
        vTargetLookAt.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vLookAt.m128_f32[1], vPlayerPosition.m128_f32[1], fRatio);
        vTargetLookAt.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vLookAt.m128_f32[2], vPlayerPosition.m128_f32[2], fRatio);
        vTargetLookAt.m128_f32[3] = 1.f;

        m_pTransformCom->Look_At(vTargetLookAt);

        // 시야각 조절
        m_fFovy = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_fRadian, XMConvertToRadians(60.f), fRatio);


        if (true == XMVector3Equal(vCamPosition, vOriginPosition) &&
            vTargetLookAt.m128_f32[1] == vPlayerPosition.m128_f32[1] && vTargetLookAt.m128_f32[2] == vPlayerPosition.m128_f32[2] &&
            m_fFovy == XMConvertToRadians(60.f))
        {
            m_fOriginTime = 0.f;
            m_isExit = false;

            CCamera_Follow::CAMERA_FOLLOW_DESC tDesc{};
            tDesc.vLookAt = vTargetLookAt;
            tDesc.isLockOn = true;
            m_pGameInstance->Change_Camera(TEXT("Camera_Follow"), &tDesc);
        }
    }

    __super::Bind_PipeLines();
  
	return S_OK;
}

void CCamera_LockOn::Late_Tick(_float fTimeDelta)
{	
}

HRESULT CCamera_LockOn::Render()
{
	return S_OK;
}

void CCamera_LockOn::Set_Level(_uint iLevel)
{
    m_iLevel = iLevel;
}

void CCamera_LockOn::OnEnter(void* pArg)
{
    if (nullptr != pArg)
    {
        CAMERA_LOCKON_DESC* pDesc = (CAMERA_LOCKON_DESC*)pArg;
        m_pTargetTransform = pDesc->vTargetTransform;
        m_isFind = true;
        Safe_AddRef(m_pTargetTransform);
    }
    else
    {
        m_pTargetTransform = nullptr;
        m_isFind = false;
    }

    Set_Distance();
}

void CCamera_LockOn::OnExit()
{
    if (nullptr != m_pTargetTransform)
        Safe_Release(m_pTargetTransform);

    m_pTargetTransform = nullptr;

    m_vMidPosition = { 0.f, 0.f, 0.f };
    m_fHeight = 0.f;
    m_vTargetPosition = { 0.f, 0.f, 0.f };
    m_vLookAt = { 0.f, 0.f, 0.f };

    m_isFind = false;
    m_isHigh = false;
    m_isExit = false;

    m_fMidTime = 0.f;
    m_fHighTime = 0.f;
    m_fMoveTime = { 0.f };
    m_fIdleTime = { 0.f };
    m_fOriginTime = 0.f;

    m_fRadian = XMConvertToRadians(60.f);
    m_fFovy = XMConvertToRadians(60.f);  

   /* if (true == m_pPlayer->isMove())
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
    else
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);*/

    
}

void CCamera_LockOn::Set_Distance()
{  
    // 최대 거리
    _float fDistance = 20.f;
    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    m_fHeight = vCamPosition.m128_f32[1] + 5;
}

CCamera_LockOn* CCamera_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_LockOn* pInstance = new CCamera_LockOn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CCamera_LockOn"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_LockOn::Clone(void* pArg)
{
    CCamera_LockOn* pInstance = new CCamera_LockOn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CCamera_LockOn"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_LockOn::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
    Safe_Release(m_pTargetTransform);
}
