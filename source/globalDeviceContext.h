#pragma once
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include"Music.h"
#include<fmod.h>
#include<iostream>
#include<fstream>
#include<string>
#include<json/json.h>
using namespace std;

#pragma region EventValues

#define OBSTACLE_SIDE 0
#define OBSTACLE_FLOOR 1
#define OBSTACLE_FALL 2

#define SIDE_LEFT 0
#define SIDE_RIGHT 1
#define	SIDE_TOP 2
#define SIDESUBTYPE_UNDER 0
#define SIDESUBTYPE_UPPER 1
#define SIDESUBTYPE_BAR 2

//1X1
#define FLOORSIZE_SMALL 0
//4X4
#define FLOORSIZE_NORMAL 1

#define FALLSIZE_SMALL 0
#define FALLSIZE_BIG 1
#pragma endregion

#pragma region EventStruct
struct FloorEvent
{
	int tick;
	int size;
	int x;
	int y;

	FloorEvent(int tick, int size, int x, int y)
	{
		this->tick = tick;
		this->size = size;
		this->x = x;
		this->y = y;
	}

	bool operator<(const FloorEvent& rhs) const { return tick < rhs.tick; }
};

struct FallEvent
{
	int tick;
	int x;
	int y;
	int genIdx;
	FallEvent(int tick,int x, int y, int genIdx)
	{
		this->tick = tick;
		this->x = x;
		this->y = y;
		this->genIdx = genIdx;
	}

	bool operator<(const FallEvent& rhs) const { return tick < rhs.tick; }
};

struct SideEvent
{
	int tick;
	int subtype;
	int side;
	int place;

	SideEvent(int tick, int subtype, int side, int place)
	{
		this->tick = tick;
		this->subtype = subtype;
		this->side = side;
		this->place = place;
	}

	bool operator<(const SideEvent& rhs) const { return tick < rhs.tick; }
};

struct CautionSideEvent
{
	int tick;
	int subtype;
	int side;
	int place;

	CautionSideEvent(int tick, int subtype, int side, int place)
	{
		this->tick = tick;
		this->subtype = subtype;
		this->side = side;
		this->place = place;
	}

	bool operator<(const CautionSideEvent& rhs) const { return tick < rhs.tick; }
};

struct CautionFallEvent
{
	int tick;
	int x;
	int y;
	int genIdx;
	CautionFallEvent(int tick, int x, int y, int genIdx)
	{
		this->tick = tick;
		this->x = x;
		this->y = y;
		this->genIdx = genIdx;
	}

	bool operator<(const CautionFallEvent& rhs) const { return tick < rhs.tick; }
};

struct CautionFloorEvent
{
	int tick;
	int size;
	int x;
	int y;

	CautionFloorEvent(int tick, int size, int x, int y)
	{
		this->tick = tick;
		this->size = size;
		this->x = x;
		this->y = y;
	}

	bool operator<(const CautionFloorEvent& rhs) const { return tick < rhs.tick; }
};
#pragma endregion

//폴더 내에 있는 바로 하위의 폴더 목록을 가져온다.
//파일 목록도 포함하여 가져오므로 주의해야 한다.
vector<string> getAllFoldersWithinFolder(string folder)
{
	vector<string> names;
	string search_path = folder + "/*";
	WIN32_FIND_DATA fd;

	std::wstring stemp = std::wstring(search_path.begin(), search_path.end());
	LPCWSTR sw = stemp.c_str();

	HANDLE hFind = ::FindFirstFile(sw, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			if (fd.cFileName[0] == '.')
				continue;

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				wstring s(fd.cFileName);
				string inString = "";
				inString.assign(s.begin(), s.end());
				names.push_back(inString);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	return names;
}

Json::Value note_obstacle_parse(string filename) {
	fstream fs;

	fs.open(filename, fstream::in | fstream::binary);
	fs.seekg(0, fstream::end);
	int filesize = fs.tellg();
	fs.seekg(0, 0);

	char *buffer = new char[filesize + 1];

	fs.read(buffer, filesize);
	fs.close();

	string config_doc = buffer;
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(config_doc, root)) {
		cerr << reader.getFormattedErrorMessages();
		//return false;
	}


	delete buffer;
	return root;
}

class Global
{
private:
	static ID3D11Device* pDevice;
	static ID3D11DeviceContext* pContext;

	static int MusicIndex;
	static vector<Music> musics;

	//global data
	static ID3D11ShaderResourceView* CautionMarkSRV;

	class Note {
	public:
		string title;
		string bpm;
		string music_path;
		string intro_music_path;
		string skin_name;
		string cover_path;
		string select_cover_path;
	};

