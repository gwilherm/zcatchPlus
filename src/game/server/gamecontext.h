/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMECONTEXT_H
#define GAME_SERVER_GAMECONTEXT_H

#include <engine/console.h>
#include <engine/server.h>
#include <engine/shared/memheap.h>
#include <engine/shared/uuid_manager.h>

#include <game/layers.h>
#include <game/voting.h>

#include "eventhandler.h"
//#include "gamecontroller.h"
#include "gameworld.h"
#include "player.h"

/* ranking system */
#include <engine/external/sqlite/sqlite3.h>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <future>
/*teehistorian*/
#include "teehistorian.h"
#include "botdetection.h"
#include <engine/shared/linereader.h>


#define MAX_MUTES 35
#define ZCATCH_VERSION "0.6.0"
#define IRREGULAR_FLAG_LENGTH 32

/*
	Tick
		Game Context (CGameContext::tick)
			Game World (GAMEWORLD::tick)
				Reset world if requested (GAMEWORLD::reset)
				All entities in the world (ENTITY::tick)
				All entities in the world (ENTITY::tick_defered)
				Remove entities marked for deletion (GAMEWORLD::remove_entities)
			Game Controller (GAMECONTROLLER::tick)
			All players (CPlayer::tick)



	Snap
		Game Context (CGameContext::snap)
			Game World (GAMEWORLD::snap)
				All entities in the world (ENTITY::snap)
			Game Controller (GAMECONTROLLER::snap)
			Events handler (EVENT_HANDLER::snap)
			All players (CPlayer::snap)

*/

class IStorage;


class CGameContext : public IGameServer
{
	IServer *m_pServer;
	class IConsole *m_pConsole;
	IStorage *m_pStorage;
	CLayers m_Layers;
	CCollision m_Collision;
	CNetObjHandler m_NetObjHandler;
	CTuningParams m_Tuning;


	static void CommandCallback(int ClientID, int FlagMask, const char *pCmd, IConsole::IResult *pResult, void *pUser);
	/*teehistorian end*/

	static void ConTuneParam(IConsole::IResult *pResult, void *pUserData);
	static void ConTuneReset(IConsole::IResult *pResult, void *pUserData);
	static void ConTuneDump(IConsole::IResult *pResult, void *pUserData);
	static void ConPause(IConsole::IResult *pResult, void *pUserData);
	static void ConChangeMap(IConsole::IResult *pResult, void *pUserData);
	static void ConRestart(IConsole::IResult *pResult, void *pUserData);
	static void ConBroadcast(IConsole::IResult *pResult, void *pUserData);
	static void ConSay(IConsole::IResult *pResult, void *pUserData);
	static void ConSayTo(IConsole::IResult *pResult, void *pUserData);
	static void ConSetTeam(IConsole::IResult *pResult, void *pUserData);
	static void ConSetTeamAll(IConsole::IResult *pResult, void *pUserData);
	static void ConSpeakAs(IConsole::IResult *pResult, void *pUserData);
	static void ConSwapTeams(IConsole::IResult *pResult, void *pUserData);
	static void ConShuffleTeams(IConsole::IResult *pResult, void *pUserData);
	static void ConLockTeams(IConsole::IResult *pResult, void *pUserData);
	static void ConAddVote(IConsole::IResult *pResult, void *pUserData);
	static void ConRemoveVote(IConsole::IResult *pResult, void *pUserData);
	static void ConForceVote(IConsole::IResult *pResult, void *pUserData);
	static void ConClearVotes(IConsole::IResult *pResult, void *pUserData);
	static void ConVote(IConsole::IResult *pResult, void *pUserData);
	static void ConchainSpecialMotdupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

	static void ConMute(IConsole::IResult *pResult, void *pUserData);
	static void ConUnmuteID(IConsole::IResult *pResult, void *pUserData);
	static void ConUnmuteIP(IConsole::IResult *pResult, void *pUserData);
	static void ConMutes(IConsole::IResult *pResult, void *pUserData);

