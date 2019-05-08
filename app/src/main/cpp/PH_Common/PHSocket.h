#ifndef PH_COMMON_PHSOCKET_H_
#define PH_COMMON_PHSOCKET_H_
#pragma once
class PHSocketHandler : public SocketHandler
{
public:
	PHSocketHandler(StdLog *p);
	~PHSocketHandler();
};

class MPLogic;

class PHSocket : public TcpSocket
{	
public:
	PHSocket(ISocketHandler &h, MPLogic* logic): TcpSocket(h), m_logic(logic) {}
	PHSocket(ISocketHandler &h);
	~PHSocket() {}

	void OnConnect();
	void OnDisconnect();
	void OnConnectTimeout();
	void OnConnectFailed();
	void OnLine(const std::string& );
	void OnRawData(const char *buf, size_t n);
	bool IsBlocking();	

	void SendBuf(const char *buf,size_t len);
	void Send(const iStringA &str);		


	inline const MPLogic* GetLogic() const { return m_logic; }
protected:
	MPLogic *m_logic;
};
#endif //PH_COMMON_PHSOCKET_H_
