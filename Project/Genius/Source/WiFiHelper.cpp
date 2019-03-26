#include "WiFiHelper.h"
#include "windows.h"
#include <wlanapi.h>
#include <Sensapi.h>
#include <NetCon.h>

#pragma comment(lib,"wlanapi.lib")
#pragma comment(lib,"Sensapi.lib")
//==============================================================================
WiFiHelper::WiFiHelper()
{
}

WiFiHelper::~WiFiHelper()
{
}

bool WiFiHelper::WirelessNetworkInvokeAPIStart(const String& strSSID, const String& strKey)
{
    if (!FindCommunicationNetworkAdapter())
        return false;

    if (!OpenServiceInvokeUtilities())
        return false;

    CHAR ckey[32] = { 0 };
    //DWORD dError = 0;
    BOOL bIsAllow = TRUE;
    DWORD dwError = 0;
    DWORD dwServiceVersion = 0;
    HANDLE hClient;

    if (ERROR_SUCCESS != WlanOpenHandle(
        WLAN_API_VERSION,
        NULL,               // reserved
        &dwServiceVersion,
        &hClient
        ))
        return false;

    // check service version
    if (WLAN_API_VERSION_MAJOR(dwServiceVersion) < WLAN_API_VERSION_MAJOR(WLAN_API_VERSION_2_0))
        WlanCloseHandle(hClient, NULL);

    WLAN_HOSTED_NETWORK_REASON dwFailedReason;
    if (WlanHostedNetworkInitSettings(hClient, &dwFailedReason, NULL) != ERROR_SUCCESS)
        return false;

    DWORD dwReturnValue = WlanHostedNetworkSetProperty(hClient,
        wlan_hosted_network_opcode_enable,
        sizeof(BOOL),
        &bIsAllow,
        &dwFailedReason,
        NULL);

    WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS whncs;
    memset(&whncs, 0, sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS));



    whncs.dwMaxNumberOfPeers = 8;
    WideCharToMultiByte(CP_ACP,
        0,
        strSSID.toWideCharPointer(),
        strSSID.length(),
        (LPSTR)whncs.hostedNetworkSSID.ucSSID,
        32,
        NULL,
        NULL);

    whncs.hostedNetworkSSID.uSSIDLength = (ULONG)strlen((const char*)whncs.hostedNetworkSSID.ucSSID);
    dwReturnValue = WlanHostedNetworkSetProperty(hClient,
        wlan_hosted_network_opcode_connection_settings,
        sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS),
        &whncs,
        &dwFailedReason,
        NULL);

    WideCharToMultiByte(CP_ACP,
        0,
        strKey.toWideCharPointer(),
        strKey.length(),
        ckey,
        32,
        NULL,
        NULL);
    dwReturnValue = WlanHostedNetworkSetSecondaryKey(hClient,
        (DWORD)(strlen((const char*)ckey) + 1),
        (PUCHAR)ckey,
        TRUE,
        FALSE,
        &dwFailedReason,
        NULL);
    if (ERROR_SUCCESS != dwReturnValue)
    {
        return false;
    }
    //strcpy_s((CHAR*)whncs.hostedNetworkSSID.ucSSID,32,cssid);
    //WlanHostedNetworkQueryStatus(hClient, , NULL);

    dwReturnValue = WlanHostedNetworkForceStart(hClient, &dwFailedReason, NULL);
    if (ERROR_SUCCESS != dwReturnValue)
    {
        return false;
    }
    if (NetSharingManagerFindAndSet())
    {
        return false;
    }
    
    WlanCloseHandle(hClient, NULL);
    return true;
}

bool WiFiHelper::WirelessNetworkInvokeAPIStop()
{
    DWORD dwError = 0;
    DWORD dwServiceVersion = 0;
    HANDLE hClient = NULL;
    if (ERROR_SUCCESS != (dwError = WlanOpenHandle(
        WLAN_API_VERSION,
        NULL,               // reserved
        &dwServiceVersion,
        &hClient
        )))
    {
        return false;
    }
    WLAN_HOSTED_NETWORK_REASON dwFailedReason;
    DWORD dwResult = WlanHostedNetworkForceStop(hClient, &dwFailedReason, NULL);
    if (dwResult != ERROR_SUCCESS)
        return false;

    return true;
}

bool WiFiHelper::FindCommunicationNetworkAdapter()
{
    DWORD flags;
    return IsNetworkAlive(&flags) == TRUE && GetLastError() == ERROR_SUCCESS;
}

