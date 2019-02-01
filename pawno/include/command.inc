#include <a_samp>
stock date_to_timestamp
(
	year = 1970,	month  = 1,	day    = 1,
	hour =    0,	minute = 0,	second = 0
)
{
	switch ( month )
	{
		case  2 : day +=  31;	case  3 : day +=  59;	case  4 : day +=  90;
		case  5 : day += 120;	case  6 : day += 151;	case  7 : day += 181;
		case  8 : day += 212;	case  9 : day += 243;	case 10 : day += 273;
		case 11 : day += 304;	case 12 : day += 334;
	}
	return ( second + minute*60 + hour*3600 + (day - 1)*86400
		+ (year - 1970)*31557600 + ( year % 4 == 0 ? 21600 : 0 ) );
}
stock timestamp_to_date
(
	unix_timestamp = 0,
	& year = 1970,		& month  = 1,		& day    = 1,
	& hour =    0,		& minute = 0,		& second = 0
)
{
	year = unix_timestamp / 31557600;
	unix_timestamp -= year * 31557600;
	year += 1970;
	if ( year % 4 == 0 ) unix_timestamp -= 21600;
	day = unix_timestamp / 86400;
	switch ( day )
	{
		// в second будет временно хранится число месяца
		case    0..30 : { second = day;       month =  1; }
		case   31..58 : { second = day -  31; month =  2; }
		case   59..89 : { second = day -  59; month =  3; }
		case  90..119 : { second = day -  90; month =  4; }
		case 120..150 : { second = day - 120; month =  5; }
		case 151..180 : { second = day - 151; month =  6; }
		case 181..211 : { second = day - 181; month =  7; }
		case 212..242 : { second = day - 212; month =  8; }
		case 243..272 : { second = day - 243; month =  9; }
		case 273..303 : { second = day - 273; month = 10; }
		case 304..333 : { second = day - 304; month = 11; }
		case 334..366 : { second = day - 334; month = 12; }
	}
	unix_timestamp -= day * 86400;
	hour = unix_timestamp / 3600;
	unix_timestamp -= hour * 3600;
	minute = unix_timestamp / 60;
	unix_timestamp -= minute * 60;
	day = second + 1;
	second = unix_timestamp;
}
stock date ( formatStr[] = "%dd.%mm.%yyyy, %hh:%ii:%ss", timestamp = 0 )
{
	const sizeOfOutput = 128; // размер возвращаемой строки
	new yyyy, mm, dd, h, m, s,
		pos, foundPos, searchStartPos, outStrLen,
		tmpNumStr[5], outStr[sizeOfOutput];
	timestamp_to_date( timestamp+36000, yyyy,mm,dd, h,m,s ); // превратим штамп в дату
	memcpy( outStr, formatStr, 0, (sizeOfOutput - 1)*4 );
	outStr[sizeOfOutput - 1] = 0; // правильный обрез строки в целях безопасности
	outStrLen = strlen(outStr);
	searchStartPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%yyyy", false, searchStartPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 5, "%04d", yyyy ); // tmpNumStr = строкое значение yyyy
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 4; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		for ( pos += foundPos; pos < outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		searchStartPos = foundPos + 4; // позиция начала след. поиска += 4
		if ( searchStartPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%yyyy", false, searchStartPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	searchStartPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%yy", false, searchStartPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 5, "%04d", yyyy ); // tmpNumStr = строкое значение yyyy
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos + 2];
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		searchStartPos = foundPos + 2; // позиция начала след. поиска изменилась
		if ( searchStartPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%yy", false, searchStartPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%mm", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", mm ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%mm", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%m", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", mm ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		if ( mm < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];
			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%m", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%dd", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", dd ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%dd", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%d", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", dd ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		if ( dd < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];
			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%d", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%hh", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", h ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%hh", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%h", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", h ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		if ( h < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];
			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%h", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%ii", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", m ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%ii", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%i", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", m ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		if ( m < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];
			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%i", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%ss", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", s ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];
		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%ss", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%s", false, foundPos ); // ищем первую позицию
	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", s ); // tmpNumStr = строкое значение числа
		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];
		if ( s < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];
			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась
		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%s", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	return outStr;
}
new engine,lights,alarm,doors,bonnet,boot,objective;

stock Engine(vehicleid,bool:launched) {
	return launched ? SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_ON, -1, -1, -1, -1, -1, -1) : SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, -1, -1, -1, -1, -1, -1);
} //Запустить|заглушить двигатель

stock EngineStatus(vehicleid) {
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	return engine;
} //Вернёт статус двигателя ( вкл , выкл )

stock Lights(vehicleid,bool:launched) {
	return launched ? SetVehicleParamsEx(vehicleid, -1,VEHICLE_PARAMS_ON, -1, -1, -1, -1, -1) : SetVehicleParamsEx(vehicleid, -1,VEHICLE_PARAMS_OFF, -1, -1, -1, -1, -1);
} //Включение|Выключение фар

stock LightsStatus(vehicleid) {
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	return lights;
} //Вернёт статус фар ( вкл , выкл )

stock Alarm(vehickeid,bool:launched) {
	return launched ? SetVehicleParamsEx(vehicleid, -1,-1, VEHICLE_PARAMS_ON, -1, -1, -1, -1) : SetVehicleParamsEx(vehicleid, -1,-1, VEHICLE_PARAMS_OFF, -1, -1, -1, -1);
} //Включение|Выключение сигнализации

stock AlarmStatus(vehicleid) {
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	return alarm;
} //Вернёт статус сигнализации ( вкл , выкл )

stock Doors(vehicleid,bool:status) {
	return status ? SetVehicleParamsEx(vehicleid, -1,-1,-1,VEHICLE_PARAMS_ON, -1, -1, -1) : SetVehicleParamsEx(vehicleid, -1,-1,-1,VEHICLE_PARAMS_OFF, -1, -1, -1);
} //Открыть|закрыть двери

stock DoorsStatus(vehicleid)  {
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	return doors;
} //Вернёт статус дверей ( открыты , закрыты )

stock Bonnet(vehicleid,bool:status) {
	return status ? SetVehicleParamsEx(vehicleid, -1,-1,-1,-1,VEHICLE_PARAMS_ON, -1, -1) : SetVehicleParamsEx(vehicleid, -1,-1,-1,-1,VEHICLE_PARAMS_OFF, -1, -1);
} //Открыть|Закрыть капот

stock BonnetStatus(vehicleid)  {
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	return bonnet;
} //Вернёт статус капота ( открыт , закрыт )

stock Boot(vehicleid,bool:status) {
	return status ? SetVehicleParamsEx(vehicleid, -1,-1,-1,-1,-1,VEHICLE_PARAMS_ON, -1) : SetVehicleParamsEx(vehicleid, -1,-1,-1,-1,-1,VEHICLE_PARAMS_OFF, -1);
} //Открыть|Закрыть багажник

stock BootStatus(vehicleid)  {
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	return boot;
} //Вернёт статус багажника ( открыт , закрыт )
new pdistance = 1;
forward NameTimer();
Float:GetPointDistanceToPointExMorph(Float:x1,Float:y1,Float:z1,Float:x2,Float:y2,Float:z2)
{
	new Float:x, Float:y, Float:z;
	x = x1-x2;
	y = y1-y2;
	z = z1-z2;
	return floatsqroot(x*x+y*y+z*z);
}
stock SetPDistance(dista)
{
	pdistance = dista;
}
stock GetPDistance()
{
	return pdistance;
}
stock EnableDistancedNameTag(delay)
{
	SetTimer("NameTimer",delay,1);
}
forward NameTimer();
public NameTimer()
	{
		for(new i = 0;i < MAX_PLAYERS;i++)
 		{
 			if(IsPlayerConnected(i))
 			{
 				for(new q = 0;q < MAX_PLAYERS;q++)
 				{
					if(IsPlayerConnected(q))
					{
					 	new Float:p1x;
						new Float:p1y;
						new Float:p1z;
						new Float:p2x;
						new Float:p2y;
						new Float:p2z;
						if(IsPlayerConnected(i) && IsPlayerConnected(q))
						{
							GetPlayerPos(i,p1x,p1y,p1z);
						    GetPlayerPos(q,p2x,p2y,p2z);
							if(GetPointDistanceToPointExMorph(p1x,p1y,p1z,p2x,p2y,p2z) < pdistance)
							{
	    						ShowPlayerNameTagForPlayer(i,q,1);
	    					}
							else
							{
								ShowPlayerNameTagForPlayer(i,q,0);
							}
						}
					}
				}
			}
		}
	}
