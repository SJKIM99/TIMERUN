#pragma once

struct PlayerInfo {
	int player_id;
	char player_nickname[NAMESIZE];
};

class DataBase
{
	SQLHENV m_henv;
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt;
	SQLRETURN m_retcode;
private:
	void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
public:
	DataBase();
	~DataBase();
public:
	bool DBConnect();
	bool DBDisConnect();

	bool IsIDRegistered(std::string ID);
	bool IsPASSWDRegistered(std::string PASSWD);
	bool RegisteredAccount(int client_id, std::string ID, std::string PASSWD, std::string NICKNAME);
	PlayerInfo ExtractPlayerInfo(std::string ID, std::string PASSWD);

};