bool WiFiHelper::InternetConnectionSharingManager(bool isClose)
{
    bool result = true;
    SC_HANDLE scm = nullptr;
    SC_HANDLE src = nullptr;

    do
    {
        if (!(src = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)))
        {
            result = false;
            break;
        }

        if (!(src = OpenService(scm, TEXT("SharedAccess"), SERVICE_ALL_ACCESS)))
        {
            result = false;
            break;
        }

        SERVICE_STATUS svr;
        ZeroMemory(&svr, sizeof(svr));
        if (QueryServiceStatus(src, &svr) == FALSE)
        {
            result = false;
            break;
        }

        if (svr.dwCurrentState != SERVICE_RUNNING && !isClose)
        {
            if (StartService(src, NULL, NULL) == FALSE)
            {
                result = false;
                break;
            }
        }

        if (svr.dwCurrentState == SERVICE_RUNNING && isClose)
        {
            if (ControlService(src, SERVICE_CONTROL_STOP, NULL) == FALSE)
            {
                result = false;
                break;
            }
        }

    } while (false);

    if (scm) CloseServiceHandle(scm);
    if (src) CloseServiceHandle(src);

    return result;
}

bool WiFiHelper::OpenServiceInvokeUtilities()
{
    bool result = true;
    SC_HANDLE scm = nullptr;
    SC_HANDLE src = nullptr;

    do
    {
        scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!scm)
        {
            result = false;
            break;
        }

        src = OpenService(scm, TEXT("WlanSvc"), SERVICE_ALL_ACCESS);
        SERVICE_STATUS svrST;
        if (!src)
        {
            result = false;
            break;
        }
        ZeroMemory(&svrST, sizeof(svrST));
        if (QueryServiceStatus(src, &svrST) == FALSE)
        {
            result = false;
            break;
        }

        if (svrST.dwCurrentState != SERVICE_RUNNING)
        {
            if (StartService(src, NULL, NULL) == FALSE)
            {
                result = false;
                break;
            }
        }
    } while (false);


    if (scm) CloseServiceHandle(scm);
    if (src) CloseServiceHandle(src);

    return result;
}

bool WiFiHelper::NetSharingManagerFindAndSet()
{
    CoInitialize(NULL);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    INetSharingManager * pNSM = NULL;
    HRESULT hr = ::CoCreateInstance(__uuidof(NetSharingManager),
        NULL,
        CLSCTX_ALL,
        __uuidof(INetSharingManager),
        (void**)&pNSM);

    INetConnectionManager *pNCMgr = NULL;
    
    /*PPPOE*/
    const WCHAR Adapterdevice[] = L"Microsoft Virtual WiFi Miniport Adapter";
    INetConnection * pNC = NULL; // fill this out for part 2 below
    INetSharingEveryConnectionCollection * pNSECC = NULL;
    hr = pNSM->get_EnumEveryConnection(&pNSECC);
    if (!pNSECC)
    {
        return false;
    }
    else 
    {
        // enumerate connections
        IEnumVARIANT * pEV = NULL;
        IUnknown * pUnk = NULL;
        hr = pNSECC->get__NewEnum(&pUnk);
        if (pUnk)
        {
            hr = pUnk->QueryInterface(__uuidof(IEnumVARIANT), (void**)&pEV);
            pUnk->Release();
        }
        if (pEV)
        {
            VARIANT v;
            VariantInit(&v);
            BOOL bFoundIt = FALSE;
            INetSharingConfiguration * pNSC = NULL;
            INetSharingConfiguration * pVWifiNSC = NULL;
            NETCON_PROPERTIES* pVWifi = NULL;
            while (S_OK == pEV->Next(1, &v, NULL)) {
                if (V_VT(&v) == VT_UNKNOWN) {
                    V_UNKNOWN(&v)->QueryInterface(__uuidof(INetConnection),
                        (void**)&pNC);
                    if (pNC) {
                        NETCON_PROPERTIES* pNP = NULL;
                        pNC->GetProperties(&pNP);
                        if (pNP->Status == NCS_CONNECTED){
                            hr = pNSM->get_INetSharingConfigurationForINetConnection(pNC, &pNSC);
                            hr = pNSC->EnableSharing(ICSSHARINGTYPE_PUBLIC);
                            pNSC->Release();
                        }
                        if (_wcsicmp(pNP->pszwDeviceName, Adapterdevice) == 0){
                            pVWifi = pNP;
                            hr = pNSM->get_INetSharingConfigurationForINetConnection(pNC, &pVWifiNSC);
                            pVWifiNSC->EnableSharing(ICSSHARINGTYPE_PRIVATE);
                        }
                    }
                }
            }
            INetSharingPortMapping * pNSPM = NULL;
            // BSTR pszwName = pVWifi->pszwName;
            if (pVWifiNSC)
            {
                pVWifiNSC->Release();
                pVWifiNSC = NULL;
            }
            if (pNSPM) {
                hr = pNSPM->Enable();
                INetSharingPortMappingProps* pMapping = NULL;
                pNSPM->get_Properties(&pMapping);
                BSTR result = NULL;
                long port = 0;
                pMapping->get_Name(&result);
                pMapping->get_ExternalPort(&port);
                pNSPM->Release();
            }
        }
    }
    return S_OK;
}