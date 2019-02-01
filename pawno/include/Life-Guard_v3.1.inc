/*******************************************************************************
 *                                                                             *
 *                         AntiCheat Powered by Kazon                          *
 *                            All rights reserved                              *
 *                              AntiCheat v3.1                                 *
 *                                                                             *
 *******************************************************************************/

#include <a_samp>
#include <mxINI>

#define COLOR_LIGHTREDL 		0xFF6347AA

#define SendDig         	"[A] Игрок %s [ID %d] Подозревается в зломе Диалога!"
#define SendRcon        	"[A] Игрок %s [ID %d] Подозревается в зломе Rcon!"
#define SendAdm         	"[A] Игрок %s [ID %d] Подозревается в читерстве!"
#define SendConnect     	"[A] Игрок %s [ID %d] авторизировался(ась) на сервере!"
#define SendDisConnect  	"[A] Игрок %s [ID %d] отсоеденился(ась) от сервера [%s]"

#define AirBrek         1   // Защита от AirBreik'a                             | 1 - Включить | 0 - Выключить
#define Dialog			1	// Защита от подмены диалогов						| 1 - Включить | 0 - Выключить

#define AirBre          1   // Air Breik                                        | 0 - Бездействие | 1 - Кик | 2 - Бан | 3 - Сообщение Админам
#define Dia           	1   // При подмене диалогов								| 0 - Бездействие | 1 - Кик | 2 - Бан | 3 - Сообщение Админам

#define SendKick        "Вы кикнуты по подозрению в читерстве!"                 //Сообщение при кике игрока
#define SendBan         "Вы забанены по подозрению в читерстве!"                //Сообщение при бане игрока
enum laInfo {
	Float:HealL,
	Float:ArmL,
	CashL,
	Float:PosX,
	Float:PosY,
	Float:PosZ
};
new LifeInfo[MAX_PLAYERS][laInfo];

new Float:pPos[MAX_PLAYERS][3];
new AntiCheat[2], Slap[2], Message[3][144];
new TOTALAIR[MAX_PLAYERS];
new strl[144];

forward Updats();

//================================[Public]======================================

public OnGameModeInit()
{
	SetTimer("Updats",1000,1);
	AntiCheat[0] = AirBrek;
	AntiCheat[1] = Dialog;
	Slap[0] = AirBre;
	Slap[1] = Dia;
    Message[0] = SendKick;
	Message[1] = SendBan;
	return CallLocalFunction("Life_OnGameModeInit", "");
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit Life_OnGameModeInit
forward Life_OnGameModeInit();

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if(strfind(inputtext, "%", true) != -1) return true;
	if(AntiCheat[1] == 1)
	{
		if(GetPVarInt(playerid,"USEDIALOGID") != dialogid)
		{
			switch(Slap[1])
   			{
      			case 0: return true;
			    case 1: SendMess(playerid, COLOR_LIGHTREDL, "%s",Message[0]), Kick(playerid);
		        case 2: SendMess(playerid, COLOR_LIGHTREDL, "%s",Message[1]), Ban(playerid);
		        case 3: { format(strl, 70, SendDig ,playername(playerid), playerid); return SendAdminMes(COLOR_LIGHTREDL, strl); }
			}
			return true;
		}
	}
	return CallLocalFunction("Life_OnDialogResponse", "iiiis",playerid, dialogid, response, listitem, inputtext);
}

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif
#define OnDialogResponse Life_OnDialogResponse
forward Life_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);

public OnPlayerSpawn(playerid)
{
    SetPVarInt(playerid, "AntiBreik", 3);
    SetHealthL(playerid, 100.0);
	return CallLocalFunction("Life_OnPlayerSpawn", "i",playerid);
}

#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn Life_OnPlayerSpawn
forward Life_OnPlayerSpawn(playerid);

public OnPlayerConnect(playerid)
{
    SetPVarInt(playerid, "AntiBreik", 500); TOTALAIR[playerid] = 0;
    pPos[playerid][0] = 0, pPos[playerid][1] = 0, pPos[playerid][2] = 0;
    return CallLocalFunction("Life_OnPlayerConnect", "i", playerid);
}

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect Life_OnPlayerConnect
forward Life_OnPlayerConnect(playerid);

public Updats()
{
    HealthCheckL(), ArmourCheckL();
	for(new i = GetMaxPlayers() - 1; i != -1; i--)
	{
	    AirBrk(i);
	    if(GetPVarInt(i, "AntiBreik") > 0) SetPVarInt(i, "AntiBreik", GetPVarInt(i, "AntiBreik") - 1);
	}
	return true;
}
//===============================[Stock]========================================
stock playername(playerid) { new name[MAX_PLAYER_NAME]; GetPlayerName(playerid, name, sizeof(name)); return name; }

stock SPDL(playerid, dialogid, style, caption[], info[], button1[], button2[])
{
	ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
	return SetPVarInt(playerid, "USEDIALOGID", dialogid);
}

