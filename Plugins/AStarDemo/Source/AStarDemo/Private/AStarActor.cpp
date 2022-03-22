// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarActor.h"

#include "DrawDebugHelpers.h"
#include "..\Public\AStarActor.h"

// Sets default values
AAStarActor::AAStarActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAStarActor::PostInitProperties()
{
	Super::PostInitProperties();
	placeActors(density);
}

bool AAStarActor::generateMap()
{
	int32 tile_count = FMath::RoundHalfToEven(float(radius) * .04);
	storedMap.Empty();
	for (int32 x = 0; x < tile_count; ++x) {
		for (int32 y = 0; y < tile_count; ++y) {
			FHitResult HitResult;
			FVector Start = { (float(x) - (tile_count/2)) * 50, (float(y) - (tile_count / 2)) * 50, 300};
			FVector End = { (float(x) - (tile_count / 2)) * 50, (float(y) - (tile_count / 2)) * 50, 50 };
			FCollisionQueryParams fcqp;
			fcqp.bTraceComplex = true;
			FCollisionObjectQueryParams fcoqp;
			fcoqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
			fcoqp.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
			if (!GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, fcoqp,fcqp)) {
			//	DrawDebugPoint(GetWorld(), Start, 10, FColor(1, 1, 0, 0), true, 360);
			}
			if (debugDraw)
				DrawDebugSolidBox(GetWorld(), FVector(Start.X, Start.Y, 0), FVector(14, 14, 1), FColor((float(x)/30.0)*255, (float(y)/30.0)*255, 0), true, 360);
			
			storedMap.Add(FVector(Start.X, Start.Y, HitResult.bBlockingHit ? -1 : 0));
			
		}
	}
	return false;
}

bool AAStarActor::placeActors(int32 v_density)
{
	if (!spawnobjects)
		return false;
	for (auto& a : ObstacleList) {
		if (a != nullptr)
			a->Destroy();
	}
	for (int32 i = 0; i < v_density; ++i) {
		FVector try_loc = GetActorLocation() + FVector(FMath::RandRange(-1 * radius, radius), FMath::RandRange(-1 * radius, radius), 0);
		AobstacleActor* newObs = GetWorld()->SpawnActor<AobstacleActor>();
		newObs->SetActorLocation(try_loc);
		ObstacleList.Add(newObs);
	}
	return false;
}

void AAStarActor::drawPath(TArray<FVector> path)
{
	for (int i = 0; i < path.Num() - 1; ++i) {
		DrawDebugLine(GetWorld(),
			FVector(path[i].X, path[i].Y, 10),
			FVector(path[i + 1].X, path[i + 1].Y, 10),
			FColor(0, 255, 0),
			true, 360, '\000', 20
		);
		if (debugDraw) {
			DrawDebugPoint(GetWorld(),
				FVector(path[i].X, path[i].Y, 10), 25,
				FColor(150, 150, 0),
				true, 360);
		}
	}
}

// Called when the game starts or when spawned
void AAStarActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAStarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#if WITH_EDITOR
void AAStarActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	placeActors(density);
	//generateMap(density);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif


