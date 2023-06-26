#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

class ATankPawn;

UCLASS()
class TANKODROM_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
		ATankPawn* TankPawn;
	UPROPERTY()
		FVector MousePos;

public:
	ATankPlayerController();
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	FVector GetMousePos() { return MousePos; };
	float Xdif = 0;
	float Ydif = 0;

protected:
	virtual void BeginPlay() override;
	void MoveForward(float AxisValue);
	void RotateRight(float AxisValue);
	void RotateTurretRight(float AxisValue);
	void Fire();
	void FireAlt();
};
