#ifndef WiFiHelper_H_INCLUDED
#define WiFiHelper_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class WiFiHelper
{
public:
    WiFiHelper();
    ~WiFiHelper();

    bool WirelessNetworkInvokeAPIStart(const String& strSSID, const String& strKey);
    bool WirelessNetworkInvokeAPIStop();

protected:
    int init(void);
    bool FindCommunicationNetworkAdapter();
    bool InternetConnectionSharingManager(bool IsClose);
    bool OpenServiceInvokeUtilities();
    bool NetSharingManagerFindAndSet();

private:
    void* hClient;
    bool allow;
    bool start;
};


#endif  // WiFiHelper_H_INCLUDED
