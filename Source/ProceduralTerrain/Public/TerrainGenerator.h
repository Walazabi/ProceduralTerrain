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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float SpawnChance = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.1f, ClampMax = 128, UIMin = 0.1f, UIMax = 128))
		float Spacing = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180, UIMin = 0, UIMax = 180))
		float AngleVariaton = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float ScaleVariation = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> VegetationActor;
};

UCLASS()
class PROCEDURALTERRAIN_API ATerrainGenerator : public AActor
{
	GENERATED_BODY()

public:
	ATerrainGenerator();
	virtual void Tick(float DeltaTime) override;

	/* Terrain Mesh Parameters */
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 32, UIMin = 1, UIMax = 32))
		int Width = 5;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 32, UIMin = 1, UIMax = 32))
		int Height = 5;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 256, UIMin = 1, UIMax = 256))
		int ChunkWidth = 512;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 256, UIMin = 1, UIMax = 256))
		int ChunkHeight = 512;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float LOD = 0.05;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere)
		bool CreateCollision = true;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere)
		bool DrawDebugVertices = false;
	UPROPERTY(Category = "Terrain Mesh", EditAnywhere, Meta = (MakeEditWidget = true))
		UMaterialInterface* Material;

	/* Terrain Noise Parameters */
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Scale = 100.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere, Meta = (ClampMin = 1, ClampMax = 10, UIMin = 1, UIMax = 10))
		int Octaves = 6;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Frequency = 1.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Amplitude = 7.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere)
		float Lacunarity = 2.0f;
	UPROPERTY(Category = "Terrain Noise", EditAnywhere, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float Persistence = 0.15f;

	/* Terrain Vegetation */
	UPROPERTY(Category = "Terrain Vegetation", EditAnywhere)
		float DefaultSpacing = 50;
	UPROPERTY(Category = "Terrain Vegetation", EditAnywhere, Meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
		float ChunkEdgeMargin = 0.05f;
	UPROPERTY(Category = "Terrain Vegetation", EditAnywhere)
		TArray<FSpawnableVegetation> SpawnableVegetation;

protected:
	virtual void BeginPlay() override;

private:
	/* Mesh Data */
	UProceduralMeshComponent* ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
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

	/* Vegetation Functions */
	void CreateVegetation();
	int GetRandomVegetationIndex();
	FVector GetTerrainZPosition(FVector Position);
	void SpawnVegetation(int VegetationIndex, FVector Position, FRotator Rotation);

	/* Debug */
	void DrawDebugVertex(FVector Position);
};