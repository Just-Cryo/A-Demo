// Fill out your copyright notice in the Description page of Project Settings.


#include "obstacleActor.h"

AobstacleActor::AobstacleActor() {
	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/AStarDemo/Cube.Cube'"));

	bool collision_enabled = FMath::RandBool();
	const TCHAR* mat_Text = collision_enabled ? TEXT("MaterialInstanceConstant'/AStarDemo/RedMatInst.RedMatInst'") : TEXT("MaterialInstanceConstant'/AStarDemo/BlueMatInst.BlueMatInst'");
	ConstructorHelpers::FObjectFinder<UMaterialInterface>MaterialAsset(mat_Text);
	SetMobility(EComponentMobility::Movable);

	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetStaticMesh(MeshAsset.Object);
	GetStaticMeshComponent()->SetMaterial(0, MaterialAsset.Object);
	SetActorEnableCollision(collision_enabled);
	GetStaticMeshComponent()->SetCollisionEnabled(collision_enabled ? ECollisionEnabled::QueryAndPhysics: ECollisionEnabled::NoCollision);
	GetStaticMeshComponent()->SetCollisionResponseToAllChannels(collision_enabled ? ECollisionResponse::ECR_Block: ECollisionResponse::ECR_Ignore);
	SetActorScale3D(FVector(FMath::FRandRange(.25,1), FMath::FRandRange(.25, 1), collision_enabled ? 1.5:1));
}