stock SetPlayerPosL(playerid, Float:x, Float:y, Float:z)
{
    SetPlayerPos(playerid, x, y, z); pPos[playerid][0] = x, pPos[playerid][1] = y;  pPos[playerid][2] = z;
	SetPVarInt(playerid, "AntiBreik", 3);
    return true;
}

stock SetHealthL(playerid, Float:Heal)
{
	LifeInfo[playerid][HealL] = Heal;
	return SetPlayerHealth(playerid, Heal);
}

stock SetArmourL(playerid, Float: Arm)
{
	LifeInfo[playerid][ArmL] = Arm;
	return SetPlayerArmour(playerid, Arm);
}

stock HealthCheckL()
{
	new Float:Heal;
    for(new i = GetMaxPlayers() - 1; i != -1; i--)
    {
        if(!IsPlayerConnected(i)) continue;
        GetPlayerHealth(i, Heal);
        if(LifeInfo[i][HealL] < Heal) SetPlayerHealth(i, LifeInfo[i][HealL]);
        else LifeInfo[i][HealL] = Heal;
    }
}

stock ArmourCheckL()
{
	new Float:Arm;
    for(new i = GetMaxPlayers() - 1; i != -1; i--)
    {
        if(!IsPlayerConnected(i)) continue;
        GetPlayerArmour(i, Arm);
        if(LifeInfo[i][ArmL] < Arm) SetPlayerArmour(i, LifeInfo[i][ArmL]);
        else LifeInfo[i][ArmL] = Arm;
    }
}

stock SendAdminMes(color, str[])
{
    for(new i = GetMaxPlayers() - 1; i != -1; i--)
	{
	    if(!IsPlayerConnected(i)) continue;
		if(IsPlayerAdmin(i)) SendClientMessage(i, color, str);
	}
	return true;
}

stock SendMess(playerid, color, fstring[], {Float, _}:...)
{
    static const STATIC_ARGS = 3;
    new n = (numargs() - STATIC_ARGS) * (cellbits / 8);
    if (n)
    {
        new message[128], arg_start, arg_end;
        #emit CONST.alt         fstring
        #emit LCTRL          	5
        #emit ADD
        #emit STOR.S.pri        arg_start
        #emit LOAD.S.alt        n
        #emit ADD
        #emit STOR.S.pri        arg_end
        do
        {
            #emit LOAD.I
            #emit PUSH.pri
            arg_end -= (cellbits / 8);
            #emit LOAD.S.pri   arg_end
        }
        while (arg_end > arg_start);
        #emit PUSH.S          fstring
        #emit PUSH.C          128
        #emit PUSH.ADR        message
        n += (cellbits / 8) * 3;
        #emit PUSH.S          n
        #emit SYSREQ.C        format
        n += (cellbits / 8);
        #emit LCTRL          4
        #emit LOAD.S.alt     n
        #emit ADD
        #emit SCTRL          4
        return SendClientMessage(playerid, color, message);
    }
    else return SendClientMessage(playerid, color, fstring);
}

stock AirBrk(playerid)
{
	if(AntiCheat[0] == 1)
	{
	   	new Float:Pos[4];
	    GetPlayerPos(playerid, Pos[0], Pos[1], Pos[2]);
	    Pos[3] = floatround(GetPlayerDistanceFromPoint(playerid, pPos[playerid][0], pPos[playerid][1], pPos[playerid][2]));
		if(GetPVarInt(playerid, "AntiBreik") == 0)
		{
			if(TOTALAIR[playerid] > 1)
			{
				switch(Slap[0])
	   			{
	      			case 0: return true;
					case 1: return SendMess(playerid, COLOR_LIGHTREDL, "ssss"), Kick(playerid);
					case 2: return SendMess(playerid, COLOR_LIGHTREDL, "%s",Message[1]), Ban(playerid);
					case 3: { format(strl, 70, SendAdm ,playername(playerid), playerid); return SendAdminMes(COLOR_LIGHTREDL, strl); }
				}
			}
			if(Pos[3] > 100)
			{
	 			switch(Slap[0])
	   			{
	      			case 0: return true;
					case 1: return SendMess(playerid, COLOR_LIGHTREDL, "SSSS"), Kick(playerid);
					case 2: return SendMess(playerid, COLOR_LIGHTREDL, "%s",Message[1]), Ban(playerid);
					case 3: { format(strl, 70, SendAdm ,playername(playerid), playerid); return SendAdminMes(COLOR_LIGHTREDL, strl); }
				}
			}
		   	else if(Pos[3] < 100 && Pos[3] > 70) { TOTALAIR[playerid]++; format(strl, 70, SendAdm ,playername(playerid), playerid); return SendAdminMes(COLOR_LIGHTREDL, strl); }
		}
        pPos[playerid][0] = Pos[0]; pPos[playerid][1] = Pos[1]; pPos[playerid][2] = Pos[2];
	}
	return true;
}
//===============================[Defines]======================================
#define ShowPlayerDialog SPDL
#define SetPlayerHealth SetHealthL
#define SetPlayerArmour SetArmourL
#define SetPlayerPos SetPlayerPosL
