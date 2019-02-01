// This is a comment
// uncomment the line below if you want to write a filterscript
//#define FILTERSCRIPT

#define PRESSED(%0) (((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

#include <a_samp>
#include <mxINI>
#include <zcmd>
#include <sscanf>

//  variables
new doska;
new bestplayername[MAX_PLAYER_NAME];
new maxkills;
new bestplayerid = -1;

new database[10] = "users.db";
new DB: users_base;
new DBResult: u_result;

new bool:IsVehicleDerby[MAX_VEHICLES] = false;
new bool:antidgunweapon[50][47];
new Text:gLevel[50];//Где name название переменной.
new Text:gPoints[50];//Где name название переменной.

new gungamemap = 1;

new humans;
new zombies;
new zmmap = 0;
new zmtimer;

enum pInfo
{
	pMute,
	pBan,
    pDeaths,
    pKills,
    pLevel,
    pExp,
    pPass[64],// Пароль
    pAdmin,// Админ уровень
}
new Player[MAX_PLAYERS][pInfo];
// /variables
main()
{
	print("\n----------------------------------");
	print("royaldm");
	print("----------------------------------\n");
}

public OnGameModeInit()
{
    AntiDeAMX();
	SetGameModeText("Royal DM");
	CreateStandartObjects();
	GetBestPlayer();
	DisableInteriorEnterExits();
	OpenDB();
	StartNextZM();
	return 1;
}

public OnGameModeExit()
{
	for(new i;i!=GetMaxPlayers();i++)
	{
		if(IsPlayerConnected(i) && GetPVarInt(i,"logged") == 1 && !IsPlayerNPC(i))//на сервере, в системе, и не бот
			SavePlayer(i);//сохраним
	}
	db_close(users_base);//при окончании работы сервера - закроем базу данных
	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
	new str[500];
	format(str,sizeof str,"SELECT NAME, PASSWORD FROM USERS WHERE NAME = '%s'",GetTheName(playerid));//отправим запрос в таблицу на поиск нужного нам игрока
	u_result = db_query(users_base,str);
	if(!db_num_rows(u_result)){
	//если строки по нужному нам запросы отсутствуют в таблице - значит игрок незарегестрирован на сервере
	ShowPlayaDialog(playerid, 2, DIALOG_STYLE_INPUT, "Регистрация","Ваш аккаунт не найден на сервере\nно ничего страшного, вы можете зарегистрироваться\nпрямо сейчас. Для этого введите желаемый пароль в окошко","Вход","Кик");
	}
	else{//нужная нам строка присутствует
	ShowPlayaDialog(playerid, 1, DIALOG_STYLE_INPUT, "Вход в систему","Впишите ваш пароль в окошко для игры на сервере","Вход","Кик");
	}
	return 1;
}

public OnPlayerConnect(playerid)
{
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    ClearZombie(playerid);
	return 1;
}

public OnPlayerSpawn(playerid)
{
	if (GetPVarFloat(playerid, "playermap") == 1)
	{
		SetPlayerDeagleSpawn(playerid);
		SetPlayerHealth(playerid, 30.0);
		GivePlayaWeapon(playerid, 24, 500);
	}
	if (GetPVarFloat(playerid, "playermap") == 2)
	{
		HideAll(playerid);
	    SetPlayerGungameSpawn(playerid);
        TextDrawShowForPlayer(playerid, gLevel[playerid]);
        TextDrawShowForPlayer(playerid, gPoints[playerid]);
        ResetPlayerWeapons(playerid);
		SetPlayerGungameWeapon(playerid);
	}
	if (GetPVarFloat(playerid, "playermap") == 3)
	{
	    SetPlayerDerbySpawn(playerid);
	    ShowPlayaDialog(playerid,5,DIALOG_STYLE_LIST,"Выбери машину","Monster\nLineruner\nDumper\nPatriot\nDozer\nRancher\nTanker\nTowtruck\nCombine\nYosemite","Выбрать","");
	}
	if (GetPVarFloat(playerid, "playermap") == 4)
	{
		if (GetPVarInt(playerid, "IsHuman") == 1)
		{
			GivePlayaWeapon(playerid,27,7);
			GivePlayaWeapon(playerid,31,20);
			GivePlayaWeapon(playerid,32,20);
		}
		else
			GivePlayaWeapon(playerid,4,0);
	}
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	Player[playerid][pDeaths] += 1;
	if (Player[killerid][pKills] > maxkills)
		SetBestPlayer(killerid);
    Player[killerid][pKills] += 1;
	if (GetPVarFloat(killerid, "playermap") == 2)// gungame here
	{
	    HideAll(killerid);
		ChangeDraw(killerid);
	}
	return 1;
}

public OnVehicleSpawn(vehicleid)
{
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
    if (IsVehicleDerby[vehicleid] == true)
		DestroyVehicle(vehicleid);
	return 1;
}

public OnPlayerText(playerid, text[])
{
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp("/mycommand", cmdtext, true, 10) == 0)
	{
		// Do something here
		return 1;
	}
	return 0;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	if (GetPVarFloat(playerid, "playermap") == 3)
	{
		ResetPlayerWeapons(playerid);
		new playervehicle = GetPlayerVehicleID(playerid);
		DestroyVehicle(playervehicle);
		ShowPlayaDialog(playerid,5,DIALOG_STYLE_LIST,"Выбери машину","Monster\nLineruner\nDumper\nPatriot\nDozer\nRancher\nTanker\nTowtruck\nCombine\nYosemite","Выбрать","");
	}
	return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{
	if (GetPlayerWeapon(issuerid) == 8 && GetPVarFloat(playerid, "playermap") == 2)
	    OnPlayerDeath(playerid, issuerid, weaponid);
	new Float:pHealth;
	GetPlayerHealth(playerid, pHealth);
	if (GetPVarInt(playerid, "IsHuman") == 1 && GetPVarInt(issuerid, "IsHuman") == 2)
	{
	    humans--;
	    OnPlayerDeath(playerid, issuerid, weaponid);
		SetPVarInt(playerid, "IsHuman", 2);
		zombies++;
		SetPlayerZombieSpawn(playerid);
		SpawnPlayer(playerid);
		if (humans == 0)
			StartNextZM();
	}
	if (GetPVarInt(playerid, "kont") == 1)
	    SetPlayerHealth(playerid, 100);
	if (GetPVarInt(playerid, "IsHuman") == 2 && GetPVarInt(issuerid, "IsHuman") == 1 && pHealth <= 30 && GetPVarInt(playerid, "kont") != 1)
	{
	    SetPlayerHealth(playerid,100);
	    SetPVarInt(playerid,"kont",1);
		SendClientMessage(playerid,0xFF0000AA,"Тебя оглушили");
		ApplyAnimation(playerid,"SWEET","SWEET_INJUREDLOOP",4.0, 1, 0, 0, 0, 0);
		SetTimerEx("stopanim",3500,false,"i",playerid);
	}
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
	return 1;
}

public OnPlayerLeaveCheckpoint(playerid)
{
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	return 1;
}

public OnPlayerLeaveRaceCheckpoint(playerid)
{
	return 1;
}

public OnRconCommand(cmd[])
{
	return 1;
}

public OnPlayerRequestSpawn(playerid)
{
	return 1;
}

public OnObjectMoved(objectid)
{
	return 1;
}

public OnPlayerObjectMoved(playerid, objectid)
{
	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	return 1;
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
	return 1;
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	return 1;
}

public OnPlayerSelectedMenuRow(playerid, row)
{
	return 1;
}

