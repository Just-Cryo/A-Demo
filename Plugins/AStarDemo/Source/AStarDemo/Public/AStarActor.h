// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "obstacleActor.h"
#include "AStarActor.generated.h"	

struct FStarNode {
public:
	FStarNode(bool v_rootNode, FVector v_position, int _id, int p_id, int s_id);
	float cost;
	float dist;
	float total;
	int map_id;
	int node_id;
	FVector position;
	int parent_id;
	bool rootNode = false;
};

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


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Meta = (Min = "0", Max = "9999"))
		int32 radius = 500; //size of map - controls both pathfinding area as well as object spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (Min = "0", Max = "500"))
		int32 density = 10; //how many objects to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool spawnobjects = false; //enable to spawn random objects that may or may not impede pathfinding
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool debugDraw = false; //additional debug draw data
	UFUNCTION(BlueprintCallable)
	bool generateMap(); //generates the actual map locations
	UFUNCTION(BlueprintCallable)
	bool placeActors(int32 v_density); //place actors at random places
	

	UFUNCTION(BlueprintCallable,BlueprintPure)
	TArray<FVector> f_solvePath(TArray<FVector> points, int32 mapWidth, FVector start, FVector end, int32 max_cycles = 9999, int32 tile_distance = 50); //raw access to the A* Algorithm

	UFUNCTION(BlueprintCallable)
		TArray<FVector> solveRandomPath(); //solve a random path based on internally generated data 

	UFUNCTION(BlueprintCallable)
	void drawPath(TArray<FVector> path); //debug draw the map

	TArray<FVector> storedMap;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY()
	TArray< AobstacleActor*> ObstacleList;

};
