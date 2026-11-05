#include "hooks.h"
#include "servers.h"
#include "players.h"
#include "exelight_patch_profiles.hpp"

namespace hooks {

	DWORD_PTR pUserData = Exelight::GameBase() + Exelight::ResolveOffset(0x14F344B0);
	DWORD_PTR pNameBuffer = Exelight::GameBase() + Exelight::ResolveOffset(0x15E056C8);

	typedef uint64_t AppId_t;
	typedef bool(__fastcall* ReadP2PPacket_t)(uintptr_t thisptr, void* pub_dest, unsigned int cub_dest, unsigned int* cub_msg_size, unsigned __int64* steam_id_remote, int n_channel);
	ReadP2PPacket_t ReadP2PPacket_o;
	typedef bool(__fastcall* Steam_bIsDlcInstalled_t)(__int64 thisPtr, uint64_t appID);
	Steam_bIsDlcInstalled_t Steam_bIsDlcInstalled_o;
	typedef bool(__fastcall* Steam_bIsAppInstalled_t)(__int64 thisPtr, uint64_t appID);
	Steam_bIsAppInstalled_t Steam_bIsAppInstalled_o;

	std::unordered_map<AppId_t, bool> dlcCache;
	std::unordered_map<AppId_t, bool> appCache;
    std::mutex contentCacheMutex;
	std::vector<int> config_strings = { 3514, 3627 };

	uintptr_t jpg_buf_o = NULL;
	char* custom_jpg_buf = NULL;
	uint32_t custom_jpg_buf_size = 0;
	uint32_t custom_thumb_buf_size = 0;
	char* custom_thumb_buf = new char[0x88000];

	const std::vector<uintptr_t> isModLoadedAddrs =
	{
		{ 0x20EB3C4 },
		{ 0x20EB3E4 },
		{ 0x20EB324 },
		{ 0x20EB2F4 },
		{ 0x20EAD97 },
		{ 0x13E6A74 },
		{ 0x15E7EDB },
		{ 0x15E87DB },
		{ 0x15EBAE9 },
		{ 0x15F1F29 },
		{ 0x15F1FD9 },
		{ 0x15F20A0 },
		{ 0x15F7F60 },
		{ 0x1A80D1D },
		{ 0x1A9AD29 },
		{ 0x1E9432B },
		{ 0x1E9C8AE },
		{ 0x1EA5669 },
		{ 0x1EBF6CD },
		{ 0x20EAD5D },
	};

	uintptr_t GetCallsiteFromReturn(void* returnAddress)
	{
		uint8_t* ret = (uint8_t*)returnAddress;

		if (ret[-5] == 0xE8)
		{
			return (uintptr_t)(ret - 5);
		}

		for (int i = 1; i <= 16; i++)
		{
			if (ret[-i] == 0xE8)
			{
				return (uintptr_t)(ret - i);
			}
		}

		return (uintptr_t)returnAddress;
	}

	bool is_user_in_game()
	{
		return Live_IsUserInGame(0);
	}

	bool local_client_is_in_game()
	{
		return CL_IsLocalClientInGame(0);
	}

	bool is_in_game()
	{
		return Com_IsInGame();
	}

	bool is_match_loaded()
	{
		return (is_user_in_game() && local_client_is_in_game() && is_in_game());
	}

