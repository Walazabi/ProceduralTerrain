// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainMesh.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API ATerrainMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	//ATerrainMesh(int Width, int Height);
	ATerrainMesh();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};