#pragma once

#include "Base.h"
#include "fmod.h"
#include "Engine_Defines.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.2f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class CSound_Manager final : public CBase
{
public:
	enum CHANNELID { BGM, PLAYER, MONSTER1, MONSTER2, MONSTER3, MONSTER4, MONSTER5, BOSS, EFFECT1, EFFECT2, EFFECT3, UI1, UI2, UI3, SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3, MAXCHANNEL };

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	_uint	VolumeUp(CHANNELID eID, _float _vol);
	_uint	VolumeDown(CHANNELID eID, _float _vol);
	_uint	BGMVolumeUp(_float _vol);
	_uint	BGMVolumeDown(_float _vol);

	void	PlayBGM(TCHAR* pSoundKey, _float fVolume, _bool isLoop);
	_bool	isPlaying(CHANNELID eID);
	void	StopSound(CHANNELID eID);
	_uint	Pause(CHANNELID eID);
	void	StopAll();

	void	Play_Once(TCHAR* pSoundKey, CHANNELID eID, _float fVolume = SOUND_DEFAULT);
	void	Play_Loop(TCHAR* pSoundKey, CHANNELID eID, _float fVolume = SOUND_DEFAULT);
	//void	Play_Once


	void	PlaySound(TCHAR* pSoundKey, CHANNELID eID, _float _vol);
	void	PlaySoundOnce(TCHAR* pSoundKey, CHANNELID eID, _float _vol);

	void	SetChannelOneOff(_bool bOneOff, CHANNELID eID) { m_bChannelOneOff[eID] = bOneOff; }
	void	PlaySoundOneOff(TCHAR* pSoundKey, CHANNELID eID, _float _vol);
	int		PlaySoundLoop(TCHAR* pSoundKey, CHANNELID eID, _float _vol);

	//static FMOD_RESULT F_CALLBACK StoppedCallback(FMOD_CHANNELCONTROL* channel, FMOD_CHANNELCONTROL_TYPE type, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2);
	void	SetLoopCount(CHANNELID eID, _uint iCount) { FMOD_Channel_SetLoopCount(m_pChannelArr[eID], iCount); }

private:
	_bool	m_bChannelOneOff[MAXCHANNEL] = { false };

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
	_bool		m_bPause = false;

public:
	//_bool		m_isFinish;;

private:
	void LoadSoundFile();

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END