public OnPlayerExitedMenu(playerid)
{
	return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if (GetPVarInt(playerid,"chooseskin") == 1)
	    ChangeFirstSkin(playerid, newkeys);
	if (GetPVarInt(playerid, "IsHuman") > 0 && PRESSED(KEY_JUMP) && GetPVarInt(playerid, "kont") == 0)
		ZombieEnergy(playerid, newkeys);
	return 1;
}

stock ZombieEnergy(playerid, newkeys)
{
    if (GetPVarInt(playerid, "IsHuman") == 2 && GetPVarInt(playerid, "CountJump") <= 3)
 	{
  		SetPVarInt(playerid, "CountJump", 1 + GetPVarInt(playerid, "CountJump"));
 	   	if (GetPVarInt(playerid, "CountJump") == 4)
 		{
 	    	SetPVarInt(playerid, "NoJump", 1);
        	SetTimerEx("NoJump",5000,false,"i",playerid);
 		}
 	}
 	if (GetPVarInt(playerid, "NoJump") == 1)
 	{
 	    SendClientMessage(playerid, 0xFF66FFAA,"Кончилась енергия для прыжка");
 	    ClearAnimations(playerid);
  	}
	if (GetPVarInt(playerid, "IsHuman") == 1 && newkeys & KEY_JUMP)
	{
	    SendClientMessage(playerid, 0xFF66FFAA,"Людям запрещено прыгать");
	    ClearAnimations(playerid);
	}
}

public OnRconLoginAttempt(ip[], password[], success)
{
	return 1;
}

public OnPlayerUpdate(playerid)
{
	return 1;
}

public OnPlayerStreamIn(playerid, forplayerid)
{
	return 1;
}

public OnPlayerStreamOut(playerid, forplayerid)
{
	return 1;
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	return 1;
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	CheckAntihider(playerid);
	switch(dialogid)
	{
		case 1:
		{
			if(response)// Если игрок нажал первую кнопку входа
			{
				if(!strlen(inputtext))// Если окно ввода пустое, выводим диалог снова
				{
					ShowPlayaDialog(playerid,1,DIALOG_STYLE_INPUT,"Окно Входа","Здравствуйте\nВаш аккаунт есть на сервере\nВведите свой пароль в окошко","Ввод","");// Показываем диалог входа в игру.
					return 1;
				}
				new pass[64];// Массив с паролем
				strmid(pass,inputtext,0,strlen(inputtext),64);// Считываем текст с диалога
				OnPlayerLogin(playerid,pass);// Запускаем паблик входа
			}
			else// Если игрок нажал Esc, вернём ему диалог
			{
				ShowPlayaDialog(playerid,1,DIALOG_STYLE_INPUT,"Окно Входа","Здравствуйте\nВаш аккаунт есть на сервере\nВведите свой пароль в окошко","Ввод","");// Показываем диалог входа в игру.
			}
		}
		case 2:
		{
		    if(response)// Если игрок нажал первую кнопку
        	{
				if(!strlen(inputtext))// Если окно ввода пустое, выводим диалог снова
            	{
					ShowPlayaDialog(playerid,2,DIALOG_STYLE_INPUT,"Окно Регистрации","Здравствуйте\nВаш аккаунт не найден.\nЗарегистрируйтесь введя пароль в окошко","Ввод","");// Показываем диалог регистрации.
					return 1;
				}
            	new pass[64];// Массив с паролем
            	strmid(pass,inputtext,0,strlen(inputtext),64);// Считываем текст с диалога
            	OnPlayerRegister(playerid,pass);// Запускаем паблик регистрации
  			}
        	else// Если игрок нажал Esc, вернём ему диалог
            	ShowPlayaDialog(playerid,2,DIALOG_STYLE_INPUT,"Окно Регистрации","Здравствуйте\nВаш аккаунт не найден.\nЗарегистрируйтесь введя пароль в окошко","Ввод","");// Показываем диалог регистрации.
		}
		case 3:
		{
			if (IsPlayerInAnyVehicle(playerid))
			    SetPlayerPos(playerid,0,0,0);
			SetPlayerInterior(playerid,17);
			switch (random(7))
			{
					case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 489.8367,-16.3351,1000.6797,55.6634, 0, 0, 0, 0, 0, 0 );
				    case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 486.1196,-12.8604,1000.6797,219.5149, 0, 0, 0, 0, 0, 0 );
				    case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 490.9361,-11.0888,1000.6797,153.4010, 0, 0, 0, 0, 0, 0 );
				    case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 486.1133,-15.2493,1000.6797,144.9409, 0, 0, 0, 0, 0, 0 );
				    case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 483.2300,-22.1853,1003.1094,17.7498, 0, 0, 0, 0, 0, 0 );
				    case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 488.8091,-4.6902,1002.0781,204.4748, 0, 0, 0, 0, 0, 0 );
				    case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 487.7292,-9.7175,1000.6719,170.3212, 0, 0, 0, 0, 0, 0 );
			}
			ClearZombie(playerid);
			HideAll(playerid);
		    SpawnPlayer(playerid);
		    SetPVarInt(playerid,"chooseskin",0);
		    SetPlayerVirtualWorld(playerid,0);
			SetPVarInt(playerid, "playermap", 0);
		}
		case 4:
		{
			if (response)
			{
				if (strval (inputtext) == 74)
				{
					ShowPlayaDialog(playerid,4,DIALOG_STYLE_INPUT,"Выбор скина","Введи ID скина, который ты хочешь выбрать","Ввод","");
					return SendClientMessage(playerid, 0xFF0000AA, "Такого скина нет");
				}
				if (0 < strval (inputtext) <= 299)
				{
					SetPlayerSkin(playerid,strval(inputtext));
					SetPVarInt(playerid,"pskin",strval(inputtext));
				}
				else
				{
 					ShowPlayaDialog(playerid,4,DIALOG_STYLE_INPUT,"Выбор скина","Введи ID скина, который ты хочешь выбрать","Ввод","");
					return SendClientMessage(playerid, 0xFF0000AA, "Число должно быть от 1 до 299");
				}
			}
		}
		case 5:
		{
		    if(response)
			{
			    new Float:ang;
			  	GetPlayerFacingAngle(playerid, ang);
				new Float:x,Float:y,Float:z;
				GetPlayerPos(playerid,x,y,z);
				new playervehicle;
				if (listitem == 0)
				    playervehicle = CreateVehicle(556,x,y,z,ang,1,1,1);
                if (listitem == 1)
				    playervehicle = CreateVehicle(403,x,y,z,ang,1,1,1);
    			if (listitem == 2)
				    playervehicle = CreateVehicle(406,x,y,z,ang,1,1,1);
			    if (listitem == 3)
				    playervehicle = CreateVehicle(470,x,y,z,ang,1,1,1);
			    if (listitem == 4)
				    playervehicle = CreateVehicle(486,x,y,z,ang,1,1,1);
			    if (listitem == 5)
				    playervehicle = CreateVehicle(489,x,y,z,ang,1,1,1);
				if (listitem == 6)
					playervehicle = CreateVehicle(514,x,y,z,ang,1,1,1);
			 	if (listitem == 7)
					playervehicle = CreateVehicle(525,x,y,z,ang,1,1,1);
			 	if (listitem == 8)
					playervehicle = CreateVehicle(532,x,y,z,ang,1,1,1);
			 	if (listitem == 9)
					playervehicle = CreateVehicle(554,x,y,z,ang,1,1,1);
                IsVehicleDerby[playervehicle] = true;
                SetVehicleVirtualWorld(playervehicle,GetPlayerVirtualWorld(playerid));
			 	PutPlayerInVehicle(playerid,playervehicle,0);
			 	LinkVehicleToInterior(playervehicle,15);
			 	GivePlayaWeapon(playerid,28,1000);
			 	SetPVarInt(playerid,"playerderbycar",playervehicle);
			 	}
	 		else
				ShowPlayaDialog(playerid,5,DIALOG_STYLE_LIST,"Выбери машину","Monster\nLineruner\nDumper\nPatriot\nDozer\nRancher\nTanker\nTowtruck\nCombine\nYosemite","Выбрать","");
		}
	}
	return 1;
}