#define STREAMER_TYPE_OBJECT (0)
#define STREAMER_TYPE_PICKUP (1)
#define STREAMER_TYPE_CP (2)
#define STREAMER_TYPE_RACE_CP (3)
#define STREAMER_TYPE_MAP_ICON (4)
#define STREAMER_TYPE_3D_TEXT_LABEL (5)
#define STREAMER_TYPE_AREA (6)
enum
{
	E_STREAMER_ATTACHED_PLAYER,
	E_STREAMER_ATTACHED_VEHICLE,
	E_STREAMER_COLOR,
	E_STREAMER_DISTANCE,
	E_STREAMER_DRAW_DISTANCE,
	E_STREAMER_EXTRA_ID,
	E_STREAMER_INTERIOR_ID,
	E_STREAMER_MAX_X,
	E_STREAMER_MAX_Y,
	E_STREAMER_MAX_Z,
	E_STREAMER_MIN_X,
	E_STREAMER_MIN_Y,
	E_STREAMER_MIN_Z,
	E_STREAMER_MODEL_ID,
	E_STREAMER_MOVE_SPEED,
	E_STREAMER_MOVE_X,
	E_STREAMER_MOVE_Y,
	E_STREAMER_MOVE_Z,
	E_STREAMER_NEXT_X,
	E_STREAMER_NEXT_Y,
	E_STREAMER_NEXT_Z,
	E_STREAMER_PLAYER_ID,
	E_STREAMER_R_X,
	E_STREAMER_R_Y,
	E_STREAMER_R_Z,
	E_STREAMER_SIZE,
	E_STREAMER_STYLE,
	E_STREAMER_TEST_LOS,
	E_STREAMER_TYPE,
	E_STREAMER_WORLD_ID,
	E_STREAMER_X,
	E_STREAMER_Y,
	E_STREAMER_Z
}

// Natives (Settings)

native Streamer_TickRate(rate);
native Streamer_MaxItems(type, items);
native Streamer_VisibleItems(type, items);
native Streamer_CellDistance(Float:distance);
native Streamer_CellSize(Float:size);

// Natives (Updates)

native Streamer_Update(playerid);
native Streamer_UpdateEx(playerid, Float:x, Float:y, Float:z);

// Natives (Data Manipulation)

native Streamer_GetFloatData(type, {Text3D,_}:id, data, &Float:result);
native Streamer_SetFloatData(type, {Text3D,_}:id, data, Float:value);
native Streamer_GetIntData(type, {Text3D,_}:id, data);
native Streamer_SetIntData(type, {Text3D,_}:id, data, value);
native Streamer_IsInArrayData(type, {Text3D,_}:id, data, value);
native Streamer_AppendArrayData(type, {Text3D,_}:id, data, value);
native Streamer_RemoveArrayData(type, {Text3D,_}:id, data, value);
native Streamer_GetUpperBound(type);

// Natives (Miscellaneous)

native Streamer_IsItemVisible(playerid, type, {Text3D,_}:id);
native Streamer_DestroyAllVisibleItems(playerid, type);
native Streamer_CountVisibleItems(playerid, type);

// Natives (Objects)

native CreateDynamicObject(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid = -1, interiorid = -1, playerid = -1, Float:distance = 1000.0);
native DestroyDynamicObject(objectid);
native IsValidDynamicObject(objectid);
native SetDynamicObjectPos(objectid, Float:x, Float:y, Float:z);
native GetDynamicObjectPos(objectid, &Float:x, &Float:y, &Float:z);
native SetDynamicObjectRot(objectid, Float:rx, Float:ry, Float:rz);
native GetDynamicObjectRot(objectid, &Float:rx, &Float:ry, &Float:rz);
native MoveDynamicObject(objectid, Float:x, Float:y, Float:z, Float:speed);
native StopDynamicObject(objectid);
native DestroyAllDynamicObjects();
native CountDynamicObjects();

// Natives (Pickups)

native CreateDynamicPickup(modelid, type, Float:x, Float:y, Float:z, worldid = -1, interiorid = -1, playerid = -1, Float:distance = 100.0);
native DestroyDynamicPickup(pickupid);
native IsValidDynamicPickup(pickupid);
native DestroyAllDynamicPickups();
native CountDynamicPickups();

// Natives (Checkpoints)

native CreateDynamicCP(Float:x, Float:y, Float:z, Float:size, worldid = -1, interiorid = -1, playerid = -1, Float:distance = 100.0);
native DestroyDynamicCP(checkpointid);
native IsValidDynamicCP(checkpointid);
native TogglePlayerDynamicCP(playerid, checkpointid, toggle);
native TogglePlayerAllDynamicCPs(playerid, toggle);
native IsPlayerInDynamicCP(playerid, checkpointid);
native DestroyAllDynamicCPs();
native CountDynamicCPs();

// Natives (Race Checkpoints)

native CreateDynamicRaceCP(type, Float:x, Float:y, Float:z, Float:nextx, Float:nexty, Float:nextz, Float:size, worldid = -1, interiorid = -1, playerid = -1, Float:distance = 100.0);
native DestroyDynamicRaceCP(checkpointid);
native IsValidDynamicRaceCP(checkpointid);
native TogglePlayerDynamicRaceCP(playerid, checkpointid, toggle);
native TogglePlayerAllDynamicRaceCPs(playerid, toggle);
native IsPlayerInDynamicRaceCP(playerid, checkpointid);
native DestroyAllDynamicRaceCPs();
native CountDynamicRaceCPs();

// Natives (Map Icons)

native CreateDynamicMapIcon(Float:x, Float:y, Float:z, type, color, worldid = -1, interiorid = -1, playerid = -1, Float:distance = 400.0);
native DestroyDynamicMapIcon(iconid);
native IsValidDynamicMapIcon(iconid);
native DestroyAllDynamicMapIcons();
native CountDynamicMapIcons();

// Natives (3D Text Labels)

native Text3D:CreateDynamic3DTextLabel(const text[], color, Float:x, Float:y, Float:z, Float:drawdistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testlos = 0, worldid = -1, interiorid = -1, playerid = -1, Float:distance = 100.0);
native DestroyDynamic3DTextLabel(Text3D:id);
native IsValidDynamic3DTextLabel(Text3D:id);
native UpdateDynamic3DTextLabelText(Text3D:id, color, const text[]);
native DestroyAllDynamic3DTextLabels();
native CountDynamic3DTextLabels();

// Natives (Areas)

native CreateDynamicCircle(Float:x, Float:y, Float:size, worldid = -1, interiorid = -1, playerid = -1);
native CreateDynamicRectangle(Float:minx, Float:miny, Float:maxx, Float:maxy, worldid = -1, interiorid = -1, playerid = -1);
native CreateDynamicSphere(Float:x, Float:y, Float:z, Float:size, worldid = -1, interiorid = -1, playerid = -1);
native CreateDynamicCube(Float:minx, Float:miny, Float:minz, Float:maxx, Float:maxy, Float:maxz, worldid = -1, interiorid = -1, playerid = -1);
native DestroyDynamicArea(areaid);
native IsValidDynamicArea(areaid);
native TogglePlayerDynamicArea(playerid, areaid, toggle);
native TogglePlayerAllDynamicAreas(playerid, toggle);
native IsPlayerInDynamicArea(playerid, areaid);
native AttachDynamicAreaToPlayer(areaid, playerid);
native AttachDynamicAreaToVehicle(areaid, vehicleid);
native DestroyAllDynamicAreas();
native CountDynamicAreas();

// Natives (Internals)

native Streamer_AddPlayer(playerid);
native Streamer_RemovePlayer(playerid);
native Streamer_VerifyPickup(playerid, pickupid);
native Streamer_VerifyCheckpoint(playerid, type, status);
native Streamer_RegisterInterface();

// Callbacks

forward OnDynamicObjectMoved(objectid);
forward OnPlayerPickUpDynamicPickup(playerid, pickupid);
forward OnPlayerEnterDynamicCP(playerid, checkpointid);
forward OnPlayerLeaveDynamicCP(playerid, checkpointid);
forward OnPlayerEnterDynamicRaceCP(playerid, checkpointid);
forward OnPlayerLeaveDynamicRaceCP(playerid, checkpointid);
forward OnPlayerEnterDynamicArea(playerid, areaid);
forward OnPlayerLeaveDynamicArea(playerid, areaid);

// Callback Hook Section

static bool:Streamer_g_OPC = false;
static bool:Streamer_g_OPDC = false;
static bool:Streamer_g_OPPP = false;
static bool:Streamer_g_OPEC = false;
static bool:Streamer_g_OPLC = false;
static bool:Streamer_g_OPERC = false;
static bool:Streamer_g_OPLRC = false;
static bool:Streamer_g_RI = false;

public OnFilterScriptInit()
{
	if (!Streamer_g_RI)
	{
		Streamer_g_OPC = funcidx("Streamer_OnPlayerConnect") != -1;
		Streamer_g_OPDC = funcidx("Streamer_OnPlayerDisconnect") != -1;
		Streamer_g_OPPP = funcidx("Streamer_OnPlayerPickUpPickup") != -1;
		Streamer_g_OPEC = funcidx("Streamer_OnPlayerEnterCP") != -1;
		Streamer_g_OPLC = funcidx("Streamer_OnPlayerLeaveCP") != -1;
		Streamer_g_OPERC = funcidx("Streamer_OnPlayerEnterRaceCP") != -1;
		Streamer_g_OPLRC = funcidx("Streamer_OnPlayerLeaveRaceCP") != -1;
		Streamer_g_RI = true;
		Streamer_RegisterInterface();
	}
	if (funcidx("Streamer_OnFilterScriptInit") != -1)
	{
		return CallLocalFunction("Streamer_OnFilterScriptInit", "");
	}
	return 1;
}

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit Streamer_OnFilterScriptInit

forward Streamer_OnFilterScriptInit();