	bool is_user_in_lobby()
	{
		return (!Live_IsUserInGame(0) && Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0));
	}

	bool AreWeInGameAndHosting() {

		bool rt = false;

		if (is_match_loaded() && Live_AreWeHost()) {
			return true;
		}
		return rt;

	}

	bool ProjectTracer(ImVec3 world,ImVec2& screen) {
        if(!std::isfinite(world.x)||!std::isfinite(world.y)||!std::isfinite(world.z)||!spoof_t)return false;
        float input[3]={world.x,world.y,world.z},out[3]{};
        if(!spoof_call(spoof_t,WorldPosToScreenPos,0,input,out)||!std::isfinite(out[0])||!std::isfinite(out[1]))return false;
        screen={out[0],out[1]};return true;
    }

	ImVec2 WorldToScreen(ImVec3 world,int type) {ImVec2 screen{};if(type==1)ProjectTracer(world,screen);return screen;}

	ImVec3 CG_DObjGetWorldTagPos(__int64 centity_t, __int64 DObj, int tag, float* whatever)
	{
		ImVec3 pos;
		float WORLD[3];
		auto DOBJ = CG_DObjGetWorldTagPosInternal(centity_t, DObj, tag, whatever, WORLD, 0xFFFF);

		pos.x = WORLD[0];
		pos.y = WORLD[1];
		pos.z = WORLD[2];

		return pos;
	}

	void load_custom_jpg()
	{
		if (custom_jpg_buf)
		{
			*(uint64_t*)(*(uint64_t*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x5A8498) = NULL;
			custom_jpg_buf = NULL;
			custom_jpg_buf_size = 0;
		}

		std::ifstream ifs;
		ifs.open("screenshot.jpg", std::ifstream::in | std::ifstream::binary);

		if (!ifs.is_open())
		{
			return;
		}

		ifs.seekg(0, std::ios::end);
		size_t fileSize = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		custom_jpg_buf_size = (uint32_t)fileSize;
		custom_jpg_buf = new char[(uint32_t)fileSize];
		ifs.read(custom_jpg_buf, (uint32_t)fileSize);

		ifs.close();

		// Thumbnail
		std::ifstream ifst;
		if (custom_thumb_buf)
		{
			custom_thumb_buf = NULL;
			custom_thumb_buf_size = 0;
		}

		ifst.open("thumbnail.jpg", std::ifstream::in | std::ifstream::binary);

		if (!ifst.is_open())
		{
			return;
		}

		ifst.seekg(0, std::ios::end);
		size_t thumbfileSize = ifst.tellg();
		ifst.seekg(0, std::ios::beg);

		custom_thumb_buf_size = (uint32_t)thumbfileSize;
		custom_thumb_buf = new char[(uint32_t)thumbfileSize];
		ifst.read(custom_thumb_buf, (uint32_t)thumbfileSize);

		ifst.close();

	}

	namespace functions {

		const char* hkCL_Rank_GetParagonIcon(unsigned int mode, int iconId, GfxImage** outName)
		{
			if (iconId > 55) return nullptr;

			return CL_Rank_GetParagonIcon(mode, iconId, outName);
		}


		GfxImage* hkR_CodeImage_Create(
			int width,
			int height,
			int imageFormat,
			int flags,
			const char* name)
		{
			const auto ret = reinterpret_cast<std::uintptr_t>(_ReturnAddress());			

			if (ret == OFFSET(0x1336080))
			{
				int overrideWidth = width;
				int overrideHeight = height;

				switch (iSelectedResolutionTier)
				{
				case 0:
					overrideWidth = 300;
					overrideHeight = 184;
					break;
				case 1:
					overrideWidth = 600;
					overrideHeight = 368;
					break;
				case 2:
					overrideWidth = 1200;
					overrideHeight = 736;
					break;
				case 3:
					overrideWidth = 2400;
					overrideHeight = 1472;
					break;
				case 4:
					overrideWidth = 8175;
					overrideHeight = 5014;
					break;
				}
				return R_CodeImage_Create(overrideWidth, overrideHeight, imageFormat, flags, name);
			}

			return R_CodeImage_Create(width, height, imageFormat, flags, name);
		}

		bool hkBG_UnlockablesItemOptionLocked(eModes mode, int controllerIndex, int itemIndex, int optionIndex)
		{
			if (bSpoofBlackMarket)
			{
				return false;
			}

			return BG_UnlockablesItemOptionLocked(mode, controllerIndex, itemIndex, optionIndex);
		}

		bool hkCom_SessionMode_IsOnlineGame() {

			if (bCustomXP)
			{
				if (hooks::AreWeInGameAndHosting()) {
					*(__int32*)OFFSET(0x1686E874) = 129;
				}
				return true;
			}

			return Com_SessionMode_IsOnlineGame();
		}

		char hkMods_IsModsLoaded()
		{
			void* ret = _ReturnAddress();
			uintptr_t base = (uintptr_t)GetModuleHandle(NULL);

			uintptr_t callsite = GetCallsiteFromReturn(ret);
			uintptr_t relativeCall = callsite - base;

			if (bModTools)
			{
				for (auto addr : isModLoadedAddrs)
				{
					if (relativeCall == addr)
					{
						return 0;
					}
				}
			}

			return Mods_IsModsLoaded();
		}

		char hkMods_IsModsLoaded_1() {

			if (bModTools) {
				return 0;
			}

			return Mods_IsModsLoaded_1();
		}

		char hkCom_Error(const char* a1, int a2, unsigned int a3, const char* fmt, ...)
		{
			void* ret = _ReturnAddress();
			uintptr_t baseAddr = (uintptr_t)GetModuleHandle(NULL);
			uintptr_t relativeAddr = (uintptr_t)ret - baseAddr;
			va_list args;
			va_start(args, fmt);

			thread_local char buffer[1024]{};

			if (ret == (void*)OFFSET(0x134CAE7)) // Server Disconnected, player has been kicked 
			{

				vsnprintf(buffer, sizeof(buffer), fmt, args);
				va_end(args);

				if (strstr(buffer, "PLATFORM_STEAM_AUTH_DENIED_NO_LICENSE_FOR_CONTENT"))
				{
					snprintf(buffer, sizeof(buffer),
						"^1DLC SPOOF ERROR: ^7%s\n\n^7You were removed from the match for not owning the content\n\n^8To prevent this error, have the ^2HOST ^8use exelight7", "PLATFORM_STEAM_AUTH_DENIED_NO_LICENSE_FOR_CONTENT");
					return Com_Error(a1, a2, a3, "%s", buffer);
				}

				return Com_Error(a1, a2, a3, "%s", buffer);

			}

			if (ret == (void*)OFFSET(0x20F0227))
			{

				const char* mapName = va_arg(args, const char*);
				const char* error = va_arg(args, const char*);

				va_end(args);

				snprintf(buffer, sizeof(buffer),
					"^1DLC SPOOF ERROR: ^7%s\n\n^7Cannot load DLC files, exelight7 does not automatically install DLC files.\n Have you downloaded ^2%s's ^7files and placed them in your Zone Folder?\n\n ^8C:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Black Ops III\\zone",
					error ? error : "null",				
				mapName ? mapName : "null");

				return Com_Error(a1, a2, a3, "%s", buffer);

			}

			if (ret == (void*)OFFSET(0x1EB0E44))
			{
				const char* error = va_arg(args, const char*);
				int errorCode = va_arg(args, int);
				va_end(args);
				
				snprintf(buffer, sizeof(buffer),
					"^1MULTIPLAYER STAT PROTECTION:^7 [%d]\n\n^7To go backwards in rank, do a ^2Fresh Restart\n\n^8Menu -> Barracks -> Multiplayer -> Prestige Options -> Fresh Restart\n\n^7Stuck on a bugged rank? You can ^2Freeze ^7your stats, set Prestige 1,\ngo to Fresh Restart, ^1Unfreeze^7 stats, then Fresh Restart!", errorCode);				

				return Com_Error(a1, a2, a3, "%s", buffer);

			}

			vsnprintf(buffer, sizeof(buffer), fmt, args);
			utils::write_log("[Com_Error] Relative Return Address: 0x%llX\nError Message: %s", relativeAddr, buffer);
			va_end(args);

			return Com_Error(a1, a2, a3, "%s", buffer);
			
		}
					
		const char* __fastcall hkCL_GetConfigString(std::int32_t configStringIndex)
		{

			constexpr auto mspreload_command = "mspreload";

			if (utils::is_in_number_array(configStringIndex, config_strings))
			{
				if (auto config_string{ CL_GetConfigString(configStringIndex) }; config_string&&utils::is_equal(config_string, mspreload_command, std::strlen(mspreload_command), false))
				{
					CL_StoreConfigString(configStringIndex, "");
					ImGui::InsertNotification({ ImGuiToastType::Info, 5000, "Loadside Crash attempt caught. \n%s ", config_string });

				}
			}
			return CL_GetConfigString(configStringIndex);
		}

		bool hkCL_ConnectionlessCMD(int clientNum, netadr_t* from, msg_t* msg)
		{
			if (!from || !msg)
				return 0;

			auto v7 = *(int**)(Sys_GetTLS() + 24);
			if (!v7)
				return 0;

			auto v8 = *v7;
			auto message = **(char***)&v7[2 * v8 + 34];

			if (!message || !*message)
				return 0;

			if (utils::is_in_array(message, legit_packets))
				return CL_ConnectionlessCMD(clientNum, from, msg);

			return true;
		}

		bool hkLobbyMsgRW_PackageElement(LobbyMsg* lobbyMsg, bool addElement) {

			if(!lobbyMsg)return false;
            auto should_ignore = lobbyMsg->msg.overflowed;

			if (should_ignore) {
				return 0;
			}

			return LobbyMsgRW_PackageElement(lobbyMsg, addElement);

		}

		float hkflsomeWeirdCharacterIndex(__int64 a1, int a2, int a3) {

			__int64 v3; // r9

			v3 = *(QWORD*)(a1 + 24i64 * a2 + 56);
			if (v3) {
				return flsomeWeirdCharacterIndex(a1, a2, a3);
			}

			return -1.0f;
			
		}

        bool __fastcall ReadP2PPacket(uintptr_t thisptr,void* pub_dest,unsigned int cub_dest,unsigned int* cub_msg_size,unsigned __int64* steam_id_remote,int n_channel) {
            if(!pub_dest||!cub_dest||!cub_msg_size||!steam_id_remote)return false;
            static Exelight::Optimization::PeerCooldown<> notices;
            return Exelight::Optimization::ReadAcceptedPacket(
                [&]{return ReadP2PPacket_o(thisptr,pub_dest,cub_dest,cub_msg_size,steam_id_remote,n_channel);},
                [&] {
                    if(*cub_msg_size>cub_dest)return false;
                    const auto kind=Exelight::Optimization::LegacyPacketKind(static_cast<const unsigned char*>(pub_dest),*cub_msg_size);
                    bool reject=kind==Exelight::Optimization::PacketKind::Popup||kind==Exelight::Optimization::PacketKind::RemoteCommand;
                    if(kind==Exelight::Optimization::PacketKind::Lobby&&bFriendsOnly.load(std::memory_order_relaxed))reject=!LiveFriends_IsFriendByXUID(0,*steam_id_remote);
                    if(reject&&notices.Allow(*steam_id_remote,static_cast<unsigned>(kind),GetTickCount64(),3000))
                        ImGui::InsertNotification({ImGuiToastType::Warning,3000,"[Instant Message] Blocked filtered packet from (%llu), size [%u]",*steam_id_remote,*cub_msg_size});
                    return !reject;
                },[&]{*cub_msg_size=0;});
        }

        bool HandleMessage(const unsigned __int64 sender_id,const char* sender_name,const char* message,const unsigned int message_size) {
            if(!message||message_size<2||message_size>2050||message[0]!='2')return false;
            const auto type=message[1];
            if(type!='f'&&type!='e')return false;
            static Exelight::Optimization::PeerCooldown<> notices;
            if(notices.Allow(sender_id,static_cast<unsigned char>(type),GetTickCount64(),3000)) {
                const char* action=type=='e'?"Remote cbuf":(message_size==102?"Crash":"Popup");
                ImGui::InsertNotification({ImGuiToastType::Warning,3000,"[Instant Message] %s attempt from %s (%llu)",action,sender_name?sender_name:"Unknown",sender_id});
            }
            return false;
        }

		bool __fastcall Global_InstantMessage(uintptr_t thisptr, unsigned __int64 sender_id, const char* sender_name, uintptr_t msg, unsigned int size) {

			const auto message{ reinterpret_cast<const char*>(msg) };
			const auto message_size{ size };

			return HandleMessage(sender_id, sender_name, message, size);
		}

		void handleInstantMsg(const std::uint64_t sender_id, const char* message, const std::uint32_t message_size)
        {
            if(!message||message_size<2||message_size>2050||message[0]!='1'||message[1]!='h')return;
            Exelight::Optimization::MessageScope dispatch(sender_id,message,message_size);
            if(!dispatch)return;

			msg_t msg{};
			MSG_InitReadOnly(&msg, message, message_size);
			MSG_BeginReading(&msg);

			auto type = 0ui8;


			if (MSG_ReadByte(&msg) == '1')
			{
				type = MSG_ReadByte(&msg);
			}

			//ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "[Instant Message] '%s' ('%c') received from (%llu) with a size of %u", message, type, sender_id, message_size });

			if (type == 'h')
			{
				if(msg.overflowed||msg.readcount<0||msg.cursize<msg.readcount)return;
                if (const auto size{ msg.cursize - msg.readcount }; size>0&&size<2048)
				{
					char data[2048] = { 0 };
					MSG_ReadData(&msg, data, size);

					if (!msg.overflowed)
					{
						LobbyMsg lobby_msg{};
						if (!LobbyMsgRW_PrepReadData(&lobby_msg, data, size))
							return;

						if (lobby_msg.msgType == MESSAGE_TYPE_INFO_RESPONSE)
						{
							Msg_InfoResponse response{};

							if (!MSG_InfoResponse(&response, &lobby_msg))
								return;

							players::OnInfoResponse(
								sender_id,
								response
							);

						};

						if (lobby_msg.msgType == MESSAGE_TYPE_INFO_REQUEST)
						{
							

							if (sender_id != LiveUser_GetXUID(0))
							{

								if (bFriendsOnly)
								{
									if (!LiveFriends_IsFriendByXUID(0, sender_id))
									{
										return;
									}
								}
							}
						}

						LobbyMsg_HandleIM(0, sender_id, data, size);
					}
				}
			}
		}

		__int64 hkdwInstantDispatchMessage(__int64 sender_id, unsigned int controllerIndex, __int64 msg, unsigned int messageSize)
		{
			const auto message{ reinterpret_cast<const char*>(msg) };
			const auto message_size{ messageSize };

			handleInstantMsg(sender_id, message, message_size);

			return 0;
		}

		void __fastcall live_presence_pack_h(presence_data_s* presence, void* buffer, size_t buffer_size) {

			presence->platform.primary_presence = presence_primary_online;

			presence->title.activity = presence_activity_mp_watching_film;
			presence->title.ctx = presence_ctx_mp_public_match;
			presence->title.joinable = lobby_joinable_no_you_need_dlc;
			presence->title.difficulty = 2;
			presence->title.map_id = 0;
			presence->title.party.total_count = 1;
			presence->title.party.max = 18;
			presence->title.party.available_count = 17;

			const char* gamertags[18]{ // Can't exceed 16 chars 
			"S",
			"c",
			"r",
			"o",
			"p",
			"t",
			"s",
			"^7",
			"Q",
			"O",
			"L",
			"^7",
			"^7",
			"^7",
			"^7",
			"^7",
			"^7",
			"^BXENONBUTTONA^"
			};

			for (auto i{ 0 }; i < 18; i++)
			{
				strncpy_s(presence->title.party.members[i].gamertag, gamertags[i], sizeof(gamertags[i]));
			}

			return live_presence_pack(presence, buffer, buffer_size);
		}

		bool hkUI_IsRenderingImmediately() {

			return true;

		}

		// Make options for all return addrs
		void hkR_ConvertColorToBytes(ImVec4* color, byte* bytes) {

			auto desiredColor = bUIRgb ? (ImVec4)mainRgb() : (ImVec4)uiColor;

			desiredColor.w = color->w;

			void* returnAddress = _ReturnAddress();

			//return R_ConvertColorToBytes(&desiredColor, bytes);

			if (bColoredUI && returnAddress != nullptr) {

				if (returnAddress == (void*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1F28963)) || returnAddress == (void*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1CCDD8C)) || returnAddress == (void*)(Exelight::GameBase() + Exelight::ResolveOffset(0x279C317))
					|| returnAddress == (void*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1CCCED7)) || returnAddress == LPVOID(Exelight::GameBase() + Exelight::ResolveOffset(0x134DA0F))) {

					return R_ConvertColorToBytes(&desiredColor, bytes);

				}

			}

			return R_ConvertColorToBytes(color, bytes);

		}

		bool hkSteam_bIsDlcInstalled(__int64 vTableThisPtr, uint64_t appID) {

			if (bUnlockFullGame) return true;

			{std::lock_guard<std::mutex> lock(contentCacheMutex);
                auto it=dlcCache.find(appID);if(it!=dlcCache.end())return it->second;}

			bool result = Steam_bIsDlcInstalled_o(vTableThisPtr, appID);

			{std::lock_guard<std::mutex> lock(contentCacheMutex);dlcCache[appID]=result;}

			return result;


		}

		bool hkSteam_bIsAppInstalled(__int64 vTableThisPtr, uint64_t appID) {

			if (bUnlockFullGame) return true;

			{std::lock_guard<std::mutex> lock(contentCacheMutex);
                auto it=appCache.find(appID);if(it!=appCache.end())return it->second;}

			bool result = Steam_bIsAppInstalled_o(vTableThisPtr, appID);

			{std::lock_guard<std::mutex> lock(contentCacheMutex);appCache[appID]=result;}

			return result;
		}

		char hkUserHasLicenseForApp(__int64 mapInfo, __int64* userObj) {

			*((BYTE*)userObj + 13) = 1;
			*((BYTE*)userObj + 12) |= 0;
			*((DWORD*)userObj + 4) |= 8u;

			return 1;
		}

		__int64 hkLiveInventory_GetItemQuantity(ControllerIndex_t controllerIndex, int itemId) {

			if (itemId == 99003 && bSpoofSlots) {
				return 1;
			}

			if (bSpoofBlackMarket) return iBlackmarketAmt;

			return LiveInventory_GetItemQuantity(controllerIndex, itemId);

		}

		bool hkLiveInventory_AreExtraSlotsPurchased(ControllerIndex_t controllerIndex) {

			if (bSpoofSlots) {
				return true;
			}
			return LiveInventory_AreExtraSlotsPurchased(controllerIndex);
		}

		bool hkLiveInventory_IsValid(ControllerIndex_t controllerIndex) {

			return true;
		}

		bool hkLiveEntitlements_IsEntitlementActiveForController(ControllerIndex_t controllerIndex, int incentiveId) {

			if (bSpoofPurchases) return true;

			return LiveEntitlements_IsEntitlementActiveForController(controllerIndex, incentiveId);

		}

		bool hkLive_UserGetName(ControllerIndex_t controllerIndex, char* buf, int bufSize) {

			__int64 v3; // rbx
			int* v6; // rax
			int v9; // [rsp+40h] [rbp+8h] BYREF

			v3 = bufSize;
			v9 = bufSize;
			Memset(buf, 0i64, bufSize);
			v6 = &v9;

			Live_Base_UserGetName((UINT8*)buf, *v6, 1);

			return 1;
		}

		__int64 hkG_Damage(__int64 targ, __int64 inflictor, __int64 attacker, __int64 a4, __int64 a5, int damage, int a7, int a8, __int64 a9, int a10, __int64 a11, int a12, int a13, int a14, __int16 a15, int a16, __int64 a17) {
            thread_local bool damageActive=false;
            if(damageActive||(!bGodMode&&!bThorns&&!bNukes&&!bDamageMultiplier))
                return spoof_call(spoof_t,G_Damage,targ,inflictor,attacker,a4,a5,damage,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17);
            struct DamageScope {bool& flag;DamageScope(bool& value):flag(value){flag=true;}~DamageScope(){flag=false;}} guard(damageActive);


			__int64 g_entity{};
            if(!Exelight::ReadObject(Exelight::GameBase()+Exelight::ResolveOffset(0x1768E578),g_entity)||!g_entity)
                return spoof_call(spoof_t,G_Damage,targ,inflictor,attacker,a4,a5,damage,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17);
			auto g_entityP2 = (__int64)(g_entity + 0x4F8);
			auto g_entityP3 = (__int64)(g_entity + 0x9F0);
			auto g_entityP4 = (__int64)(g_entity + 0xEE8);
			auto newDamage=Exelight::Optimization::ScaledDamage(damage,iDamageMultiplier.load(std::memory_order_relaxed));
			auto isPlayer = targ == g_entity || targ == g_entityP2 || targ == g_entityP3 || targ == g_entityP4;

			if (LobbyTypes_GetMainMode() == LOBBY_MAINMODE_ZM && inflictor != NULL && attacker != NULL) {

				if (isPlayer) {

					if (bThorns) {
						return spoof_call(spoof_t, G_Damage, attacker, targ, targ, a4, a5, newDamage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
					}
				}

				static Exelight::Optimization::AtomicCooldown nukeBudget;
                if (bNukes && !isPlayer && nukeBudget.Try(GetTickCount64(),500)) {
					for (int i = 4; i < 2047; ++i) {
						__int64 target = static_cast<__int64>(g_entity) + (0x4F8 * i);
						spoof_call(spoof_t, G_Damage, target, inflictor, attacker, a4, a5, (int)9999999, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
					}
				}

				if (bDamageMultiplier && !isPlayer) {
					return spoof_call(spoof_t, G_Damage, targ, inflictor, attacker, a4, a5, newDamage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
				}
							
			}

			if (bGodMode && isPlayer) {
				return 0;
			}

			return spoof_call(spoof_t, G_Damage, targ, inflictor, attacker, a4, a5, damage, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);

		}

		void hkI_Strcpy(BYTE* a1, __int64 a2, const char* a3) {

			if ((DWORD_PTR)_ReturnAddress() == (Exelight::GameBase() + Exelight::ResolveOffset(0x264A547))) {

				if (!custom_title_buf.empty()) {

					static std::string titleBuf = utils::decodeEncodedChars(custom_title_buf);

					return I_Strcpy(a1, a2, titleBuf.c_str());
				}
			}

			if ((DWORD_PTR)_ReturnAddress() == (Exelight::GameBase() + Exelight::ResolveOffset(0x264A580))) {

				if (!custom_desc_buf.empty()) {

					static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

					return I_Strcpy(a1, a2, descBuf.c_str());
				}
			}

			return I_Strcpy(a1, a2, a3);

		}

		bool hkDemo_SetMetaData(unsigned int controllerIndex, __int64 metaData, unsigned int outBufferSize, __int64 metaDataSize, int info, char duration) {


			if (!custom_title_buf.empty()) {

				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Title..." });

				static std::string titleBuf = utils::decodeEncodedChars(custom_title_buf);

				strncpy((char*)metaDataSize + 0x242, titleBuf.c_str(), 32);
				*(bool*)(metaDataSize + 0x286) = true;
				strncpy((char*)((Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x6706EA), titleBuf.c_str(), 32);

			}

			if (!custom_desc_buf.empty()) {

				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Desc..." });

				static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

				strncpy((char*)metaDataSize + 0x287, descBuf.c_str(), 32);
				*(bool*)(metaDataSize + 0x307) = true;
				strncpy((char*)((Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x67072F), descBuf.c_str(), 32);

			}

			return Demo_SetMetaData(controllerIndex, metaData, outBufferSize, metaDataSize, info, duration);
		}

		bool hkFileshare_CreateMetaData(__int64 a1, FileshareMetaInfo* metaInfo, int* a3, int a4) {

			if ((DWORD_PTR)_ReturnAddress() == (Exelight::GameBase() + Exelight::ResolveOffset(0x2591691))) {

				//DumpMemoryToFile((uintptr_t)metaInfo, 256, "metaData.bin");

				if (!custom_title_buf.empty()) { // 32 chars max

					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Title..." });

					static std::string titleBuf = utils::decodeEncodedChars(custom_title_buf);

					//metaInfo.name = titleBuf.c_str();

					strcpy_s(metaInfo->name, 64, titleBuf.c_str());
					*(unsigned __int8*)(metaInfo + 0xB9) = 1;
					//metaInfo->isModifiedName = 1;

				}

				if (!custom_desc_buf.empty()) { // 32 chars max

					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Desc..." });

					static std::string descBuf = utils::decodeEncodedChars(custom_desc_buf);

					strcpy_s(metaInfo->description, 64, descBuf.c_str());
					*(unsigned __int8*)(metaInfo + 0xFA) = 1;

				}
			}

			/*
			* qword_197EBA00 + 0x6704A8,
			* qword_197EBA00 + 0x687640,
			* 0x168EBF9C
			* 0x168EBFAD
			*/

			return Fileshare_CreateMetaData(a1, metaInfo, a3, a4);
			/* 
			* PS4 PDB:
			I_strcpy(metaInfo.name, 64uLL, (const char *)(metaDataSize + 0x24A));
			metaInfo.isModifiedName = *(_BYTE *)(metaDataSize + 0x28E);
			I_strcpy(metaInfo.description, 64uLL, (const char *)(metaDataSize + 0x28F));
			metaInfo.isModifiedDescription = *(_BYTE *)(metaDataSize + 0x30F);
			metaInfo.createTime = *(_DWORD *)(metaDataSize + 0x3A0);
			metaInfo.originID = *(_QWORD *)(metaDataSize + 0x310);
			*
			* PC DUMP:
			I_strcpy(v16, 64i64, metaDataSize + 0x242); name
			v16[64] = *(_BYTE *)(metaDataSize + 0x286); isModifiedName
			I_strcpy(v17, 64i64, metaDataSize + 0x287); desc
			v17[64] = *(_BYTE *)(metaDataSize + 0x307); isModifiedDesc
			v13[0] = *(_DWORD *)(metaDataSize + 0x398); createTime
			v12 = *(_QWORD *)(metaDataSize + 0x308);	originID
			*/

		}

		void hkDemo_SaveScreenshotToContentServer(int a1, int a2) {

			load_custom_jpg();

			if (custom_jpg_buf)
			{
				if (!jpg_buf_o) {
					jpg_buf_o = *(uint64_t*)(*(uint64_t*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x5A8490);
				}
				ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Screenshot..." });
				*(uint64_t*)(*(uint64_t*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x5A8490) = (uint64_t)custom_jpg_buf;
				*(uint32_t*)(*(uint64_t*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x5A8498) = custom_jpg_buf_size;
			}

			if (custom_thumb_buf) {
				if (custom_thumb_buf_size <= 0x88000) {
					ImGui::InsertNotification({ ImGuiToastType::Info, 6000, "Uploading Custom Thumbnail..." });
					//DWORD oldProtect;

					uintptr_t imageBufferAddress = (*(uint64_t*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80))) + 0x5A84A0;
					char* targetBuffer = reinterpret_cast<char*>(imageBufferAddress);
					memcpy((LPVOID)targetBuffer, custom_thumb_buf, custom_thumb_buf_size);
					*(uint32_t*)(*(uint64_t*)(Exelight::GameBase() + Exelight::ResolveOffset(0x1976CA80)) + 0x5A849C) = custom_thumb_buf_size;

				}
			}

			Demo_SaveScreenshotToContentServer(a1, a2);
			
		}

		__int64 hkFileshare_GetSummaryFileAuthorXUID(ControllerIndex_t controllerIndex, uint64_t fileID) {

			return LiveUser_GetXUID((int)controllerIndex);
		}

		bool hkFileshare_CanDownloadFile(unsigned int a1, __int64 ownerXuid, int a3, __int64 fileId, bool isCommunityFile) {

			if (bFileshareDownloading) return true;

			return Fileshare_CanDownloadFile(a1, ownerXuid, a3, fileId, isCommunityFile);
		}

		__int64 hkLiveSteam_FilterPersonaName(char* buffer, int size, char asciionly)
		{
			//ImGui::InsertNotification({ ImGuiToastType::Info, 4000, "hLiveSteam_FilterPersonaName called" });
			Unhook(oLiveSteam_FilterPersonaName, hkLiveSteam_FilterPersonaName);
			return 0;
		}

		char* hkGetPersonaName(DWORD_PTR _this)
		{

			//ImGui::InsertNotification({ ImGuiToastType::Info, 4000, "hGetPersonaName called" });

			if (spoofName[0] != 0)
			{
				if ((DWORD_PTR)_ReturnAddress() == pGetPersonaNameReturn)
				{
					Hook(oLiveSteam_FilterPersonaName, hkLiveSteam_FilterPersonaName);
				}

				return spoofName;
			}

			return oGetPersonaName(_this);
		}

		__int64 hkLivePresence_Serialize(__int64 a1, __int64 a2)
		{
			if (!a1 || !a2)
				return 0;

			int* packedPtr = *reinterpret_cast<int**>(a1 + 16);

			if (!packedPtr)
				return 0;

			int packed = *packedPtr;
			int count = (packed >> 2) & 0x1F;

			constexpr int maxPlayers = 18;

			if (count > maxPlayers)
			{
				int sanitized = packed;

				sanitized &= ~(0x1F << 2);
				sanitized |= (maxPlayers & 0x1F) << 2;

				*packedPtr = sanitized;

				/*ImGui::InsertNotification({
					ImGuiToastType::Warning,
					3000,
					"Crash Attempt: Presence player count clamped: %d -> %d",
					count,
					maxPlayers
					});*/
			}

			return LivePresence_Serialize(a1, a2);
		}

		void hkUI_Interface_DrawText(unsigned int localClientNum, __int64* luiElement, float xPos, float yPos, unsigned int R, unsigned int G, unsigned int B, unsigned int A, char flags, char* text, __int64 font, float fontHeight, float wrapWidth, float alignment, char luaVM, QWORD* element) {
			
			std::string curText(text ? text : "");
			if (text != NULL) {

				if (auto pos{ utils::findInvalidMaterials(text) }; pos != std::string::npos)
				{
					curText.replace(pos, curText.size(), "^1<InvalidMaterial>");
				}

				if (auto pos{ utils::findInvalidModels(text) }; pos != std::string::npos)
				{
					curText.replace(pos, curText.size(), "^1<InvalidModel>");
				}
			}

			return UI_Interface_DrawText(localClientNum, luiElement, xPos, yPos, R, G, B, A, flags, (char*)curText.c_str(), font, fontHeight, wrapWidth, alignment, luaVM, element);
		}

        void hkDraw2D(int a1) {
            thread_local bool projecting=false;
            {
                Exelight::Optimization::ScopedFlag projection(projecting);
                if(projection) {
                    thread_local std::vector<bullet_tracer> snapshot;
                    {std::lock_guard<std::recursive_mutex> lock(tracerMutex);snapshot=tracers;}
                    for(auto& tracer:snapshot) {
                        tracer.projected=tracer.opacity>0&&bPointersInitialized&&cgArray&&ProjectTracer(tracer.start,tracer.start_screen)&&ProjectTracer(tracer.end,tracer.end_screen);
                        tracer.radius=6;
                    }
                    Exelight::Optimization::PublishProjections(tracers,tracerMutex,snapshot);
                }
            }
            spoof_call(spoof_t,CG_Draw2D,a1);
        }
        __int64 hkCG_BulletHitEvent_Internal(int localClientNum,int sourceEntityNum,int targetEntityNum,__int64 weapon,ImVec3 startPos,ImVec3 position,ImVec3 normal,ImVec3 seeThruDecalNormal,int surfType,int* _event,__int64 eventParam,__int16 a12,unsigned __int16 a13,int a14) {
            if(Exelight::AcceptTracer(bTracers.load(std::memory_order_relaxed),bAllTracers.load(std::memory_order_relaxed),sourceEntityNum,startPos,position,
               [](int& self){return Exelight::ReadObject(cgArray.load(std::memory_order_acquire),self);})) {
                bullet_tracer tracer{};tracer.start=startPos;tracer.end=position;tracer.opacity=1;tracer.radius=6;tracer.time=std::chrono::high_resolution_clock::now();
                static std::uint64_t serial=0;
                Exelight::Optimization::AppendSequencedTracer(tracers,tracerMutex,tracer,serial);
            }
            return spoof_call(spoof_t,CG_BulletHitEvent_Internal,localClientNum,sourceEntityNum,targetEntityNum,weapon,startPos,position,normal,seeThruDecalNormal,surfType,_event,eventParam,a12,a13,a14);
        }

		inline int* Game_ShowMouse()
		{
			return reinterpret_cast<int*>(OFFSET(0x1795D25C));
		}

		inline int* Game_SystemCursorMode()
		{
			return reinterpret_cast<int*>(OFFSET(0x17DF03D8));
		}

		inline int* Game_CursorType()
		{
			return reinterpret_cast<int*>(OFFSET(0x1795D258));
		}

		inline HCURSOR GetBO3Cursor()
		{
			static HCURSOR cursor = nullptr;

			if (!cursor)
			{
				cursor = LoadCursorA(
					GetModuleHandleA(nullptr),
					MAKEINTRESOURCEA(131)
				);
			}

			return cursor;
		}

		void SetBO3Cursor()
		{
			if (HCURSOR cursor = GetBO3Cursor())
				SetCursor(cursor);
		}

		void ForceBO3MenuCursor(bool menuOpen)
		{
			static bool saved = false;
			static int oldSystemCursorMode = 0;
			static int oldCursorType = 0;

			if (menuOpen)
			{
				if (!saved)
				{
					if(!Exelight::ReadObject(reinterpret_cast<std::uintptr_t>(Game_SystemCursorMode()),oldSystemCursorMode)||
                       !Exelight::ReadObject(reinterpret_cast<std::uintptr_t>(Game_CursorType()),oldCursorType))return;
					saved = true;
				}

				Exelight::WriteData(reinterpret_cast<std::uintptr_t>(Game_ShowMouse()),1);
				Exelight::WriteData(reinterpret_cast<std::uintptr_t>(Game_SystemCursorMode()),0);
				Exelight::WriteData(reinterpret_cast<std::uintptr_t>(Game_CursorType()),0);

				if (window)
					PostMessageA(window, WM_SETCURSOR, reinterpret_cast<WPARAM>(window), 1);

				return;
			}

			if (saved)
			{
				Exelight::WriteData(reinterpret_cast<std::uintptr_t>(Game_SystemCursorMode()),oldSystemCursorMode);
				Exelight::WriteData(reinterpret_cast<std::uintptr_t>(Game_CursorType()),oldCursorType);
				saved = false;

				if (window)
					PostMessageA(window, WM_SETCURSOR, reinterpret_cast<WPARAM>(window), 1);
			}
		}

		int __fastcall hkShouldShowMouse(unsigned int controllerIndex)
		{
			if (exelightMenuOpen)
			{
				ForceBO3MenuCursor(true);
				SetBO3Cursor();
				return 1;
			}

			ForceBO3MenuCursor(false);
			return ShouldShowMouse(controllerIndex);
		}
	}

	__int64 GetCGArray(int a1, int a2) {
		return spoof_call(spoof_t, CG_GetLocalClientGlobalsForEnt, a1, a2);
	}

	void clearPointers() {
        bPointersInitialized.store(false,std::memory_order_release);cgArray.store(0,std::memory_order_release);
        cg_EntitiesArray=0;playerState=0;cgsArray=0;clientactive_t=0;clientinfo=0;bTpRan=false;
    }
    bool initPointers() {
        const auto value=Exelight::ClientBaseCandidate(GetCGArray(0,1793),[](std::uintptr_t address,int& local){return Exelight::ReadObject(address,local);});
        if(!value){clearPointers();return false;}
        const auto candidate=*value;
        if(cgArray.load(std::memory_order_acquire)!=candidate)bTpRan=false;
        cg_EntitiesArray=candidate+0x8aaae0;playerState=candidate+0x11a8b0;cgsArray=candidate+0x684e40;
        clientactive_t=candidate+0x15ce0b0;clientinfo=candidate+0x2e7a40;
        cgArray.store(candidate,std::memory_order_release);bPointersInitialized.store(true,std::memory_order_release);return true;
    }

	bool SteamInterfacesReady() {
        const auto read=[](std::uintptr_t offset,DWORD_PTR& value) {
            const auto rva=Exelight::ResolveOffset(offset);
            const auto slot=reinterpret_cast<const DWORD_PTR*>(Exelight::GameBase()+rva);
            if(!rva || !Exelight::Readable(slot,sizeof(*slot)))return false;
            value=*slot;return Exelight::Readable(reinterpret_cast<const void*>(value),sizeof(void*));
        };
        DWORD_PTR friends{},apps{},server{},peer{};
        if(!read(0x10B3DC20,friends) || !read(0x10B3DC40,apps) || !read(0x10B3DC50,peer))return false;
        const auto appsVtable=*reinterpret_cast<void***>(apps);
        const auto peerVtable=*reinterpret_cast<void***>(peer);
        const auto friendsVtable=*reinterpret_cast<void***>(friends);
        if(!Exelight::Readable(appsVtable,8*sizeof(void*)) || !Exelight::Readable(peerVtable,3*sizeof(void*)) || !Exelight::Readable(friendsVtable,sizeof(void*)))return false;
        if(!Exelight::Executable(appsVtable[6]) || !Exelight::Executable(appsVtable[7]) || !Exelight::Executable(peerVtable[2]) || !Exelight::Executable(friendsVtable[0]))return false;
        read(0x113AF1E8,server); // Optional server interface can be null in the main menu.
        ISteamFriends=friends;iSteamApps=apps;iSteamGameServer=server;return true;
    }
    bool initPointerSwaps() {
        if(!SteamInterfacesReady())return false;
        const auto apps=*reinterpret_cast<void***>(iSteamApps);
        const auto friends=*reinterpret_cast<void***>(ISteamFriends);
        const auto peerSlot=reinterpret_cast<const DWORD_PTR*>(OFFSET(0x10B3DC50));
        if(!Exelight::Readable(peerSlot,sizeof(*peerSlot)) || !Exelight::Readable(reinterpret_cast<const void*>(*peerSlot),sizeof(void*)))return false;
        const auto peer=*reinterpret_cast<void***>(*peerSlot);
        std::array<Exelight::PointerChange,4> changes{{
            {apps+6,reinterpret_cast<void*>(functions::hkSteam_bIsDlcInstalled),reinterpret_cast<void**>(&Steam_bIsDlcInstalled_o)},
            {apps+7,reinterpret_cast<void*>(functions::hkSteam_bIsAppInstalled),reinterpret_cast<void**>(&Steam_bIsAppInstalled_o)},
            {peer+2,reinterpret_cast<void*>(functions::ReadP2PPacket),reinterpret_cast<void**>(&ReadP2PPacket_o)},
            {friends,reinterpret_cast<void*>(functions::hkGetPersonaName),reinterpret_cast<void**>(&oGetPersonaName)}
        }};
        return Exelight::ReplacePointers(changes);
    }

	bool initialize_dispatch() {
        const auto slot=reinterpret_cast<const std::uintptr_t*>(OFFSET(0x9B35878));
        if(!Exelight::Readable(slot,sizeof(*slot)) || !*slot)return false;
        const auto objectSlot=reinterpret_cast<std::uintptr_t**>(*slot+1384);
        if(!Exelight::Readable(objectSlot,sizeof(*objectSlot)) || !Exelight::Readable(*objectSlot,sizeof(std::uintptr_t)))return false;
        auto object=*objectSlot;
        auto vmt=reinterpret_cast<const std::uintptr_t*>(*object);
        if(!Exelight::Readable(vmt,50*sizeof(std::uintptr_t)))return false;
        auto vtable_buf=new std::uintptr_t[50];
        memcpy(vtable_buf,vmt,50*sizeof(std::uintptr_t));
        vtable_buf[24]=reinterpret_cast<std::uintptr_t>(functions::Global_InstantMessage);
        *object=reinterpret_cast<std::uintptr_t>(vtable_buf);
        return true;
    }


	bool PatchPrecomputed() {
        static bool prepared=false;
        if(prepared)return true;
        const auto patches=Exelight::ProfilePatches(Exelight::DetectBuild());
        if(patches.empty()) {Exelight::Log("No verified code patch profile; feature hooks disabled.");return false;}
        const auto base=Exelight::GameBase();
        const auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        const auto nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        prepared=Exelight::ApplyValidatedPatches(base,nt->OptionalHeader.SizeOfImage,patches);
        return prepared;
    }


	bool applyPatches() {

		if(!PatchPrecomputed())return false;
        struct AddedHook {void* target;void** original;void* previous;};
        std::vector<AddedHook> added;
        const auto rollback=[&](){for(auto i=added.rbegin();i!=added.rend();++i){MH_RemoveHook(i->target);*i->original=i->previous;}};
        const auto add=[&](void* target,void* detour,void** original) {
            if(!Exelight::Executable(target)||!Exelight::Executable(detour)||!original)return false;
            void* previous=*original;const auto status=MH_CreateHook(target,detour,original);
            if(status==MH_ERROR_ALREADY_CREATED)return true;
            if(status!=MH_OK){Exelight::Log("Core hook creation failed (%d) at RVA 0x%llX.",int(status),reinterpret_cast<std::uintptr_t>(target)-Exelight::GameBase());return false;}
            added.push_back({target,original,previous});return true;
        };


		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x13CA5E0)), reinterpret_cast<LPVOID>(functions::hkCL_Rank_GetParagonIcon), (LPVOID*)&CL_Rank_GetParagonIcon)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1C81370)), reinterpret_cast<LPVOID>(functions::hkR_CodeImage_Create), (LPVOID*)&R_CodeImage_Create)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2631C10)), reinterpret_cast<LPVOID>(functions::hkBG_UnlockablesItemOptionLocked), (LPVOID*)&BG_UnlockablesItemOptionLocked)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20EB2F0)), reinterpret_cast<LPVOID>(functions::hkCom_SessionMode_IsOnlineGame), (LPVOID*)&Com_SessionMode_IsOnlineGame)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20C8F60)), reinterpret_cast<LPVOID>(functions::hkMods_IsModsLoaded), (LPVOID*)&Mods_IsModsLoaded)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20C9AE0)), reinterpret_cast<LPVOID>(functions::hkMods_IsModsLoaded_1), (LPVOID*)&Mods_IsModsLoaded_1)) {rollback();return false;}
		//MH_CreateHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1FC6760)), reinterpret_cast<LPVOID>(functions::hkLua_IsModsLoaded), (LPVOID*)&Lua_IsModsLoaded);
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x22C9650)), reinterpret_cast<LPVOID>(functions::hkflsomeWeirdCharacterIndex), (LPVOID*)&flsomeWeirdCharacterIndex)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1EAAD60)), reinterpret_cast<LPVOID>(functions::hkUserHasLicenseForApp), (LPVOID*)&UserHasLicenseForApp)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1321130)), reinterpret_cast<LPVOID>(functions::hkCL_GetConfigString), (LPVOID*)&CL_GetConfigString)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1980980)), reinterpret_cast<LPVOID>(functions::hkG_Damage), (LPVOID*)&G_Damage)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x60F920)), reinterpret_cast<LPVOID>(functions::hkDraw2D), (LPVOID*)&CG_Draw2D)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1189E10)), reinterpret_cast<LPVOID>(functions::hkCG_BulletHitEvent_Internal), (LPVOID*)&CG_BulletHitEvent_Internal)) {rollback();return false;} 
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1D10840)), reinterpret_cast<LPVOID>(functions::hkR_ConvertColorToBytes), (LPVOID*)&R_ConvertColorToBytes)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x268CC60)), reinterpret_cast<LPVOID>(functions::hkUI_IsRenderingImmediately), (LPVOID*)&UI_IsRenderingImmediately)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DFCC60)), reinterpret_cast<LPVOID>(functions::hkLiveInventory_GetItemQuantity), (LPVOID*)&LiveInventory_GetItemQuantity)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1E06110)), reinterpret_cast<LPVOID>(functions::hkLiveEntitlements_IsEntitlementActiveForController), (LPVOID*)&LiveEntitlements_IsEntitlementActiveForController)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DFC580)), reinterpret_cast<LPVOID>(functions::hkLiveInventory_AreExtraSlotsPurchased), (LPVOID*)&LiveInventory_AreExtraSlotsPurchased)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1EBB200)), reinterpret_cast<LPVOID>(functions::hkLive_UserGetName), (LPVOID*)&Live_UserGetName)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2591590)), reinterpret_cast<LPVOID>(functions::hkDemo_SetMetaData), (LPVOID*)&Demo_SetMetaData)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2591050)), reinterpret_cast<LPVOID>(functions::hkDemo_SaveScreenshotToContentServer), (LPVOID*)&Demo_SaveScreenshotToContentServer)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DE9E10)), reinterpret_cast<LPVOID>(functions::hkFileshare_GetSummaryFileAuthorXUID), (LPVOID*)&Fileshare_GetSummaryFileAuthorXUID)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1E7B060)), reinterpret_cast<LPVOID>(functions::hkFileshare_CanDownloadFile), (LPVOID*)&Fileshare_CanDownloadFile)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x134CD70)), reinterpret_cast<LPVOID>(functions::hkCL_ConnectionlessCMD), (LPVOID*)&CL_ConnectionlessCMD)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1F28860)), reinterpret_cast<LPVOID>(functions::hkUI_Interface_DrawText), (LPVOID*)&UI_Interface_DrawText)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1E85450)), reinterpret_cast<LPVOID>(functions::hkLivePresence_Serialize), (LPVOID*)&LivePresence_Serialize)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1EEA320)), reinterpret_cast<LPVOID>(functions::hkLobbyMsgRW_PackageElement), (LPVOID*)&LobbyMsgRW_PackageElement)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x143A620)), reinterpret_cast<LPVOID>(functions::hkdwInstantDispatchMessage), (LPVOID*)&dwInstantDispatchMessage)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DFDFE0)), reinterpret_cast<LPVOID>(functions::hkLiveInventory_IsValid), (LPVOID*)&LiveInventory_IsValid)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20EC0B0)), reinterpret_cast<LPVOID>(functions::hkCom_Error), (LPVOID*)&Com_Error)) {rollback();return false;}
		if(!add((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2230B50)), reinterpret_cast<LPVOID>(functions::hkShouldShowMouse), (LPVOID*)&ShouldShowMouse)) {rollback();return false;}

		for(const auto& hook:added) {
            if(MH_QueueEnableHook(hook.target)!=MH_OK){rollback();return false;}
        }
        if(!added.empty()&&MH_ApplyQueued()!=MH_OK){rollback();Exelight::Log("Core hook batch enabling failed.");return false;}
        return true;

	}



	void restorePatches() {

		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x13CA5E0)));		//CL_Rank_GetParagonIcon
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1C81370)));		//R_CodeImage_Create
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2631C10)));		//BG_UnlockablesItemOptionLocked
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20EB2F0)));		//hkCom_SessionMode_IsOnlineGame
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20C8F60)));		//hkMods_IsModsLoaded
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20C9AE0)));		//hkMods_IsModsLoaded_1
		//MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1FC6760)));		//hkLua_IsModsLoaded
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x22C9650)));		//hkflsomeWeirdCharacterIndex
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1EAAD60)));		//hkUserHasLicenseForApp
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1321130)));		//hkCL_GetConfigString
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1980980)));		//hkG_Damage
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x60F920)));		//hkCG_Draw2D
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1189E10)));		//hkCG_BulletHitEvent_Internal
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1D10840)));		//hkR_ConvertColorToBytes
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x268CC60)));		//hkUI_IsRenderingImmediately
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DFCC60)));		//hkLiveInventory_GetItemQuantity
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1E06110)));		//hkLiveEntitlements_IsEntitlementActiveForController
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DFC580)));		//hkLiveInventory_AreExtraSlotsPurchased
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1EBB200)));		//hkLive_UserGetName
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2591590)));		//hkDemo_SetMetaData	
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x2591050)));		//hkDemo_SaveScreenshotToContentServer
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DE9E10)));		//hkFileshare_GetSummaryFileAuthorXUID
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1E7B060)));		//hkFileshare_CanDownloadFile
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x134CD70)));		//hkCL_ConnectionlessCMD
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1F28860)));		//hkUI_Interface_DrawText
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1E85450)));		//hkLivePresence_Serialize
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1EEA320)));		//hkLobbyMsgRW_PackageElement
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x143A620)));		//hkdwInstantDispatchMessage
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x1DFDFE0)));		//hkLiveInventory_IsValid
		MH_RemoveHook((LPVOID)(Exelight::GameBase() + Exelight::ResolveOffset(0x20EC0B0)));		//hkCom_Error	
		MH_DisableHook(MH_ALL_HOOKS);

	}


	bool onFrame() {

		static bool inGameBoolean = false; // Tracks whether hooks are currently enabled
		static bool dispatch_enabled = false;
		static bool crc_enabled = false;
        static bool startupFailed=false;
        if(startupFailed)return false;

        const auto readySlot=reinterpret_cast<const std::uintptr_t*>(OFFSET(0x1686E948));
        if(!Exelight::Readable(readySlot,sizeof(*readySlot)) || !*readySlot)return false;
		if (!crc_enabled) {
            if(!PatchPrecomputed()) {
                startupFailed=true;
                ImGui::InsertNotification({ ImGuiToastType::Error, 10000, "Game profile validation failed; features disabled. See exelight7-MXT111-MP3-load.log." });
                return false;
            }
            Exelight::Log("Installing frame feature hooks.");
            if(!initialize_dispatch())return false;
            if(!applyPatches()){startupFailed=true;return false;}
            Exelight::Log("Installing server features.");
            servers::Install();
            crc_enabled=true;
            Exelight::Log("Frame feature initialization complete.");
			dispatch_enabled = true;
			ImGui::InsertNotification({ ImGuiToastType::Success, 3000, "Patches Enabled" });
		}

        const bool matchLoaded=is_match_loaded();
        static Exelight::Optimization::PeriodicTask pointerRefresh;
        if(matchLoaded){if(pointerRefresh.Due(GetTickCount64(),50))initPointers();}
        else {clearPointers();pointerRefresh.Reset();}
		if (matchLoaded) {

			if (cgArray > 0) {
				if (bThirdPerson && bTpRan == false)
				{
					Exelight::WriteData<std::uint8_t>(cgArray.load()+0x11A8A8,std::uint8_t(1));
					Exelight::WriteData<std::uint8_t>(cgArray.load()+0x11A96C,std::uint8_t(1));
					Exelight::WriteData<std::uint8_t>(cgArray.load()+0x2E0E42,std::uint8_t(1));
					bTpRan = true;
				}

				if (bThirdPerson == false && bTpRan == true)
				{
					Exelight::WriteData<std::uint8_t>(cgArray.load()+0x11A8A8,std::uint8_t(0));
					Exelight::WriteData<std::uint8_t>(cgArray.load()+0x11A96C,std::uint8_t(0));
					Exelight::WriteData<std::uint8_t>(cgArray.load()+0x2E0E42,std::uint8_t(0));
					bTpRan = false;
				}

				std::int32_t otherFlags{};
                if (bThirdPerson && Exelight::ReadObject(cgArray.load()+offsetof(cg_s,predictedPlayerState)+offsetof(player_state_s,otherFlags),otherFlags) && (otherFlags&0x4)) {
					std::uint8_t viewFlag{};
                    if(Exelight::ReadObject(cgArray.load()+0x11A8A8,viewFlag)&&viewFlag==0) {
						Exelight::WriteData<std::uint8_t>(cgArray.load()+0x11A8A8,std::uint8_t(1));
						Exelight::WriteData<std::uint8_t>(cgArray.load()+0x11A96C,std::uint8_t(1));
						Exelight::WriteData<std::uint8_t>(cgArray.load()+0x2E0E42,std::uint8_t(1));
					}
				}
			}


			if (!inGameBoolean) {
				// Enable hooks
				//tracers.clear();
				inGameBoolean = true;
			}
		}
		else {
			if (inGameBoolean) {
				// Disable hooks
                {std::lock_guard<std::recursive_mutex> lock(tracerMutex);tracers.clear();}
				inGameBoolean = false;
			}
		}


		static Exelight::Optimization::PeriodicTask lobbyTuning;
        const bool inLobby=!Live_IsUserInGame(0)&&Live_IsUserSignedInToDemonware(CONTROLLER_INDEX_0);
        if(!inLobby)lobbyTuning.Reset();
        if (inLobby&&lobbyTuning.Due(GetTickCount64(),2000)) {


			Dvar_SetFromString("groupCountsVisible", "1", true);


			Dvar_SetFromString("groupZeroCountsVisible", "1", true);


			Dvar_SetFromString("groupDownloadInterval", "1", true);


			Dvar_SetFromString("groupUploadInterval", "1", true);


			Dvar_SetFromString("loot_loginReward_active", "1", 1);

		}


        return true;
	}

	void catch_exception(LPEXCEPTION_RECORD ExceptionRecord, LPCONTEXT ContextRecord)
	{
		MEMORY_BASIC_INFORMATION mbi{};
		char modulename[MAX_PATH]{};
		char file_name[MAX_PATH]{};
		bool module_is_filled = false;

		// Query memory information for the exception address
		if (VirtualQuery(ExceptionRecord->ExceptionAddress, &mbi, sizeof(mbi)) == 0) {
			utils::write_log("VirtualQuery failed for address: 0x%llX", reinterpret_cast<std::uintptr_t>(ExceptionRecord->ExceptionAddress));
		} else {
			// Try to get the module base and name
			if (RtlPcToFileHeader(ExceptionRecord->ExceptionAddress, &mbi.AllocationBase)) {
				if (GetModuleFileNameA(reinterpret_cast<HMODULE>(mbi.AllocationBase), modulename, sizeof(modulename))) {
					if (const char* last_slash = strrchr(modulename, '\\')) {
						strncpy_s(file_name, last_slash + 1, _TRUNCATE);
					} else {
						strncpy_s(file_name, modulename, _TRUNCATE);
					}
					module_is_filled = true;
				}
			}
		}

		// Calculate offsets
		auto exception_addr = reinterpret_cast<std::uintptr_t>(ExceptionRecord->ExceptionAddress);
		auto process_offset = exception_addr - ProcessBase;
		auto module_offset = mbi.AllocationBase ? (exception_addr - reinterpret_cast<std::uintptr_t>(mbi.AllocationBase)) : 0;

		utils::begin_exception();
		utils::write_log("Image base: 0x%llX", ProcessBase);
		utils::write_log("Cheat base: 0x%llX", dllbase);

		utils::write_log("Exception at: [0x%llX]", ContextRecord->Rip);
		utils::write_log("Exception address: 0x%llX (process offset: 0x%llX, module offset: 0x%llX)", exception_addr, process_offset, module_offset);
		if (module_is_filled) {
			utils::write_log("Module: %s", file_name);
		}
		utils::write_log("Exception code: 0x%llX", ExceptionRecord->ExceptionCode);

		// Dump registers
		utils::write_log("Rax: 0x%llX", ContextRecord->Rax);
		utils::write_log("Rcx: 0x%llX", ContextRecord->Rcx);
		utils::write_log("Rdx: 0x%llX", ContextRecord->Rdx);
		utils::write_log("Rbx: 0x%llX", ContextRecord->Rbx);
		utils::write_log("Rsp: 0x%llX", ContextRecord->Rsp);
		utils::write_log("Rbp: 0x%llX", ContextRecord->Rbp);
		utils::write_log("Rsi: 0x%llX", ContextRecord->Rsi);
		utils::write_log("Rdi: 0x%llX", ContextRecord->Rdi);
		utils::write_log("R8: 0x%llX", ContextRecord->R8);
		utils::write_log("R9: 0x%llX", ContextRecord->R9);
		utils::write_log("R10: 0x%llX", ContextRecord->R10);
		utils::write_log("R11: 0x%llX", ContextRecord->R11);
		utils::write_log("R12: 0x%llX", ContextRecord->R12);
		utils::write_log("R13: 0x%llX", ContextRecord->R13);
		utils::write_log("R14: 0x%llX", ContextRecord->R14);
		utils::write_log("R15: 0x%llX", ContextRecord->R15);

		// Dump stack
		for (int i = 0; i < 50; ++i) {
            const auto slot=reinterpret_cast<const std::uintptr_t*>(ContextRecord->Rsp)+i;
            if(!Exelight::Readable(slot,sizeof(*slot)))break;
			utils::write_log("Rsp+%i: 0x%llX", i * sizeof(std::uintptr_t), *slot);
		}

		utils::end_exception();

	}
	
	LONG CALLBACK hookHandler(LPEXCEPTION_POINTERS ex)
	{
		if (!ex || !ex->ExceptionRecord || !ex->ContextRecord)
			return EXCEPTION_CONTINUE_SEARCH;

		auto record = ex->ExceptionRecord;
		auto ctx = ex->ContextRecord;

		if (record->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
			return EXCEPTION_CONTINUE_SEARCH;

		const auto rip = ctx->Rip;

		if (rip == OFFSET(0x11D2580))
		{
			ctx->Rax = FALSE;
			ctx->Rip = OFFSET(0x11D2592);
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (rip == OFFSET(0x22C4935))
		{
			ctx->Rax = FALSE;
			ctx->Rip = OFFSET(0x22C49FE);
			return EXCEPTION_CONTINUE_EXECUTION;
		}

        Exelight::Log("Access violation: code=0x%lX RIP=0x%llX gameRVA=0x%llX DLLRVA=0x%llX target=0x%llX",record->ExceptionCode,ctx->Rip,ctx->Rip-Exelight::GameBase(),ctx->Rip-reinterpret_cast<std::uintptr_t>(dllbase),record->NumberParameters>1?record->ExceptionInformation[1]:0);
		catch_exception(record, ctx);

		return EXCEPTION_CONTINUE_SEARCH;
	}

	void initExceptionHandler() {		

		AddVectoredExceptionHandler(1, hookHandler);

	}


}