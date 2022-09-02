// Procedural Terrain Generation made for school assignment. Created by Mattias Gustafsson (Walazabi): https://github.com/Walazabi/ProceduralTerrain

#include "TerrainGenerator.h"

#pragma region Events

ATerrainGenerator::ATerrainGenerator()
{
    PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
    ProceduralMesh->SetupAttachment(GetRootComponent());  
}

void ATerrainGenerator::BeginPlay()
{
    Super::BeginPlay();

    /* Initialize Seed */
    srand((unsigned int)time(NULL));

    CalculateLOD();
    CreateVertices();
    CreateTriangles();
    CreateMesh();
    CreateVegetation();
}

void ATerrainGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

#pragma endregion

#pragma region Chunks

void ATerrainGenerator::CreateChunk()
{
    for (int y = 0; y < Height; y++)
    {
        for (int x = 0; x < Width; x++)
        {
        }
    }
}

#pragma endregion

#pragma region Mesh

void ATerrainGenerator::CalculateLOD()
{
    /* Calculate amount of quads per axis depending on LOD */
    QuadAmount.X = FMath::RoundToInt(FMath::Clamp(ChunkWidth * LOD, 1, 256));
    QuadAmount.Y = FMath::RoundToInt(FMath::Clamp(ChunkHeight * LOD, 1, 256));

    /* Calcualte size of each quad */
    QuadSize.X = (float)ChunkWidth / QuadAmount.X;
    QuadSize.Y = (float)ChunkHeight / QuadAmount.Y;
}

void ATerrainGenerator::CreateVertices()
{
    Offset = FVector((float)ChunkWidth / 2, (float)ChunkHeight / 2, 0);

    for (int x = 0; x <= QuadAmount.X; x++)
    {
        for (int y = 0; y <= QuadAmount.Y; y++)
        {
            /* Set vertex position and offset height with noise */
            FVector VertexPosition = FVector(x * QuadSize.X, y * QuadSize.Y, 0) - Offset;
            VertexPosition.Z = PerlinNoise2D(VertexPosition.X, VertexPosition.Y, Scale, Octaves, Frequency, Amplitude, Lacunarity, Persistence);

            Vertices.Add(VertexPosition);
            DrawDebugVertex(VertexPosition);
            UV0.Add(FVector2D(VertexPosition.X, VertexPosition.Y));
        }
    }
}

void ATerrainGenerator::CreateTriangles()
{
    int VertexIndex = 0;
    for (int x = 0; x < QuadAmount.X; x++)
    {
        for (int y = 0; y < QuadAmount.Y; y++)
        {
            Triangles.Add(VertexIndex);
            Triangles.Add(VertexIndex + 1);
            Triangles.Add(VertexIndex + QuadAmount.Y + 1);

            Triangles.Add(VertexIndex + 1);
            Triangles.Add(VertexIndex + QuadAmount.Y + 2);
            Triangles.Add(VertexIndex + QuadAmount.Y + 1);

            VertexIndex++;
        }
        VertexIndex++;
    }
}

void ATerrainGenerator::CreateMesh()
{
    ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), CreateCollision);
    ProceduralMesh->SetMaterial(0, Material);
}

#pragma endregion

#pragma region Noise

/* Unreal Engine 5 implementation of Noise Library created by Sebastien Rombauts (sebastien.rombauts@gmail.com) */
/* https://github.com/SRombauts/SimplexNoise/blob/master/LICENSE.txt */

static const uint8_t Perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

static uint8_t Hash(int32_t i)
{
    return Perm[static_cast<uint8_t>(i)];
}

static int32_t Fastfloor(float fp)
{
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
}

static float Grad(int32_t Hash, float x, float y)
{
    /* Convert low 3 bits of hash code -> */
    const int32_t h = Hash & 0x3F;

    /* Into 8 simple gradient directions -> */
    const float u = h < 4 ? x : y;
    const float v = h < 4 ? y : x;

    /* And compute the dot product with (x,y) */
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float ATerrainGenerator::Noise(float x, float y)
{
    /* Noise contributions from the three corners */
    float n0, n1, n2;

    /* Skewing/Unskewing factors for 2D */
    /* F2 = (sqrt(3) - 1) / 2 */
    static const float F2 = 0.366025403f;
    /* G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K) */
    static const float G2 = 0.211324865f;

    /* Skew the input space to determine which simplex cell we're in */
    const float s = (x + y) * F2;
    const float xs = x + s;
    const float ys = y + s;
    const int32_t i = Fastfloor(xs);
    const int32_t j = Fastfloor(ys);

    /* Unskew the cell origin back to (x,y) space */
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0;
    const float y0 = y - Y0;

    /* Determine which simplex we are in */
    /* Offsets for second (middle) corner of simplex in (i,j) coords */
    int32_t i1, j1;

    /* Lower triangle, XY order: (0,0)->(1,0)->(1,1) */
    if (x0 > y0) {
        i1 = 1;
        j1 = 0;
    }
    /* Upper triangle, YX order: (0,0)->(0,1)->(1,1) */
    else {
        i1 = 0;
        j1 = 1;
    }

    /* Offsets for middle corner in (x,y) unskewed coords */
    const float x1 = x0 - i1 + G2;
    const float y1 = y0 - j1 + G2;
    /* Offsets for last corner in (x,y) unskewed coords */
    const float x2 = x0 - 1.0f + 2.0f * G2;
    const float y2 = y0 - 1.0f + 2.0f * G2;

    /* Work out the hashed gradient indices of the three simplex corners */
    const int gi0 = Hash(i + Hash(j));
    const int gi1 = Hash(i + i1 + Hash(j + j1));
    const int gi2 = Hash(i + 1 + Hash(j + 1));

    /* Calculate the contribution from the first corner */
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    }
    else {
        t0 *= t0;
        n0 = t0 * t0 * Grad(gi0, x0, y0);
    }

    /* Calculate the contribution from the second corner */
    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    }
    else {
        t1 *= t1;
        n1 = t1 * t1 * Grad(gi1, x1, y1);
    }

    /* Calculate the contribution from the third corner */
    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    }
    else {
        t2 *= t2;
        n2 = t2 * t2 * Grad(gi2, x2, y2);
    }

    /* Add contributions from each corner to get the final noise value. */
    /* The result is scaled to return values in the interval [-1,1]. */
    return 45.23065f * (n0 + n1 + n2);
}