public OnPlayerClickPlayer(playerid, clickedplayerid, source)
{
	return 1;
}

CMD:skin(playerid)
{
	if (GetPVarFloat(playerid,"playermap") == 0)
		ShowPlayaDialog(playerid,4,DIALOG_STYLE_INPUT,"Выбор скина","Введи ID скина, который ты хочешь выбрать","Ввод","Отмена");
	else
		return SendClientMessage(playerid, 0xFF0000AA, "Ты должен быть не на ДМ-зоне, чтобы сменить скин");
	return 1;
}

CMD:gungame(playerid)
{
    if(GetPVarFloat(playerid, "playermap") !=0)
        return SendClientMessage(playerid,0xAA3333AA, "Ты уже на ДМ-зоне. используй /spawn чтобы выйти");
    gPoints[playerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	gLevel[playerid] = TextDrawCreate(470.0, 398,"Level 1 : 9mm");//Это мы задаём координаты TextDraw'а
	gDraw(playerid);
	SetPlayerInterior(playerid,0);
	SetPlayerVirtualWorld(playerid,12);
	SetPVarInt(playerid,"gunscore",1);
    SetPVarFloat(playerid, "playermap",2);
	SetPlayerGungameSpawn(playerid);
	SpawnPlayer(playerid);
	return 1;
}

CMD:zombie(playerid)
{
	if(GetPVarFloat(playerid, "playermap") != 0 && GetPVarFloat(playerid, "playermap") != 4.1)
	    return SendClientMessage(playerid,0xAA3333AA, "Ты уже на ДМ-зоне. используй /spawn чтобы выйти");
	SetPVarFloat(playerid, "playermap", 4);
    SetPlayerInterior(playerid,0);
	SetPlayerVirtualWorld(playerid,14);
	ApplyAnimation(playerid,"SWEET","null",0.0,0,0,0,0,0);
	SetPlayerHumanOrZom(playerid);
	SetPlayerZombieSpawn(playerid);
	SpawnPlayer(playerid);
	return 1;
}

stock CreateStandartObjects()
{
    CreateObject(17060,602.52886962891,956.19195556641,-19.644834518433,0,0,0);
	CreateObject(6976,610.56829833984,968.80727539063,-17.23213195801,0,0,180);
	CreateObject(8168,602.89477539063,801.35534667969,-31.07794380188,0,0,0);
	CreateObject(11478,622.654296875,866.56860351563,-34.732086181641,0,0,0);
	CreateObject(3279,566.99523925781,767.54827880859,-17.566102981567,0,0,160);
	CreateObject(10245,678.53161621094,836.8525390625,-36.027015686035,0,0,10);
	CreateObject(10245,673.23492431641,840.44067382813,-40.427017211914,0,0,10);
	CreateObject(10245,568.9509,948.6711,-24.3,0,0,160);
	CreateObject(6976,588.3,948.6,-36.1,0,0,205);
	CreateObject(3279,717.4925,814.9747,-31.2605,0,0,90);
	CreateObject(3279,643.14660644531,981.96142578125,-8.4113388061523,0,0,180);
	doska = CreateObject(7914,485,-0.3,1002.6,0,0,0);
	/////////////////////////////////////////////////
	CreateObject(2975,1876.2,2865.3999,9.8,0,0,0,150);
	CreateObject(2975,1889.2,2865.3999,9.8,0,0,0,150);
	CreateObject(3093,1878.6,2851,11.2,0,0,270,150);
	/////////////////////////////////////////////////
	CreateObject(3354,-1982.5,716.40002,46.9,0,0,90,150);
	CreateObject(3354,-1982.5,716.40002,49,0,0,90,150);
	CreateObject(3354,-1989.4,712.40002,46.9,0,0,0,150);
	CreateObject(3354,-1989.4004,712.40039,49,0,0,0,150);
	CreateObject(3354,-1920.85,716.59998,46.9,0,0,90,150);
	CreateObject(3354,-1920.85,716.59998,49,0,0,90,150);
	CreateObject(3354,-1914,712.45001,49.2,0,0,0,150);
	CreateObject(3354,-1914,712.45001,46.9,0,0,0,150);
	CreateObject(2944,-1378.5996,1494.9004,2.6,0,0,0,150);
	CreateObject(8378,-1631,1419,10.9,0,0,315,150);
	CreateObject(8378,-1710.2,1348.9,7.6,0,0,135,150);
	CreateObject(10575,-1681.8,1330.8,8.2,0,0,135,150);
	CreateObject(10575,-1664.8,1344.2,9.5,90,0,135,150);
	CreateObject(10575,-1643.4,1365.6,8.2,0,0,135,150);
	CreateObject(10575,-1705.4,1372.5,8.2,0,0,135,150);
	CreateObject(10575,-1698.8,1379.6,9.5,90,0,135,150);
	CreateObject(10575,-1643.4,1365.6,12,0,0,135,150);
	CreateObject(8378,-1624.8,1385,10.9,0,0,70,150);
	CreateObject(8378,-1665.5,1431,10.9,0,0,5,150);
	CreateObject(8378,-1673.4,1416.1,21,0,0,225,150);
	CreateObject(8378,-1711,1366.8,21,0,0,225,150);
	CreateObject(8378,-1689.5,1397.5,21,0,0,245,150);
	CreateObject(8378,-1665.2002,1431.7002,21,0,0,4.999,150);
	CreateObject(8378,-1631.8,1418.9,21,0,0,315,150);
	CreateObject(8378,-1624.8,1385,21,0,0,69.999,150);
	CreateObject(8378,-1642.5,1367.8,21,0,0,225,150);
	CreateObject(8378,-1702,1345,21,0,0,314.995,150);
	CreateObject(8378,-1671,1343,21,0,0,215,150);
	CreateObject(10575,-1669,1419.5,13.4,0,0,135,150);
	CreateObject(10575,-1683,1403.2002,11,0,90,135,150);
	CreateObject(10575,-1679,1407,11,0,90,135,150);
	CreateObject(10575,-1684,1404.5,13,0,0,135,150);
	CreateObject(10575,-1679.3,1409.2,13,0,0,135,150);
	CreateObject(10575,-1686.8,1401.7,13,90,180,45,150);
	CreateObject(8378,829.70001,-1129.6,31.4,0,0,0,150);
	CreateObject(8378,872.90002,-1129.6,31.4,0,0,0,150);
	CreateObject(8378,917.40002,-1129.6,31.4,0,0,0,150);
	CreateObject(8378,951.29999,-1134.5,31.4,0,0,90,150);
	CreateObject(8378,961.90002,-1129.6,31.4,0,0,0,150);
	CreateObject(8378,951.29999,-1075,34,0,0,90,150);
	CreateObject(8378,952.70001,-1089.4,31.4,0,0,90,150);
	CreateObject(8378,807.79999,-1107.9,31.4,0,0,90,150);
	CreateObject(8378,807.5,-1065.6,31.4,0,0,90,150);
	CreateObject(8378,830.40002,-1070.8,31.4,0,0,0,150);
	CreateObject(8378,898.09998,-1055.6,31.4,0,0,0,150);
	CreateObject(8378,919.70001,-1055.6,31.4,0,0,0,150);
	CreateObject(8378,963.59998,-1054.23,33.7,0,355,4,150);
	CreateObject(8378,846,-1070.8,31.4,0,0,0,150);
	CreateObject(8378,878.29999,-1046,31.4,0,0,78,150);
	CreateObject(10575,869.5,-1070,29.5,90,40,90,150);
	CreateObject(10575,872.59998,-1067.4,29.5,90,39.996,90,150);
	CreateObject(10575,869.5,-1070,34,90,39.996,90,150);
	CreateObject(10575,872.59998,-1067.4,34,90,39.996,90,150);
/////////////////////////////////////////////////////////////////
	new Float:x1=4.7;
	CreateObject(8955, 2046.6466, -2502.3445,13.5469, 0, 0, 270, 120);
	CreateObject(8955, 2046.6466, -2486.8499,13.5391, 0, 0, 270, 120);
	while (2100-x1 > 1437.7297)
	{
		CreateObject(13188, 2046.8-x1, -2509.2, 14.0, 0, 0, 90, 120);
		x1 +=4.8;
	}
	new Float:x2=4.7;
	while (2100-x2 > 1437.7297)
	{
	CreateObject(13188, 2046.8-x2, -2478.6501, 14.0, 0, 0, 90, 120);
	x2 +=4.8;
	}
	new Float:x3=8;
	while (2100-x3 > 1437.7297)
	{
	CreateObject(979, 2046.8-x3, -2493.8577, 14.0, 0, 0, 180, 120);
	x3 +=8;
	}
}

stock GetBestPlayer()
{
    new string[100];
	new iniFile = ini_openFile("players/bestplayer.ini");// Открываем файл по тому пути который указали.
	ini_getString(iniFile,"bestplayer",bestplayername);// Записываем пароль игрока в файл
	ini_getInteger(iniFile,"maxkills",maxkills);// Записываем пароль игрока в файл
	ini_closeFile(iniFile);// Закрываем файл
	format(string, sizeof(string), "\n                      Доска почета:\n                      %s", bestplayername);// Форматирование строки и
	SetObjectMaterialText(doska,string,0,OBJECT_MATERIAL_SIZE_256x128,"Arial",20,0xE91313FF,0xE91313FF, 1);
}

stock SetBestPlayer(killerid)
{
		new string[100];
		format(string, sizeof(string), "\n                      Доска почета:\n                      %s", GetTheName(killerid));// Форматирование строки и
		maxkills = Player[killerid][pKills];
		if (bestplayerid != killerid)
		SetObjectMaterialText(doska,string,0,OBJECT_MATERIAL_SIZE_256x128,"Arial",20,0xE91313FF,0xE91313FF, 1);
		new iniFile = ini_openFile("players/bestplayer.ini");// Открываем файл по тому пути который указали.
		ini_setString(iniFile,"bestplayer",GetTheName(killerid));//
		ini_setInteger(iniFile,"maxkills",Player[killerid][pKills]);//
		ini_closeFile(iniFile);// Закрываем файл
		bestplayerid = killerid;
}

stock OpenDB()
{
    if(!fexist(database)){//если БД отсутствует
	users_base = db_open(database);//функция 'db_open(base[])' при отсутствии нужной нам базы данных создает пустую БД
	db_query(users_base,"CREATE TABLE USERS(NAME varchar, PASSWORD varchar, pExp int, pAdmin int, pDeaths int, tutorial int, pBan int, pMute int, pKills int, pLevel int, numberacc int)");//ну и как итог - заполним базу данных со столбцами "Name, Password, Money, Kills"
	}
	else{//если БД присутствует
	users_base = db_open(database);//тупо откроем ее
	}
}

stock CheckAntihider(playerid)
{
	if (GetPVarInt(playerid,"antihider") == 1)
	{
		SendClientMessage(playerid,-1,"Сработал Anti DialogHider");
		//Kick(playerid);
	}
	SetPVarInt(playerid,"antihider",1);
}

stock SavePlayer(playerid)
{
    new str[500];
	format(str,500,"UPDATE USERS SET pMute = '%d', pBan = '%d', pDeaths = '%d', pKills = '%d', pLevel = '%d', pExp = '%d' WHERE NAME = '%s'",Player[playerid][pMute],Player[playerid][pBan],Player[playerid][pDeaths],Player[playerid][pKills],Player[playerid][pLevel],Player[playerid][pExp],GetTheName(playerid));
	db_query(users_base, str);//обновим значения денег и убийств в базе для игрока %s
}

stock GetTheName(playerid)
{
	new pName[MAX_PLAYER_NAME];
	GetPlayerName(playerid, pName, sizeof pName);
	return pName;
}

CMD:pm(playerid, params[])
{
	new string[200],message[100],id;// Создаем стринг для форматирования строки, переменная сообщения и переменная получателя
	if(sscanf(params,"us",id,message))// Первый параметр для id персонажа получателя(u), а вторая переменная сообщения(s)
		return SendClientMessage(playerid, 0xFF0000AA, "Используй: /pm [id] [message]");// Если игрок не ввел сообщения например
	if(playerid == id)
		return SendClientMessage(playerid, 0xFF0000AA, "зачем писать самому себе?");
	format(string, sizeof(string), "Вы отправили личное сообщение %s", GetTheName(id));// Форматирование строки и
	SendClientMessage(playerid, 0xFF0000FF, string);//                                                                                                                      отправление сообщения отправившему
	format(string, sizeof(string), "Личное сообщение от %s: %s", GetTheName(playerid),message);
	SendClientMessage(id, 0xFF0000FF, string);//                                                                                                                                                                                                    отправление сообщения получателю
	return 1;
}

CMD:mystats(playerid)
{
	new string[200];
	format(string, sizeof(string), "Уровень: %d\nОчки опыта: %d\nКоличество смертей: %d\nКоличество убийств: %d", Player[playerid][pLevel],Player[playerid][pExp],Player[playerid][pDeaths],Player[playerid][pKills]);// Форматирование строки и
	SendClientMessage(playerid, 0xFF0000FF, string);//
	ShowPlayaDialog(playerid,556,DIALOG_STYLE_MSGBOX,"Статистика",string,"ок","");
	return 1;
}

CMD:derby(playerid)
{
	if (GetPVarFloat(playerid, "playermap") != 0)
	    return SendClientMessage(playerid,0xAA3333AA, "Ты уже на ДМ-зоне. используй /spawn чтобы выйти");
    SetPVarFloat(playerid, "playermap", 3);
    SendClientMessage(playerid,0xAA3333AA, "Ты зашел на Derby");
    SetPlayerVirtualWorld(playerid,13);
    SetPlayerDerbySpawn(playerid);
	SpawnPlayer(playerid);
	ShowPlayaDialog(playerid,5,DIALOG_STYLE_LIST,"Выбери машину","Monster\nLineruner\nDumper\nPatriot\nDozer\nRancher\nTanker\nTowtruck\nCombine\nYosemite","Выбрать","");
	return 1;
}

CMD:deagle(playerid)
{
	if (GetPVarFloat(playerid, "playermap") != 0)
	    return SendClientMessage(playerid,0xAA3333AA, "Ты уже на ДМ-зоне. используй /spawn чтобы выйти");
    SetPVarFloat(playerid, "playermap", 1);
    SendClientMessage(playerid,0xAA3333AA, "Ты зашел на Deagle");
    SetPlayerInterior(playerid, 0);
	SetPlayerDeagleSpawn(playerid);
	SpawnPlayer(playerid);
	SetPlayerHealth(playerid, 30.0);
	GivePlayaWeapon(playerid, 24, 500);
	SetPlayerVirtualWorld(playerid,10);
    return 1;
}

CMD:spawn(playerid)
{
    ShowPlayaDialog(playerid,3,DIALOG_STYLE_MSGBOX,"Spawn","Ты действительно хочешь выйти из ДМ-зоны?","Да","Нет");
    return 1;
}

AntiDeAMX()
{
    new a[][] =
    {
            "Unarmed (Fist)",
            "Brass K"
    };
    #pragma unused a
}

forward ShowPlayaDialog(playerid,dialogid, style, caption[], info[], button1[], button2[]);

public ShowPlayaDialog(playerid,dialogid, style, caption[], info[], button1[], button2[])
{
	SetPVarInt(playerid,"antihider",0);
	ShowPlayerDialog(playerid,dialogid, style, caption, info, button1, button2);
	return 1;
}

stock GivePlayaWeapon(playerid,weapon,ammo)
{
	antidgunweapon[playerid][weapon] = true;
	GivePlayerWeapon(playerid,weapon,ammo);
}

forward OnPlayerRegister(playerid, password[]);
public OnPlayerRegister(playerid, password[])// Паблик регистрации
{
	if(!valid_password(password)){//если введен некорректный пароль
	return ShowPlayaDialog(playerid, 0, DIALOG_STYLE_INPUT, "Регистрация","Ваш аккаунт не найден на сервере\nно ничего страшного, вы можете зарегистрироваться\nпрямо сейчас. Для этого введите желаемый пароль в окошко","Вход","Кик");
	}
    new str[500];
	u_result = db_query(users_base, "SELECT * FROM USERS WHERE NAME = 'nextnumber'");
	new Field[100];
	db_get_field_assoc( u_result, "pKills", Field, sizeof( Field ) ), SetPVarInt(playerid,"nextacc",strval(Field));
	format(str,500,"INSERT INTO `USERS` (`NAME`, `PASSWORD`, `pMute`, `pBan`,`pAdmin`,`pDeaths`,`pKills`,`pLevel`,`pExp`,`numberacc`) VALUES ('%s', '%s', '0', '0','0','0','0','0','0','%d')", GetTheName(playerid), password, GetPVarInt(playerid,"nextacc"));
	db_query(users_base, str);//добавим игрока в таблицу ко всем бабушкам
	SetPVarInt(playerid,"logged",1);//установим пивоварку на 1, т.е. игрок вошел в систему
	SendClientMessage(playerid,-1,"Вы успешно зарегестрировались на сервере!");
	ShowPlayaDialog(playerid, 1, DIALOG_STYLE_INPUT, "Вход в систему","Впишите ваш пароль в окошко для игры на сервере","Вход","Кик");
	format(str,500,"UPDATE USERS SET pMute = '0', pKills = '%d', pAdmin = '0', pDeaths = '0' WHERE NAME = 'nextnumber'",GetPVarInt(playerid,"nextacc") + 1);
	db_query(users_base, str);//обновим значения денег и убийств в базе для игрока %s
	return true;
}

forward OnPlayerLogin(playerid,password[]);
public OnPlayerLogin(playerid,password[])
{
    	if(IsPlayerConnected(playerid))// Проверка на подключение игрокаs
    	{
if(!valid_password(password)){//если введен некорректный пароль
return ShowPlayaDialog(playerid, 1, DIALOG_STYLE_INPUT, "Вход в систему","Впишите ваш пароль в окошко для игры на сервере","Вход","Кик");
}
new str[500];
format(str,500,"SELECT * FROM USERS WHERE NAME = '%s'",GetTheName(playerid));
u_result = db_query(users_base, str);
new Field[100];
db_get_field_assoc( u_result, "PASSWORD", Field, sizeof( Field ) );//запишем пароль из строки в переменную Field
if(strcmp(Field, password, true) == 0){//если пароли совпали
db_get_field_assoc( u_result, "pMute", Field, sizeof( Field ) ),SetPlayerScore(playerid, strval(Field));
Player[playerid][pMute] = strval(Field);
db_get_field_assoc( u_result, "pBan", Field, sizeof( Field ) ), SetPlayerScore(playerid, strval(Field));
Player[playerid][pBan] = strval(Field);
db_get_field_assoc( u_result, "pAdmin", Field, sizeof( Field ) ), SetPlayerScore(playerid, strval(Field));
Player[playerid][pAdmin] = strval(Field);
db_get_field_assoc( u_result, "pKills", Field, sizeof( Field ) ), SetPlayerScore(playerid, strval(Field));
Player[playerid][pKills] = strval(Field);
db_get_field_assoc( u_result, "pLevel", Field, sizeof( Field ) ), SetPlayerScore(playerid, strval(Field));
Player[playerid][pLevel] = strval(Field);
db_get_field_assoc( u_result, "pExp", Field, sizeof( Field ) ), SetPlayerScore(playerid, strval(Field));
Player[playerid][pExp] = strval(Field);
FirstSpawn(playerid);
        }
        else ShowPlayaDialog(playerid, 1, DIALOG_STYLE_INPUT, " ","Войдите в систему","Вход","Кик");
if (Player[playerid][pBan] > getdate())
{
    SendClientMessage(playerid, -1, "Сработал бан");
	//Kick(playerid);
}
    }
    	return 1;
}

valid_password(password[]){
	if(!strlen(password))
		return false;//пароль тупо пустой
	for(new i;i!=strlen(password);i++)
	{
		switch(password[i])
		{
			case 'a'..'z','A'..'Z','0'..'9': continue;//допустимые символы: английская расскладка и цифры
			default: return false;//во всех остальных случаях пароль некорректный
		}
	}
	return true;
}

stock FirstSpawn(playerid)
{
	SetPVarInt(playerid, "pskin", 1);
	SendClientMessage(playerid,-1,"Вы успешно вошли на сервер!");
	SetPVarInt(playerid,"plogged",1);
	SetSpawnInfo(playerid,0,GetPVarInt(playerid,"pskin"),2090.4368,1683.2643,12.5,97.9870,0,0,0,0,0,0);
	SpawnPlayer(playerid);
	TogglePlayerControllable(playerid,false);
	SetPVarInt(playerid,"chooseskin",1);
	SetPlayerVirtualWorld(playerid,playerid);
	SetPlayerCameraPos(playerid,2084.6387,1683.4137,14.5);
	SetPlayerCameraLookAt(playerid,2084.6387,1683.4137,14.5);
	SendClientMessage(playerid,0xE2194BFF,"Выбери скин, которым ты хочешь играть");
	SendClientMessage(playerid,0xE2194BFF,"Используй аналоговые клавиши {24C0C0}Num4 {E2194B}и {24C0C0}Num6{E2194B}, чтобы переключаться между скинами");
	SendClientMessage(playerid,0xE2194BFF,"Убедись, что включена клавиша {24C0C0}Num Lock {E2194B}(обычно она над аналоговыми клавишами)");
	SendClientMessage(playerid,0xE2194BFF,"Когда выберешь понравившийся скин, нажми клавишу {24C0C0}бега(по умолч. пробел)");
	if (Player[playerid][pAdmin] >= 1)
	{
	    SendClientMessage(playerid,0x0BDBE9FF,"Ты вошел в систему, как админ");
	    SendClientMessage(playerid,0x0BDBE9FF,"Чтобы узнать админ-команды, введи /acmd");
	}
}

stock ChangeFirstSkin(playerid, newkeys)
{
	if (newkeys == 8)
	{
	    SetPVarInt(playerid, "antihider", 0);
	    SetPVarInt(playerid,"chooseskin",0);
		OnDialogResponse(playerid, 3, 1, 0, "23");
	}
	if (newkeys == KEY_ANALOG_RIGHT)
	{
		if (GetPlayerSkin(playerid) == 73)
			SetPlayerSkin(playerid,75);
		if (GetPlayerSkin(playerid) == 299)
			SetPlayerSkin(playerid,1);
		else
			SetPlayerSkin(playerid,GetPlayerSkin(playerid) + 1 );
		SetPVarInt(playerid,"pskin",GetPlayerSkin(playerid));
	}
	if(newkeys == KEY_ANALOG_LEFT)
	{
		if (GetPlayerSkin(playerid) == 75)
			SetPlayerSkin(playerid,73);
		if (GetPlayerSkin(playerid) == 1)
			SetPlayerSkin(playerid,299);
		else
			SetPlayerSkin(playerid,GetPlayerSkin(playerid) - 1 );
		SetPVarInt(playerid,"pskin",GetPlayerSkin(playerid));
	}
}

public OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
    if (!success)
	{
		SendClientMessage (playerid,0xAFAFAFAA ,"Такой команды нет.Используй /commands");
		return 1;
	}
	return 1;
}

