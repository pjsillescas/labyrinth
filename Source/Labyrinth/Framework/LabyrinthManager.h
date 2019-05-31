// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrinthManager.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	ED_None		UMETA(DisplayName = "None"),
	ED_Up		UMETA(DisplayName = "Up"),
	ED_Down		UMETA(DisplayName = "Down"),
	ED_Right	UMETA(DisplayName = "Right"),
	ED_Left		UMETA(DisplayName = "Left")
};

UCLASS()
class LABYRINTH_API ALabyrinthManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALabyrinthManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CreateRoom(FVector2D Origin, float Width, float Height, EDirection Direction);
	void CreateCorridor(FVector2D Origin, int32 Length, EDirection Direction, float Width, float Height);
	bool CheckCorridor(FVector2D Origin, int32 Length, EDirection Direction, float Width, float Height);
	EDirection GetAntiDirection(EDirection Direction);
	bool CheckRoom(FVector2D Origin, float Width, float Height, EDirection Direction);

	bool IsEmptyPosition(FVector2D Origin, int32 I, int32 J,FColor Color = FColor::Red);
	
	void SpawnItems();
	void SpawnEnemies();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Labyrinth Generation")
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Labyrinth Generation")
	int32 MinCorridorLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Labyrinth Generation")
	int32 MaxCorridorLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Labyrinth Generation")
	float ZFloor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Labyrinth Generation")
	TSubclassOf<AActor> WallClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Labyrinth Generation")
	TSubclassOf<AActor> FloorClass;

	AActor* SpawnElement(UClass* Class, FVector Location);
	AActor* GetActorAtPosition(FVector2D Origin, int32 I, int32 J, FColor Color);
	FVector ToVector3D(FVector2D Vector2D, int32 I, int32 J);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable,Category = "Labyrinth Generation")
	void GenerateLabyrinth(FVector2D Origin,float Width,float Height,EDirection Direction,int32 Count);

};
