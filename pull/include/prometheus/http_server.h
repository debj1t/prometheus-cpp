#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <http.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "prometheus/detail/future_std.h"
#pragma comment(lib, "httpapi.lib")

//
// Macros.
//
#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )    \
    do                                                      \
    {                                                       \
        RtlZeroMemory( (resp), sizeof(*(resp)) );           \
        (resp)->StatusCode = (status);                      \
        (resp)->pReason = (reason);                         \
        (resp)->ReasonLength = (USHORT) strlen(reason);     \
    } while (FALSE)

#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)               \
    do                                                               \
    {                                                                \
        (Response).Headers.KnownHeaders[(HeaderId)].pRawValue =      \
                                                          (RawValue);\
        (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength = \
            (USHORT) strlen(RawValue);                               \
    } while(FALSE)

#define ALLOC_MEM(cb) HeapAlloc(GetProcessHeap(), 0, (cb))

#define FREE_MEM(ptr) HeapFree(GetProcessHeap(), 0, (ptr))

class HttpServer;


typedef struct ServerContext {
    HANDLE server;
    HANDLE hReqQueue;
} serverContext, *pServerContext;

namespace prometheus {
//
// Prototypes.
//

namespace detail {

class MetricsHandler;

typedef ULONG (MetricsHandler::*get_metrics_handler)(HANDLE hReqQueue, PHTTP_REQUEST pRequest);

}
//typedef int (*request_handler)(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
 
class RequestHandler
{
  public:

    RequestHandler(){};
	virtual ~RequestHandler()
	{
	}

	//virtual ULONG handleGet(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
	ULONG handleGet(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
    ULONG (RequestHandler::*reqHandler_)(HANDLE, PHTTP_REQUEST);
};


class HttpServer {
public:
    HttpServer( std::vector<std::wstring> urls){
        urls_ = urls;
        SetupServer(urls);
    }

    std::wstring GetUrl() {
        return urls_[0];
    }

   // void SetLastRequestContext(HANDLE hReqQueue, PHTTP_REQUEST pRequest) {
   //     hReqQueue_ = hReqQueue;
   //     pRequest_ = pRequest;
   // }

    // virtual ULONG handleGet(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
    /**
	 * getListeningPorts()
	 *
	 * Returns a list of ports that are listening
	 *
	 * @return A vector of ports
	 */
//    void addHandler(ULONG (* reqHandler)(HANDLE, PHTTP_REQUEST)) {
    void addHandler(detail::MetricsHandler* metricsInstance, detail::get_metrics_handler clientHandler) {
        wprintf(L"HttpServer::addHandler\n");
        metricsHandler = metricsInstance;    
        getHandler = clientHandler;
        wprintf(L"HttpServer::addHandler gethandler %p\n", getHandler);
        wprintf(L"HttpServer::addHandler clienthandler %p\n", clientHandler);
        //wprintf(L"HttpServer::addHandler metrics gethandler %p\n", metricsInstance->getHandler);
    }

    void HttpCleanup();

    ~HttpServer() {
        HttpCleanup();
    }

	//std::vector<int> getListeningPorts();
    //ULONG (HANDLE, HTTP_REQUEST) GetReqHandler() {
    //    return reqHandler_;
    //}

    HANDLE hReqQueue() {
        return hReqQueue_;
    }

    std::vector<std::wstring> urls() {
        return urls_;
    }
    int  UrlAdded;
    //ULONG (RequestHandler::*reqHandler_)(HANDLE, PHTTP_REQUEST);
    //RequestHandler handler;
    detail::get_metrics_handler getHandler;
    detail::MetricsHandler* metricsHandler;
    HANDLE hThread_;
    pServerContext threadData_; 
private:
    int SetupServer(std::vector<std::wstring> urls);
    HANDLE hReqQueue_;
    static PHTTP_REQUEST pRequest_;    
    std::vector<std::wstring> urls_;
};



    //DWORD DoReceiveRequests(HANDLE hReqQueue, HttpServer* server);
    DWORD DoReceiveRequests(LPVOID serverContext);

    DWORD
    SendHttpResponse(
        IN HANDLE        hReqQueue,
        IN PHTTP_REQUEST pRequest,
        IN USHORT        StatusCode,
        IN PSTR          pReason,
        IN PSTR          pEntity
        );

    DWORD
    SendHttpPostResponse(
        IN HANDLE        hReqQueue,
        IN PHTTP_REQUEST pRequest
        );
} // namespace prometheus

