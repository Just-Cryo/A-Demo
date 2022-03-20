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


	//placeActors(density);
}

void AAStarActor::PostInitProperties()
{
	Super::PostInitProperties();
	placeActors(density);
}

bool AAStarActor::generateMap(int32 v_density)
{
	storedMap.Empty();
	for (int32 x = 0; x < 30; ++x) {
		for (int32 y = 0; y < 30; ++y) {
			FHitResult HitResult;
			FVector Start = { (float(x) - 15) * 50, (float(y) - 15) * 50, 300};
			FVector End = { (float(x) - 15) * 50, (float(y) - 15) * 50, 50 };
			FCollisionQueryParams fcqp;
			fcqp.bTraceComplex = true;
			FCollisionResponseParams fcrp;
			FCollisionObjectQueryParams fcoqp;
			fcoqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
			fcoqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
			if (!GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, fcoqp,fcqp)) {
			//	DrawDebugPoint(GetWorld(), Start, 10, FColor(1, 1, 0, 0), true, 360);
			}
		//	DrawDebugLine(
		//		GetWorld(),
		//		Start,
		//		HitResult.bBlockingHit?HitResult.Location:End,
		//		FColor(255, 0, 0),
		//		false, 360, 0,
		//		.5
		//	);
			storedMap.Add(FVector(Start.X, Start.Y, HitResult.bBlockingHit ? -1 : 0));
			
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

void AAStarActor::drawPath(TArray<FVector> path)
{
	for (int i = 0; i < path.Num()-1; ++i) {
		//UE_LOG(LogTemp, Warning, TEXT("%i: %x, %y"),i,path[i].X,path[i].Y);
		DrawDebugLine(GetWorld(),
			FVector(path[i].X,path[i].Y,10),
			FVector(path[i+1].X, path[i+1].Y, 10),
			FColor(0,255,0),
			true, 360,'\000', 20
			);
		DrawDebugPoint(GetWorld(),
			FVector(path[i].X, path[i].Y, 10), 25,
			FColor(150, 150, 0),
			true, 360);
	}
}

// Called when the game starts or when spawned
void AAStarActor::BeginPlay()
{
	Super::BeginPlay();

	generateMap(density);
	for (int i = 0; i < storedMap.Num();++i) {
		UE_LOG(LogTemp, Log, TEXT("%d: (%f, %f, %f)"), i, storedMap[i].X, storedMap[i].Y, storedMap[i].Z);
	}
	FVector start = FVector(-1, -1, -1);// storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	FVector end = FVector(-1, -1, -1);//storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	while (start.Z == -1) {
		start = storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	}
	while (end.Z == -1) {
		end = storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	}

	DrawDebugPoint(GetWorld(),
		FVector(start.X, start.Y, 10),50,
		FColor(0, 255, 0),
		true, 360);
	DrawDebugPoint(GetWorld(),
		FVector(end.X, end.Y, 10), 50,
		FColor(255, 0, 150),
		true, 360);
	TArray<FVector> t_path = f_solvePath(storedMap, 30, start, end);
	drawPath(t_path);

	int32 i = 0;
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


TArray<FVector> AAStarActor::f_solvePath(TArray<FVector> points, int32 mapWidth, FVector start, FVector end, int32 max_cycles, int32 tile_distance) {

	int32 startindx = -1;
	points.Find(start, startindx);
	FStarNode firstNode(true, start, nullptr, startindx,-1,0);
	firstNode.cost = 0;
	firstNode.dist = FVector::Dist(end, start) / tile_distance;
	firstNode.total = firstNode.dist;
	TArray<FStarNode> nodes;
	TArray<FStarNode*> openNodes, closedNodes;
	nodes.Add(firstNode);
	openNodes.Add(&nodes[0]);
	// openList
	// closedList


	//startNode
	for (int32 m_c = 0; openNodes.Num() > 0 && m_c < max_cycles;++m_c) {
		FStarNode* curNode = openNodes[0];
		int32 curIndx = 0;
		for (int32 i = 0; i < openNodes.Num(); ++i) {//find the cheapest costing node
			if (openNodes[i]->total < curNode->total) {
				curNode = openNodes[i];
				curIndx = i;
			}
		}
		openNodes.RemoveAt(curIndx);
		closedNodes.Add(curNode);
		
		DrawDebugPoint(GetWorld(),
			FVector(curNode->position.X, curNode->position.Y, 50), 5,
			FColor(0*float(m_c)/ max_cycles, curNode->dist*0, curNode->cost),
			true, 360);


		if (curNode->position == end) {
			//make path and return :)
			TArray<FVector> returnPath;
			while (curNode->parent_id>=0) {
				UE_LOG(LogTemp, Warning, TEXT("%i: (%f, %f) -> %f"), curNode->node_id, curNode->position.X, curNode->position.Y, curNode->parent_id);
				returnPath.Insert(curNode->position,0);
				curNode = &nodes[curNode->parent_id];
			}//do one last time as the root node location wont be added
			return returnPath;
		}
		TArray<FStarNode> childNodes;

		for (int i = 0; i < 8; ++i) { // do for each neighboring spot
			//check if the spot is valid
			/*
			012
			3X4
			567

			-x	: []-1
			+x	: []+1
			-y	: []-width
			+y	: []+width

			*/
			FVector newNodePos;
			int32 newNodeIndx = -1;
			switch (i) {
			case 0:
				newNodeIndx = curNode->map_id - 1 - mapWidth;
				break;
			case 1:
				newNodeIndx = curNode->map_id - mapWidth;
				break;
			case 2:
				newNodeIndx = curNode->map_id + 1 - mapWidth;
				break;
			case 3:
				newNodeIndx = curNode->map_id - 1;
				break;
			case 4:
				newNodeIndx = curNode->map_id + 1;
				break;
			case 5:
				newNodeIndx = curNode->map_id - 1 + mapWidth;
				break;
			case 6:
				newNodeIndx = curNode->map_id + mapWidth;
				break;
			case 7:
				newNodeIndx = curNode->map_id +1+ mapWidth;
				break;
			}
			if (points.IsValidIndex(newNodeIndx)) { //check if its a valid index before continuing;
				if (points[newNodeIndx].Z != -1) {
					newNodePos = points[newNodeIndx]; 

					//UE_LOG(LogTemp, Log, TEXT("%d: (%f, %f) -> (%f, %f)"), i, curNode->position.X, curNode->position.Y, newNodePos.X, newNodePos.Y);
					FStarNode n_child(false, points[newNodeIndx], curNode, newNodeIndx, curNode->node_id, -1);
					childNodes.Add(n_child);
				}
			}
			//if (points[newNodeIndx].Z == -1)//as this will be for a 2d map, we can use the Z variable to store if its walkable---
			//	continue;
			//newNodePos = points[newNodeIndx];


			


		}

		for (int32 c = 0; c < childNodes.Num(); ++c) {
			bool skip = false;
			for (int32 i = 0; i < closedNodes.Num(); ++i) {
				if (childNodes[c].map_id == closedNodes[i]->map_id) {// we dont want duplicates
					skip = true;
				}
			}

			for (int32 i = 0; i < openNodes.Num(); ++i) {
				if (childNodes[c].map_id == openNodes[i]->map_id) {// we dont want duplicates
					skip = true;
				}
			}



			if (!skip) {
				childNodes[c].cost = curNode->cost + 1;
				childNodes[c].dist = FVector::Dist(end,childNodes[c].position) / tile_distance;
				childNodes[c].total = childNodes[c].cost + childNodes[c].dist;

				int32 t_indx = nodes.Add(childNodes[c]);
				nodes[t_indx].node_id = t_indx;
				openNodes.Add(&nodes[t_indx]);
			}
		}











	}
	//while !openList.Size()==0

		//cnode = min(f)
		//rem cn openlist
		//add cn to closelist

		// if cn == target
			//retrace path






	return TArray<FVector>();
}






FStarNode::FStarNode(bool v_rootNode, FVector v_position, FStarNode* v_pNode,int32 _id, int32 p_id, int32 s_id)
{
	rootNode = v_rootNode;
	map_id = _id;
	parent_id = p_id;
	node_id = s_id;
	if (!rootNode) {
		position = v_position;
		parentNode = v_pNode;
	}
	else {
		position = v_position;
		parentNode = nullptr;
	}
}
