#include "pch.h"
#include "DataBase.h"

void DataBase::show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    /************************************************************************
    /* HandleDiagnosticRecord : display error/warning information
    /*
    /* Parameters:
    /*      hHandle     ODBC handle
    /*      hType       Type of handle (SQL_HANDLE_STMT, SQL_HANDLE_ENV, SQL_HANDLE_DBC)
    /*      RetCode     Return code of failing command
    /************************************************************************/
    {
        SQLSMALLINT iRec = 0;
        SQLINTEGER  iError;
        WCHAR       wszMessage[1000];
        WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];


        if (RetCode == SQL_INVALID_HANDLE) {
            std::wcout << L"Invalid handle!\n";
            return;
        }
        while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
            (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
            // Hide data truncated..
            if (wcsncmp(wszState, L"01004", 5)) {
                std::wcout << L"[" << wszState << L"] " << wszMessage << L"  " << iError << std::endl;
            }
        }
    }
}

DataBase::DataBase()
{
    m_henv = NULL;
    m_hdbc = NULL;
    m_hstmt = NULL;
    m_retcode = NULL;
}

DataBase::~DataBase()
{
    DBDisConnect();
}

bool DataBase::DBConnect()
{
    m_retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

    // Set the ODBC version environment attribute  
    if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        m_retcode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
    }
    else {
        return false;
    }

    // Allocate connection handle  
    if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        m_retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
    }
    else {
        return false;
    }

    // Set login timeout to 5 seconds  
    if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
    }
    else {
        return false;
    }

    // Connect to data source  
    m_retcode = SQLConnect(m_hdbc, (SQLWCHAR*)L"DB_TIMERUN", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

    // Allocate statement handle  
    if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        m_retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
    }
    else {
        return false;
    }

    return true;
}

bool DataBase::DBDisConnect()
{
    SQLDisconnect(m_hdbc);

    SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_STMT, m_henv);

    m_henv = NULL;
    m_hdbc = NULL;
    m_hstmt = NULL;
    m_retcode = 0;

    return true;
}

bool DataBase::IsIDRegistered(std::string ID)
{
    SQLCHAR isRegistered{};
    SQLLEN cb_isRegistered{};

    std::wstring sqlQuery = L"EXEC isIDRegistered ?";
    m_retcode = SQLPrepare(m_hstmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    m_retcode = SQLBindParameter(m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)ID.c_str(), 0, NULL);

    m_retcode = SQLExecute(m_hstmt);

    if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        m_retcode = SQLBindCol(m_hstmt, 1, SQL_BIT, &isRegistered, sizeof(isRegistered), &cb_isRegistered);
    }
    else {
        show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }

    m_retcode = SQLFetch(m_hstmt);

    if (m_retcode == SQL_ERROR) {
        show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }
    else if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);  // 매개변수 재설정
        SQLCloseCursor(m_hstmt);  // 문장과 연결된 커서 닫기
        return (isRegistered == 1);
    }
    else {
        return false;
    }
}

bool DataBase::IsPASSWDRegistered(std::string PASSWD)
{
    SQLCHAR isRegistered{};
    SQLLEN cb_isRegistered{};

    std::wstring sqlQuery = L"EXEC isPASSWDRegistered ?";
    m_retcode = SQLPrepare(m_hstmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    m_retcode = SQLBindParameter(m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)PASSWD.c_str(), 0, NULL);

    m_retcode = SQLExecute(m_hstmt);

    if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        m_retcode = SQLBindCol(m_hstmt, 1, SQL_BIT, &isRegistered, sizeof(isRegistered), &cb_isRegistered);
    }
    else {
        show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }

    m_retcode = SQLFetch(m_hstmt);

    if (m_retcode == SQL_ERROR) {
        show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }
    else if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);  // 매개변수 재설정
        SQLCloseCursor(m_hstmt);  // 문장과 연결된 커서 닫기
        return (isRegistered == 1);
    }
    else {
        return false;
    }
}