public OnGameModeInit()
{
	if (!Streamer_g_RI)
	{
		Streamer_g_OPC = funcidx("Streamer_OnPlayerConnect") != -1;
		Streamer_g_OPDC = funcidx("Streamer_OnPlayerDisconnect") != -1;
		Streamer_g_OPPP = funcidx("Streamer_OnPlayerPickUpPickup") != -1;
		Streamer_g_OPEC = funcidx("Streamer_OnPlayerEnterCP") != -1;
		Streamer_g_OPLC = funcidx("Streamer_OnPlayerLeaveCP") != -1;
		Streamer_g_OPERC = funcidx("Streamer_OnPlayerEnterRaceCP") != -1;
		Streamer_g_OPLRC = funcidx("Streamer_OnPlayerLeaveRaceCP") != -1;
		Streamer_g_RI = true;
		Streamer_RegisterInterface();
	}
	if (funcidx("Streamer_OnGameModeInit") != -1)
	{
		return CallLocalFunction("Streamer_OnGameModeInit", "");
	}
	return 1;
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit Streamer_OnGameModeInit

forward Streamer_OnGameModeInit();

public OnPlayerConnect(playerid)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_AddPlayer(playerid);
	}
	if (Streamer_g_OPC)
	{
		return CallLocalFunction("Streamer_OnPlayerConnect", "d", playerid);
	}
	return 1;
}

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect Streamer_OnPlayerConnect

forward Streamer_OnPlayerConnect(playerid);

public OnPlayerDisconnect(playerid, reason)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_RemovePlayer(playerid);
	}
	if (Streamer_g_OPDC)
	{
		return CallLocalFunction("Streamer_OnPlayerDisconnect", "dd", playerid, reason);
	}
	return 1;
}

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect Streamer_OnPlayerDisconnect

forward Streamer_OnPlayerDisconnect(playerid, reason);

public OnPlayerPickUpPickup(playerid, pickupid)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_VerifyPickup(playerid, pickupid);
	}
	if (Streamer_g_OPPP)
	{
		return CallLocalFunction("Streamer_OnPlayerPickUpPickup", "dd", playerid, pickupid);
	}
	return 1;
}

#if defined _ALS_OnPlayerPickUpPickup
	#undef OnPlayerPickUpPickup
#else
	#define _ALS_OnPlayerPickUpPickup
#endif
#define OnPlayerPickUpPickup Streamer_OnPlayerPickUpPickup

forward Streamer_OnPlayerPickUpPickup(playerid, pickupid);

public OnPlayerEnterCheckpoint(playerid)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_VerifyCheckpoint(playerid, STREAMER_TYPE_CP, 0);
	}
	if (Streamer_g_OPEC)
	{
		return CallLocalFunction("Streamer_OnPlayerEnterCP", "d", playerid);
	}
	return 1;
}

#if defined _ALS_OnPlayerEnterCheckpoint
	#undef OnPlayerEnterCheckpoint
#else
	#define _ALS_OnPlayerEnterCheckpoint
#endif
#define OnPlayerEnterCheckpoint Streamer_OnPlayerEnterCP

forward Streamer_OnPlayerEnterCP(playerid);

public OnPlayerLeaveCheckpoint(playerid)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_VerifyCheckpoint(playerid, STREAMER_TYPE_CP, 1);
	}
	if (Streamer_g_OPLC)
	{
		return CallLocalFunction("Streamer_OnPlayerLeaveCP", "d", playerid);
	}
	return 1;
}

#if defined _ALS_OnPlayerLeaveCheckpoint
	#undef OnPlayerLeaveCheckpoint
#else
	#define _ALS_OnPlayerLeaveCheckpoint
#endif
#define OnPlayerLeaveCheckpoint Streamer_OnPlayerLeaveCP

forward Streamer_OnPlayerLeaveCP(playerid);

public OnPlayerEnterRaceCheckpoint(playerid)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_VerifyCheckpoint(playerid, STREAMER_TYPE_RACE_CP, 0);
	}
	if (Streamer_g_OPERC)
	{
		return CallLocalFunction("Streamer_OnPlayerEnterRaceCP", "d", playerid);
	}
	return 1;
}

#if defined _ALS_OnPlayerEnterRaceCP
	#undef OnPlayerEnterRaceCheckpoint
#else
	#define _ALS_OnPlayerEnterRaceCP
#endif
#define OnPlayerEnterRaceCheckpoint Streamer_OnPlayerEnterRaceCP

forward Streamer_OnPlayerEnterRaceCP(playerid);

public OnPlayerLeaveRaceCheckpoint(playerid)
{
	if (!IsPlayerNPC(playerid))
	{
		Streamer_VerifyCheckpoint(playerid, STREAMER_TYPE_RACE_CP, 1);
	}
	if (Streamer_g_OPLRC)
	{
		return CallLocalFunction("Streamer_OnPlayerLeaveRaceCP", "d", playerid);
	}
	return 1;
}

#if defined _ALS_OnPlayerLeaveRaceCP
	#undef OnPlayerLeaveRaceCheckpoint
#else
	#define _ALS_OnPlayerLeaveRaceCP
#endif
#define OnPlayerLeaveRaceCheckpoint Streamer_OnPlayerLeaveRaceCP

forward Streamer_OnPlayerLeaveRaceCP(playerid);

// PAWN Invoke Section

forward Streamer_PublicFunction();

public Streamer_PublicFunction()
{
	new Float:value;
	CreatePickup(0, 0, 0.0, 0.0, 0.0, 0);
	CreatePlayer3DTextLabel(0, "", 0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0);
	CreatePlayerObject(0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	DeletePlayer3DTextLabel(0, PlayerText3D:0);
	DestroyPickup(0);
	DestroyPlayerObject(0, 0);
	DisablePlayerCheckpoint(0);
	DisablePlayerRaceCheckpoint(0);
	GetPlayerInterior(0);
	GetPlayerPos(0, value, value, value);
	GetPlayerState(0);
	GetPlayerVirtualWorld(0);
	GetVehiclePos(0, value, value, value);
	MovePlayerObject(0, 0, 0.0, 0.0, 0.0, 0.0);
	RemovePlayerMapIcon(0, 0);
	SetPlayerCheckpoint(0, 0.0, 0.0, 0.0, 0.0);
	SetPlayerMapIcon(0, 0, 0.0, 0.0, 0.0, 0, 0, 0);
	SetPlayerObjectPos(0, 0, 0.0, 0.0, 0.0);
	SetPlayerObjectRot(0, 0, 0.0, 0.0, 0.0);
	SetPlayerRaceCheckpoint(0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	StopPlayerObject(0, 0);
	UpdatePlayer3DTextLabelText(0, PlayerText3D:0, 0, "");
	return 1;
}

IsNumeric(const string[])
{
	for (new i = 0, j = strlen(string); i < j; i++)
	{
		if (string[i] > '9' || string[i] < '0') return 0;
	}
	return 1;
}

ReturnUser(text[], playerid = INVALID_PLAYER_ID)
{
	new pos = 0;
	while (text[pos] < 0x21) // Strip out leading spaces
	{
		if (text[pos] == 0) return INVALID_PLAYER_ID; // No passed text
		pos++;
	}
	new userid = INVALID_PLAYER_ID;
	if (IsNumeric(text[pos])) // Check whole passed string
	{
		// If they have a numeric name you have a problem (although names are checked on id failure)
		userid = strval(text[pos]);
		if (userid >=0 && userid < MAX_PLAYERS)
		{
			if(!IsPlayerConnected(userid))
			{
				/*if (playerid != INVALID_PLAYER_ID)
				{
					SendClientMessage(playerid, 0xFF0000AA, "User not connected");
				}*/
				userid = INVALID_PLAYER_ID;
			}
			else
			{
				return userid; // A player was found
			}
		}
		/*else
		{
			if (playerid != INVALID_PLAYER_ID)
			{
				SendClientMessage(playerid, 0xFF0000AA, "Invalid user ID");
			}
			userid = INVALID_PLAYER_ID;
		}
		return userid;*/
		// Removed for fallthrough code
	}
	// They entered [part of] a name or the id search failed (check names just incase)
	new len = strlen(text[pos]);
	new count = 0;
	new name[MAX_PLAYER_NAME];
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i))
		{
			GetPlayerName(i, name, sizeof (name));
			if (strcmp(name, text[pos], true, len) == 0) // Check segment of name
			{
				if (len == strlen(name)) // Exact match
				{
					return i; // Return the exact player on an exact match
					// Otherwise if there are two players:
					// Me and MeYou any time you entered Me it would find both
					// And never be able to return just Me's id
				}
				else // Partial match
				{
					count++;
					userid = i;
				}
			}
		}
	}
	if (count != 1)
	{
		if (playerid != INVALID_PLAYER_ID)
		{
			if (count)
			{
				SendClientMessage(playerid, 0xFF0000AA, "Multiple users found, please narrow earch");
			}
			else
			{
				SendClientMessage(playerid, 0xFF0000AA, "No matching user found");
			}
		}
		userid = INVALID_PLAYER_ID;
	}
	return userid; // INVALID_USER_ID for bad return
}

// Список цветов