TArray<FVector> AAStarActor::f_solvePath(TArray<FVector> points, int32 mapWidth, FVector start, FVector end, int32 max_cycles, int32 tile_distance) {

	int32 startindx = -1;
	points.Find(start, startindx);
	FStarNode firstNode(true, start,  startindx,-1,0);
	firstNode.cost = 0;
	firstNode.dist = FVector::Dist(end, start) / tile_distance;
	firstNode.total = firstNode.dist;
	TArray<FStarNode> nodes;
	TArray<int> oN, cN;
	nodes.Add(firstNode);
	oN.Add(0);

	for (int32 m_c = 0; oN.Num() > 0 && m_c < max_cycles;++m_c) {
		//FStarNode* curNode = openNodes[0];
		int32 curIndx = 0;
		for (int32 i = 0; i < oN.Num(); ++i) {//find the cheapest costing node
			if (nodes[oN[i]].total < nodes[oN[curIndx]].total) {
				//curNode = openNodes[i];
				curIndx = i;
			}
		}
		if (debugDraw) {
			DrawDebugPoint(GetWorld(),
				FVector(nodes[oN[curIndx]].position.X, nodes[oN[curIndx]].position.Y, 50), 5,
				FColor(0 * float(m_c) / max_cycles, nodes[oN[curIndx]].dist * 0, nodes[oN[curIndx]].dist > 255 ? 255 : nodes[oN[curIndx]].dist),
				true, 360);
		}

		if (nodes[oN[curIndx]].position == end) {
			//make path and return :)
			TArray<FVector> returnPath;
			curIndx = oN[curIndx];
			while (nodes[curIndx].parent_id>=0) {
				UE_LOG(LogTemp, Warning, TEXT("%d: (%f, %f) -> %d   [%f , %f, %f]"), 
					nodes[curIndx].node_id,
					nodes[curIndx].position.X,
					nodes[curIndx].position.Y,
					nodes[curIndx].parent_id,
					float(nodes[curIndx].cost),
					nodes[curIndx].dist,
					nodes[curIndx].total);
				returnPath.Insert(nodes[curIndx].position,0);
				curIndx = nodes[curIndx].parent_id;
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
			int newNodeIndx = -1;
			switch (i) {
			case 0:
				if (nodes[oN[curIndx]].map_id % mapWidth != 0) //edge check
					newNodeIndx = nodes[oN[curIndx]].map_id - 1 - mapWidth;
				break;
			case 1:
				newNodeIndx = nodes[oN[curIndx]].map_id - mapWidth;
				break;
			case 2:
				if (nodes[oN[curIndx]].map_id % mapWidth != mapWidth-1) //edge check
				newNodeIndx = nodes[oN[curIndx]].map_id + 1 - mapWidth;
				break;
			case 3:
				if (nodes[oN[curIndx]].map_id % mapWidth != 0) //edge check
				newNodeIndx = nodes[oN[curIndx]].map_id - 1;
				break;
			case 4:
				if (nodes[oN[curIndx]].map_id % mapWidth != mapWidth - 1) //edge check
				newNodeIndx = nodes[oN[curIndx]].map_id + 1;
				break;
			case 5:
				if (nodes[oN[curIndx]].map_id % mapWidth != 0) //edge check
					newNodeIndx = nodes[oN[curIndx]].map_id - 1 + mapWidth;
				break;
			case 6:
				newNodeIndx = nodes[oN[curIndx]].map_id + mapWidth;
				break;
			case 7:

				if (nodes[oN[curIndx]].map_id % mapWidth != mapWidth - 1) //edge check
				newNodeIndx = nodes[oN[curIndx]].map_id +1+ mapWidth;
				break;
			}
			if (points.IsValidIndex(newNodeIndx)) { //check if its a valid index before continuing;
				if (points[newNodeIndx].Z != -1) { // precalculated to be an obstructed location
					newNodePos = points[newNodeIndx]; 
					FStarNode n_child(false, newNodePos,  newNodeIndx, nodes[oN[curIndx]].node_id, -1);
					childNodes.Add(n_child);
				}
			}
		}

		for (int32 c = 0; c < childNodes.Num(); ++c) {
			bool skip = false;
			for (int32 i = 0; i < cN.Num(); ++i) {
				if (childNodes[c].map_id == nodes[cN[i]].map_id) {// we dont want duplicates
					skip = true;
				}
			}

			for (int32 i = 0; i < oN.Num(); ++i) {
				if (childNodes[c].map_id == nodes[oN[i]].map_id) {// we dont want duplicates
					skip = true;
				}
			}



			if (!skip) {
				childNodes[c].cost = nodes[oN[curIndx]].cost + 1;
				childNodes[c].dist = FVector::DistXY(end,childNodes[c].position) / tile_distance;
				childNodes[c].total = childNodes[c].cost + childNodes[c].dist;

				int t_indx = nodes.Add(childNodes[c]);
				nodes[t_indx].node_id = t_indx;
				nodes[t_indx].position = points[nodes[t_indx].map_id];
				oN.Add(t_indx);
			}
		}
		oN.RemoveAt(curIndx);
		cN.Add(curIndx);
	}
	return TArray<FVector>();
}



TArray<FVector>  AAStarActor::solveRandomPath() {
	FVector start = FVector(-1, -1, -1);// storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	FVector end = FVector(-1, -1, -1);//storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	while (start.Z == -1) {
		start = storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	}
	while (end.Z == -1) {
		end = storedMap[FMath::RandRange(0, storedMap.Num() - 1)];
	}

	DrawDebugPoint(GetWorld(),
		FVector(start.X, start.Y, 10), 50,
		FColor(0, 255, 0),
		true, 360);
	DrawDebugPoint(GetWorld(),
		FVector(end.X, end.Y, 10), 50,
		FColor(255, 0, 0),
		true, 360);
	int32 tile_count = FMath::RoundHalfToEven(float(radius) * .04);
	TArray<FVector> t_path = f_solvePath(storedMap, tile_count, start, end);
	return t_path;
}


FStarNode::FStarNode(bool v_rootNode, FVector v_position,int32 _id, int p_id, int32 s_id)
{
	rootNode = v_rootNode;
	map_id = _id;
	parent_id = p_id;
	node_id = s_id;
	if (!rootNode) {
		position = v_position;
	}
	else {
		position = v_position;
	}
}
