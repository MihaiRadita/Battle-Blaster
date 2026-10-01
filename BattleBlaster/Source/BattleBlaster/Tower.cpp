// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle FireTimerHandle;
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATower::ChekcFireCondition, FIreRate, true);

}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsInFireRange())
	{
		RotateTurret(Tank->GetActorLocation());
	}
}

void ATower::ChekcFireCondition()
{
	//UE_LOG(LogTemp, Display, TEXT("Timeout"));


	if (Tank && Tank->IsAlive && IsInFireRange())
	{
		Fire();
	}
}

bool ATower::IsInFireRange()
{

	bool Result = false;

	if (Tank)
	{
		float DistanceToTank = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
		Result = DistanceToTank <= FIreRange;
	}
	
	return Result;
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();

	UE_LOG(LogTemp, Display, TEXT("Tower HandleDestruction!"));

	Destroy();
}