// Список градиентных цветов
#define COLOR_GRAD1 0xB4B5B7FF
#define COLOR_GRAD2 0xBFC0C2FF
#define COLOR_GRAD3 0xCBCCCEFF
#define COLOR_GRAD4 0xD8D8D8FF
#define COLOR_GRAD5 0xE3E3E3FF
#define COLOR_GRAD6 0xF0F0F0FF

#define OBJECTIVE_COLOR 0x64000064
#define TEAM_GREEN_COLOR 0xFFFFFFAA
#define TEAM_JOB_COLOR 0xFFB6C1AA
#define TEAM_HIT_COLOR 0xFFFFFF00
#define TEAM_BLUE_COLOR 0x8D8DFF00
#define COLOR_ADD 0x63FF60AA
#define TEAM_GROVE_COLOR 0x00D900C8
#define TEAM_VAGOS_COLOR 0xFFC801C8
#define TEAM_BALLAS_COLOR 0xD900D3C8
#define TEAM_AZTECAS_COLOR 0x01FCFFC8
#define TEAM_CYAN_COLOR 0xFF8282AA
#define TEAM_ORANGE_COLOR 0xFF830000
#define TEAM_COR_COLOR 0x39393900
#define TEAM_BAR_COLOR 0x00D90000
#define TEAM_TAT_COLOR 0xBDCB9200
#define TEAM_CUN_COLOR 0xD900D300
#define TEAM_STR_COLOR 0x01FCFF00
#define TEAM_ADMIN_COLOR 0x00808000
#define COLOR_INVIS 0xAFAFAF00
#define COLOR_SPEC 0xBFC0C200
#define COLOR_XD 0xA9C4E4AA
#define COLOR_GREEN2 0x0D74FFF
#define COLOR_RED2 0xFF5F3CFF
#define COLOR_ORANGE 0xFF9900AA
#define COLOR_LIME 0x10F441AA
#define COLOR_MAGENTA 0xFF00FFFF
#define COLOR_NAVY 0x000080AA
#define COLOUR_MONEY_BAD 0xDAA520AA
#define COLOR_AQUA 0xF0F8FFAA
#define COLOR_CRIMSON 0xDC143CAA
#define COLOR_FLBLUE 0x6495EDAA
#define COLOR_BISQUE 0xFFE4C4AA
#define COLOR_BLACK 0x000000AA
#define COLOR_CHARTREUSE 0x7FFF00AA
#define COLOR_BROWN 0xA52A2AAA
#define COLOR_CORAL 0xFF7F50AA
#define COLOR_GOLD 0xB8860BAA
#define COLOR_GREENYELLOW 0xADFF2FAA
#define COLOR_INDIGO 0x4B00B0AA
#define COLOR_IVORY 0xFFFF82AA
#define COLOR_LAWNGREEN 0x7CFC00AA
#define COLOR_LIMEGREEN 0x32CD32AA
#define COLOR_MIDNIGHTBLUE 0x191970AA
#define COLOR_MAROON 0x800000AA
#define COLOR_OLIVE 0x808000AA
#define COLOR_ORANGERED 0xFF4500AA
#define COLOR_PINK 0xFFC0CBAA
#define COLOR_SPRINGGREEN 0x00FF7FAA
#define COLOR_TOMATO 0xFF6347AA
#define COLOR_YELLOWGREEN 0x9ACD32AA
#define COLOR_MEDIUMAQUA 0x83BFBFAA
#define COLOR_MEDIUMMAGENTA 0x8B008BAA
#define COLOR_BRIGHTRED 0xDC143CAA
#define COLOR_SYSTEM 0xEFEFF7AA

#define COLOR_GREY 0xAFAFAFAA
#define COLOR_GREEN 0x33AA33AA
#define COLOR_RED 0xAA3333AA
#define COLOR_LIGHTRED 0xFF6347AA
#define COLOR_LIGHTBLUE 0x33CCFFAA
#define COLOR_LIGHTGREEN 0x9ACD32AA
#define COLOR_YELLOW 0xFFFF00AA
#define COLOR_YELLOW2 0xF5DEB3AA
#define COLOR_WHITE 0xFFFFFFAA
#define COLOR_FADE1 0xE6E6E6E6
#define COLOR_FADE2 0xC8C8C8C8
#define COLOR_FADE3 0xAAAAAAAA
#define COLOR_FADE4 0x8C8C8C8C
#define COLOR_FADE5 0x6E6E6E6E
#define COLOR_PURPLE 0xC2A2DAAA
#define COLOR_DBLUE 0x2641FEAA
#define COLOR_ALLDEPT 0xFF8282AA
#define COLOR_NEWS 0xFFA500AA
#define COLOR_OOC 0xE0FFFFAA
#define COLOR_YELLOW3d 0xFFFF00FF
stock CreateStreamObject(modelid,Float:xpos,Float:ypos,Float:zpos,Float:xrot,Float:yrot,Float:zrot,Float:viewdist)
{
	return CallRemoteFunction("Core_CreateStreamObject","ifffffff",modelid,xpos,ypos,zpos,xrot,yrot,zrot,viewdist);
}
stock DestroyStreamObject(id)
{
    CallRemoteFunction("Core_DestroyStreamObject","i",id);
}
stock GetStreamObjectPos(id,&Float:xpos,&Float:ypos,&Float:zpos)
{
    xpos = Float:CallRemoteFunction("Core_GetXPos","i",id);
    ypos = Float:CallRemoteFunction("Core_GetYPos","i",id);
    zpos = Float:CallRemoteFunction("Core_GetZPos","i",id);
}
stock GetStreamObjectRot(id,&Float:xrot,&Float:yrot,&Float:zrot)
{
    xrot = Float:CallRemoteFunction("Core_GetXRot","i",id);
    yrot = Float:CallRemoteFunction("Core_GetYRot","i",id);
    zrot = Float:CallRemoteFunction("Core_GetZRot","i",id);
}
stock SetStreamObjectPos(id,Float:xpos,Float:ypos,Float:zpos)
{
    CallRemoteFunction("Core_SetStreamObjectPos","ifff",id,xpos,ypos,zpos);
}
stock SetStreamObjectRot(id,Float:xrot,Float:yrot,Float:zrot)
{
    CallRemoteFunction("Core_SetStreamObjectRot","ifff",id,xrot,yrot,zrot);
}
stock AttachStreamObjectToPlayer(id,playerid,Float:xoff,Float:yoff,Float:zoff,Float:xrot,Float:yrot,Float:zrot)
{
    CallRemoteFunction("Core_AttachStreamObjectToPlayer","iiffffff",id,playerid,xoff,yoff,zoff,xrot,yrot,zrot);
}
stock MoveStreamObject(id,Float:movx,Float:movy,Float:movz,Float:speed)
{
    CallRemoteFunction("Core_MoveStreamObject","iffff",id,movx,movy,movz,speed);
}
stock MidoStreamDisconnect(playerid)
{
    CallRemoteFunction("Core_MidoStreamDisconnect","i",playerid);
}
const
    INI_MAX_OPENED_FILES    = 30,        // максимум, открытых одновременно, файлов
    INI_MAX_FILE_SIZE       = 65536,    // байт,        макс. размер файла
    INI_MAX_FILENAME_SIZE   = 128,      // символов,    макс. размер пути к файлу
    INI_MAX_KEYS_IN_FILE    = 2048,     // максимум ключей в открытом файле
    INI_INTEGER_SIZE        = 12,       // размер строки с целочисленным значением
    INI_FLOAT_SIZE          = 40,       // размер строки с дробным числовым значением
    INI_STRING_DELIMITER    = '\n',     // разделитель строк
    INI_DELIMITER           = '=',      // разделитель ключа и значения
    INI_OK                  =  0,       // функция успешно выполнена
    INI_FILE_NOT_FOUND      = -1,       // файл не найден по указанному пути
    INI_FILE_ALREADY_EXIST  = -2,       // файл не найден по указанному пути
    INI_TOO_LARGE_FILE      = -3,       // размер файла превысил допустимый лимит
    INI_WRONG_PATH_SIZE     = -4,       // неправильный размер пути к файлу
    INI_READ_ERROR          = -5,       // ошибка чтения файла
    INI_WRITE_ERROR         = -6,       // ошибка при записи в файл
    INI_NO_FREE_SLOT        = -7,       // нет свободного слота для открытия файла
    INI_WRONG_SLOT          = -8,       // указан неверный слот открытого файла
    INI_KEY_NOT_FOUND       = -9,       // ключ в открытом файле не найден
    INI_WRONG_RETURN_SIZE   = -10,      // размер строки, в которую будет помещено значение ключа - указан неверно (<= 0)
    cellbytes = cellbits / charbits;    // кол-во байт в одной ячейке
