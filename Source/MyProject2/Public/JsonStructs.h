#pragma once

#include "CoreMinimal.h"
#include "JsonStructs.generated.h"

USTRUCT()
struct FBoxTypeData
{
    GENERATED_BODY()

    // Name of the box type
    FString Name;

    // Color used for the box material
    FColor Color;

    // Initial health of the box
    int32 Health;

    // Score awarded when destroyed
    int32 Score;
};

// Stores transform data for spawning boxes
USTRUCT()
struct FBoxTransformData
{
    GENERATED_BODY()

    // Box spawn location
    FVector Location;

    // Box spawn location
    FRotator Rotation;

    // Box scale in the world
    FVector Scale;
};

// Box scale in the world
USTRUCT()
struct FBoxObjectData
{
    GENERATED_BODY()

    // Stores individual box object data from JSON
    FString Type;

    // Transform information for spawning
    FBoxTransformData Transform;
};