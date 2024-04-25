#include "stdafx.h"
#include "Camera_Puzzle.h"

#include "Player.h"

CCamera_Puzzle::CCamera_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Puzzle::CCamera_Puzzle(const CCamera_Puzzle& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCamera_Puzzle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Puzzle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Puzzle::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
	_vector vPlayerPosition = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1] - 1.f, -58.f, 1.f};
	_vector vDir = XMVector3Normalize(vPlayerPosition - vOrigin);
	_float fDistance = 30.f;

	_vector vTargetPosition = vOrigin + vDir * fDistance;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPosition);
	m_pTransformCom->Look_At(vPlayerPosition);

    __super::Bind_PipeLines();

    return S_OK;
}

void CCamera_Puzzle::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Puzzle::Render()
{
    return S_OK;
}

void CCamera_Puzzle::Set_Level(_uint iLevel)
{
}

void CCamera_Puzzle::OnEnter(void* pArg)
{
}

void CCamera_Puzzle::OnExit()
{
}

CCamera_Puzzle* CCamera_Puzzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Puzzle* pInstance = new CCamera_Puzzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CCamera_Puzzle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Puzzle::Clone(void* pArg)
{
	CCamera_Puzzle* pInstance = new CCamera_Puzzle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCamera_Puzzle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Puzzle::Free()
{
	__super::Free();
}
