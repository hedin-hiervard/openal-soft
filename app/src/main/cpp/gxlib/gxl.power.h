#ifndef GXLIB_GXL_POWER_H_
#define GXLIB_GXL_POWER_H_

class iLightHolder
{
public:
	iLightHolder() { HoldBacklight(); }
	~iLightHolder() { RestoreBacklight(); }

private:
	
	void HoldBacklight() 
	{
#ifndef OS_APPLE
		HKEY key;
		DWORD dwDesc = REG_OPENED_EXISTING_KEY;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("ControlPanel\\Backlight"), 0,KEY_ALL_ACCESS,&key) == ERROR_SUCCESS) {
			DWORD dwType = REG_DWORD;
			DWORD tmpT = 0x7FFFFFFF;
			DWORD dwS;

			RegQueryValueEx(key,TEXT("BatteryTimeout"),0,&dwType,(PBYTE)&m_orgBtTimeout,&dwS);
			RegQueryValueEx(key,TEXT("ACTimeout"),0,&dwType,(PBYTE)&m_orgAcTimeout,&dwS);

			RegSetValueEx(key,TEXT("BatteryTimeout"),0,dwType,(PBYTE)&tmpT,4);
			RegSetValueEx(key,TEXT("ACTimeout"),0,dwType,(PBYTE)&tmpT,4);
			RegCloseKey(key);

			HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("BackLightChangeEvent"));

			if (hEvent) {
				SetEvent(hEvent);
				CloseHandle(hEvent);
			}
		}
#endif
	}

	void RestoreBacklight() 
	{
#ifndef OS_APPLE
		HKEY key;

		if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("ControlPanel\\Backlight"), 0,KEY_ALL_ACCESS,&key) == ERROR_SUCCESS) {

			DWORD dwType = REG_DWORD;
			RegSetValueEx(key,TEXT("BatteryTimeout"),0,dwType,(PBYTE)&m_orgBtTimeout,4);
			RegSetValueEx(key,TEXT("ACTimeout"),0,dwType,(PBYTE)&m_orgAcTimeout,4);
			RegCloseKey(key);

			HANDLE hEvent =	CreateEvent(NULL, FALSE, FALSE, TEXT("BackLightChangeEvent"));
			if (hEvent) {
				SetEvent(hEvent);
				CloseHandle(hEvent);
			}
		}
#endif
	}

private:
	DWORD	m_orgBtTimeout;
	DWORD	m_orgAcTimeout;
};


#endif //GXLIB_GXL_POWER_H_
