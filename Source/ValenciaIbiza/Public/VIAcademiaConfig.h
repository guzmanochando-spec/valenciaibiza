#pragma once

#include "CoreMinimal.h"

/** Production pages on academia.riomares.com (gated by riomares-auth). */
namespace VIAcademia
{
	inline const TCHAR* CartaUrl() { return TEXT("https://academia.riomares.com/carta"); }
	inline const TCHAR* RadarUrl() { return TEXT("https://academia.riomares.com/radar"); }
	inline const TCHAR* RadioUrl() { return TEXT("https://academia.riomares.com/radio"); }
	inline const TCHAR* HubBase() { return TEXT("https://academia.riomares.com/wp-json/maria-hub/v1"); }
	inline const TCHAR* MariaChat() { return TEXT("https://academia.riomares.com/wp-json/maria-hub/v1/maria/chat"); }
}
