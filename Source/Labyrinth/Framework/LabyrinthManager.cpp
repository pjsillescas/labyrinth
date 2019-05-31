// Fill out your copyright notice in the Description page of Project Settings.


#include "LabyrinthManager.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

// Sets default values
ALabyrinthManager::ALabyrinthManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MinCorridorLength = 3;
	MaxCorridorLength = 5;

	TileSize = 32;

}

// Called when the game starts or when spawned
void ALabyrinthManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALabyrinthManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALabyrinthManager::SpawnItems()
{
	/*
	n = floor(random(instance_number(obj_suelo)));
	with(instance_find(obj_suelo, n)) { instance_create(x, y, obj_escalera); }


	i = 0;
	while (i < 10)
	{
		n = floor(random(instance_number(obj_suelo)));
		with(instance_find(obj_suelo, n))
		{
			if (place_meeting(x, y, obj_escalera)) { other.i -= 1; }
			else { instance_create(x, y, obj_cofre); }
		}
		i += 1;
	}

	i = 0;
	while (i < 30)
	{
		n = floor(random(instance_number(obj_suelo)));
		with(instance_find(obj_suelo, n))
		{
			if (place_meeting(x, y, obj_escalera) or place_meeting(x, y, obj_cofre)) { other.i -= 1; }
			else { instance_create(x, y, obj_coin); }
		}
		i += 1;
	}
	*/
}

void ALabyrinthManager::SpawnEnemies()
{
	/*
	i = 0;
	while (i < 20)
	{
		n = floor(random(instance_number(obj_suelo)));
		with(instance_find(obj_suelo, n))
		{
			instance_create(x, y, obj_enemigo);
		}
		i += 1;
	}
	*/
}

FVector ALabyrinthManager::ToVector3D(FVector2D Vector2D,int32 I,int32 J)
{
	float XCoordinate = (Vector2D.X + I) * TileSize;
	float YCoordinate = (Vector2D.Y + J) * TileSize;
	
	return FVector (XCoordinate, YCoordinate, ZFloor);
}

AActor* ALabyrinthManager::GetActorAtPosition(FVector2D Origin, int32 I, int32 J, FColor Color)
{
	const float Offset = 200;

	float XCoordinate = (Origin.X + I) * TileSize;
	float YCoordinate = (Origin.Y + J) * TileSize;

	FCollisionQueryParams Params;
	FHitResult OutHit;
	//FVector Start(XCoordinate, YCoordinate, ZFloor - Offset / 2), End(XCoordinate, YCoordinate, ZFloor + Offset / 2);
	FVector OffsetVector(0, 0, Offset / 2);
	FVector Start = ToVector3D(Origin,I,J) - OffsetVector,End = ToVector3D(Origin,I,J) + OffsetVector;
	UWorld* World = GetWorld();
	bool Success = World != nullptr && World->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_WorldDynamic, Params);
	//DrawDebugLine(GetWorld(), Start, End, Color, true, 100);

	return (Success) ? OutHit.GetActor() : nullptr;
}

bool ALabyrinthManager::IsEmptyPosition(FVector2D Origin, int32 I, int32 J,FColor Color)
{
	/*
	const float Offset = 200;

	float XCoordinate = (Origin.X + I) * TileSize;
	float YCoordinate = (Origin.Y + J) * TileSize;

	FCollisionQueryParams Params;
	FHitResult OutHit;
	FVector Start(XCoordinate, YCoordinate,ZFloor - Offset/2), End(XCoordinate, YCoordinate, ZFloor + Offset/2);
	UWorld* World = GetWorld();
	bool Success = World != nullptr && !World->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_WorldDynamic, Params);
	DrawDebugLine(GetWorld(), Start, End, Color, true, 100);
	*/

	AActor* Actor = GetActorAtPosition(Origin,I,J,Color);

	return Actor == nullptr;
}

