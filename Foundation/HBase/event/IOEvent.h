#pragma once

using SocketHandle = SOCKET;
using Handle = HANDLE;

class IEventHandler
{
public:
	virtual void OnConnect(SocketHandle& handle) {}
	virtual void OnAccept(SocketHandle& handle) {}
	virtual void OnWriteData(const char* data, int size) {}
	virtual void OnReceiveData(const char* data, int size) {}
	virtual void OnError(int code, const char* message) {}
	virtual void OnClose() {}
};

class IOEvent
{
public:
	bool Initialize(IEventHandler* handler);
	void Finalize();

	bool Connect(const std::string& host, int port);
	bool Connect(const std::string& host, int port, const char* data, int size);
	bool Accept(int port);
	bool WriteData(const char* data, int size);
	bool Close();

public:
	IOEvent();
	virtual ~IOEvent();

private:
	IEventHandler* m_EventHandler;
	Handle* m_IOHandle;
	std::vector<std::thread> m_Threads;
};

