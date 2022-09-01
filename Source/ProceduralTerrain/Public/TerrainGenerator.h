// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <cstddef>
#include <cstdint>  
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "TerrainGenerator.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct PROCEDURALTERRAIN_API FSpawnableVegetation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
	int32 Test;

};

UCLASS()
class PROCEDURALTERRAIN_API ATerrainGenerator : public AActor
{
	GENERATED_BODY()

public:
	ATerrainGenerator();
	virtual void Tick(float DeltaTime) override;

	/* Mesh Parameters */
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 32, UIMin = 1, UIMax = 32))
		int Width = 5;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 32, UIMin = 1, UIMax = 32))
		int Height = 5;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 256, UIMin = 1, UIMax = 256))
		int ChunkWidth = 25;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 256, UIMin = 1, UIMax = 256))
		int ChunkHeight = 25;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float LOD = 0;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere)
		bool CreateCollision = false;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere)
		bool DrawDebugVertices = false;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (MakeEditWidget = true))
		UMaterialInterface* Material;

	/* Noise Parameters */
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Scale = 1.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 10, UIMin = 1, UIMax = 10))
		int Octaves = 1;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Frequency = 1.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Amplitude = 1.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Lacunarity = 2.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float Persistence = 0.5f;

	/* Vegetation */
	//UPROPERTY(Category = "Vegetation", EditAnywhere)
		//UStaticMesh CactusMesh;

protected:
	virtual void BeginPlay() override;

private:
	/* Mesh Data */
	//TArray<UProceduralMeshComponent*> TerrainMeshes;
	UProceduralMeshComponent* ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	UPROPERTY(EditAnywhere)
	TArray<FVector2D> UV0;
	FVector Offset;

	FIntPoint QuadAmount;
	FVector QuadSize;

	/* Chunk Functions */
	void CreateChunk();

	/* Mesh Functions */
	void CalculateLOD();
	void CreateVertices();
	void CreateTriangles();
	void CreateMesh();

	/* Noise Functions */
	static float Noise(float x, float y);
	static float PerlinNoise2D(float x, float y, float scale = 1.0f, int octaves = 1, float frequency = 1.0f, float amplitude = 1.0f, float lacunarity = 2.0f, float persistence = 0.5f);

	/* Debug */
	void DrawDebugVertex(FVector Position);
};