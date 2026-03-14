// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Variant_Shooter/ShooterCharacter.h"

// Sets default values
ABoxActor::ABoxActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");
	SetRootComponent(BoxMesh);

}

// Called when the game starts or when spawned
void ABoxActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Set box color using a dynamic material
void ABoxActor::SetBoxColor(FColor Color)
{
    // Ensure mesh exists
    if (!BoxMesh) return;

    // Create dynamic material instance
    DynamicMaterial = BoxMesh->CreateAndSetMaterialInstanceDynamic(0);

    if (DynamicMaterial)
    {
        // Convert FColor to FLinearColor
        FLinearColor LinearColor = FLinearColor(Color);

        // Apply color to material parameter
        DynamicMaterial->SetVectorParameterValue(TEXT("BoxColor"), LinearColor);
    }
}

// Handle damage when box is hit by projectile
float ABoxActor::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    // Reduce box health
    Health -= 1;

    // If box health reaches zero
    if (Health <= 0)
    {
        if (EventInstigator)
        {
            // Get player who caused the damage
            AShooterCharacter* Player = Cast<AShooterCharacter>(EventInstigator->GetPawn());

            if (Player)
            {
                // Add score to the player
                Player->AddScore(Score);
            }
        }
        // Destroy the box actor
        Destroy();
    }

    return DamageAmount;
}