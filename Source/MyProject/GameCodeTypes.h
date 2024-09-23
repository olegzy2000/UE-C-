#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_RunWall ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4
const FName CollisionProfilePawn = FName("Pawn");
const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketWeaponMuzzleSocket = FName("MuzzleSocket");
const FName CollisionProfilePawnInterationVolume = FName("PawnInteractionVolume");
const FName CollisionProfileRunWall = FName("RunWall");
const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryIkCalculation = FName("IkCalculation");
const FName DebugCategoryRunWallDetector = FName("DebugCategoryRunWallDetector");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttribute");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");
const FName CollisionProfileRagdoll = FName("Ragdoll");
UENUM(BlueprintType)
enum class EEquipableItemType :uint8 {
	None,
	Pistol
};