stock SetPlayerDeagleSpawn(playerid)
{
	switch(random(7))
    {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 247.6485,1859.3453,14.0840,95.1204, 0, 0, 0, 0, 0, 0 );
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 239.9375,1879.3922,11.4609,184.3746, 0, 0, 0, 0, 0, 0 );
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 239.5178,1860.4952,8.7578,269.8687, 0, 0, 0, 0, 0, 0 );
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 244.5775,1872.3716,8.7650,272.0853, 0, 0, 0, 0, 0, 0 );
		case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 262.2199,1871.9211,8.7578,90.3502, 0, 0, 0, 0, 0, 0 );
		case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 248.6438,1838.8771,7.6413,359.8194, 0, 0, 0, 0, 0, 0 );
		case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 235.3955,1872.1096,11.4609,272.4454, 0, 0, 0, 0, 0, 0 );
    }
}

stock gDraw(playerid)
{
		TextDrawAlignment(gLevel [playerid],0);
		TextDrawBackgroundColor(gLevel [playerid],0x000000FF);//Цвет обводки TextDraw'a
		TextDrawFont(gLevel [playerid],1);
		TextDrawSetOutline(gLevel [playerid], 1);
		TextDrawLetterSize(gLevel [playerid],0.3200,1.600);//размер TextDraw'a
		TextDrawColor(gLevel [playerid],0x3721BAFF);//Цвет самого TextDraw'a
		TextDrawSetProportional(gLevel [playerid],1);
		TextDrawAlignment(gPoints [playerid],0);
		TextDrawBackgroundColor(gPoints [playerid],0x000000FF);//Цвет обводки TextDraw'a
		TextDrawFont(gPoints [playerid],1);
		TextDrawSetOutline(gPoints [playerid], 1);
		TextDrawLetterSize(gPoints [playerid],0.3200,1.600);//размер TextDraw'a
		TextDrawColor(gPoints [playerid],0xE82727FF);//Цвет самого TextDraw'a
		TextDrawSetProportional(gPoints [playerid],1);
		TextDrawShowForPlayer(playerid, gLevel[playerid]);
		TextDrawShowForPlayer(playerid, gPoints[playerid]);
		return 1;
}

