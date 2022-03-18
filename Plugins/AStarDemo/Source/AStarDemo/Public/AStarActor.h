// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "obstacleActor.h"
#include "AStarActor.generated.h"	

UCLASS()
class ASTARDEMO_API AAStarActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAStarActor();
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	USceneComponent* rootComp;

	UPROPERTY(EditAnywhere, Meta = (Min = "0", Max = "9999"))
		int32 radius = 1000;
	UPROPERTY(EditAnywhere, Meta = (Min = "0", Max = "500"))
		int32 density = 10;
	bool generateMap(int32 v_density); //generates the map, returns true if successful
	bool placeActors(int32 v_density); //place actors at their corresponding locations
	TArray<FVector> findPath(FVector2D start, FVector2D target); //solve for the best path
	void drawPath(TArray<FVector>& path); //debug draw the map
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	TArray< UobstacleActor*> ObstacleList;
	static UActorComponent* AddActorComponent(AActor* Owner, FName ActorComponentClass);

};
