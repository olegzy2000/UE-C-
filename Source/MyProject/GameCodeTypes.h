#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_RunWall ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4
const FName CollisionProfilePawn = FName("Pawn");
const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName ThroableItemSocket = FName("ThrowableItemSocket");
const FName SocketWeaponMuzzleSocket = FName("MuzzleSocket");
const FName WeaponForGribSocket = FName("ForGribSocket");
const FName CollisionProfilePawnInterationVolume = FName("PawnInteractionVolume");
const FName CollisionProfileRunWall = FName("RunWall");
const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryIkCalculation = FName("IkCalculation");
const FName DebugCategoryRunWallDetector = FName("DebugCategoryRunWallDetector");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttribute");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName FXParamTraceEnd = FName("TraceEnd");
const FName SectionMontageReloadEnd = FName("EndReload");
UENUM(BlueprintType)
enum class EEquipableItemType :uint8 {
	None,
	Pistol,
	Rifle,
	Throwable
};
UENUM(BlueprintType)
enum class EAmunitionType :uint8 {
	None,
	Pistol,
	Rifle,
	ShotgunShells,
	Grenete,
	MAX UMETA(Hidden)
};
UENUM(BlueprintType)
enum class EEquipmentSlots :uint8 {
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	PrivaryItemSlot,
	MAX UMETA(Hidden)
};
