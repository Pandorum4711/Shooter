// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    //** Called for forwards/backwards input */
    void MoveForward(float Value);

    //** Called for left/right input */
    void MoveRight(float Value);

    //** Called via input to turn at a given rate */
    void TurnAtRate(float Rate);

    //** Called via input to look up/down at a given rate */
    void LookUpAtRate(float Rate);

    void Turn(float Value);
    void LookUp(float Value);

    /** Called when fire button is pressed */
    void FireWeapon();

    bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

    /** Set bAiming */
    void AimingButtonPressed();
    void AimingButtonReleased();

    void CameraInterpZoom(float DeltaTime);
    void SetLookRates();

    void CalculateCrosshairSpread(float DeltaTime); 

    void StartCrosshairBulletFire();
    UFUNCTION()
    void FinishCrosshairBulletFire();

    void FireButtonPressed();
    void FireButtonReleased();

    void StartFireTimer();
    UFUNCTION()
    void AutoFireReset();

    /* Line trace for items under the crosshair */
    bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

    void TraceForItems();

    class AWeapon* SpawnDefaultWeapon();

    void EquipWeapon(AWeapon* WeaponToEquip);

    void DropWeapon();

    void SelectButtonPressed();
    void SelectButtonReleased();

    void SwapWeapon(AWeapon* WeaponToSwap);
        
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Camera that follows the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    /** Base turn rate in deg/sec. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float BaseTurnRate;

    /** Base look up/down rate in deg/sec. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float BaseLookUpRate;

    /** Hip turn rate in deg/sec. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float HipTurnRate;

    /** Hip look up/down rate in deg/sec. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float HipLookUpRate;

    /** Aiming turn rate in deg/sec. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float AimingTurnRate;

    /** Aiming look up/down rate in deg/sec. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float AimingLookUpRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipTurnRate;
 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipLookUpRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimingTurnRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimingLookUpRate;

    /** Randomized gunshot sound cue */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class USoundCue* FireSound;

    /** Flash spawned at barrel socket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class UParticleSystem* MuzzleFlash;

    /** Montage for firing the weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class UAnimMontage* HipFireMontage;

    /** Particles spawned upon bullet impact */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* ImpactParticles;

    /** Smoke trail for bullets */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* BeamParticles;

    /**  True when aiming */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    bool bAiming;

    /**  Default camera field of view */
    float CameraDefaultFOV;

    /**  Zoomed-in camera field of view */
    float CameraZoomedFOV;

    /**  Current camera field of view */
    float CameraCurrentFOV;

    /** Interp speed for zooming when aiming */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float ZoomInterpSpeed;

    /**  -------------------- Category: Crooshairs --------------------*/
    /**                                                                */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairSpreadMultiplier;;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairVelocityFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairInAirFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairAimFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairShootingFactor;

    float ShootTimeDuration;
    bool bFiringBullet;
    FTimerHandle CrosshairShootTimer;

    bool bFireButtonPressed;
    bool bShouldFire;
    float AutomaticFireRate;
    FTimerHandle AutoFireTimer;

    bool bShouldTraceForItems;
    int8 OverlappedItemCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    class AItem* TraceHitItemLastFrame;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    AWeapon* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AWeapon> DefaultWeaponClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    AItem* TraceHitItem;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float CameraInterpDistance;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float CameraInterpElevation;

public:
    /** Returns CameraBoom subobject */
    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /** Returns FollowCamera subobject */
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    /** Returns FollowCamera subobject */
    FORCEINLINE bool GetAiming() const { return bAiming; }

    UFUNCTION(BlueprintCallable)
    float GetCrosshairSpreadMultiplier() const; 

    /**  */
    FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

    void IncrementOverlappedItemCount(int8 Amount);

    FVector GetCameraInterpLocation();

    void GetPickupItem(AItem* Item);
};
