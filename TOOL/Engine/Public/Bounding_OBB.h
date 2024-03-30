#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct Bounding_OBB_Desc : public CBounding::BOUNDING_DESC
	{
		_float3	vSize;
		_float3 vRotation;
	}BOUNDING_OBB_DESC;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual void*	Get_BoundingDesc() { return m_pBoundingDesc; }

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void	Tick(_fmatrix WorldMatrix) override;
	virtual _bool	Check_Collision(CCollider::TYPE eType, CBounding* pBounding) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif // _DEBUG

private:
	BoundingOrientedBox*	m_pBoundingDesc_Origin = { nullptr };
	BoundingOrientedBox*	m_pBoundingDesc = { nullptr };

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END