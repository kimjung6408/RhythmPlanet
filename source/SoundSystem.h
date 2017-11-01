#pragma once
#pragma comment(lib, "fmodex_vc.lib")
#include<iostream>
#include<fmod.h>
#include<Windows.h>
#include"globalDeviceContext.h"

#define EFFECT_SOUND_COUNT 6
//0~3 UI 효과음
#define SOUND_MAIN_SCROLL 0
#define SOUND_SELECT 1
#define SOUND_MUSIC_SCROLL 2
#define SOUND_OPTION_SCROLL 3
//4 이전 씬으로 이동 효과음
#define SOUND_PREV_SCENE 4
//폭탄 터지는 소리
#define SOUND_BOMB 5


//5~ 인게임 효과음

typedef int SoundIndex;

class SoundSystem
{
private:
	//채널 0 : 음악
	//채널 1 : 화면상 키 조작 효과음
	//채널 2 : scene 전환 효과음
	//채널 3 : 인게임 효과음 1
	//채널 4 : 인게임 효과음 2
	//채널 5 : 인게임 효과음 3
	//채널 6 : 인게임 효과음 4
	FMOD_SYSTEM* pFMOD_System;

	//미리 로딩해 둘, 용량 얼마 안되는 효과음들.
	FMOD_SOUND* pGlobalSounds[EFFECT_SOUND_COUNT];

	FMOD_SOUND* pMusic;
	FMOD_CHANNEL* MusicChannel;

	bool MusicIsPlay;
public:
	SoundSystem()
	{
		FMOD_System_Create(&pFMOD_System);
		FMOD_System_Init(pFMOD_System, 32, FMOD_INIT_NORMAL, NULL);

		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/TitleScroll.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_MAIN_SCROLL]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/MusicScroll.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_MUSIC_SCROLL]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/OptionScroll.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_OPTION_SCROLL]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/SceneBack.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_PREV_SCENE]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/Select.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_SELECT]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/bomb.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_BOMB]);
		pMusic = nullptr;
		MusicIsPlay = false;
	}
	
	// SOUND_MAIN_SCROLL : 메인화면 메뉴 스크롤 효과음
	// SOUND_SELECT : 메인화면 메뉴 및 음악 스크롤 선택 효과음
	// SOUND_SCROLL : 음악 스크롤 효과음
	// SOUND_PREV_SCENE : 이전 씬으로 돌아가는 효과음
	// SOUND_MUSIC_SCROLL : 곡 선택 창에서의 스크롤 효과음
	// SOUND_BOMB : 폭탄 터지는 소리
	void PlaySoundEffect(SoundIndex Sound)
	{
		FMOD_CHANNEL* Channel;
		if(0<= Sound && Sound<=3)
			FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pGlobalSounds[Sound], false, &Channel);
		else if(Sound==SOUND_PREV_SCENE)
		{
			FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pGlobalSounds[Sound], false, &Channel);
		}
		else if (5 <= Sound)
		{
			FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pGlobalSounds[Sound], false, &Channel);
		}
	}

	void PlayMusic(const char* musicPath, bool loop)
	{
		if(loop)
			FMOD_System_CreateStream(pFMOD_System, musicPath, FMOD_LOOP_NORMAL, NULL, &pMusic);
		else
			FMOD_System_CreateStream(pFMOD_System, musicPath, FMOD_DEFAULT, NULL, &pMusic);

		FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pMusic, false, &MusicChannel);
		MusicIsPlay = true;
	}

	void StopMusic()
	{
		if (MusicIsPlay)
		{
			MusicIsPlay = false;
			FMOD_Sound_Release(pMusic);
		}
	}

	void Update()
	{
		FMOD_System_Update(pFMOD_System);
	}

	//현재 재생중인 곡의 총 길이를 밀리세컨드 단위로 리턴한다.
	unsigned int getMusicTotalLength() {
		FMOD_SOUND* pCurrentMusic;
		unsigned int TotalTimeMillis;
		FMOD_Channel_GetCurrentSound(MusicChannel, &pCurrentMusic);
		FMOD_Sound_GetLength(pCurrentMusic, &TotalTimeMillis, FMOD_TIMEUNIT_MS);
		return TotalTimeMillis;
	}

	//현재 재생중인 곡의 현재 진행중인 time을 밀리세컨드 단위로 리턴한다.
	unsigned int getMusicCurrentPlayTime() {
			unsigned int CurrentTimeMillis;
			FMOD_Channel_GetPosition(MusicChannel, &CurrentTimeMillis, FMOD_TIMEUNIT_MS);
			return CurrentTimeMillis;
	}

	void Release()
	{
		delete MusicChannel;

		for (int i = 0; i < EFFECT_SOUND_COUNT; i++)
		{
			FMOD_Sound_Release(pGlobalSounds[i]);
			delete pGlobalSounds[i];
		}

		delete pGlobalSounds;


		FMOD_Sound_Release(pMusic);
		delete pMusic;

		FMOD_System_Close(pFMOD_System);
		FMOD_System_Release(pFMOD_System);
	}

	~SoundSystem()
	{
		Release();
		delete pFMOD_System;
	}
};