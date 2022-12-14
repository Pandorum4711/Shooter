// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
    BaseTurnRate(45.f),
    BaseLookUpRate(45.f),
    HipTurnRate(90.f),
    HipLookUpRate(90.f),
    AimingTurnRate(20.f),
    AimingLookUpRate(20.f),
    MouseHipTurnRate(1.0f),
    MouseHipLookUpRate(1.0f),
    MouseAimingTurnRate(0.2f),
    MouseAimingLookUpRate(0.2f),
    bAiming(false),
    CameraDefaultFOV(0.f),
    CameraZoomedFOV(35.f),
    CameraCurrentFOV(0.f),
    ZoomInterpSpeed(20.f),

    CrosshairSpreadMultiplier(0.f),
    CrosshairVelocityFactor(0.f),
    CrosshairInAirFactor(0.f),
    CrosshairAimFactor(0.f),
    CrosshairShootingFactor(0.f),

    ShootTimeDuration(0.05f),
    bFiringBullet(false),
    AutomaticFireRate(0.1f),
    bShouldFire(true),
    bFireButtonPressed(false),
    bShouldTraceForItems(false),

    CameraInterpDistance(250.f),
    CameraInterpElevation(65.f)


{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //* Create a camera boom (pulls in towards the charater if there is a collision) */
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 180.f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

    //* Create a follow camera */
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    //* Don't rotate when the controller rotates */
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw   = true;
    bUseControllerRotationRoll = false;

    //* Configure character movement */
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;
    }

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//* UE_LOG(LogTemp, Warning, TEXT("BeginPlay() called!"));
	
    if (FollowCamera)
    {
        CameraDefaultFOV = GetFollowCamera()->FieldOfView;
        CameraCurrentFOV = CameraDefaultFOV;
    }
    EquipWeapon(SpawnDefaultWeapon());
}

/**/
void AShooterCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation{ Controller->GetControlRotation() };
        const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

        const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
        AddMovementInput(Direction, Value);
    }
}
/**/
void AShooterCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation{ Controller->GetControlRotation() };
        const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

        const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
        AddMovementInput(Direction, Value);
    }
}
/**/
void AShooterCharacter::TurnAtRate(float Rate)
{
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

/**/
void AShooterCharacter::LookUpAtRate(float Rate)
{
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
    float TurnScaleFactor;
    if (bAiming)
    {
        TurnScaleFactor = MouseAimingTurnRate;
    }
    else
    {
        TurnScaleFactor = MouseHipTurnRate;
    }
    AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
    float LookUpScaleFactor;
    if (bAiming)
    {
        LookUpScaleFactor = MouseAimingLookUpRate;
    }
    else
    {
        LookUpScaleFactor = MouseHipLookUpRate;
    }
    AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
    if(FireSound)
    {
        UGameplayStatics::PlaySound2D(this, FireSound);
    }
    const  USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
    if (BarrelSocket)
    {
        const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

        if (MuzzleFlash)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
        }
 
        FVector BeamEnd;
        bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

        if (bBeamEnd)
        {
            if (ImpactParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
            }
            UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);

            if (Beam)
            {
                Beam->SetVectorParameter(FName("Target"), BeamEnd);
            }
        }
   }
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HipFireMontage)
    {
        AnimInstance->Montage_Play(HipFireMontage);
        AnimInstance->Montage_JumpToSection(FName("StartFire"));
    }
    StartCrosshairBulletFire();
}

bool AShooterCharacter::GetBeamEndLocation(
    const FVector& MuzzleSocketLocation,
    FVector& OutBeamLocation)
{
    /* Check for crosshair trace hit */
    FHitResult CrosshairHitResult;
    bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

    if (bCrosshairHit)
    {
        /* Tentative beam location - still need to trace from gun */
        OutBeamLocation = CrosshairHitResult.Location;
    }
    else /* no crosshair trace hit */
    {
        /* OutBeamLocation is the End location for the line trace */
    }

    /** Perform a second trace from the gun barrel */
    FHitResult WeaponTraceHit;
    const FVector WeaponTraceStart{ MuzzleSocketLocation };
    const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
    const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

    /** Trace outward from barrel world location */
    GetWorld()->LineTraceSingleByChannel(
        WeaponTraceHit,
        WeaponTraceStart,
        WeaponTraceEnd,
        ECollisionChannel::ECC_Visibility);

    if (WeaponTraceHit.bBlockingHit)
    {
        OutBeamLocation = WeaponTraceHit.Location;
        return true;
    }
    return false;
}

