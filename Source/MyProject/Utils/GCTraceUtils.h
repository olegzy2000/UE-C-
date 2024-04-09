#pragma once

namespace GCTraceUtils {
	bool SweepCapsuleSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rot, ECollisionChannel TraceChannel, float CapsuleRadius, float CapsuleHalfHeight, const FCollisionQueryParams& Params= FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam= FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug=false, float DrawTime=-1.0, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);
	bool SweepSphereSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rot, ECollisionChannel TraceChannel, float Radius, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1.0, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);
	bool OverlapCapsuleAnyByProfile(const UWorld* World, const FVector Pos, float CapsuleRadius,float CapsuleHalfHeight,FQuat Rotation,FName ProfileName,const FCollisionQueryParams& Params, bool bDrawDebug = false, float DrawTime = -1.0, FColor HitColor = FColor::Red);
	bool OverlapCapsuleBlockingByProfile(const UWorld* World, const FVector Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rotation, FName ProfileName, const FCollisionQueryParams& Params, bool bDrawDebug = false, float DrawTime = -1.0, FColor HitColor = FColor::Red);

	bool LineTraceSingleByChannel(const UWorld* World, FHitResult& OutHit, const FVector Start, const FVector End, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, bool bDrawDebug, float DrawTime, FColor HitColor);

}