stock SetPlayerGungameSpawn(playerid)
{
	if (gungamemap == 1)
	{
	    switch(random(10)) // генерируем число от 0 до 3 (включая)
        {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2532.1077,-1667.0415,15.1684,91.5623, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2521.5845,-1634.3936,14.1219,164.2329, 0, 0, 0, 0, 0, 0 );
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2445.0444,-1715.2518,13.7561,326.5176, 0, 0, 0, 0, 0, 0 );
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2440.1187,-1660.0447,26.1089,262.5970, 0, 0, 0, 0, 0, 0 ); /* аналогично. Внимание: это не действущие координаты, вы должны заменить их на свои */
       	case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2504.4470,-1693.6731,13.5595,355.1485, 0, 0, 0, 0, 0, 0 );
       	case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2505.2520,-1711.4269,13.5315,313.1557, 0, 0, 0, 0, 0, 0 );
       	case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2480.3831,-1699.6036,13.5271,351.0460, 0, 0, 0, 0, 0, 0 );
       	case 7: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2427.5791,-1678.1395,13.7390,292.1622, 0, 0, 0, 0, 0, 0 );
   		case 8: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2440.1187,-1660.0447,26.1089,262.5970, 0, 0, 0, 0, 0, 0 );
   		case 9: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 2477.4226,-1637.3247,13.4238,255.0600, 0, 0, 0, 0, 0, 0 );
		}
	}
	if (gungamemap == 2)
	{
	    switch(random(16)) // генерируем число от 0 до 3 (включая)
        {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -456.9073,2221.4951,43.0246,255.0600, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -433.1681,2250.9082,42.5980,255.0600, 0, 0, 0, 0, 0, 0 ); // ...
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -408.7240,2261.6597,42.4297,255.0600, 0, 0, 0, 0, 0, 0 ); // ...
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -400.2243,2228.3457,42.4297,255.0600, 0, 0, 0, 0, 0, 0 );
       	case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -389.2149,2213.6875,42.4253,255.0600, 0, 0, 0, 0, 0, 0 );
       	case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -390.9748,2197.8042,42.4240,255.0600, 0, 0, 0, 0, 0, 0 );
       	case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -358.9847,2211.2424,42.4844,255.0600, 0, 0, 0, 0, 0, 0 );
       	case 7: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -382.9482,2206.4070,42.4236,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 8: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -391.2118,2222.0623,42.4297,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 9: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -396.9840,2251.7239,42.4297,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 10: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -408.4073,2261.8003,42.4194,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 11: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -366.8639,2269.8223,42.1496,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 12: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -354.2245,2244.2285,42.4844,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 13: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -381.2924,2241.1990,42.3659,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 14: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -403.6906,2260.0396,42.3835,255.0600, 0, 0, 0, 0, 0, 0 );
   		case 15: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -418.1114,2229.0120,42.4297,255.0600, 0, 0, 0, 0, 0, 0 );
		}
	}
	if (gungamemap == 3)
	{
	    switch(random(11)) // генерируем число от 0 до 3 (включая)
        {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1289.5151,490.6995,11.1953,90.0276, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1288.0399,494.8220,11.1953,58.3572, 0, 0, 0, 0, 0, 0 );
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1320.7620,515.9354,11.1971,98.7541, 0, 0, 0, 0, 0, 0 );
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1341.5024,492.6096,11.1953,95.6439, 0, 0, 0, 0, 0, 0 ); /* аналогично. Внимание: это не действущие координаты, вы должны заменить их на свои */
       	case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1329.5602,501.8484,11.1953,60.2370, 0, 0, 0, 0, 0, 0 );
       	case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1318.8229,488.8604,11.1953,306.1592 , 0, 0, 0, 0, 0, 0 );
       	case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1351.8612,490.5950,11.1953,88.4374, 0, 0, 0, 0, 0, 0 );
       	case 7: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1367.4121,501.8121,11.1953,54.9102, 0, 0, 0, 0, 0, 0 );
   		case 8: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1400.6764,498.4074,11.1953,105.9841, 0, 0, 0, 0, 0, 0 );
   		case 9: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1371.7208,515.8424,11.1971,270.1723, 0, 0, 0, 0, 0, 0 );
   		case 10: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1327.9410,511.0578,11.1953,286.9240, 0, 0, 0, 0, 0, 0 );
		}
	}
	if (gungamemap == 4)
	{
	    switch(random(10)) // генерируем число от 0 до 3 (включая)
        {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1094.8600,2077.7554,15.3504,90.6871, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1094.8641,2093.1140,15.3504,90.2055, 0, 0, 0, 0, 0, 0 );
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1082.3738,2121.7468,15.3504,180.4230, 0, 0, 0, 0, 0, 0 );
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1094.0630,2077.5593,10.8203,357.3130, 0, 0, 0, 0, 0, 0 ); /* аналогично. Внимание: это не действущие координаты, вы должны заменить их на свои */
       	case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1057.2749,2079.2581,10.8203,59.4261, 0, 0, 0, 0, 0, 0 );
       	case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1056.3480,2140.2075,10.8203,256.0821 , 0, 0, 0, 0, 0, 0 );
       	case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1081.6697,2140.3530,10.8203,159.8878, 0, 0, 0, 0, 0, 0 );
       	case 7: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1094.8503,2121.0562,10.8203,119.9492, 0, 0, 0, 0, 0, 0 );
   		case 8: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1067.4702,2099.2554,10.8203,9.8231, 0, 0, 0, 0, 0, 0 );
   		case 9: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 1058.9515,2110.0156,10.8203,271.1221, 0, 0, 0, 0, 0, 0 );
		}
	}
	if (gungamemap == 5)
	{
        switch(random(23)) // генерируем число от 0 до 3 (включая)
        {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2115.9341,135.4416,35.1814,285.3724, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.6770,156.7240,35.3165,255.6054, 0, 0, 0, 0, 0, 0 );
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.6978,170.0323,42.2500,196.6981, 0, 0, 0, 0, 0, 0 );
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.6982,197.2888,35.3057,209.8817, 0, 0, 0, 0, 0, 0 ); /* аналогично. Внимание: это не действущие координаты, вы должны заменить их на свои */
       	case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.6992,171.6018,42.2500,293.0607, 0, 0, 0, 0, 0, 0 );
       	case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.2952,170.0441,42.2500,209.2314 , 0, 0, 0, 0, 0, 0 );
       	case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.4915,170.2702,46.5156,176.5742, 0, 0, 0, 0, 0, 0 );
       	case 7: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2122.2458,221.9835,38.8127,277.3235, 0, 0, 0, 0, 0, 0 );
   		case 8: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2116.2332,219.1505,35.2843,39.0429, 0, 0, 0, 0, 0, 0 );
   		case 9: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2136.1736,258.4490,35.3281,208.8479, 0, 0, 0, 0, 0, 0 );
		case 10: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.8828,219.3659,35.3919,260.6209, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 11: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2131.9492,259.6829,38.9768,298.8221, 0, 0, 0, 0, 0, 0 );
		case 12: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2136.1724,279.5112,37.5524,244.1567, 0, 0, 0, 0, 0, 0 );
		case 13: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2063.4619,248.9645,35.2965,0.8398, 0, 0, 0, 0, 0, 0 ); /* аналогично. Внимание: это не действущие координаты, вы должны заменить их на свои */
       	case 14: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2060.8079,253.4921,37.6178,168.7881, 0, 0, 0, 0, 0, 0 );
       	case 15: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2093.3730,261.1904,35.7666,187.4913 , 0, 0, 0, 0, 0, 0 );
       	case 16: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2115.1062,226.7047,34.8946,278.4559, 0, 0, 0, 0, 0, 0 );
       	case 17: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2101.9917,155.5766,35.1283,0.7674, 0, 0, 0, 0, 0, 0 );
   		case 18: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2092.6660,173.9203,35.0547,39.7662, 0, 0, 0, 0, 0, 0 );
   		case 19: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2067.9602,309.2092,41.9922,170.4766, 0, 0, 0, 0, 0, 0 );
		case 20: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2067.5867,308.5032,46.2578,100.7476, 0, 0, 0, 0, 0, 0 ); // телепортируем игрока в координаты X1,Y1,Z1
		case 21: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2130.1458,303.5495,34.4733,222.1538, 0, 0, 0, 0, 0, 0 );
		case 22: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -2135.6663,280.6132,35.3154,258.3558, 0, 0, 0, 0, 0, 0 );
		}
	}
}

