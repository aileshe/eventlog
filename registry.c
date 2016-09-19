#include <mbstring.h>
#include "main.h"
#include "log.h"
#include "syslog.h"
#include "eventlog.h"



// List of registry items to store //
struct RegistryData {
	char * name;			// Name of key			//
	DWORD key_type;			// Type of key			//
	void * value;			// Value of key			//
	DWORD size;				// Size of key			//
	int required;			// Key is required		//
};


// Location of eventlog keys //
static char RegistryEventlogKeyPath[] = "SYSTEM\\CurrentControlSet\\Services\\Eventlog";


// Gather list of keys //
int RegistryGather()
{
	DWORD size;
	HKEY registry_handle;
	char name[EVENTLOG_NAME_SZ];  /*128*/
	int errnum;
	int index;

	// Open location for enumerating key information //
/*
	LONG RegOpenKey( HKEY hKey, // Ҫ�򿪼��ľ��
                                 LPCTSTR lpSubKey, // Ҫ���Ӽ������ֵĵ�ַ
                                 PHKEY phkResult // Ҫ�򿪼��ľ���ĵ�ַ
                               );
       RegistryEventlogKeyPath[] = "SYSTEM\\CurrentControlSet\\Services\\Eventlog";
*/
	if (RegOpenKey(HKEY_LOCAL_MACHINE, RegistryEventlogKeyPath, &registry_handle)) {
		Log(LOG_ERROR|LOG_SYS, "Cannot initialize access to registry: \"%s\"", RegistryEventlogKeyPath);
		return 1;
	}

	// Process keys until end of list //
	index = 0;
	while (1) {

		// Get next key //
		size = sizeof(name);
		/*
                hKey Long��һ���Ѵ���ľ��������ָ��һ����׼����
                dwIndex Long������ȡ���������������һ��������������Ϊ��
                lpName String������װ��ָ��������������һ��������
                cbName Long��lpName�������ĳ���
            return: Long���㣨ERROR_SUCCESS����ʾ�ɹ���
                               �����κ�ֵ������һ���������
		*/
		errnum = RegEnumKey(registry_handle, index, name, size);

		// Stop if last item //
		if (errnum == ERROR_NO_MORE_ITEMS)
			break;

		// Check for error //
		if (errnum) {
			Log(LOG_ERROR|LOG_SYS, "Cannot enumerate registry key: \"%s\"", RegistryEventlogKeyPath);
			break;
		}

		// Create new eventlog //
		if (EventlogCreate(name))
			break;

		// Advance index number //
		index++;
	}
	// Close registry //
	RegCloseKey(registry_handle);

	// Return status //
	return errnum != ERROR_NO_MORE_ITEMS;
}


