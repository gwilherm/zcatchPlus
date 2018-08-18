/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_PLAYER_H
#define GAME_SERVER_PLAYER_H

// this include should perhaps be removed
#include "entities/character.h"
#include "gamecontext.h"
#include <vector> // std::vector

// player object
class CPlayer
{
	MACRO_ALLOC_POOL_ID()

public:
	CPlayer(CGameContext *pGameServer, int ClientID, int Team);
	~CPlayer();

	void Init(int CID);

	void TryRespawn();
	void Respawn();
	void SetTeam(int Team, bool DoChatMsg = true);
	void SetTeamDirect(int Team); //zCatch
	int GetTeam() const { return m_Team; };
	int GetCID() const { return m_ClientID; };

	void Tick();
	void PostTick();
	void Snap(int SnappingClient);

	void OnDirectInput(CNetObj_PlayerInput *NewInput);
	void OnPredictedInput(CNetObj_PlayerInput *NewInput);
	void OnDisconnect(const char *pReason);

	void KillCharacter(int Weapon = WEAPON_GAME);
	CCharacter *GetCharacter();

	//---------------------------------------------------------
	// this is used for snapping so we know how we can clip the view for the player
	vec2 m_ViewPos;

	// states if the client is chatting, accessing a menu etc.
	int m_PlayerFlags;

	// used for snapping to just update latency if the scoreboard is active
	int m_aActLatency[MAX_CLIENTS];

	// used for spectator mode
	int m_SpectatorID;

	bool m_IsReady;

	//
	int m_Vote;
	int m_VotePos;
	//
	int m_LastVoteCall;
	int m_LastVoteTry;
	int m_LastChat;
	int m_LastSetTeam;
	int m_LastSetSpectatorMode;
	int m_LastChangeInfo;
	int m_LastEmote;
	int m_LastKill;

	// TODO: clean this up
	struct
	{
		char m_SkinName[64];
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;
	} m_TeeInfos;

	// rainbow stuff
	bool IsRainbowTee(){ return m_IsRainbowBodyTee || m_IsRainbowFeetTee;}
	void ResetRainbowTee(){ m_IsRainbowBodyTee = false; m_IsRainbowFeetTee = false;}
	void GiveBodyRainbow(){ m_IsRainbowBodyTee = true;}
	void GiveFeetRainbow(){ m_IsRainbowFeetTee = true;}
	bool m_IsRainbowBodyTee{false};
	bool m_IsRainbowFeetTee{false};
	// rainbow end

	int m_RespawnTick;
	int m_DieTick;
	int m_Score;
	int m_ScoreStartTick;
	bool m_ForceBalanced;
	int m_LastActionTick;
	int m_TeamChangeTick;
	struct
	{
		int m_TargetX;
		int m_TargetY;
	} m_LatestActivity;

	// network latency calculations
	struct
	{
		int m_Accum;
		int m_AccumMin;
		int m_AccumMax;
		int m_Avg;
		int m_Min;
		int m_Max;
	} m_Latency;

	//zCatch:
	int m_CaughtBy;
	bool m_SpecExplicit;
	int m_Deaths;
	int m_Kills;
	int m_LastKillTry;

	int m_TicksSpec;
	int m_TicksIngame;
	int m_ChatTicks;
	//Anticamper
	int Anticamper();
	bool m_SentCampMsg;
	int m_CampTick;
	vec2 m_CampPos;

	// zCatch/TeeVi
	enum
	{
		ZCATCH_NOT_CAUGHT = -1,
		ZCATCH_RELEASE_ALL = -1,
		ZCATCH_CAUGHT_REASON_JOINING = 0,
		ZCATCH_CAUGHT_REASON_KILLED,
	};
	struct CZCatchVictim
	{
		int ClientID;
		int Reason;
		CZCatchVictim *prev;
	};
	CZCatchVictim *m_ZCatchVictims;
	int m_zCatchNumVictims;
	int m_zCatchNumKillsInARow;
	int m_zCatchNumKillsReleased;
	bool m_zCatchJoinSpecWhenReleased;
	void AddZCatchVictim(int ClientID, int reason = ZCATCH_CAUGHT_REASON_JOINING);
	void ReleaseZCatchVictim(int ClientID, int limit = 0, bool manual = false);
	bool HasZCatchVictims() { return (m_ZCatchVictims != NULL); }
	int LastZCatchVictim() { return HasZCatchVictims() ? m_ZCatchVictims->ClientID : -1; }

	/* ranking system */
	struct {
		int m_Points;
		int m_NumWins;
		int m_NumKills;
		int m_NumKillsWallshot;
		int m_NumDeaths;
		int m_NumShots;
		int m_TimePlayed; // ticks
		int m_TimeStartedPlaying; // tick
	} m_RankCache;
	void RankCacheStartPlaying();
	void RankCacheStopPlaying();

	// zCatch/TeeVi hard mode
	struct {
		bool m_Active;
		char m_Description[256];
		unsigned int m_ModeAmmoLimit;
		unsigned int m_ModeAmmoRegenFactor;
		struct {
			bool m_Active;
			CCharacter *m_Character;
		} m_ModeDoubleKill;

		struct {
			bool m_Active;
			int m_TimeSeconds;
			int m_LastKillTick;
		} m_ModeKillTimelimit;
		bool m_ModeHookWhileKilling;

		struct {
			bool m_Active;
			unsigned int m_Heat;
		} m_ModeWeaponOverheats;

		struct {
			bool m_Active;
			unsigned int m_Max;
			unsigned int m_Fails;
		} m_ModeTotalFails;
		bool m_ModeInvisibleProjectiles; // TODO
		bool m_ModeInvisiblePlayers; // TODO
	} m_HardMode;

	void DoRainbowBodyStep();
	void DoRainbowFeetStep();
	bool AddHardMode(const char*);
	const char* AddRandomHardMode();
	void ResetHardMode();
	void HardModeRestart();
	void HardModeFailedShot();

	// bot detection
	// old bot detection
	/*
	int m_IsAimBot;
	int m_AimBotIndex;
	int m_AimBotRange;
	float m_AimBotTargetSpeed;
	vec2 m_CurrentTarget;
	vec2 m_LastTarget;
	int m_AimBotLastDetection;
	vec2 m_AimBotLastDetectionPos;
	vec2 m_AimBotLastDetectionPosVictim;
	*/
	/*teehistorian player tracking*/
	bool m_TeeHistorianTracked;

	void SetTeeHistorianTracked(bool tracked){m_TeeHistorianTracked = tracked;}
	bool GetTeeHistorianTracked(){return m_TeeHistorianTracked;}
	/*teehistorian player tracking*/

private:
	CCharacter *m_pCharacter;
	CGameContext *m_pGameServer;

	CGameContext *GameServer() const { return m_pGameServer; }
	IServer *Server() const;

	// rainbow stuff
	int m_RainbowBodyStep{0};
	int m_RainbowFeetStep{0};

	//
	bool m_Spawning;
	int m_ClientID;
	int m_Team;
};

#endif