stock SetPlayerGungameWeapon(playerid)
{
	if (GetPVarInt(playerid,"gunscore") == 1)
	    GivePlayaWeapon(playerid, 22, 500);
    if (GetPVarInt(playerid,"gunscore") == 2)
		GivePlayaWeapon(playerid, 24, 500);
   	if (GetPVarInt(playerid,"gunscore") == 3)
		GivePlayaWeapon(playerid, 24, 500);
   	if (GetPVarInt(playerid,"gunscore") == 4)
		GivePlayaWeapon(playerid, 25, 200);
   	if (GetPVarInt(playerid,"gunscore") == 5)
		GivePlayaWeapon(playerid, 25, 200);
   	if (GetPVarInt(playerid,"gunscore") == 6)
		GivePlayaWeapon(playerid, 26, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 7)
		GivePlayaWeapon(playerid, 26, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 8)
		GivePlayaWeapon(playerid, 29, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 9)
		GivePlayaWeapon(playerid, 29, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 10)
		GivePlayaWeapon(playerid, 28, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 11)
		GivePlayaWeapon(playerid, 28, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 12)
		GivePlayaWeapon(playerid, 32, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 13)
    	GivePlayaWeapon(playerid, 32, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 14)
    	GivePlayaWeapon(playerid, 30, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 15)
		GivePlayaWeapon(playerid, 30, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 16)
		GivePlayaWeapon(playerid, 31, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 17)
		GivePlayaWeapon(playerid, 31, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 18)
		GivePlayaWeapon(playerid, 38, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 19)
		GivePlayaWeapon(playerid, 38, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 20)
    	GivePlayaWeapon(playerid, 38, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 21)
		GivePlayaWeapon(playerid, 38, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 22)
		GivePlayaWeapon(playerid, 38, 1500);
   	if (GetPVarInt(playerid,"gunscore") == 23)
		GivePlayaWeapon(playerid, 34, 300);
   	if (GetPVarInt(playerid,"gunscore") == 24)
		GivePlayaWeapon(playerid, 34, 300);
   	if (GetPVarInt(playerid,"gunscore") == 25)
		GivePlayaWeapon(playerid, 34, 300);
   	if (GetPVarInt(playerid,"gunscore") == 26)
		GivePlayaWeapon(playerid, 8,0);
}

