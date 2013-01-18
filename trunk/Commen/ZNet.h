//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once


#include "ZThread.h"
#include "TaskMgr.h"

//网络连接状态
#define ZNET_NONE       0   //无状态
#define ZNET_CREATE     1   //创建socket
#define ZNET_BIND       2   //绑定端口
#define ZNET_LISTEN     3   //监听
#define ZNET_ACCEPT     4   //接收客户端连接
#define ZNET_CONNECT    5   //连接服务器

typedef TaskMgr<std::string> MessageMgr;
//////////////////////////////////////////////////////////////////////////
//网络基类
class ZNet : public IBase
{
public:
    ZNet(void);
    virtual ~ZNet(void);
};

//////////////////////////////////////////////////////////////////////////
//客户端基类
class ZClientBase
{
public:
    ZClientBase();

    virtual ~ZClientBase();

    //关闭连接
    virtual void close();

    //发送数据。线程安全
    virtual void send(std::string msg);

    //接收到数据。
    virtual void onReceivMsg(std::string msg);

    //收/发线程出错
    virtual void onError(DWORD code);

    //接收线程
    virtual void threadReceiv();

    //发送线程
    virtual void threadSend();

public:

    SOCKET  socket(){ return m_socket; }

    int     port(){ return m_port; }

    std::string address(){ return m_address; }

    MessageMgr& getMsgMgr(){ return m_msgMgr;}

    virtual bool isRunning(){ return m_running; }

protected:
    bool        m_running; //是否正在运行
    SOCKET      m_socket;  //套接字
    int         m_port;     //端口
    std::string m_address;  //ip地址
    MessageMgr  m_msgMgr;   //消息管理器

    ZThreadPtr  m_threadRead;   //接收线程
    ZThreadPtr  m_threadWrite;  //发送线程
};

//////////////////////////////////////////////////////////////////////////
//服务器端
class ZServer : public ZNet
{
public:
    //与远程客户端对应的连接
    class ZClientNode : public ZClientBase, public IBase
    {
    public:
        ZClientNode(ZServer* server, SOCKET sock, std::string addr, int port_);

        bool isRunning();

        virtual void onReceivMsg(std::string msg);

        int id(){ return m_id ; }
    private:
        int         m_id;
        ZServer*    m_server;
        static int  s_idAllocator;
    };

    typedef RefPtr<ZClientNode>         ZClientNodePtr;
    typedef std::list<ZClientNodePtr>   ClientPool;
    typedef ClientPool::iterator        ClientIterator;

public:
    ZServer() 
        : m_running(false)
        , m_status(ZNET_NONE)
    {}

    ~ZServer() {}

    //启动网络连接
    bool start(int port, int acceptMax);

    //关闭网络连接
    void close();

    //添加一个客户端
    void addClient(ZClientNodePtr node);

    //移除一个客户端
    void removeClient(ZClientNodePtr node);

    //查询客户端
    ZClientNodePtr getClient(int id);

    //向客户端发送数据
    virtual void send(int id, std::string msg);

    //接收到来自客户端的消息
    //子类需要重载此函数，处理接受到的消息。
    virtual void onReceivMsg(int id, std::string msg);

    //检查死亡的连接
    void checkDead();

    //接受连接线程函数
    void threadProcAccept();

public:
    SOCKET socket(){ return m_socket; }

    int port(){ return m_port; }

    int status(){ return m_status; }

    virtual bool isRunning(){ return m_running; }

protected:

    //创建socket
    bool create();

    //绑定端口
    bool bind(int port_);

    //监听
    bool listen(int maxAmount);

    //接收连接
    bool accept();

protected:
    SOCKET          m_socket;
    int             m_port;
    int             m_status;
    bool            m_running;
    ZThreadPtr      m_threadAccept;
    ZSimpleLock     m_clientLock;
    ClientPool      m_pool;
};

//////////////////////////////////////////////////////////////////////////
//客户端
class ZClient : public ZNet, public ZClientBase
{
public:
    ZClient() : m_status(0) {}

    virtual bool start(std::string addr, int port_);

    int status(){ return m_status; }

    virtual void close();
protected:
    bool create();

    bool connect(std::string addr, int port_);

protected:
    int m_status;
};