bool DataBase::RegisteredAccount(int client_id, std::string ID, std::string PASSWD, std::string NICKNAME)
{
    //SQLINTEGER ID{};
    SQLCHAR player_id[IDSIZE];
    SQLCHAR player_passwd[PASSWDSIZE];
    SQLCHAR player_nickname[NAMESIZE];
    SQLLEN cb_id{}, cb_passwd{}, cb_nickname{};
    SQLCHAR successFlag; // Assuming 1 byte for BIT type

    std::wstring sqlQuery = L"EXEC RegisteredAccount ?, ?, ?, ?";
    m_retcode = SQLPrepare(m_hstmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    m_retcode = SQLBindParameter(m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)ID.c_str(), 0, NULL);
    m_retcode = SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)PASSWD.c_str(), 0, NULL);
    m_retcode = SQLBindParameter(m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)NICKNAME.c_str(), 0, NULL);
    m_retcode = SQLBindParameter(m_hstmt, 4, SQL_PARAM_OUTPUT, SQL_C_BIT, SQL_BIT, 1, 0, &successFlag, 0, NULL);
    m_retcode = SQLExecute(m_hstmt);

    strcpy_s(reinterpret_cast<char*>(player_id), sizeof(player_id), ID.c_str());
    strcpy_s(reinterpret_cast<char*>(player_passwd), sizeof(player_id), PASSWD.c_str());
    strcpy_s(reinterpret_cast<char*>(player_nickname), sizeof(player_id), NICKNAME.c_str());

    /*if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
       m_retcode = SQLBindCol(m_hstmt, 1, SQL_CHAR, player_id, IDSIZE, &cb_id);
       m_retcode = SQLBindCol(m_hstmt, 2, SQL_CHAR, player_passwd, PASSWDSIZE, &cb_passwd);
       m_retcode = SQLBindCol(m_hstmt, 3, SQL_CHAR, player_nickname, NAMESIZE, &cb_nickname);
    }
    else {
       show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }*/

    if (m_retcode == SQL_ERROR) {
        show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }
    else if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        if (0 == successFlag) {
            SQLFreeStmt(m_hstmt, SQL_CLOSE);
            SQLFreeStmt(m_hstmt, SQL_UNBIND);
            SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);
            //SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);  // 매개변수 재설정
            SQLCloseCursor(m_hstmt);  // 문장과 연결된 커서 닫기
            return false;

        }
        else {
            SQLFreeStmt(m_hstmt, SQL_CLOSE);
            SQLFreeStmt(m_hstmt, SQL_UNBIND);
            SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);
            //SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);  // 매개변수 재설정
            SQLCloseCursor(m_hstmt);  // 문장과 연결된 커서 닫기
            return true;
        }
    }
}

PlayerInfo DataBase::ExtractPlayerInfo(std::string ID, std::string PASSWD)
{
    PlayerInfo playerinfo;

    SQLCHAR player_nickname[NAMESIZE];
    SQLLEN cb_nickname{};

    std::wstring sqlQuery = L"EXEC ExtractPlayerInfo ?, ?";
    m_retcode = SQLPrepare(m_hstmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    m_retcode = SQLBindParameter(m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)ID.c_str(), 0, NULL);
    m_retcode = SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)PASSWD.c_str(), 0, NULL);
    m_retcode = SQLExecute(m_hstmt);
    // Bind columns 1
    m_retcode = SQLBindCol(m_hstmt, 1, SQL_CHAR, player_nickname, NAMESIZE, &cb_nickname);

    m_retcode = SQLFetch(m_hstmt);

    if (m_retcode == SQL_ERROR) {
        show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);
    }
    else if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO) {
        SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);  // 매개변수 재설정
        SQLCloseCursor(m_hstmt);  // 문장과 연결된 커서 닫기
        memcpy(playerinfo.player_nickname, reinterpret_cast<const char*>(player_nickname), sizeof player_nickname);
        return playerinfo;
    }
}

