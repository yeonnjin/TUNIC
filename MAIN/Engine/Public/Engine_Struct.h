#pragma once

namespace Engine
{
	/* Engine */
	typedef struct
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;

	/* Mesh */
	typedef struct
	{
		class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	/* Channel */
	typedef struct
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		float			fTime;
	}KEYFRAME;

	/* Vertext */
	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXPOSNORTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];
	}VTXMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점에게 영향을 주는 뼈들의 인덱스와 가중치 */
		XMUINT4			vBlendIndices;	// 이 메시에게 영향을 주는 뼈들의 인덱스
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXANIMMESH;

	/* File */
	typedef struct ENGINE_DLL
	{
		// Mesh	
		_uint						iNumMeshes = { 0 };
		vector<class CMesh*>		Meshes;

		// Material
		_uint						iNumMaterials = { 0 };
		vector<MESH_MATERIAL>		Materials;

		// Bone
		_float4x4					TransformMatrix;
		// _uint iNumBones;
		vector<class CBone*>		Bones;

		// Animation
		_uint						iNumAnimations = { 0 };
		_uint						iCurrentAnimIndex = { 0 };
		_bool						isLoop = { false };
		vector<class CAnimation*>	Animations;
		
		_float4x4*					MeshBoneMatrices; // [512]
	}MODELFILE;

	typedef struct ENGINE_DLL
	{
		_char*						szName = { "" };

		_uint						iMaterialIndex = { 0 };

		_uint						iNumFaces = { 0 };

		_uint						iNumBones = { 0 };
		vector<_uint>				Bones;

		_uint						iNumOffsetMatrices = { 0 };
		vector<_float4x4>			OffsetMatrices;
	}MESHFILE;

	typedef struct ENGINE_DLL
	{
		_char*						szName = { "" };

		_float4x4					TransformationMatrix;

		_int						iParentBoneIndex = { -1 };
	}BONEFILE;

	typedef struct ENGINE_DLL
	{
		_char*						szName = { "" };

		_float						fDuration = { 0.f };			
		_float						fTicksPerSecond = { 0.f };	
		_float						fTrackPosition = { 0.f };		

		_uint						iNumChannels = { 0 };
		vector<class CChannel*>		Channels;
		vector<_uint>				CurrentKeyFrameIndices;
	}ANIMFILE;

	typedef struct ENGINE_DLL
	{
		_char*						szName = { "" };

		_int						iBoneIndex = { -1 };

		_uint						iNumKeyFrames = { 0 };
		vector<KEYFRAME>			KeyFrames;
	}CHANNELFILE;
}