stock ChangeDraw(killerid)
{
    if (GetPVarInt(killerid,"gunscore") == 1)
    {
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 2 : Deagle");//Это мы задаём координаты TextDraw'а
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons(killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 2)
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 3)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 3 : Shotgun");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 4)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 5)
    {
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 4 : Sawnoff Shotgun");//Это мы задаём координаты TextDraw'а
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а;
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 6)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 7)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 5 : MP5");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 8)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 9)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 6 : Micro SMG");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 10)
    	gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 11)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 7 : Tec 9");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 12)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
 	if (GetPVarInt(killerid,"gunscore") == 13)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 8 : AK-47");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 14)
    	gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 15)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 9 : M4");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 16)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 17)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 5 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 10 : Minigun");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 18)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 4 points");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 19)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 3 points");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 20)
    	gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 21)
    	gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 22)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 3 points");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 11 : Sniper Rifle");//Это мы задаём координаты TextDraw'а
	    ResetPlayerWeapons (killerid);
    }
	if (GetPVarInt(killerid,"gunscore") == 23)
    	gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 2 points");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 24)
		gPoints[killerid] = TextDrawCreate(464, 415,"Next level: 1 point");//Это мы задаём координаты TextDraw'а
	if (GetPVarInt(killerid,"gunscore") == 25)
    {
	    gPoints[killerid] = TextDrawCreate(464, 415,"Final level");//Это мы задаём координаты TextDraw'а
	    gLevel[killerid] = TextDrawCreate(470.0, 398,"Level 12 : Katana");
	    ResetPlayerWeapons (killerid);
    }
    if (GetPVarInt(killerid,"gunscore") == 26)
	{
	    SendClientMessage(killerid,0x300FFAAB , "Ты стал победителем Gungame!");
	    gungamemap++;
	    if (gungamemap == 6)
	        gungamemap = 1;
        for(new l,zx = GetMaxPlayers( ); l < zx; l++)
        {
            if(!IsPlayerConnected(l) || GetPVarFloat(l, "playermap") != 2)
				continue;
 		 	SetPVarInt(l, "antihider", 0);
	  	  	HideAll(l);
			OnDialogResponse(l, 3, 1, 0, "23");// спавним игрока а потом опять прописываем ему /gungame
			cmd_gungame(l);
        }
        return 1;
	}
	SetPVarInt(killerid,"gunscore", 1 + GetPVarInt(killerid,"gunscore"));
    SetPlayerGungameWeapon(killerid);
	gDraw(killerid);
	return 1;
}