#define INI_SPACE_CHAR  ' ', '\t'                       // строковые пробельные символы
#define INI_KEY_STARTS  ' ', '\t', '\r', '\n', '\0'     // символы, перед началом ключа
#define INI_STRING_ENDS '\r', '\n', '\0'                // символы, завершающие значение
#define INI_NUMBER_ENDS ' ', '\t', '\r', '\n', '\0'     // символы, завершающие численное значение
static stock
    _ini_nSlotUsed      [ INI_MAX_OPENED_FILES ],                               // флаг: занят ли слот
    _ini_nFileChanged   [ INI_MAX_OPENED_FILES ],                               // флаг: был ли изменен файл
    _ini_nFileBusy      [ INI_MAX_OPENED_FILES ],                               // флаг: изменяется ли в данный момент содержимое файла
    _ini_nFileSize      [ INI_MAX_OPENED_FILES ],                               // размер открытого файла
    _ini_nDelimPos      [ INI_MAX_OPENED_FILES ] [ INI_MAX_KEYS_IN_FILE ],      // список позиций INI_DELIMITER
    _ini_nKeysCount     [ INI_MAX_OPENED_FILES ],                               // кол-во ключей открытого файла
    _ini_szFilePath     [ INI_MAX_OPENED_FILES ] [ INI_MAX_FILENAME_SIZE ],     // путь к файлу
    _ini_szFileContent  [ INI_MAX_OPENED_FILES ] [ INI_MAX_FILE_SIZE char ];    // контент файла
