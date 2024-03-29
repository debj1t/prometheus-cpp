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

namespace prometheus {
//
// Prototypes.
//


typedef int (*request_handler)(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
 
class RequestHandler
{
  public:
	virtual ~RequestHandler()
	{
	}

	virtual ULONG handleGet(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
};

class HttpServer {
public:
    HttpServer( std::vector<std::wstring> urls) {
        urls_ = urls;
        SetupServer(urls);
    }

    ~HttpServer() {
        // Cleanup
        //HttpRemoveUrlFromUrlGroup(hUrlGroup, url, 0);
        //HttpCloseUrlGroup(hUrlGroup);
        //HttpCloseRequestQueue(hReqQueue);
    }

    std::wstring GetUrl() {
        return urls_[0];
    }

    static void SetLastRequestContext(HANDLE hReqQueue, PHTTP_REQUEST pRequest) {
        hReqQueue_ = hReqQueue;
        pRequest_ = pRequest;
    }

    // virtual ULONG handleGet(HANDLE hReqQueue, PHTTP_REQUEST pRequest);
    /**
	 * getListeningPorts()
	 *
	 * Returns a list of ports that are listening
	 *
	 * @return A vector of ports
	 */
    void addHandler(void* reqHandler) {
        reqHandler_ = (RequestHandler*) reqHandler;
    }

	//std::vector<int> getListeningPorts();
    RequestHandler* GetReqHandler() {
        return reqHandler_;
    }
private:
    int SetupServer(std::vector<std::wstring> urls);
    static HANDLE hReqQueue_;
    static PHTTP_REQUEST pRequest_;    
    std::vector<std::wstring> urls_;
//    request_handler reqHandler_;
    RequestHandler* reqHandler_;
};


    DWORD DoReceiveRequests(HANDLE hReqQueue, HttpServer* server);

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