stock HideAll(playerid)
{
	TextDrawHideForPlayer(playerid,gLevel[playerid]);
	TextDrawHideForPlayer(playerid,gPoints[playerid]);
}

SetPlayerDerbySpawn(playerid)
{
	SetPlayerInterior(playerid,15);
    switch(random(10))
    {
		case 0: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1490.8297,952.4456,1036.9009,331.8577, 0, 0, 0, 0, 0, 0 );
		case 1: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1514.3409,988.1185,1037.5355,276.0839, 0, 0, 0, 0, 0, 0 );
		case 2: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1501.8676,1027.8240,1038.1696,242.5570, 0, 0, 0, 0, 0, 0 );
		case 3: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1472.4421,1050.3198,1038.4980,210.6200, 0, 0, 0, 0, 0, 0 );
       	case 4: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1392.6992,1059.4355,1038.5072,178.3464, 0, 0, 0, 0, 0, 0 );
       	case 5: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1340.3876,1055.1573,1038.3481,153.2795, 0, 0, 0, 0, 0, 0 );
       	case 6: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1299.0833,1033.1553,1037.9121,120.3792, 0, 0, 0, 0, 0, 0 );
       	case 7: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1284.1338,981.3269,1037.0121,72.4387, 0, 0, 0, 0, 0, 0 );
   		case 8: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1301.4777,955.7100,1036.6188,51.1318, 0, 0, 0, 0, 0, 0 );
   		case 9: SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1330.5013,942.1129,1036.4462,22.9315, 0, 0, 0, 0, 0, 0 );
	}
}

stock SetPlayerHumanOrZom(playerid)
{
	if (humans == zombies)
	{
	    new var = random(2) + 1;
	    SetPVarInt(playerid, "IsHuman", var);
	    if (var == 1)
	        humans++;
		if (var == 2)
		    zombies++;
		return 1;
	}
	if (humans < zombies)
	{
		SetPVarInt(playerid, "IsHuman", 1);
	    humans++;
	    return 1;
	}
	zombies++;
	SetPVarInt(playerid, "IsHuman", 2);
	return 1;
}

stock SetPlayerZombieSpawn(playerid)
{
	if (GetPVarInt(playerid, "IsHuman") == 1)
	{
	    SendClientMessage(playerid, -1, "{FF9191} Ты - человек. Выживи!");
	    switch(zmmap)
	    {
	        case 1:SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1951.8339,639.4008,46.5625,359.8894, 0, 0, 0, 0, 0, 0 );
			case 2:SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1433.5021,1483.5115,1.8672,288.1588, 0, 0, 0, 0, 0, 0 );
			case 3:SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), -1641.8674,1401.6919,9.8047,134.9374, 0, 0, 0, 0, 0, 0 );
			case 4:SetSpawnInfo(playerid, 0, GetPVarInt(playerid,"pskin"), 890.3646,-1101.7557,23.5000,86.6836, 0, 0, 0, 0, 0, 0 );
	    }
	}
	else
	{
	    SendClientMessage(playerid, -1, "{FF9191} Ты - зомби. Зарази всех!");
	    SetPlayerColor(playerid,0x5B5050FF);
	    switch(zmmap)
	    {
	        case 1: SetSpawnInfo(playerid,0,162,-1950.4437,711.0497,46.5625,180.7170,0,0,0,0,0,0);
			case 2: SetSpawnInfo(playerid,0,162,-1380.7405,1494.1173,1.8516,90.1069,0,0,0,0,0,0);
			case 3: SetSpawnInfo(playerid,0,162,-1678.3654,1365.3523,7.1797,318.0707,0,0,0,0,0,0);
			case 4: SetSpawnInfo(playerid,0,162,811.8546,-1098.2386,25.9063,274.6620,0,0,0,0,0,0);
	    }
	}
}

stock ClearZombie(playerid)
{
	if (GetPVarInt(playerid, "IsHuman") == 1)
	    humans--;
	if(GetPVarInt(playerid, "IsHuman") == 2)
	    zombies--;
	SetPVarInt(playerid, "IsHuman", 0);
	SetPlayerColor(playerid, playerid);
	if (GetPVarFloat(playerid, "playermap") == 4)
	    SetPVarFloat(playerid, "playermap", 4.2);
	if (humans == 0 && GetPVarFloat(playerid, "playermap") == 4.2)
			StartNextZM();
}

CMD:test(playerid)
{
	StartNextZM();
	return 1;
}

forward StartNextZM();
public StartNextZM()
{
	zmmap++;
	if (zmmap == 5)
		zmmap = 1;
	for(new l = 0; l < MAX_PLAYERS; l++)
	{
	    if (!IsPlayerConnected(l) || GetPVarFloat(l, "playermap") != 4)
	        continue;
		if (humans == 0)
		    SendClientMessage(l, -1, "Победа Зомби");
		else
		    SendClientMessage(l, -1, "Победа людей");
		SetPVarFloat(l, "playermap", 4.1);// флаг на запуск следующей карты
	}
	humans = 0;
	zombies = 0;
	for(new l = 0; l < MAX_PLAYERS; l++)
	{
	    if (!IsPlayerConnected(l) || GetPVarFloat(l, "playermap") != 4.1)
	        continue;
		cmd_zombie(l);
	}
	KillTimer(zmtimer);
	zmtimer = SetTimer("StartNextZM", 1000*90, false);
}

forward stopanim(playerid);
public stopanim(playerid)
{
	SetPVarInt(playerid, "kont",0);
	ClearAnimations(playerid);
}

stock IsPlayerJumping(playerid)
{
	new
	    index = GetPlayerAnimationIndex(playerid),
	    keys,
	    ud,
	    lr
	;

	GetPlayerKeys(playerid, keys, ud, lr);

	return (keys & KEY_JUMP) && (1196 <= index <= 1198);
}

forward NoJump(playerid);

public NoJump(playerid)
{
    SetPVarInt(playerid, "CountJump", 0);
    SetPVarInt(playerid, "NoJump", 0);
}
