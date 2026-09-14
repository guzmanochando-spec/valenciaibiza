#pragma once

#include "CoreMinimal.h"
#include "VIGeo.h"

/** What is local, downloaded on approach, or always streamed. */
UENUM()
enum class EVIContentTier : uint8
{
	CoreLocal,
	ChunkValencia,
	ChunkOpenSea,
	ChunkIbiza,
	StreamTiles,
	OnlineAcademy
};

struct FVIContentBudget
{
	/** Keep only the active coastal chunk plus core. */
	EVIContentTier ActiveCoast = EVIContentTier::ChunkValencia;
	bool bAllowStreamTiles = true;
	bool bAcademyReachable = true;
	float MaxChunkRamMb = 1400.0f;
};

struct FVIIbizaVisibility
{
	/** Sa Talaia ~475 m. From a 4 m flybridge, first hint ~90 km / 48 nm in clear air. */
	static constexpr double FirstHintM = 90000.0;
	static constexpr double SilhouetteM = 55000.0;
	static constexpr double ReliefM = 22000.0;
	static constexpr double HarborM = 8000.0;

	static float OpacityForDistanceM(double DistM)
	{
		if (DistM >= FirstHintM)
		{
			return 0.0f;
		}
		if (DistM >= SilhouetteM)
		{
			return static_cast<float>(FMath::Clamp((FirstHintM - DistM) / (FirstHintM - SilhouetteM) * 0.35, 0.0, 0.35));
		}
		if (DistM >= ReliefM)
		{
			return static_cast<float>(FMath::Clamp(0.35 + (SilhouetteM - DistM) / (SilhouetteM - ReliefM) * 0.45, 0.35, 0.80));
		}
		return 1.0f;
	}
};

struct FVIContentPolicy
{
	FVIContentBudget Budget;
	FVILatLon IbizaMassif = FVILatLon(38.9089, 1.2910); // Sa Talaia
	FVILatLon IbizaHarbor = FVILatLon(38.9089, 1.4367);

	EVIContentTier TierForDistanceToIbizaM(double DistM) const
	{
		if (DistM < FVIIbizaVisibility::HarborM)
		{
			return EVIContentTier::ChunkIbiza;
		}
		if (DistM < FVIIbizaVisibility::FirstHintM)
		{
			return EVIContentTier::ChunkOpenSea;
		}
		return EVIContentTier::ChunkValencia;
	}
};
