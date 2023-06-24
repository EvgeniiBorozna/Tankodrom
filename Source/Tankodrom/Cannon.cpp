#include "Cannon.h"

//#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* sceeneCpm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceeneCpm;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);
}
void ACannon::Fire(ECannonType FireType)
{
	if (!ReadyToFire)
	{
		return;
	}
	ReadyToFire = false;

	if (FireType == ECannonType::FireProjectile)
	{
		if (ShellsCount > 0) {
			GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - projectile");
			ShellsCount--;
			GEngine->AddOnScreenDebugMessage(25, 1, FColor::Green, FString::SanitizeFloat(ShellsCount));
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
		}
		else
			GEngine->AddOnScreenDebugMessage(25, 1, FColor::Red, "No shells");
	}
	else
	{
		for (int i = 0; i < 3; i++) {
			if (ShellsCount > 0) {
				GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - trace");
				ShellsCount--;
				GEngine->AddOnScreenDebugMessage(25 + i, 1, FColor::Green, FString::SanitizeFloat(ShellsCount));
				GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
			}
			else
				GEngine->AddOnScreenDebugMessage(25 + i, 1, FColor::Red, "No shells");
		}
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
}

bool ACannon::IsReadyToFire()
{
	return ReadyToFire;
}

void ACannon::Reload()
{
	ReadyToFire = true;
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	Reload();
}