stock ini_createFile ( szFilePath[], szDefaultContent[] = "" )
{
    new nFileNameSize = strlen( szFilePath ); // узнаем размер пути
    if ( nFileNameSize <= 0 || nFileNameSize >= INI_MAX_FILENAME_SIZE )
        return INI_WRONG_PATH_SIZE; // вернем код ошибки
    if ( fexist( szFilePath ) ) // если файл уже существует
        return INI_FILE_ALREADY_EXIST; // вернем код ошибки - файл уже существует
    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // перебор всех слотов ОЗУ
        if // если уже есть такой открытый файл
        (
                _ini_nSlotUsed[slot] != 0
            &&  strcmp( szFilePath, _ini_szFilePath[slot], false ) == 0
        )
            return INI_FILE_ALREADY_EXIST; // вернем код ошибки - файл уже существует
    new nFileSize = strlen( szDefaultContent ); // узнаем размер контентa файла по умолчанию
    if ( nFileSize < 0 || nFileSize >= INI_MAX_FILE_SIZE )
        return INI_TOO_LARGE_FILE; // вернем код ошибки
    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // перебор всех слотов ОЗУ
    {
        if ( _ini_nSlotUsed[slot] != 1 ) // если слот найден
        {
            _ini_nSlotUsed[slot]        = 1; // застолбим найденное место
            _ini_nFileChanged[slot]     = 0; // файл не был изменен
            _ini_nFileSize[slot]        = nFileSize; // скопируем размер файла в слот
            _ini_nKeysCount[slot]       = 0; // кол-во ключей выставим 0
            for ( new i = 0;  i < nFileSize && i < INI_MAX_FILE_SIZE;  i++ )
            {
                if (
                        szDefaultContent[i] == INI_DELIMITER // если символ это INI_DELIMITER
                    &&  _ini_nKeysCount[slot] < INI_MAX_KEYS_IN_FILE // и лимит ключей еще не исчерпан
                ) {
                    _ini_nDelimPos[slot][ _ini_nKeysCount[slot] ] = i; // добавим еще одну позицию INI_DELIMITER
                    _ini_nKeysCount[slot]++; // увеличим кол-во найденных ключей
                }
                _ini_szFileContent[slot]{i} = szDefaultContent[i];
            }
            _ini_szFileContent[slot]{nFileSize} = 0; // символ конца строки для контента
            memcpy( _ini_szFilePath[slot], szFilePath, 0, nFileNameSize * cellbytes );
            _ini_szFilePath[slot][nFileNameSize] = 0; // символ конца строки для пути
            return slot; // вернем ИД слота
        }
    }
    return INI_NO_FREE_SLOT; // вернем код ошибки
}
stock ini_openFile ( szFilePath[] )
{
    new nFileNameSize = strlen( szFilePath ); // узнаем размер пути
    if ( nFileNameSize <= 0 || nFileNameSize >= INI_MAX_FILENAME_SIZE )
        return INI_WRONG_PATH_SIZE; // вернем код ошибки
    if ( ! fexist( szFilePath ) ) // если файл не найден
        return INI_FILE_NOT_FOUND; // вернем код ошибки
    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // перебор всех слотов ОЗУ
        if // если уже есть такой открытый файл
        (
                _ini_nSlotUsed[slot] != 0
            &&  strcmp( szFilePath, _ini_szFilePath[slot], false ) == 0
        )
            return slot; // просто вернем его слот
    new File: pFile = fopen( szFilePath, io_read ); // пытаемся открыть файл для чтения
    if ( ! pFile ) // если файл не открылся
        return INI_READ_ERROR; // вернем код ошибки
    new nFileSize = flength( pFile ); // узнаем размер файла
    if ( nFileSize >= INI_MAX_FILE_SIZE ) // если размер файла слишком большой
    {
        fclose(pFile); // закроем файл
        return INI_TOO_LARGE_FILE; // вернем код ошибки
    }
    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // перебор всех слотов ОЗУ
    {
        if ( _ini_nSlotUsed[slot] != 1 ) // если слот найден
        {
            _ini_nSlotUsed[slot]        = 1; // застолбим найденное место
            _ini_nFileChanged[slot]     = 0; // файл не был изменен
            _ini_nFileSize[slot]        = nFileSize; // скопируем размер файла в слот
            _ini_nKeysCount[slot]       = 0; // кол-во ключей выставим 0
            for ( new i = 0, symbol; i < nFileSize && i < INI_MAX_FILE_SIZE; i++ )
            {
                symbol = fgetchar( pFile, 0, false ); // читаем из файла следующий символ

                if (
                        symbol == INI_DELIMITER // если символ это INI_DELIMITER
                    &&  _ini_nKeysCount[slot] < INI_MAX_KEYS_IN_FILE // и лимит ключей еще не исчерпан
                ) {
                    _ini_nDelimPos[slot][ _ini_nKeysCount[slot] ] = i; // добавим еще одну позицию INI_DELIMITER
                    _ini_nKeysCount[slot]++; // увеличим кол-во найденных ключей
                }
                _ini_szFileContent[slot]{i} = symbol;
            }
            _ini_szFileContent[slot]{nFileSize} = 0; // символ конца строки для контента
            fclose(pFile); // закроем файл
            memcpy( _ini_szFilePath[slot], szFilePath, 0, nFileNameSize * cellbytes );
            _ini_szFilePath[slot][nFileNameSize] = 0; // символ конца строки для пути
            return slot; // вернем ИД слота
        }
    }
    fclose(pFile); // закроем файл
    return INI_NO_FREE_SLOT; // вернем код ошибки
}
stock ini_closeFile ( nFilePointer )
{
    if
    (   // если ИД открытого файла указан верно
            nFilePointer >= 0
        &&  nFilePointer < INI_MAX_OPENED_FILES
        &&  _ini_nSlotUsed[nFilePointer] != 0
    )
    {
        if ( _ini_nFileChanged[nFilePointer] != 0 ) // если файл был изменен
        {
            new File: pFile = fopen( _ini_szFilePath[nFilePointer], io_write ); // пытаемся открыть файл для записи
            if ( ! pFile ) // если файл не открылся
                return INI_WRITE_ERROR; // вернем код ошибки
            for ( new i = 0;  i < _ini_nFileSize[nFilePointer];  i++ )
       	    fputchar( pFile, _ini_szFileContent[nFilePointer]{i}, false );
            fclose(pFile); // закроем файл
        }
        _ini_nSlotUsed[nFilePointer] = 0; // освободить слот открытого файла
        return INI_OK; // вернуть код об успешном выполнении функции
    }
    return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
}
stock ini_getString ( nFilePointer, szKeyName[], szReturnValue[], nSizeOfReturn = sizeof(szReturnValue) )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    if ( nSizeOfReturn <= 0 ) // если по какой-то причине размер возвращаемого значения указан/рассчитан как 0
        return INI_WRONG_RETURN_SIZE;
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа

    if ( nKeyLen <= 0 ) // если указан пустой ключ
        return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found = 0;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ найден в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            for ( found = 0;  found < nSizeOfReturn;  found++, curFilePos++ )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_STRING_ENDS : // если это символ конца значения
                    {
                        szReturnValue[found] = 0; // запишем символ конца строки
                        break; // конец копирования
                    }
                    default :
                        // копируем символ из файла в szReturnValue
                        szReturnValue[found] = _ini_szFileContent[nFilePointer]{curFilePos};
                }
            }
            szReturnValue[nSizeOfReturn - 1] = 0; // на всякий случай обрежем правильно строку
            return INI_OK;
        }
    }
    return INI_KEY_NOT_FOUND;
}
stock ini_getInteger ( nFilePointer, szKeyName[], & nReturnValue )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа
    if ( nKeyLen <= 0 ) // если указан пустой ключ
        return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found = 0;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ найден в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            new strValue[INI_INTEGER_SIZE]; // временная строка для численного значения
            for ( found = 0;  found < INI_INTEGER_SIZE;  found++, curFilePos++ )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS : // если это символ конца численного значения
                    {
                        strValue[found] = 0; // запишем символ конца строки
                        break; // конец копирования
                    }
                    default :
                        // копируем символ из файла в strValue
                        strValue[found] = _ini_szFileContent[nFilePointer]{curFilePos};
                }
            }
            strValue[INI_INTEGER_SIZE - 1] = 0; // на всякий случай обрежем правильно строку
            nReturnValue = strval(strValue); // запишем в nReturnValue численное значение ключа
            return INI_OK;
        }
    }
    return INI_KEY_NOT_FOUND;
}
stock ini_getFloat ( nFilePointer, szKeyName[], & Float: fReturnValue )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа
    if ( nKeyLen <= 0 ) // если указан пустой ключ
        return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found = 0;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ найден в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            new strValue[INI_FLOAT_SIZE]; // временная строка для дробного значения
            for ( found = 0;  found < INI_INTEGER_SIZE;  found++, curFilePos++ )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS : // если это символ конца численного значения
                    {
                        strValue[found] = 0; // запишем символ конца строки
                        break; // конец копирования
                    }
                    default :
                        // копируем символ из файла в strValue
                        strValue[found] = _ini_szFileContent[nFilePointer]{curFilePos};
                }
            }
            strValue[INI_FLOAT_SIZE - 1] = 0; // на всякий случай обрежем правильно строку
            fReturnValue = floatstr(strValue); // запишем в fReturnValue дробное значение ключа
            return INI_OK;
        }
    }
    return INI_KEY_NOT_FOUND;
}
stock ini_setString ( nFilePointer, szKeyName[], szKeyValue[] )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа
    if ( nKeyLen <= 0 ) // если указан пустой ключ
        return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ НАЙДЕН в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            new nValueLen = strlen(szKeyValue); // узнаем размер значения указанного ключа
            if ( ( curFilePos + nValueLen ) >= INI_MAX_FILE_SIZE )
                return INI_TOO_LARGE_FILE; // вернем код ошибки о переполнении файла
            new fileValueStartPos = curFilePos; // сохраним позицию начала значения
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_STRING_ENDS :  break; // если это конец строки - стоп
                    default :               continue; // если это другой символ, перейдем к след. символу
                }
            }
            new filePosOffset = nValueLen - ( /*текущ длина значения*/ curFilePos - fileValueStartPos );
            if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
                return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
            _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
            if ( filePosOffset != 0 ) // если длины старого и нового значений разные
            {
                if ( filePosOffset < 0 ) // если длина нового значения меньше
                {
                    for // копируем символы, стоящие после текущего значения на их новые места,
                    ( // которые теперь будут ближе к началу файла
                        ;
                        curFilePos < _ini_nFileSize[nFilePointer];
                        curFilePos++
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }
                else if ( filePosOffset > 0 ) // если длина нового значения больше
                {
                    new fileValueEndPos = curFilePos;
                    for // копируем символы, стоящие после текущего значения на их новые места,
                    ( // начиная с конца файла
                        curFilePos = _ini_nFileSize[nFilePointer] - 1;
                        curFilePos >= fileValueEndPos;
                        curFilePos--
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }
                for ( kPos++;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                    _ini_nDelimPos[nFilePointer][kPos] += filePosOffset;
                _ini_nFileSize[nFilePointer] += filePosOffset; // изменим размер файла
                _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // конец строки для конца файла
            }
            curFilePos = fileValueStartPos; // текущ поз файла = поз начала старого значения
            for // запишем новое значение поверх старого
            (
                new valuePos = 0;
                valuePos < nValueLen;
                curFilePos++, valuePos++
            )
                _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[valuePos];
            _ini_nFileChanged[nFilePointer]     = 1; // флаг: файл изменен = 1
            _ini_nFileBusy[nFilePointer]        = 0; // флаг: файл изменяется = 0
            return INI_OK;
        }
    }
    new nValueLen = strlen(szKeyValue); // узнаем размер значения ключа
    // если будущий размер файла превышает лимит
    if ( ( _ini_nFileSize[nFilePointer] + 5 + nKeyLen + nValueLen ) >= INI_MAX_FILE_SIZE )
        return INI_TOO_LARGE_FILE; // вернем код ошибки о переполнении файла
    if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
        return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
    _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
    if // если последний символ файла не является разделителем строк
    (
            _ini_nFileSize[nFilePointer] > 0
        &&  _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] - 1 } != INI_STRING_DELIMITER
    )
    {
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] }        = '\r';
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] + 1 }    = INI_STRING_DELIMITER;
        _ini_nFileSize[nFilePointer] += 2; // увеличим размер файла на 2
    }
    new curFilePos, strPos; // будут временным хранилищами позиций в файле/ключе/значении
    for // добавим посимвольно имя ключа в файл
    (
        curFilePos = _ini_nFileSize[nFilePointer], strPos = 0;
        strPos < nKeyLen;
        curFilePos++, strPos++
    )
    _ini_szFileContent[nFilePointer]{curFilePos} = szKeyName[strPos];
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // после ключа добавим пробел
    _ini_nDelimPos[nFilePointer][ _ini_nKeysCount[nFilePointer] ]   = curFilePos; // добавим новую позицию INI_DELIMITER
    _ini_nKeysCount[nFilePointer]++; // кол-во ключей в файле +1
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = INI_DELIMITER; // после пробела выше добавим INI_DELIMITER
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // после INI_DELIMITER добавим пробел
    for ( strPos = 0;  strPos < nValueLen;  curFilePos++, strPos++ )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[strPos];
    _ini_szFileContent[nFilePointer]{curFilePos}    = 0; // добавим символ конца строки
    _ini_nFileSize[nFilePointer]                    = curFilePos; // изменим размер файла
    _ini_nFileChanged[nFilePointer]                 = 1; // флаг: файл изменен = 1
    _ini_nFileBusy[nFilePointer]                    = 0; // флаг: файл изменяется = 0

    return INI_OK;
}
stock ini_setInteger ( nFilePointer, szKeyName[], nKeyValue )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
    return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа
    if ( nKeyLen <= 0 ) // если указан пустой ключ
    return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ НАЙДЕН в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            new szKeyValue[INI_INTEGER_SIZE]; // создадим строку для хранения целочисленного значения
            format( szKeyValue, INI_INTEGER_SIZE, "%i", nKeyValue );
            new nValueLen = strlen(szKeyValue); // узнаем размер значения указанного ключа
            if ( ( curFilePos + nValueLen ) >= INI_MAX_FILE_SIZE )
            return INI_TOO_LARGE_FILE; // вернем код ошибки о переполнении файла
            new fileValueStartPos = curFilePos; // сохраним позицию начала значения
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS :  break; // если это конец числа - стоп
                    default :               continue; // если это другой символ, перейдем к след. символу
                }
            }
            new filePosOffset = nValueLen - ( /*текущ длина значения*/ curFilePos - fileValueStartPos );
            if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
                return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
            _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
            if ( filePosOffset != 0 ) // если длины старого и нового значений разные
            {
                if ( filePosOffset < 0 ) // если длина нового значения меньше
                {
                    for // копируем символы, стоящие после текущего значения на их новые места,
                    ( // которые теперь будут ближе к началу файла
                        ;
                        curFilePos < _ini_nFileSize[nFilePointer];
                        curFilePos++
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                        _ini_szFileContent[nFilePointer]{curFilePos};
                }
                else if ( filePosOffset > 0 ) // если длина нового значения больше
                {
                    new fileValueEndPos = curFilePos;
                    for // копируем символы, стоящие после текущего значения на их новые места,
                    ( // начиная с конца файла
                        curFilePos = _ini_nFileSize[nFilePointer] - 1;
                        curFilePos >= fileValueEndPos;
                        curFilePos--
                    )
					_ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
     				_ini_szFileContent[nFilePointer]{curFilePos};
                }
                for ( kPos++;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                _ini_nDelimPos[nFilePointer][kPos] += filePosOffset;
                _ini_nFileSize[nFilePointer] += filePosOffset; // изменим размер файла
                _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // конец строки для конца файла
            }
            curFilePos = fileValueStartPos; // текущ поз файла = поз начала старого значения
            for // запишем новое значение поверх старого
            (
                new valuePos = 0;
                valuePos < nValueLen;
                curFilePos++, valuePos++
            )
            _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[valuePos];
            _ini_nFileChanged[nFilePointer]     = 1; // флаг: файл изменен = 1
            _ini_nFileBusy[nFilePointer]        = 0; // флаг: файл изменяется = 0
            return INI_OK;
        }
    }
    new szKeyValue[INI_INTEGER_SIZE]; // создадим строку для хранения целочисленного значения
    format( szKeyValue, INI_INTEGER_SIZE, "%i", nKeyValue );
    new nValueLen = strlen(szKeyValue); // узнаем размер значения ключа
    if ( ( _ini_nFileSize[nFilePointer] + 5 + nKeyLen + nValueLen ) >= INI_MAX_FILE_SIZE )
    return INI_TOO_LARGE_FILE; // вернем код ошибки о переполнении файла
    if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
    return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
    _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
    if // если последний символ файла не является разделителем строк
    (
        _ini_nFileSize[nFilePointer] > 0
        &&  _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] - 1 } != INI_STRING_DELIMITER
    )
    {
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] }        = '\r';
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] + 1 }    = INI_STRING_DELIMITER;
        _ini_nFileSize[nFilePointer] += 2; // увеличим размер файла на 2
    }
    new curFilePos, strPos; // будут временным хранилищами позиций в файле/ключе/значении
    for // добавим посимвольно имя ключа в файл
    (
        curFilePos = _ini_nFileSize[nFilePointer], strPos = 0;
        strPos < nKeyLen;
        curFilePos++, strPos++
    )
    _ini_szFileContent[nFilePointer]{curFilePos} = szKeyName[strPos];
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // после ключа добавим пробел
    _ini_nDelimPos[nFilePointer][ _ini_nKeysCount[nFilePointer] ]   = curFilePos; // добавим новую позицию INI_DELIMITER
    _ini_nKeysCount[nFilePointer]++; // кол-во ключей в файле +1
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = INI_DELIMITER; // после пробела выше добавим INI_DELIMITER
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // после INI_DELIMITER добавим пробел
    for ( strPos = 0;  strPos < nValueLen;  curFilePos++, strPos++ )
    _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[strPos];
    _ini_szFileContent[nFilePointer]{curFilePos}    = 0; // добавим символ конца строки
    _ini_nFileSize[nFilePointer]                    = curFilePos; // изменим размер файла
    _ini_nFileChanged[nFilePointer]                 = 1; // флаг: файл изменен = 1
    _ini_nFileBusy[nFilePointer]                    = 0; // флаг: файл изменяется = 0
    return INI_OK;
}
stock ini_setFloat ( nFilePointer, szKeyName[], Float: fKeyValue )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
    return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа
    if ( nKeyLen <= 0 ) // если указан пустой ключ
    return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
            	curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ НАЙДЕН в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            new szKeyValue[INI_FLOAT_SIZE]; // создадим строку для хранения дробного значения
            format( szKeyValue, INI_FLOAT_SIZE, "%f", fKeyValue ); // поместим строковое значение fKeyValue в szKeyValue
            new nValueLen = strlen(szKeyValue); // узнаем размер значения указанного ключа
            if ( ( curFilePos + nValueLen ) >= INI_MAX_FILE_SIZE )
            return INI_TOO_LARGE_FILE; // вернем код ошибки о переполнении файла
            new fileValueStartPos = curFilePos; // сохраним позицию начала значения
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS :  break; // если это конец числа - стоп
                    default :               continue; // если это другой символ, перейдем к след. символу
                }
            }
            new filePosOffset = nValueLen - ( /*текущ длина значения*/ curFilePos - fileValueStartPos );
            if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
            return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
            _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
            if ( filePosOffset != 0 ) // если длины старого и нового значений разные
            {
                if ( filePosOffset < 0 ) // если длина нового значения меньше
                {
                    for // копируем символы, стоящие после текущего значения на их новые места,
                    ( // которые теперь будут ближе к началу файла
                        ;
                        curFilePos < _ini_nFileSize[nFilePointer];
                        curFilePos++
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                        _ini_szFileContent[nFilePointer]{curFilePos};
                }
                else if ( filePosOffset > 0 ) // если длина нового значения больше
                {
                    new fileValueEndPos = curFilePos;
                    for // копируем символы, стоящие после текущего значения на их новые места,
                    ( // начиная с конца файла
                        curFilePos = _ini_nFileSize[nFilePointer] - 1;
                        curFilePos >= fileValueEndPos;
                        curFilePos--
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                        _ini_szFileContent[nFilePointer]{curFilePos};
                }
                for ( kPos++;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                _ini_nDelimPos[nFilePointer][kPos] += filePosOffset;
                _ini_nFileSize[nFilePointer] += filePosOffset; // изменим размер файла
                _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // конец строки для конца файла
            }
            curFilePos = fileValueStartPos; // текущ поз файла = поз начала старого значения
            for // запишем новое значение поверх старого
            (
                new valuePos = 0;
                valuePos < nValueLen;
                curFilePos++, valuePos++
            )
            _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[valuePos];
            _ini_nFileChanged[nFilePointer]     = 1; // флаг: файл изменен = 1
            _ini_nFileBusy[nFilePointer]        = 0; // флаг: файл изменяется = 0
            return INI_OK;
        }
    }
    new szKeyValue[INI_FLOAT_SIZE]; // создадим строку для хранения дробного значения
    format( szKeyValue, INI_FLOAT_SIZE, "%f", fKeyValue ); // поместим строковое значение fKeyValue в szKeyValue
    new nValueLen = strlen(szKeyValue); // узнаем размер значения ключа
    if ( ( _ini_nFileSize[nFilePointer] + 5 + nKeyLen + nValueLen ) >= INI_MAX_FILE_SIZE )
    return INI_TOO_LARGE_FILE; // вернем код ошибки о переполнении файла
    if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
    return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
    _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
    if // если последний символ файла не является разделителем строк
    (
            _ini_nFileSize[nFilePointer] > 0
        &&  _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] - 1 } != INI_STRING_DELIMITER
    )
    {
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] }        = '\r';
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] + 1 }    = INI_STRING_DELIMITER;
        _ini_nFileSize[nFilePointer] += 2; // увеличим размер файла на 2
    }
    new curFilePos, strPos; // будут временным хранилищами позиций в файле/ключе/значении
    for // добавим посимвольно имя ключа в файл
    (
        curFilePos = _ini_nFileSize[nFilePointer], strPos = 0;
        strPos < nKeyLen;
        curFilePos++, strPos++
    )
    _ini_szFileContent[nFilePointer]{curFilePos} = szKeyName[strPos];
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // после ключа добавим пробел
    _ini_nDelimPos[nFilePointer][ _ini_nKeysCount[nFilePointer] ]   = curFilePos; // добавим новую позицию INI_DELIMITER
    _ini_nKeysCount[nFilePointer]++; // кол-во ключей в файле +1
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = INI_DELIMITER; // после пробела выше добавим INI_DELIMITER
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // после INI_DELIMITER добавим пробел
    for ( strPos = 0;  strPos < nValueLen;  curFilePos++, strPos++ )
    _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[strPos];
    _ini_szFileContent[nFilePointer]{curFilePos}    = 0; // добавим символ конца строки
    _ini_nFileSize[nFilePointer]                    = curFilePos; // изменим размер файла
    _ini_nFileChanged[nFilePointer]                 = 1; // флаг: файл изменен = 1
    _ini_nFileBusy[nFilePointer]                    = 0; // флаг: файл изменяется = 0
    return INI_OK;
}
stock ini_removeKey ( nFilePointer, szKeyName[] )
{
    if // если ИД открытого файла указан неверно
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
    return INI_WRONG_SLOT; // вернуть код: неверный указатель на открытый файл
    new nKeyLen = strlen(szKeyName); // узнаем длину имени указанного ключа
    if ( nKeyLen <= 0 ) // если указан пустой ключ
    return INI_KEY_NOT_FOUND;
    for // перебор и сравнение всех ключей файла с указанным ключом
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // флаг, найдена ли позиция конца ключа = 0
        for // ищем позицию конца ключа
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // текущ. поз. файла = поз. текущ. INI_DELIMITER - 1
            curFilePos >= 0; // продолжать пока поз. файла >= 0
            curFilePos-- // после каждого повтора текущ. поз. файла -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // узнаем что за символ в текущ. поз. файла
            {
                case INI_SPACE_CHAR :           continue; // если это пробельный символ, перейдем к пред. символу файла
                case INI_STRING_DELIMITER :     break; // если это конец строки
                default : // если это другой символ
                {
                    found = 1; // позиция конца ключа найдена
                    break; // конец цикла
                }
            }
        }
        if ( found != 1 ) continue;
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // если поз файла стала < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // если символы из ключей не равны
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // если символ из ключа это INI_STRING_DELIMITER
            )
            {
                found = 0; // флаг, ключ не найден
                break; // конец сравнения
            }
        }
        if ( found != 0 ) // если указанный ключ НАЙДЕН в файле
        {
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }
            new keyStartPos = ( curFilePos <= 0 ) ? 0 : curFilePos + 1;
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // если это пробельный символ, перейдем к след. символу
                    default :               break; // если это другой символ
                }
            }
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_STRING_ENDS :  break; // если это конец строки - стоп
                    default :               continue; // если это другой символ, перейдем к след. символу
                }
            }
            new filePosOffset = keyStartPos - curFilePos;
            if ( _ini_nFileBusy[nFilePointer] != 0 ) // если прямо сейчас файл изменяется другой функцией
            return INI_WRITE_ERROR; // вернем код ошибки при записи в файл
            _ini_nFileBusy[nFilePointer] = 1; // флаг: файл изменяется = 1
            for ( ; curFilePos <= _ini_nFileSize[nFilePointer]; curFilePos++ )
            _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
            _ini_szFileContent[nFilePointer]{curFilePos};
            for ( ;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
            _ini_nDelimPos[nFilePointer][kPos] = _ini_nDelimPos[nFilePointer][kPos + 1] + filePosOffset;
            _ini_nKeysCount[nFilePointer]--;
            _ini_nFileSize[nFilePointer] += filePosOffset; // изменим размер файла
            _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // конец строки для конца файла
            _ini_nFileChanged[nFilePointer]     = 1; // флаг: файл изменен = 1
            _ini_nFileBusy[nFilePointer]        = 0; // флаг: файл изменяется = 0
            return INI_OK;
        }
    }
    return INI_KEY_NOT_FOUND;
}
stock ini_getErrorInfo ( nErrorCode )
{
    new error_ms[200] = "Ошибок нет";

    if ( nErrorCode >= 0 ) return error_ms;

    switch ( nErrorCode )
    {
        case INI_FILE_NOT_FOUND :     error_ms = "INI файл, указанный в `ini_openFile`, не существует, возможно, указан неверный путь к нему";
        case INI_FILE_ALREADY_EXIST : error_ms = "INI файл, указанный в `ini_createFile`, уже существует, укажите другое имя/путь к файлу";
        case INI_TOO_LARGE_FILE :     error_ms = "Невозможно добавить новый ключ в INI файл, т.к. размер INI файла превысит допустимый лимит в " #INI_MAX_FILE_SIZE " символ(ов), или будет превышен лимит в " #INI_MAX_KEYS_IN_FILE " ключей";
        case INI_WRONG_PATH_SIZE :    error_ms = "Длина пути к INI файлу, указанного в `ini_openFile` / `ini_createFile`, превышает лимит в " #INI_MAX_FILENAME_SIZE " символ(ов)";
        case INI_READ_ERROR :         error_ms = "Ошибка при чтении INI файла с диска, возможно, файл занят другим процессом";
        case INI_WRITE_ERROR :        error_ms = "Ошибка при записи содержимого INI файла на диск, возможно, файл занят другим процессом";
        case INI_NO_FREE_SLOT :       error_ms = "Лимит в " #INI_MAX_OPENED_FILES " одновременно открытых INI файлов исчерпан, поэтому открыть/создать еще 1 файл нельзя, пока не закрыт хоть 1 из открытых файлов";
        case INI_WRONG_SLOT :         error_ms = "Указан неверный ID открытого INI файла, возможно, при открытии INI файла произошла ошибка, поэтому вместо ID файла вы получили код ошибки";
        case INI_KEY_NOT_FOUND :      error_ms = "Указанный ключ не найден в открытом INI файле";
        case INI_WRONG_RETURN_SIZE :  error_ms = "При попытке чтения строкового значения из INI файла, последний параметр `nSizeOfReturn` в `ini_getString` оказался <= 0, укажите вручную значение > 0 для него";

        default:                      error_ms = "Неизвестная ошибка";
    }

    return error_ms;
}
#if defined _Float_included
  #endinput