	static void ConKill(IConsole::IResult *pResult, void *pUserData);

	CGameContext(int Resetting);
	void Construct(int Resetting);

	bool m_Resetting;

	bool MuteValidation(CPlayer *player);

	/* ranking system: sqlite connection */
	sqlite3 *m_RankingDb;
	std::timed_mutex m_RankingDbMutex;

	// zCatch/TeeVi: hard mode
	struct HardMode
	{
		const char* name;
		bool laser;
		bool grenade;
	};
	std::vector<HardMode> m_HardModes;

public:
	IServer *Server() const { return m_pServer; }
	class IConsole *Console() { return m_pConsole; }
	IStorage *Storage() { return m_pStorage; }
	CCollision *Collision() { return &m_Collision; }
	CTuningParams *Tuning() { return &m_Tuning; }
	class CServerBan *GetBanServer() { return Server()->GetBanServer();}

	static void TeeHistorianWrite(const void *pData, int DataSize, void *pUser);

	/*teehistrorian*/
	CTeeHistorian m_TeeHistorian;

	/*nickname bans*/
	std::vector<std::string> m_BannedNicks;



	CGameContext();
	~CGameContext();

	void Clear();

	CEventHandler m_Events;
	CPlayer *m_apPlayers[MAX_CLIENTS];

	IGameController *m_pController;
	CGameWorld m_World;

	// helper functions
	class CCharacter *GetPlayerChar(int ClientID);

	int m_LockTeams;

	// voting
	void StartVote(const char *pDesc, const char *pCommand, const char *pReason);
	void EndVote();
	void SendVoteSet(int ClientID);
	void SendVoteStatus(int ClientID, int Total, int Yes, int No);
	void AbortVoteKickOnDisconnect(int ClientID);

	int m_VoteCreator;
	int64 m_VoteCloseTime;
	bool m_VoteUpdate;
	int m_VotePos;
	char m_aVoteDescription[VOTE_DESC_LENGTH];
	char m_aVoteCommand[VOTE_CMD_LENGTH];
	char m_aVoteReason[VOTE_REASON_LENGTH];
	int m_NumVoteOptions;
	int m_VoteEnforce;
	enum
	{
		VOTE_ENFORCE_UNKNOWN = 0,
		VOTE_ENFORCE_NO,
		VOTE_ENFORCE_YES,
		// ddnet
		VOTE_ENFORCE_NO_ADMIN,
	};
	CHeap *m_pVoteOptionHeap;
	CVoteOptionServer *m_pVoteOptionFirst;
	CVoteOptionServer *m_pVoteOptionLast;

	// helper functions
	void CreateDamageInd(vec2 Pos, float AngleMod, int Amount);
	void CreateExplosion(vec2 Pos, int Owner, int Weapon, bool NoDamage, bool limitVictims = false, const bool *victims = NULL, int ownerLastDieTickBeforceFiring = 0);
	void CreateHammerHit(vec2 Pos);
	void CreatePlayerSpawn(vec2 Pos);
	void CreateDeath(vec2 Pos, int Who);
	void CreateSound(vec2 Pos, int Sound, int Mask = -1);
	void CreateSoundGlobal(int Sound, int Target = -1);



	enum
	{
		CHAT_ALL = -2,
		CHAT_SPEC = -1,
		CHAT_RED = 0,
		CHAT_BLUE = 1
	};

	struct CMutes
	{
		char m_aIP[NETADDR_MAXSTRSIZE];
		int m_Expires;
	};
	CMutes m_aMutes[MAX_MUTES];
	// helper functions
	void AddMute(const char* pIP, int Secs);
	void AddMute(int ClientID, int Secs, bool Auto = false);
	int Muted(const char *pIP);
	int Muted(int ClientID);
	void CleanMutes();

