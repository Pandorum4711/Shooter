// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (ShooterCharacter == nullptr)
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
    }
    if (ShooterCharacter)
    {
        FVector Velocity{ ShooterCharacter->GetVelocity() };
        Velocity.Z = 0;
        Speed = Velocity.Size();

        bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

        if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
        {
            bIsAccelerating = true;
        //*    UE_LOG(LogTemp, Warning, TEXT("bIsAccelerating: %d"), bIsAccelerating);
        }
        else
        {
            bIsAccelerating = false;
        //*    UE_LOG(LogTemp, Warning, TEXT("bIsAccelerating: %d"), bIsAccelerating);
        }

        FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
        if (ShooterCharacter->GetVelocity().Size() > 0.f)
        {
            LastMovementOffsetYaw = MovementOffsetYaw;
        }
        bAiming = ShooterCharacter->GetAiming();
    
        //FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
        //FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);

        if (GEngine)
        {
            //GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, RotationMessage);
            //GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, MovementRotationMessage);
        }

    }
    
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
