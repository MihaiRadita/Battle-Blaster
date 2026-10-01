// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"

#include "Tower.h"

#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::ReceiveBeginPlay();

	TArray<AActor*>Towers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();

	UE_LOG(LogTemp, Display, TEXT("Number of Towers: %d"), TowerCount);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		Tank = Cast<ATank>(PlayerPawn);

		if (!Tank)
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode: Failed to find the tank actor!"));
		}

		int32 LoopIndex = 0;

		while (LoopIndex < TowerCount)
		{
			AActor* TowerActor = Towers[LoopIndex];


			if (TowerActor)
			{
				ATower* Tower = Cast<ATower>(TowerActor);

				if (Tower && Tank)
				{
					Tower->Tank = Tank;
				}
			}
			LoopIndex++;
		}
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);

		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");
		}
	}

	CountdownSeconds = CountdownDelay;
	GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout, 1.0f, true);
}


void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
	
	

	if (CountdownSeconds > 0)
	{
		ScreenMessageWidget->SetMessageText(FString::FromInt(CountdownSeconds));
		CountdownSeconds--;
	}
	else if (CountdownSeconds == 0)
	{
		ScreenMessageWidget->SetMessageText("GO!");
		Tank->SetPlayerEnabled(true);
		CountdownSeconds--;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool IsGameOver = false;
	

	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		IsGameOver = true;
	}
	else
	{
		ATower* DeadTower = Cast<ATower>(DeadActor);

		if (DeadTower)
		{
			DeadTower->HandleDestruction();

			TowerCount--;

			if (TowerCount == 0)
			{
				
				IsVictory = true;
				IsGameOver = true;
			}

		}
	}

	if (IsGameOver)
	{
		FString GameOVerStateMessage = "";
		GameOVerStateMessage = IsVictory ? "Victory" : "Defeat";

	
		ScreenMessageWidget->SetMessageText(GameOVerStateMessage);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);

		FTimerHandle GameOverTimerHandle;
		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOvertTimerTimeout, GameOverDelay, false);

	}
}

void ABattleBlasterGameMode::OnGameOvertTimerTimeout()
{
	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance);

		if (BattleBlasterGameInstance)
		{
			if (IsVictory)
			{
				//Load the next level

				BattleBlasterGameInstance->LoadNextLevel();
			}
			else
			{
				//Reload the current level
				BattleBlasterGameInstance->RestartCurrentLevel();
			}
		}
	}


}
