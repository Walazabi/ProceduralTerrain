// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainMesh.h"

ATerrainMesh::ATerrainMesh()
{
	PrimaryActorTick.bCanEverTick = true;
	//int test = Width + Height;
}

void ATerrainMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATerrainMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}