	static bool note_metadata_parse(string filename, Note *note) {

		fstream fs;
		fs.open(filename, fstream::in | fstream::binary);
		fs.seekg(0, fstream::end);
		int filesize = fs.tellg();
		fs.seekg(0, 0);

		char *buffer = new char[filesize + 1];

		fs.read(buffer, filesize);
		fs.close();

		string config_doc = buffer;
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(config_doc, root)) {
			//cout << reader.getFormattedErrorMessages();
			return false;
		}
		note->bpm = root.get("bpm", "").asString();
		note->title = root.get("title", "").asString();
		note->music_path = root.get("musicPath", "").asString();
		note->intro_music_path = root.get("musicIntroPath", "").asString();
		note->skin_name = root.get("skin", "").asString();
		note->cover_path = root.get("mainCover", "").asString();
		note->select_cover_path = root.get("selectCover", "").asString();
		delete(buffer);
		return true;
	}

	static bool isClear;
	static long long score;

public:
	static void LoadSongs(ID3D11Device* inDevice, ID3D11DeviceContext* inContext)
	{
		string ignore;
		vector<string> songFolders;
		//songs에서 folder 목록을 얻어온다.
		songFolders = getAllFoldersWithinFolder("Songs");
		Note* note;
		//각 폴더 마다,
		//곡 정보를 얻어서 저장한다. 단, 게임 데이터 로딩은 인게임 창 시작시에 한다.
		for (int i = 0;i<songFolders.size(); i++)
		{
			//Read Metadata.json
				string temp;
				temp = "Songs\\" + songFolders[i] + "\\metadata.json";

				note = new Note();
				note_metadata_parse(temp, note);


			string path = "Songs/" + songFolders[i] + "/songInfo.txt";
			string title = note->title;
			string artist="";
			string BPM = note->bpm;
			string SoundFile=note->music_path;
			string IntroSoundFile=note->intro_music_path;
			string BG_file=note->cover_path;
			string Intro_BG_file=note->select_cover_path;
			string theme = note->skin_name;
			string GameDataPath="Obstacle.json";

			musics.push_back(Music(songFolders[i], title, artist, BPM, SoundFile, IntroSoundFile, BG_file, Intro_BG_file, GameDataPath, theme, inDevice));
			delete note;
		}

		//여기에서 저장하는 것은 BG의 곡명, 음원파일명, 인트로음원파일명, SRV, BPM, 게임데이터 파일명이다.
	}

	static void InitializeDevice(ID3D11Device* inDevice, ID3D11DeviceContext* inContext)
	{
		pDevice = inDevice;
		pContext = inContext;

		LoadSongs(inDevice, inContext);
		MusicIndex = 6 % musics.size();

		//Load Global other data
		HR(D3DX11CreateShaderResourceViewFromFileW(inDevice, L"Textures/Ingame/MagicCircle.png", 0, 0, &CautionMarkSRV, 0));
	}
	static ID3D11Device* Device() { return pDevice; }
	static ID3D11DeviceContext* Context() { return pContext; }
	static void finishRender()
	{
		Global::Context()->RSSetState(0);
		//depth test 초기화
		Global::Context()->OMSetDepthStencilState(0, 0);
		//blend state 초기화
		Global::Context()->OMSetBlendState(0, 0, 0xffffffff);
	}

	static void SetCurrentMusicIndex(int Index)
	{
		MusicIndex = Index;
	}
	
	static int GetNumOfSongs()
	{
		return musics.size();
	}

	static int GetCurrentMusicIndex()
	{
		return MusicIndex;
	}

	static Music* GetCurrentMusic()
	{
		return &musics[MusicIndex];
	}

	static vector<Music>* GetMusicList()
	{
		return &musics;
	}

	static void UpWheelMusicIndex()
	{
		MusicIndex+=musics.size();
		MusicIndex -= 1;
		MusicIndex%=musics.size();
	}

	static void DownWheelMusicIndex()
	{
		MusicIndex += 1;
		MusicIndex%=musics.size();
	}

	static ID3D11ShaderResourceView* GetGlobalSRV(string srvName)
	{
		if (srvName == "CautionMark")
			return CautionMarkSRV;
		
		return nullptr;
	}

	static void SetScore(long long in_score)
	{
		score = in_score;
	}

	static long long GetScore()
	{
		return score;
	}

	static void SetMusicClear(bool cleared)
	{
		isClear = cleared;
	}

	static bool IsClear()
	{
		return isClear;
	}
};

ID3D11Device* Global::pDevice = 0;
ID3D11DeviceContext* Global::pContext = 0;
ID3D11ShaderResourceView* Global::CautionMarkSRV = 0;
int Global::MusicIndex = 0;
vector<Music> Global::musics = vector<Music>();
long long Global::score = 0;
bool Global::isClear = false;