#endif
#define _Float_included
#pragma library Float

/* Different methods of rounding */
enum floatround_method {
  floatround_round,
  floatround_floor,
  floatround_ceil,
  floatround_tozero,
  floatround_unbiased
}
enum anglemode {
  radian,
  degrees,
  grades
}

/**************************************************/
/* Convert an integer into a floating point value */
native Float:float(value);

/**************************************************/
/* Convert a string into a floating point value */
native Float:floatstr(const string[]);

/**************************************************/
/* Multiple two floats together */
native Float:floatmul(Float:oper1, Float:oper2);

/**************************************************/
/* Divide the dividend float by the divisor float */
native Float:floatdiv(Float:dividend, Float:divisor);

/**************************************************/
/* Add two floats together */
native Float:floatadd(Float:oper1, Float:oper2);

/**************************************************/
/* Subtract oper2 float from oper1 float */
native Float:floatsub(Float:oper1, Float:oper2);

/**************************************************/
/* Return the fractional part of a float */
native Float:floatfract(Float:value);

/**************************************************/
/* Round a float into a integer value */
native floatround(Float:value, floatround_method:method=floatround_round);

/**************************************************/
/* Compare two integers. If the two elements are equal, return 0.
   If the first argument is greater than the second argument, return 1,
   If the first argument is less than the second argument, return -1. */