	// network
	void SendChatTarget(int To, const char *pText);
	void SendPrivateMessage(int From, int To, const char *pText);
	void SendChat(int ClientID, int Team, const char *pText);
	void SendEmoticon(int ClientID, int Emoticon);
	void SendWeaponPickup(int ClientID, int Weapon);
	void SendBroadcast(const char *pText, int ClientID);
	virtual void InformPlayers(const char *pText) { SendChatTarget(-1, pText); }

	//
	void CheckPureTuning();
	void SendTuningParams(int ClientID);

	//
	void SwapTeams();

	// engine events
	virtual void OnInit();
	virtual void OnConsoleInit();
	virtual void OnShutdown();

	virtual void OnTick();
	virtual void OnPreSnap();
	virtual void OnSnap(int ClientID);
	virtual void OnPostSnap();

	virtual void OnMessage(int MsgID, CUnpacker *pUnpacker, int ClientID);

	virtual void OnClientConnected(int ClientID);
	virtual void OnClientEnter(int ClientID);
	virtual void OnClientDrop(int ClientID, const char *pReason);
	virtual void OnClientDirectInput(const char* ClientName, int ClientID, void *pInput);
	virtual void OnClientPredictedInput(int ClientID, void *pInput);

	virtual void OnSetAuthed(int ClientID, int Level);

	/*teehistorian*/
	virtual void OnClientEngineJoin(int ClientID);
	virtual void OnClientEngineDrop(int ClientID, const char *pReason);
	/*teehistorian end*/


	virtual bool IsClientReady(int ClientID);
	virtual bool IsClientPlayer(int ClientID);

	virtual const char *GameType();
	virtual const char *Version();
	virtual const char *NetVersion();

	// bot detection
	// old bot detection
	/*
	enum
	{
		BOT_DETECTION_FAST_AIM=1,
		BOT_DETECTION_FOLLOW=2,
	};
	virtual bool IsClientAimBot(int ClientID);
	*/

	/*future stuff*/
	std::queue<std::future<void> > m_Futures;
	void AddFuture(std::future<void> Future) {m_Futures.push(std::move(Future));};
	void CleanFutures() {
		unsigned long size = m_Futures.size();

		for (unsigned long i = 0; i < size; ++i)
		{
			std::future<void> f = std::move(m_Futures.front());
			m_Futures.pop();
			auto status = f.wait_for(std::chrono::milliseconds(0));
			if (status == std::future_status::ready)
			{

			} else {
				m_Futures.push(std::move(f));
			}
		}


	};

	void WaitForFutures() {
		unsigned long size = m_Futures.size();

		for (unsigned long i = 0; i < size; ++i)
		{
			std::future<void> f = std::move(m_Futures.front());
			m_Futures.pop();
			f.wait();
		}

	};

	/* ranking system */
	sqlite3* GetRankingDb() { return m_RankingDb; };
	bool RankingEnabled() { return m_RankingDb != NULL; };
	bool LockRankingDb(int ms = -1);
	void UnlockRankingDb();

	// zCatch/TeeVi: hard mode
	std::vector<HardMode> GetHardModes() { return std::vector<HardMode>(m_HardModes.begin(), m_HardModes.end()); };


	// jxsl13 was here
	/*zCatch extended features*/
	static void ConMergeRecords(IConsole::IResult *pResult, void *pUserData);
	static void ConMergeRecordsId(IConsole::IResult *pResult, void *pUserData);
	static void ConSaveTeehistorian(IConsole::IResult *pResult, void *pUserData);

	/*bot detection*/
	class CBotDetection *m_BotDetection;
	static void ConPrintBotData(IConsole::IResult *pResult, void *pUserData);
	/*bot detection*/

	/*teehistorian player tracking*/
	static void ConTeehistorianTrackPlayer(IConsole::IResult *pResult, void *pUserData);
	static void ConTeehistorianUntrackPlayer(IConsole::IResult *pResult, void *pUserData);
	static void ConTeehistorianUntrackAllPlayers(IConsole::IResult *pResult, void *pUserData);


