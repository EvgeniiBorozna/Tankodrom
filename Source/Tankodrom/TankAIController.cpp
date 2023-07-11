#include "TankAIController.h"
#include "TankPawn.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

void ATankAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TankPawn)
        Initialize();

    if (!TankPawn)
        return;

    TankPawn->MoveForward(1);

    float rotationValue = GetRotationValue();
    TankPawn->RotateRight(rotationValue);

    Targeting();
}

float ATankAIController::GetRotationValue()
{
    FVector currentPoint = PatrollingPoints[CurrentPatrolPointIndex];
    FVector pawnLocation = TankPawn->GetActorLocation();
    if (FVector::Distance(currentPoint, pawnLocation) <= MovementAccurency)
    {
        CurrentPatrolPointIndex++;
        if (CurrentPatrolPointIndex >= PatrollingPoints.Num())
            CurrentPatrolPointIndex = 0;
    }

    FVector moveDirection = currentPoint - pawnLocation;
    moveDirection.Normalize();
    FVector forwardDirection = TankPawn->GetActorForwardVector();
    FVector rightDirection = TankPawn->GetActorRightVector();

    //DrawDebugLine(GetWorld(), pawnLocation, currentPoint, FColor::Green, false, 5, 0, 5);

    float forwardAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(forwardDirection, moveDirection)));
    float rightAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(rightDirection, moveDirection)));

    float rotationValue = 0;
    if (forwardAngle > 5)
        rotationValue = 1;
    if (rightAngle > 90)
        rotationValue = -rotationValue;

    return rotationValue;
}

void ATankAIController::Targeting()
{
    
    if (CanFire()) {
        GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, "FIRE!!!");
        Fire();
    }
    else
        RotateToPlayer();
}

void ATankAIController::RotateToPlayer()
{
    if (IsPlayerInRange())
        TankPawn->RotateTurretTo(PlayerPawn->GetActorLocation());
}

bool ATankAIController::IsPlayerInRange()
{
    if (!PlayerPawn)
        Initialize();

    if (!PlayerPawn)
        return false;

    return FVector::Distance(TankPawn->GetActorLocation(), PlayerPawn->GetActorLocation()) <= TargetingRange;
}

bool ATankAIController::CanFire()
{
    if (!IsPlayerSeen())
        return false;

    FVector targetingDir = TankPawn->GetTurretForwardVector();
    FVector dirToPlayer = PlayerPawn->GetActorLocation() - TankPawn->GetActorLocation();
    dirToPlayer.Normalize();
    float aimAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(targetingDir, dirToPlayer)));
    GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, FString::SanitizeFloat(aimAngle-Accurency));
    return aimAngle <= Accurency;
}

bool ATankAIController::IsPlayerSeen()
{
    if (!PlayerPawn)
        Initialize();

    if (!PlayerPawn)
        return false;

    FVector playerPos = PlayerPawn->GetActorLocation();
    FVector eyesPos = TankPawn->GetEyesPosition();
    //eyesPos.Z = playerPos.Z;

    FHitResult hitResult;
    FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
    traceParams.bTraceComplex = true;
    traceParams.AddIgnoredActor(TankPawn);
    traceParams.bReturnPhysicalMaterial = false;

    //GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, playerPos.ToString());
    //GEngine->AddOnScreenDebugMessage(18, 15.0f, FColor::Red, eyesPos.ToString());

    //if 
    (GetWorld()->LineTraceSingleByChannel(hitResult, eyesPos, playerPos, ECollisionChannel::ECC_Visibility, traceParams));
    {
        GEngine->AddOnScreenDebugMessage(18, 15.0f, FColor::Red, hitResult.ToString());
        if (hitResult.GetActor())
        {
            DrawDebugLine(GetWorld(), eyesPos, hitResult.Location, FColor::Cyan, false, 0.5f, 0, 10);
            return hitResult.GetActor() == PlayerPawn;
        }
    }
    return false;
}

void ATankAIController::Fire()
{
    TankPawn->Fire();
    
}

void ATankAIController::Initialize()
{
    TankPawn = Cast<ATankPawn>(GetPawn());
    if (TankPawn)
    {
        PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
        FVector pawnLocation = TankPawn->GetActorLocation();
        MovementAccurency = TankPawn->GetMovementAccurency();
        TArray<FVector> points = TankPawn->GetPatrollingPoints();
        for (FVector point : points)
        {
            PatrollingPoints.Add(point);
        }
        CurrentPatrolPointIndex = 0;
    }
}