// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarActor.h"

#include "DrawDebugHelpers.h"
#include "..\Public\AStarActor.h"

// Sets default values
AAStarActor::AAStarActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
//	SetRootComponent(&rootComp);
	//DrawDebugCircle(GetWorld(),FMatrix(),radius,16,FColor(),true); //show user the radius of the generated map 
	//TODO - Set to be a box not radius
	//possibly nvm? revisit

	/*
	generate map
	-randomly place objects in area

	solve possible positions
	-multiraycast down every x distance, record okay spots

	decide start/end
	-random positions, raycast down to make sure its valid

	solve path
	-A*

	draw path
	
	
	
	
	
	*/

	USceneComponent* SceneLayout = CreateDefaultSubobject<USceneComponent>( TEXT("SceneLayout"));
	SceneLayout->RegisterComponent();
	//SetRootComponent(SceneLayout);


	placeActors(density);
}

void AAStarActor::PostInitProperties()
{
	Super::PostInitProperties();
	placeActors(density);
}

bool AAStarActor::generateMap(int32 v_density)
{
	for (int32 x = 0; x < 30; ++x) {
		for (int32 y = 0; y < 30; ++y) {
			FHitResult HitResult;
			FVector Start = { (float(x) - 15) * 50, (float(y) - 15) * 50, 500};
			FVector End = { (float(x) - 15) * 50, (float(y) - 15) * 50, -100 };
			FCollisionQueryParams fcqp;
			fcqp.bTraceComplex = true;
			FCollisionResponseParams fcrp;
			FCollisionObjectQueryParams fcoqp;
			fcoqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
			fcoqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
			if (!GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, fcoqp,fcqp)) {
				DrawDebugPoint(GetWorld(), Start, 10, FColor(1, 1, 0, 0), true, 360);
			}
			DrawDebugLine(
				GetWorld(),
				Start,
				HitResult.bBlockingHit?HitResult.Location:End,
				FColor(255, 0, 0),
				false, 360, 0,
				6.333
			);
			
		}
	}
	return false;
}

bool AAStarActor::placeActors(int32 v_density)
{

	for (auto& a : ObstacleList) {
		if (a != nullptr)
			//a->
			a->DestroyComponent();
	}
	for (int32 i = 0; i < v_density; ++i) {

		FVector try_loc = GetActorLocation() + FVector(FMath::RandRange(-1 * radius, radius), FMath::RandRange(-1 * radius, radius), 0);
		UClass* obstClass = FindObject<UClass>(ANY_PACKAGE, TEXT("Class'/Script/AStarDemo.obstacleActor'"));
		UobstacleActor* newObs = NewObject<UobstacleActor>(this, MakeUniqueObjectName(this, obstClass, FName("ObstaclesA")));
		newObs->SetWorldLocation(try_loc);
		ObstacleList.Add(newObs);


		//UActorComponent* newObs = AddActorComponent(this, TEXT("Class'/Script/AStarDemo.obstacleActor'"));
		//if (newObs) {
		//	ObstacleList.Add(dynamic_cast<UobstacleActor*>(newObs));
		//}
		//AddComponent(TEXT("Class'/Script/AStarDemo.obstacleActor'"), true, FTransform(),);

		//UobstacleActor* newObs = GetWorld()->SpawnActor<UobstacleActor>(UobstacleActor::StaticClass(), try_loc, FRotator());
		//ObstacleList.Add(newObs);

	}
	return false;
}

// Called when the game starts or when spawned
void AAStarActor::BeginPlay()
{
	Super::BeginPlay();

	generateMap(density);
}

// Called every frame
void AAStarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UActorComponent* AAStarActor::AddActorComponent(AActor* Owner, FName ActorComponentClass)
{

	//UClass* baseClass = FindObject<UClass>(ANY_PACKAGE, TEXT("ActorComponent"));
	//if (ActorComponentClass->IsChildOf(baseClass))
	//{
		UActorComponent* NewComp = Owner->CreateDefaultSubobject<UobstacleActor>(ActorComponentClass);
		if (!NewComp)
		{
			return nullptr;
		}

		NewComp->RegisterComponent();     

		return NewComp;
//	}
//
	return nullptr;
}

#if WITH_EDITOR
void AAStarActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	placeActors(density);
	generateMap(density);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif