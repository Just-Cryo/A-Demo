// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "obstacleActor.h"
#include "AStarActor.generated.h"	

struct FStarNode {
public:
	FStarNode(bool v_rootNode, FVector v_position, FStarNode* v_pNode,int _id, int p_id, int s_id);
	float cost;
	float dist;
	float total;
	int map_id;
	int node_id;
	FVector position;
	FStarNode* parentNode;
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

	USceneComponent* rootComp;

	UPROPERTY(EditAnywhere, Meta = (Min = "0", Max = "9999"))
		int32 radius = 1000;
	UPROPERTY(EditAnywhere, Meta = (Min = "0", Max = "500"))
		int32 density = 10;
	UPROPERTY(EditAnywhere)
		bool spawnobjects = false;
	bool generateMap(int32 v_density); //generates the map, returns true if successful
	bool placeActors(int32 v_density); //place actors at their corresponding locations
	TArray<FVector> findPath(FVector2D start, FVector2D target); //solve for the best path
	void drawPath(TArray<FVector> path); //debug draw the map

	TArray<FVector> f_solvePath(TArray<FVector> points, int32 mapWidth, FVector start, FVector end, int32 max_cycles = 9999, int32 tile_distance = 50);


	TArray<FVector> storedMap;
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
