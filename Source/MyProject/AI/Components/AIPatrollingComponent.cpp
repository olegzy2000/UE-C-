// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Components/AIPatrollingComponent.h"
#include "Actors/Navigations/PatrollingPath.h"
bool UAIPatrollingComponent::CanPatrol()
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWayPoints().Num()>0;
}
/*
FVector UAIPatrollingComponent::GetClosestWayPoint()
{	++CurrentIndexWayPoint;
	const TArray<FVector>WayPoints = PatrollingPath->GetWayPoints();
	if (CurrentIndexWayPoint == PatrollingPath->GetWayPoints().Num()) {
		CurrentIndexWayPoint = 0;
	}
	FTransform PathTransform = PatrollingPath->GetActorTransform();
	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentIndexWayPoint]);
	return WayPoint;
}

FVector UAIPatrollingComponent::SelectNextWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector>WayPoints = PatrollingPath->GetWayPoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();
	FVector ClosestWayPoint;
	float MinSqDistance = FLT_MAX;
	int32 ClosestWayPointIndex = CurrentIndexWayPoint;
	if (PatrollingType == FPatrollingType::Circle) {
		for (int32 i = 0; i < WayPoints.Num(); i++) {
			FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
			float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
			if (CurrentSqDistance < MinSqDistance && CurrentIndexWayPoint != i) {
				ClosestWayPoint = WayPointWorld;
				MinSqDistance = CurrentSqDistance;
				ClosestWayPointIndex = i;
			}
		}
		CurrentIndexWayPoint = ClosestWayPointIndex;
	}
	else {
		if (!bReturnToStart) {
			for (int32 i = 0; i < WayPoints.Num(); i++) {
				FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
				float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
				if (CurrentSqDistance < MinSqDistance && CurrentIndexWayPoint != i) {
					ClosestWayPoint = WayPointWorld;
					MinSqDistance = CurrentSqDistance;
					ClosestWayPointIndex = i;
				}
			}
		}
		else {
			for (int32 i = WayPoints.Num()-1; i >0; i--) {
				FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
				float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
				if (CurrentSqDistance < MinSqDistance && CurrentIndexWayPoint != i) {
					ClosestWayPoint = WayPointWorld;
					MinSqDistance = CurrentSqDistance;
					ClosestWayPointIndex = i;
				}
			}
		}
		CurrentIndexWayPoint = ClosestWayPointIndex;
		if (ClosestWayPointIndex == 0 || ClosestWayPointIndex== WayPoints.Num() - 1) {
			bReturnToStart = !bReturnToStart;
		}
	}
	return ClosestWayPoint;
}
*/

FVector UAIPatrollingComponent::GetClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector>WayPoints = PatrollingPath->GetWayPoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();
	FVector ClosestWayPoint;
	float MinSqDistance = FLT_MAX;
	int32 ClosestWayPointIndex = CurrentIndexWayPoint;
	for (int32 i = 0; i < WayPoints.Num(); i++) {
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if (CurrentSqDistance < MinSqDistance && CurrentIndexWayPoint != i) {
			ClosestWayPoint = WayPointWorld;
			MinSqDistance = CurrentSqDistance;
			ClosestWayPointIndex = i;
		}
	}
	CurrentIndexWayPoint = ClosestWayPointIndex;
	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWayPoint()
{
	FVector WayPoint;
	const TArray<FVector>WayPoints = PatrollingPath->GetWayPoints();
	if (PatrollingType == FPatrollingType::Circle) {
		CurrentIndexWayPoint++;
		if (CurrentIndexWayPoint == PatrollingPath->GetWayPoints().Num()) {
			CurrentIndexWayPoint = 0;
		}
		FTransform PathTransform = PatrollingPath->GetActorTransform();
		WayPoint = PathTransform.TransformPosition(WayPoints[CurrentIndexWayPoint]);
	}
	else {
		if (!bReturnToStart) {
			CurrentIndexWayPoint++;
			FTransform PathTransform = PatrollingPath->GetActorTransform();
			if(CurrentIndexWayPoint< WayPoints.Num())
			WayPoint = PathTransform.TransformPosition(WayPoints[CurrentIndexWayPoint]);
		}
		else {
			CurrentIndexWayPoint--;
			FTransform PathTransform = PatrollingPath->GetActorTransform();
			if (CurrentIndexWayPoint > -1)
			WayPoint = PathTransform.TransformPosition(WayPoints[CurrentIndexWayPoint]);
		}
		
		if (CurrentIndexWayPoint == 0 || CurrentIndexWayPoint == WayPoints.Num() - 1) {
			bReturnToStart = !bReturnToStart;
		}
	}
	return WayPoint;
}