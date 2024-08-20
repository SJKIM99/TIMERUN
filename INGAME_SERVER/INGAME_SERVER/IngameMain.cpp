#include "pch.h"
#include "IngameMain.h"

IngameMain::IngameMain()
{
   
}

IngameMain::~IngameMain()
{
}

bool IngameMain::InitSocket()
{
    WSAStartup(MAKEWORD(2, 2), &WsaData);
    g_server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(g_server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    listen(g_server_socket, SOMAXCONN);

    workerThread.h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_server_socket), workerThread.h_iocp, 9999, 0);
    g_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    int addr_size = sizeof(client_addr);
    g_a_over.comp_type = OP_ACCEPT;
    AcceptEx(g_server_socket, g_client_socket, g_a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over.over);

    std::cout << "Init Success" << std::endl;
    return true;
}

void IngameMain::ServeurRun()
{
    int num_threads = std::thread::hardware_concurrency();
    worker_threads.reserve(num_threads);

    for (int i = 0; i < num_threads - 2; ++i)
        worker_threads.emplace_back([this]() {workerThread.woker_thread(workerThread.h_iocp); });

    std::thread timerThread{ [this]() { workerThread.timer(); } };

    std::cout << "ServerRun" << std::endl;
    for (auto& th : worker_threads)
        th.join();
    timerThread.join();
}