	/*teehistorian player tracking*/
	static void ConList(IConsole::IResult *pResult, void *pUserData);
	static void ConTrackedPlayers(IConsole::IResult *pResult, void *pUserData);


	// nickname bans feature
	// get nicks from file
	static void ConShowBannedNicks(IConsole::IResult *pResult, void *pUserData);
	static void ConRemoveFromBannedNicks(IConsole::IResult *pResult, void *pUserData);
	static void ConBanNickByName(IConsole::IResult *pResult, void *pUserData);
	static void ConBanNickByID(IConsole::IResult *pResult, void *pUserData);

	// range ban simplyfication
	static void ConBanRangeLevel(IConsole::IResult *pResult, void *pUserData);


	// snapshot stuff
	static void ConSnapshotId(IConsole::IResult *pResult, void *pUserData);
	static void ConPrintSnapshotId(IConsole::IResult *pResult, void *pUserData);
	static void ConPrintSnapshotIdCore(IConsole::IResult *pResult, void *pUserData);
	static void ConPrintSnapshotIdInput(IConsole::IResult *pResult, void *pUserData);
	static void PrintSnapShot(CGameContext *pSelf, int playerID, int mode);
	//TODO adding some analyzing functions based on snapshots

	// flags stuff
	static void ConShowIrregularFlags(IConsole::IResult *pResult, void *pUserData);
	static void ConShowCurrentFlags(IConsole::IResult *pResult, void *pUserData);
	bool PrintIrregularFlags(int ClientID, bool currentFlags = false);

	// long term data stuff
	static void ConShowLongTermData(IConsole::IResult *pResult, void *pUserData);
	static void ConShowCurrentData(IConsole::IResult *pResult, void *pUserData);

	// weird client messages stuff
	static void ConShowWeirdClientMessages(IConsole::IResult *pResult, void *pUserData);


	void PrintStringStreamToConsoleAndReset(std::stringstream &s, const char* ConsoleLabel);
	void PrintLongTermData(int ClientID);
	void PrintCurrentData(int ClientID);

	// fun stuff
	static void ConGiveRainbow(IConsole::IResult *pResult, void *pUserData);
	static void ConGiveRainbowBody(IConsole::IResult *pResult, void *pUserData);
	static void ConGiveRainbowFeet(IConsole::IResult *pResult, void *pUserData);


	void RetrieveNicknameBanListFromFile();
	void InitNicknameBanList();
	void SaveNicknameBanListToFile();

	void AddToNicknameBanList(int ID);
	void AddToNicknameBanList(const char* Nickname);
	void AddToNicknameBanList(std::string Nickname);

	void RemoveFromNicknameBanList(int ID);
	void RemoveFromNicknameBanList(const char* Nickname);
	void RemoveFromNicknameBanList(std::string Nickname);

	bool IsInNicknameBanList(const char* Nickname);
	bool IsInNicknameBanList(std::string Nickname);

	void BanIf(bool Condition, int ID, int TimeMinutes = 60, std::string Reason = "");



private:

	// trim from start (in place)
	static inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

// trim from end (in place)
	static inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

// trim from both ends (in place)
	static inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

// trim from start (copying)
	static inline std::string ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}

// trim from end (copying)
	static inline std::string rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}

// trim from both ends (copying)
	static inline std::string trim_copy(std::string s) {
		trim(s);
		return s;
	}

	// Plus stuff
	void EyeEmote(int ClientID, char *args);
};

inline int CmaskAll() { return -1; }
inline int CmaskOne(int ClientID) { return 1 << ClientID; }
inline int CmaskAllExceptOne(int ClientID) { return 0x7fffffff ^ CmaskOne(ClientID); }
inline bool CmaskIsSet(int Mask, int ClientID) { return (Mask & CmaskOne(ClientID)) != 0; }
#endif