bool ALabyrinthManager::CheckRoom(FVector2D Origin,float Width,float Height,EDirection Direction)
{
	for (int32 I = -FMath::FloorToInt(Width / 2); I < FMath::CeilToInt(Width / 2); I++)
	{
		for (int32 J = -FMath::FloorToInt(Height / 2); J < FMath::CeilToInt(Height / 2); J++)
		{
			if (!IsEmptyPosition(Origin,I,J)) { return false; }
		}
	}
	return true;
}

bool ALabyrinthManager::CreateRoom(FVector2D Origin, float Width, float Height, EDirection Direction)
{
	if (!CheckRoom(Origin, Width, Height, Direction))
	{
		UE_LOG(LogTemp,Warning,TEXT("Check room false"));
		FVector FloorPosition;
		//Taponar el puente:
		switch (Direction)
		{
		case EDirection::ED_Down:
			FloorPosition.Set(Origin.X * TileSize, (Origin.Y - Height / 2) * TileSize,ZFloor);
			break;
		case EDirection::ED_Up:
			FloorPosition.Set(Origin.X * TileSize, (Origin.Y + Height / 2) * TileSize, ZFloor);
			break;
		case EDirection::ED_Left:
			FloorPosition.Set((Origin.X + Width / 2) * TileSize, Origin.Y * TileSize, ZFloor);
			break;
		case EDirection::ED_Right:
			FloorPosition.Set((Origin.X - Width / 2) * TileSize, Origin.Y * TileSize, ZFloor);
			break;
		default:
			return false;
		}

		UWorld* World = GetWorld();

		if (World == nullptr)
			return false;

		FTransform Transform;
		Transform.SetLocation(FloorPosition);
		FActorSpawnParameters ActorSpawnParameters;

		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor(WallClass, &Transform, ActorSpawnParameters);
		
		
		
		return false; //No construir
	}

	//Construir:
	for(int32 I = -FMath::FloorToInt((float)Width / 2); I <= FMath::FloorToInt((float)Width / 2); I++)
	{
		for (int32 J = -FMath::FloorToInt((float)Height / 2); J <= FMath::CeilToInt((float)Height / 2); J++)
		{
			if (I == -FMath::FloorToInt((float)Width / 2) || J == -FMath::FloorToInt((float)Height / 2) || I == FMath::CeilToInt((float)Width / 2) || J == FMath::CeilToInt((float)Height / 2) )
			{
				bool cond1 = ((Direction == EDirection::ED_Right && J == 0 && I < 0));
				bool cond2 = ((Direction == EDirection::ED_Left && J == 0 && I > 0));
				bool cond3 = ((Direction == EDirection::ED_Up && I == 0 && J > 0));
				bool cond4 = ((Direction == EDirection::ED_Down && I == 0 && J < 0));
				
				if (!(cond1 || cond2 || cond3 || cond4))
				{
					SpawnElement(WallClass,FVector((Origin.X + I) * TileSize,(Origin.Y + J) * TileSize,ZFloor));
				}
				else
				{
					SpawnElement(FloorClass, FVector((Origin.X + I) * TileSize, (Origin.Y + J) * TileSize, ZFloor));
				}
			}
			else
			{
				SpawnElement(FloorClass, FVector((Origin.X + I) * TileSize, (Origin.Y + J) * TileSize, ZFloor));
			}
		}
	}

	return true;
}

AActor* ALabyrinthManager::SpawnElement(UClass* Class, FVector Location)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
		return nullptr;

	FTransform Transform;
	Transform.SetLocation(Location);
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	return World->SpawnActor(Class, &Transform, ActorSpawnParameters);
}

