// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxActor.generated.h"

UCLASS()
class MYPROJECT2_API ABoxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxActor();

	// Static mesh used for the box visual
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BoxMesh;

	// Remaining health of the box
	int32 Health;
	// Score awarded when box is destroyed
	int32 Score;

	// Dynamic material instance for changing box color
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	// Change box color using dynamic material
	void SetBoxColor(FColor Color);

	// Handle damage when projectile hits the box
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