void AShooterCharacter::AimingButtonPressed()
{
    bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
    bAiming = false;
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{

    if (bAiming)
    {
        CameraCurrentFOV = FMath::FInterpTo(
            CameraCurrentFOV,
            CameraZoomedFOV,
            DeltaTime,
            ZoomInterpSpeed);
    }
    else
    {
        CameraCurrentFOV = FMath::FInterpTo(
            CameraCurrentFOV,
            CameraDefaultFOV,
            DeltaTime,
            ZoomInterpSpeed);
    }
    GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
 }

void AShooterCharacter::SetLookRates()
{

    if (bAiming)
    {
        BaseTurnRate = AimingTurnRate;
        BaseLookUpRate = AimingLookUpRate;
    }
    else
    {
        BaseTurnRate = HipTurnRate;
        BaseLookUpRate = HipLookUpRate;
    }
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
    FVector2D WalkSpeedRange{0.f, 600.f};
    FVector2D VelocityMultiplierRange{ 0.f, 1.f };
    FVector Velocity{ GetVelocity() };
    Velocity.Z = 0.f;

    CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
        WalkSpeedRange,
        VelocityMultiplierRange,
        Velocity.Size());

    if(GetCharacterMovement()->IsFalling())
    {
        CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
    }
    else
    {
        CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
    }

    if (bAiming)
    {
        CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
    }
    else
    {
        CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
    }
    if (bFiringBullet)
    {
        CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
    }
    else
    {
        CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 60.f);
    }
    CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
    bFiringBullet = true;

    GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
    bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
    bFireButtonPressed = true;
    StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
    bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
    if (bShouldFire)
    {
        FireWeapon();
        bShouldFire = false;
        GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
    }
}

void AShooterCharacter::AutoFireReset()
{
    bShouldFire = true;
    if (bFireButtonPressed)
    {
        StartFireTimer();
    }
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
    /** Get current size of viewport */
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    /** Get screen space location of crosshair */
    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    //* Get world position and direction of crosshair */
    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection);

    //* was deprojection successful? */
    if (bScreenToWorld)
    {
        const FVector Start{ CrosshairWorldPosition };
        const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50000.f };
        OutHitLocation = End;

        /** Trace outward from crosshairs world location */
        GetWorld()->LineTraceSingleByChannel(
            OutHitResult,
            Start,
            End,
            ECollisionChannel::ECC_Visibility);

        if (OutHitResult.bBlockingHit)
        {
            OutHitLocation = OutHitResult.Location;
            return true;
        }
    }

    return false;
}

void AShooterCharacter::TraceForItems()
{
    if (bShouldTraceForItems)
    {
        FHitResult ItemTraceResult;
        FVector HitLocation;
        TraceUnderCrosshairs(ItemTraceResult, HitLocation);
        if (ItemTraceResult.bBlockingHit)
        {
            TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
            if (TraceHitItem && TraceHitItem->GetPickupWidget())
            {
                TraceHitItem->GetPickupWidget()->SetVisibility(true);
            }
            if (TraceHitItemLastFrame)
            {
                if (TraceHitItem != TraceHitItemLastFrame)
                {
                    TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
                }
            }
            TraceHitItemLastFrame = TraceHitItem;
        }
    }
    else if (TraceHitItemLastFrame)
    {
        TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
    }
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
    if (DefaultWeaponClass)
    {
        return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
    }
    return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
    if (WeaponToEquip)
    {
             const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
        
        if (HandSocket)
        {
            HandSocket->AttachActor(WeaponToEquip, GetMesh());
        }
        EquippedWeapon = WeaponToEquip;
        EquippedWeapon->SetItemState(EItemState::EIS_Equipped);

    }
}

void AShooterCharacter::DropWeapon()
{
    if (EquippedWeapon)
    {
        FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
        EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

        EquippedWeapon->SetItemState(EItemState::EIS_Falling);
        EquippedWeapon->ThrowWeapon();
    }
}

void AShooterCharacter::SelectButtonPressed()
{
    if (TraceHitItem)
    {
        TraceHitItem->StartItemCurve(this);
    }
}

void AShooterCharacter::SelectButtonReleased()
{

}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
    DropWeapon();
    EquipWeapon(WeaponToSwap);
    TraceHitItem = nullptr;
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    CameraInterpZoom(DeltaTime);

    SetLookRates();

    CalculateCrosshairSpread(DeltaTime);

    TraceForItems();

 }



// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

    PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

    PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
    PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

    PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
    PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

    PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
    PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
    return CrosshairSpreadMultiplier;;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
    if (OverlappedItemCount + Amount <= 0)
    {
        OverlappedItemCount = 0;
        bShouldTraceForItems = false;
    }
    else
    {
        OverlappedItemCount += Amount;
        bShouldTraceForItems = true;
    }
}

FVector AShooterCharacter::GetCameraInterpLocation()
{
    const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
    const FVector CameraForward{ FollowCamera->GetForwardVector() };
           
    return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
    auto Weapon = Cast<AWeapon>(Item);
    if (Weapon)
    {
        SwapWeapon(Weapon);
    }

}


