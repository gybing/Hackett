
namespace IOEventHelpers
{
	static LPFN_ACCEPTEX FAcceeptEx = nullptr;
	static LPFN_GETACCEPTEXSOCKADDRS FGetAcceptExSockAddrs = nullptr;
	static LPFN_CONNECTEX FConnectEx = nullptr;

	static void InitializeSocket()
	{
		static bool socketInitialize = false;

		if (!socketInitialize)
		{
			socketInitialize = true;

			WSADATA wsaData;
			const WORD wVersionRequest = MAKEWORD(2, 2);
			WSAStartup(wVersionRequest, &wsaData);
		}

		if (FAcceeptEx == nullptr)
		{

		}
	}
}

enum IOType
{
	IOTypeAccept = 1,
	IOTypeConnect = 2,
	IOTypeWrite = 3,
	IOTypeReceive = 4
};

struct IOContext : public WSAOVERLAPPED
{
	IOType op;
	IEventHandler* handler;
	SocketHandle socket;
	WSABUF wsadata;
	DWORD count;
	char* data;

	IOContext(IEventHandler* h, IOType o)
	{
		std::memset(this, 0, sizeof(IOContext));
		op = o;
		handler = h;
	}

	~IOContext()
	{
		if (data) std::free(data);
	}
};

IOEvent::IOEvent()
{
	
}

IOEvent::~IOEvent()
{
}

bool IOEvent::Initialize(IEventHandler* handler)
{
	IOEventHelpers::InitializeSocket();

	assert(handler);

	m_EventHandler = handler;
}

void IOEvent::Finalize()
{

}

bool IOEvent::Connect(const std::string& host, int port)
{
	return Connect(handler, host, port, nullptr, 0);
}

bool IOEvent::Connect(const std::string& host, int port, const char* data, int size)
{

}

bool IOEvent::Accept(int port)
{

}

bool IOEvent::WriteData(const char* data, int size)
{
	if (data == nullptr || size <= 0)
	{
		return false;
	}

	IOContext* ol = new IOContext(handler, IOTypeWrite);

	ol->count = size;
	ol->data = (char*)std::malloc(size);
	std::memcpy(ol->data, data, size);

	ol->wsadata.buf = ol->data;
	ol->wsadata.len = ol->count;

	DWORD dwSend = 0;
	DWORD dwFlags = 0;

	if (SOCKET_ERROR == WSASend(nethandle, &pOver->wBuf, 1, &dwSend, dwFlags, &pOver->overlaped, NULL))
	{
		int nErrCode = WSAGetLastError();
		if (nErrCode != WSA_IO_PENDING)
		{
			delete[]pOver->wBuf.buf;
			delete pOver;
			pOver = NULL;
			add_log(e_iocp_loglevel_normal, "nethandle[%d]WSASendÊý¾ÝÊ§°Ü:errcode=%d", nethandle, nErrCode);
			return false;
		}
	}

	return true;
}

bool IOEvent::Close()
{

}
