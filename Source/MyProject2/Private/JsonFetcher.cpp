// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonFetcher.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "BoxActor.h"

AJsonFetcher::AJsonFetcher()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AJsonFetcher::BeginPlay()
{
    Super::BeginPlay();

    FetchJson();
}

// Create and send HTTP request to download JSON file
void AJsonFetcher::FetchJson()
{
    // URL containing box configuration data
    FString Url = "https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json";

    // Create HTTP request
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL(Url);
    Request->SetVerb("GET");
    // Bind response callback function
    Request->OnProcessRequestComplete().BindUObject(this, &AJsonFetcher::OnResponseReceived);

    // Send the request
    Request->ProcessRequest();
}

// Called when HTTP request finishes
void AJsonFetcher::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    // Check if request failed
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Request Failed"));
        return;
    }

    // Check HTTP response code
    if (Response->GetResponseCode() != 200)
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Error Code: %d"), Response->GetResponseCode());
        return;
    }

    // Convert response to string
    FString ResponseString = Response->GetContentAsString();

    // Parse JSON data
    ParseJson(ResponseString);
}

// Convert JSON string into usable data
void AJsonFetcher::ParseJson(const FString& JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("JSON Parse Error"));
        return;
    }

    // Parse types
    const TArray<TSharedPtr<FJsonValue>>* TypesArray;

    if (JsonObject->TryGetArrayField(TEXT("types"), TypesArray))
    {
        for (auto& Value : *TypesArray)
        {
            TSharedPtr<FJsonObject> TypeObject = Value->AsObject();

            FBoxTypeData TypeData;

            TypeData.Name = TypeObject->GetStringField(TEXT("name"));

            const TArray<TSharedPtr<FJsonValue>>* ColorArray;
            TypeObject->TryGetArrayField(TEXT("color"), ColorArray);

            // Convert RGB array to FColor

            int32 R = (*ColorArray)[0]->AsNumber();
            int32 G = (*ColorArray)[1]->AsNumber();
            int32 B = (*ColorArray)[2]->AsNumber();

            TypeData.Color = FColor(R, G, B);

            TypeData.Health = TypeObject->GetIntegerField(TEXT("health"));
            TypeData.Score = TypeObject->GetIntegerField(TEXT("score"));

            BoxTypes.Add(TypeData);
        }
    }

    // Parse objects
    const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;

    if (JsonObject->TryGetArrayField(TEXT("objects"), ObjectsArray))
    {
        for (auto& Value : *ObjectsArray)
        {
            TSharedPtr<FJsonObject> Obj = Value->AsObject();

            FBoxObjectData ObjectData;

            ObjectData.Type = Obj->GetStringField(TEXT("type"));

            TSharedPtr<FJsonObject> Transform = Obj->GetObjectField(TEXT("transform"));

            const TArray<TSharedPtr<FJsonValue>>* LocationArray;
            const TArray<TSharedPtr<FJsonValue>>* RotationArray;
            const TArray<TSharedPtr<FJsonValue>>* ScaleArray;

            // Read transform data (location, rotation, scale)
            Transform->TryGetArrayField(TEXT("location"), LocationArray);
            Transform->TryGetArrayField(TEXT("rotation"), RotationArray);
            Transform->TryGetArrayField(TEXT("scale"), ScaleArray);

            // Convert JSON arrays to UE transform values
            ObjectData.Transform.Location = FVector(
                (*LocationArray)[0]->AsNumber(),
                (*LocationArray)[1]->AsNumber(),
                (*LocationArray)[2]->AsNumber()
            );

            ObjectData.Transform.Rotation = FRotator(
                (*RotationArray)[0]->AsNumber(),
                (*RotationArray)[1]->AsNumber(),
                (*RotationArray)[2]->AsNumber()
            );

            ObjectData.Transform.Scale = FVector(
                (*ScaleArray)[0]->AsNumber(),
                (*ScaleArray)[1]->AsNumber(),
                (*ScaleArray)[2]->AsNumber()
            );

            // Store object data
            BoxObjects.Add(ObjectData);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Parsed %d Box Types"), BoxTypes.Num());
    UE_LOG(LogTemp, Warning, TEXT("Parsed %d Box Objects"), BoxObjects.Num());

    // Spawn boxes in the world
    SpawnBoxes();
}

// Spawn box actors based on parsed JSON data
void AJsonFetcher::SpawnBoxes()
{
    // Get current game world
    UWorld* World = GetWorld();

    if (!World) return;

    for (const FBoxObjectData& Obj : BoxObjects)
    {
        FTransform SpawnTransform;
        // Create transform for spawning

        SpawnTransform.SetLocation(Obj.Transform.Location);
        SpawnTransform.SetRotation(FQuat(Obj.Transform.Rotation));
        SpawnTransform.SetScale3D(Obj.Transform.Scale);

        // Spawn box actor
        ABoxActor* SpawnedBox = World->SpawnActor<ABoxActor>(BoxClass, SpawnTransform);

        if (!SpawnedBox)
            continue;

        for (const FBoxTypeData& Type : BoxTypes)
        {
            // Match object type with box type definition
            if (Type.Name == Obj.Type)
            {
                // Apply health, score, and color
                SpawnedBox->Health = Type.Health;
                SpawnedBox->Score = Type.Score;
                SpawnedBox->SetBoxColor(Type.Color);

                break;
            }
        }
    }
}