float ATerrainGenerator::PerlinNoise2D(float x, float y, float scale, int octaves, float frequency, float amplitude, float lacunarity, float persistence)
{
    float output = 0.f;
    float denom = 0.f;
    float t_frequency = frequency;
    float t_amplitude = amplitude;

    for (int i = 0; i < octaves; i++) {
        output += (t_amplitude * Noise(x / scale * t_frequency, y / scale * t_frequency));
        denom += t_amplitude;

        t_frequency *= lacunarity;
        t_amplitude *= persistence;
    }

    //return (output / denom);
    return (output);
}

#pragma endregion

#pragma region Vegetation

void ATerrainGenerator::CreateVegetation() 
{
    /* Calculate placement EdgeMargin */
    FVector2D EdgeMargin = FVector2D(ChunkWidth * ChunkEdgeMargin, ChunkHeight * ChunkEdgeMargin);

    for (float y = EdgeMargin.Y; y < ChunkHeight - EdgeMargin.Y;)
    {
        for (float x = EdgeMargin.X; x < ChunkWidth - EdgeMargin.X;)
        {
            int VegetationIndex = GetRandomVegetationIndex();

            /* Random function resulted in Vegetation to spawn */
            if (VegetationIndex >= 0 && VegetationIndex < SpawnableVegetation.Num()) 
            {
                SpawnVegetation(VegetationIndex, GetTerrainZPosition(FVector(x, y, 0) - Offset), FRotator(0, 0, 0));
                x += SpawnableVegetation[VegetationIndex].Spacing;
            }
            /* No Vegetation spawned */
            else 
            {
                x += DefaultSpacing;
            } 
        }
        y += DefaultSpacing;
    }
}

FVector ATerrainGenerator::GetTerrainZPosition(FVector Position)
{
    FHitResult Hit;
    FVector Start = Position + 256;
    FVector End = Position - 256;

    FCollisionQueryParams TraceParameters;
    bool TraceHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParameters);

    if (TraceHit) 
    {
        return Hit.ImpactPoint;
    }
    else 
    {
        return Position;
    }
}

int ATerrainGenerator::GetRandomVegetationIndex() 
{
    /* Return integer by using 'Roulette Wheel Selection' */
    float Total = 0;
    float RandomFloat = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    /* Iterate Array of Spawnable Vegetation */
    for (int i = 0; i < SpawnableVegetation.Num(); i++)
    {
        /* Compare if SpawnChance <= RandomFloat */
        Total += SpawnableVegetation[i].SpawnChance;
        if (RandomFloat <= Total)
        {
            /* Return Index of SpawnableVegetation */
            return i;
        }
    }
    /* Return -1 for "Null/IsValid check" */
    return -1;
}

void ATerrainGenerator::SpawnVegetation(int VegetationIndex, FVector Position, FRotator Rotation)
{
    FActorSpawnParameters SpawnParameters;
    AActor* ActorReference = GetWorld()->SpawnActor<AActor>(SpawnableVegetation[VegetationIndex].VegetationActor, Position, Rotation, SpawnParameters);
    
    /* Add Random Scale */
    float RandomScaleVariation = FMath::FRandRange(-SpawnableVegetation[VegetationIndex].ScaleVariation, SpawnableVegetation[VegetationIndex].ScaleVariation);
    FVector RandomizedScale = ActorReference->GetActorScale3D();
    RandomizedScale.X += RandomizedScale.X * RandomScaleVariation;
    RandomizedScale.Y += RandomizedScale.Y * RandomScaleVariation;
    RandomizedScale.Z += RandomizedScale.Z * RandomScaleVariation;
    ActorReference->SetActorScale3D(RandomizedScale);

    /* Add Random Rotation */
    ActorReference->AddActorLocalRotation(FRotator(FMath::FRandRange(-SpawnableVegetation[VegetationIndex].AngleVariaton, -SpawnableVegetation[VegetationIndex].AngleVariaton), FMath::FRandRange(-180, 180), FMath::FRandRange(-SpawnableVegetation[VegetationIndex].AngleVariaton, -SpawnableVegetation[VegetationIndex].AngleVariaton)));
}

#pragma endregion


#pragma region Debug

void ATerrainGenerator::DrawDebugVertex(FVector Position)
{
    if (DrawDebugVertices && ChunkWidth <= 25 && ChunkHeight <= 25) {
        DrawDebugSphere(GetWorld(), Position, (0.5f / 5), 16, FColor::Red, true, -1.0f, 0U, 0.0f);
    }
}

#pragma endregion