native floatcmp(Float:oper1, Float:oper2);

/**************************************************/
/* Return the square root of the input value, same as floatpower(value, 0.5) */
native Float:floatsqroot(Float:value);

/**************************************************/
/* Return the value raised to the power of the exponent */
native Float:floatpower(Float:value, Float:exponent);

/**************************************************/
/* Return the logarithm */
native Float:floatlog(Float:value, Float:base=10.0);

/**************************************************/
/* Return the sine, cosine or tangent. The input angle may be in radian,
   degrees or grades. */
native Float:floatsin(Float:value, anglemode:mode=radian);
native Float:floatcos(Float:value, anglemode:mode=radian);
native Float:floattan(Float:value, anglemode:mode=radian);

/**************************************************/
/* Return the absolute value */
native Float:floatabs(Float:value);


/**************************************************/
#pragma rational Float

/* user defined operators */
native Float:operator*(Float:oper1, Float:oper2) = floatmul;
native Float:operator/(Float:oper1, Float:oper2) = floatdiv;
native Float:operator+(Float:oper1, Float:oper2) = floatadd;
native Float:operator-(Float:oper1, Float:oper2) = floatsub;
native Float:operator=(oper) = float;

stock Float:operator++(Float:oper)
    return oper+1.0;

stock Float:operator--(Float:oper)
    return oper-1.0;

stock Float:operator-(Float:oper)
    return oper^Float:cellmin;                  /* IEEE values are sign/magnitude */

stock Float:operator*(Float:oper1, oper2)
    return floatmul(oper1, float(oper2));       /* "*" is commutative */

stock Float:operator/(Float:oper1, oper2)
    return floatdiv(oper1, float(oper2));

stock Float:operator/(oper1, Float:oper2)
    return floatdiv(float(oper1), oper2);

stock Float:operator+(Float:oper1, oper2)
    return floatadd(oper1, float(oper2));       /* "+" is commutative */

stock Float:operator-(Float:oper1, oper2)
    return floatsub(oper1, float(oper2));

stock Float:operator-(oper1, Float:oper2)
    return floatsub(float(oper1), oper2);

stock bool:operator==(Float:oper1, Float:oper2)
    return floatcmp(oper1, oper2) == 0;

stock bool:operator==(Float:oper1, oper2)
    return floatcmp(oper1, float(oper2)) == 0;  /* "==" is commutative */

stock bool:operator!=(Float:oper1, Float:oper2)
    return floatcmp(oper1, oper2) != 0;

stock bool:operator!=(Float:oper1, oper2)
    return floatcmp(oper1, float(oper2)) != 0;  /* "!=" is commutative */

stock bool:operator>(Float:oper1, Float:oper2)
    return floatcmp(oper1, oper2) > 0;

stock bool:operator>(Float:oper1, oper2)
    return floatcmp(oper1, float(oper2)) > 0;

stock bool:operator>(oper1, Float:oper2)
    return floatcmp(float(oper1), oper2) > 0;

stock bool:operator>=(Float:oper1, Float:oper2)
    return floatcmp(oper1, oper2) >= 0;

stock bool:operator>=(Float:oper1, oper2)
    return floatcmp(oper1, float(oper2)) >= 0;

stock bool:operator>=(oper1, Float:oper2)
    return floatcmp(float(oper1), oper2) >= 0;

stock bool:operator<(Float:oper1, Float:oper2)
    return floatcmp(oper1, oper2) < 0;

stock bool:operator<(Float:oper1, oper2)
    return floatcmp(oper1, float(oper2)) < 0;

stock bool:operator<(oper1, Float:oper2)
    return floatcmp(float(oper1), oper2) < 0;

stock bool:operator<=(Float:oper1, Float:oper2)
    return floatcmp(oper1, oper2) <= 0;

stock bool:operator<=(Float:oper1, oper2)
    return floatcmp(oper1, float(oper2)) <= 0;

stock bool:operator<=(oper1, Float:oper2)
    return floatcmp(float(oper1), oper2) <= 0;

stock bool:operator!(Float:oper)
    return (_:oper & cellmax) == 0;

/* forbidden operations */
forward operator%(Float:oper1, Float:oper2);
forward operator%(Float:oper1, oper2);
forward operator%(oper1, Float:oper2);
#if defined _time_included
  #endinput
#endif
#define _time_included
#pragma library Time

native gettime(&hour=0, &minute=0, &second=0);
native getdate(&year=0, &month=0, &day=0);
native tickcount(&granularity=0);
#if defined _file_included
  #endinput
#endif
#define _file_included
#pragma library File

enum filemode
    {
    io_read,            /* file must exist */
    io_write,           /* creates a new file */
    io_readwrite,       /* opens an existing file, or creates a new file */
    io_append,          /* appends to file (write-only) */
    }

enum seek_whence
    {
    seek_start,
    seek_current,
    seek_end,
    }

const EOF = -1;

native File:fopen(const name[], filemode: mode = io_readwrite);
native bool:fclose(File: handle);
native File:ftemp();
native bool:fremove(const name[]);

native fwrite(File: handle, const string[]);
native fread(File: handle, string[], size = sizeof string, bool: pack = false);
native bool:fputchar(File: handle, value, bool: utf8 = true);
native fgetchar(File: handle, value, bool: utf8 = true);
native fblockwrite(File: handle, const buffer[], size = sizeof buffer);
native fblockread(File: handle, buffer[], size = sizeof buffer);

native fseek(File: handle, position = 0, seek_whence: whence = seek_start);
native flength(File: handle);
native fexist(const pattern[]);
native bool:fmatch(name[], const pattern[], index = 0, size = sizeof name);