void ALabyrinthManager::CreateCorridor(FVector2D Origin, int32 Length, EDirection Direction, float Width, float Height)
{
	FVector2D NewOrigin = Origin;
	AActor* Actor1;
	switch (Direction)
	{
	case EDirection::ED_Right:
		NewOrigin.X += FMath::CeilToInt(Width / 2) - 1;

		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Yellow);
		if (Actor1 != nullptr)
			Actor1->Destroy();
		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Yellow);
		if (Actor1 != nullptr)
			Actor1->Destroy();

		for (int32 I = 0; I < Length; I++)
		{
			SpawnElement(FloorClass, ToVector3D(NewOrigin, I, 0));
			if (I < Length)
			{
				SpawnElement(WallClass, ToVector3D(NewOrigin, I, -1));
				SpawnElement(WallClass, ToVector3D(NewOrigin, I, 1));
			}
		}

		break;
	case EDirection::ED_Left:
		NewOrigin.X -= FMath::FloorToInt(Width / 2);

		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Cyan);
		if (Actor1 != nullptr)
			Actor1->Destroy();
		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Cyan);
		if (Actor1 != nullptr)
			Actor1->Destroy();

		for (int32 I = 0; I < Length; I++)
		{
			SpawnElement(FloorClass, ToVector3D(NewOrigin, -I, 0));
			if (I < Length)
			{
				SpawnElement(WallClass, ToVector3D(NewOrigin, -I, -1));
				SpawnElement(WallClass, ToVector3D(NewOrigin, -I, 1));
			}
		}

		break;
	case EDirection::ED_Up:
		NewOrigin.Y -= FMath::FloorToInt(Height / 2); // + 1;

		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Blue);
		if (Actor1 != nullptr)
			Actor1->Destroy();
		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Blue);
		if (Actor1 != nullptr)
			Actor1->Destroy();

		for (int32 I = 0; I < Length; I++)
		{
			SpawnElement(FloorClass, ToVector3D(NewOrigin, 0, -I));
			if (I < Length)
			{
				SpawnElement(WallClass, ToVector3D(NewOrigin, -1, -I));
				SpawnElement(WallClass, ToVector3D(NewOrigin, 1, -I));
			}
		}

		break;
	case EDirection::ED_Down:
		NewOrigin.Y += FMath::CeilToInt(Height / 2) - 1;

		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Black);
		if (Actor1 != nullptr)
			Actor1->Destroy();
		Actor1 = GetActorAtPosition(NewOrigin, 0, 0, FColor::Black);
		if (Actor1 != nullptr)
			Actor1->Destroy();

		for (int32 I = 0; I < Length; I++)
		{
			SpawnElement(FloorClass, ToVector3D(NewOrigin, 0, I));
			if (I < Length)
			{
				SpawnElement(WallClass, ToVector3D(NewOrigin, -1, I));
				SpawnElement(WallClass, ToVector3D(NewOrigin, 1, I));
			}
		}

		break;
	}
}

FString DirectionToString(EDirection Dir)
{
	FString Output = "";
	switch (Dir)
	{
	case EDirection::ED_None: Output = "None"; break;
	case EDirection::ED_Up: Output = "Up"; break;
	case EDirection::ED_Down: Output = "Down"; break;
	case EDirection::ED_Left: Output = "Left"; break;
	case EDirection::ED_Right: Output = "Right"; break;
	}

	return Output;
}

