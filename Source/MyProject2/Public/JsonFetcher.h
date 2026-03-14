// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "JsonStructs.h"
#include "JsonFetcher.generated.h"

class ABoxActor;

// Actor responsible for fetching JSON and spawning boxes
UCLASS()
class MYPROJECT2_API AJsonFetcher : public AActor
{
	GENERATED_BODY()

public:
	AJsonFetcher();

protected:
	virtual void BeginPlay() override;

private:

	// Sends HTTP request to download JSON data
	void FetchJson();

	// Callback executed when HTTP response arrives
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Parse JSON string into game data structures
	void ParseJson(const FString& JsonString);

	// Spawn boxes in the world using parsed data
	void SpawnBoxes();

	// Stores all box type definitions from JSON
	TArray<FBoxTypeData> BoxTypes;

	// Stores individual box objects from JSON
	TArray<FBoxObjectData> BoxObjects;

	// Box actor class used for spawning
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABoxActor> BoxClass;

};