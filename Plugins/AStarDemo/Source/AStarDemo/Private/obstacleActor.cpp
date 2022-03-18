// Fill out your copyright notice in the Description page of Project Settings.


#include "obstacleActor.h"

UobstacleActor::UobstacleActor() {
	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/AStarDemo/Cube.Cube'"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface>MaterialAsset(TEXT("MaterialInstanceConstant'/AStarDemo/BlueMatInst.BlueMatInst'"));
	SetStaticMesh(MeshAsset.Object);
	SetMaterial(0, MaterialAsset.Object);
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}