bool ALabyrinthManager::CheckCorridor(FVector2D Origin, int32 Length, EDirection Direction, float Width, float Height)
{
	int32 FixedLength = Length + 5;
	
	//Desde el centro avanzar el w1/2 o el h1/2 hasta un extremo,
	//ahi romper el bloque que habia en la pared (cuenta como 1º elemento del puente)
	//y seguir trazando el puente con su borde todo lo largo que tenga que ser
	FVector2D NewOrigin = Origin;
	if (Direction == EDirection::ED_Right)
	{
		NewOrigin.X += FMath::CeilToInt((float)Width / 2);
		for (int32 I = 0; I < FixedLength; I++)
		{
			if (!IsEmptyPosition(NewOrigin,I,0,FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,I,-1, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,I,1, FColor::Green)) { return false; }
		}
	}
	else if (Direction == EDirection::ED_Left)
	{
		NewOrigin.X -= FMath::FloorToInt((float)Width / 2) + 1;
		for (int32 I = 0; I < FixedLength; I++)
		{
			if (!IsEmptyPosition(NewOrigin,-I,0, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,-I,-1, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,-I,1, FColor::Green)) { return false; }
		}
	}
	else if (Direction == EDirection::ED_Down)
	{
		NewOrigin.Y -= FMath::CeilToInt((float)Height / 2);
		for (int32 I = 0; I < FixedLength; I++)
		{
			if (!IsEmptyPosition(NewOrigin,0,I, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,-1,I, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,1,I, FColor::Green)) { return false; }
		}
	}
	else if (Direction == EDirection::ED_Up)
	{
		NewOrigin.Y -= FMath::FloorToInt((float)Height / 2) + 1;
		for (int32 I = 0; I < FixedLength; I++)
		{
			if (!IsEmptyPosition(NewOrigin,0,-I, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,-1,-I, FColor::Green)) { return false; }
			if (!IsEmptyPosition(NewOrigin,1,-I, FColor::Green)) { return false; }
		}
	}

	return true;
}

EDirection ALabyrinthManager::GetAntiDirection(EDirection Direction)
{
	return EDirection::ED_None;
}

int Choose(TArray<int32> Values)
{
	return Values[FMath::RandRange(0, Values.Num() - 1)];
}

void ALabyrinthManager::GenerateLabyrinth(FVector2D Origin, float Width, float Height, EDirection Direction,int32 Count)
{
	bool Success = CreateRoom(Origin, Width, Height, Direction);

	if (Count > 0 && Success)
	{
		int32 Repetitions = FMath::RandRange(1, 2);
		int32 K = 0;
		while(K < Repetitions)
		{
			//CAMINO:
			int32 Length = FMath::RandRange(MinCorridorLength, MaxCorridorLength);

			EDirection OldDirection = Direction;

			TArray<EDirection> PossibleDirections;
			PossibleDirections.Add(EDirection::ED_Down);
			PossibleDirections.Add(EDirection::ED_Up);
			PossibleDirections.Add(EDirection::ED_Left);
			PossibleDirections.Add(EDirection::ED_Right);

			PossibleDirections.Sort([this](const EDirection Item1, const EDirection Item2) {
				return FMath::FRand() < 0.5f;
			});
			
			int32 K1 = 0;
			EDirection NewDirection = EDirection::ED_None;
			while (NewDirection == EDirection::ED_None && K1 < PossibleDirections.Num())
			{
				if (PossibleDirections[K1] != GetAntiDirection(Direction) && CheckCorridor(Origin, Length, PossibleDirections[K1], Width, Height))
					NewDirection = PossibleDirections[K1];

				K1++;
			}
			CreateCorridor(Origin, Length, NewDirection, Width, Height);

			//Next room:
			TArray<int32> NextSizeValues;
			NextSizeValues.Add(6);
			NextSizeValues.Add(8);
			NextSizeValues.Add(10);

			int32 NewWidth = Choose(NextSizeValues);
			int32 NewHeight = Choose(NextSizeValues);
			FVector2D NewOrigin = Origin;

			switch (NewDirection)
			{
			case EDirection::ED_Down:
				NewOrigin.Y += Length + NewHeight / 2 + Height / 2 - 1;
				break;
			case EDirection::ED_Left:
				NewOrigin.X -= Length + NewWidth / 2 + Width / 2 - 1;
				break;
			case EDirection::ED_Right:
				NewOrigin.X += Length + NewWidth / 2 + Width / 2 - 1;
				break;
			case EDirection::ED_Up:
				NewOrigin.Y -= Length + NewHeight / 2 + Height / 2 - 1;
				break;
			default:
				;
			}

			GenerateLabyrinth(NewOrigin, NewWidth, NewHeight, NewDirection, Count - 1);

			K++;
		}
	}
}
