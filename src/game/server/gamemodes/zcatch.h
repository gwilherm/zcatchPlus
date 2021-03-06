/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
/* zCatch by erd and Teetime                                                                 */

#ifndef GAME_SERVER_GAMEMODES_ZCATCH_H
#define GAME_SERVER_GAMEMODES_ZCATCH_H

#include <game/server/gamecontroller.h>
#include <thread>


class CGameController_zCatch: public IGameController
{
	// jxsl13 added
	int m_OldAllowJoin;
	int m_OldSvReleaseGame;
	int m_OldPlayersIngame{0};
	// player information
	int m_PlayersOnline{0};
	int m_PlayersExplicitlySpectating{0};
	int m_PlayersPlaying{0};
	int m_PlayersNotCaught{0};
	int m_PlayersCaughtSpectating{0};
	int m_PlayerIdWithMostCaughtPlayers{ -1};
	int m_PlayerMostCaughtPlayers{0};


	void RewardWinner(int winnerId);

	/* ranking system */
	static void ChatCommandTopFetchDataAndPrint(CGameContext* GameServer, int clientId, const char *column);
	static void ChatCommandTopFetchDataFromViewAndPrint(CGameContext* GameServer, int clientId, const char *column);

	static void ChatCommandRankFetchDataAndPrint(CGameContext* GameServer, int clientId, char *name);
	static void SaveScore(CGameContext* GameServer, char *name, int score, int numWins, int numKills, int numKillsWallshot, int numDeaths, int numShots, int highestSpree, int timePlayed, int GameMode, int Free = 0);
	static void FormatRankingColumn(const char* column, char buf[32], int value);

	// jxsl13 added
	void ToggleLastStandingDeathmatchAndRelease(int Players_Ingame, int caughtPlayers);
	void CheckGameConfigStatus();
	static const char* GetGameModeTableName(int GameMode = 0);

public:
	CGameController_zCatch(class CGameContext *pGameServer);
	~CGameController_zCatch();
	virtual void Tick();
	virtual void DoWincheck();

	virtual void StartRound();
	virtual void OnCharacterSpawn(class CCharacter *pChr);
	virtual void OnPlayerInfoChange(class CPlayer *pP);
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int WeaponID);
	virtual bool OnEntity(int Index, vec2 Pos);
	virtual bool CanChangeTeam(CPlayer *pPlayer, int JoinTeam);
	virtual void EndRound();



	/* ranking system */
	virtual void SaveRanking(CPlayer *player);
	virtual void OnInitRanking(sqlite3 *rankingDb);
	virtual void OnChatCommandTop(CPlayer *pPlayer, const char *category = "");
	virtual void OnChatCommandOwnRank(CPlayer *pPlayer);
	virtual void OnChatCommandRank(CPlayer *pPlayer, const char *name);

	// jxsl13 was here
	static void MergeRankingIntoTarget(CGameContext* GameServer, char* Source, char *Target);
	static void DeleteRanking(CGameContext* GameServer, char* Name, int GameMode = 0, int Free = 0);

	// rainbow game stuff
	int m_RainbowCount{0};
	bool m_SomoneHasRainbow{false};
	/**
	 * @brief gives rainbow to a person that is not the victim
	 * @details [long description]
	 *
	 * @param VictimID set this to -1 to choose between everyone.
	 * @param condition [description]
	 */
	void GiveRainbowToRandomPlayer(int VictimID, bool condition = true, bool body = true, bool feet = true);

};

#endif
